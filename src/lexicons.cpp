/*
** Copyright (©) 2003-2009 Teus Benschop.
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**  
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#include "lexicons.h"
#include <gtk/gtk.h>
#include "gwrappers.h"
#include "progresswindow.h"
#include <libxml/xmlreader.h>
#include "utilities.h"
#include "books.h"
#include "tiny_utilities.h"
#include "directories.h"
#include "unixwrappers.h"
#include "sqlite_reader.h"


ustring lexicons_get_strongs_greek_xml_filename()
// Gives the filename for the Strong's Greek lexicon XML file that comes with bibledit.
{
  return gw_build_filename(directories_get_package_data(), "strongsgreek.xml");
}


ustring lexicons_get_sql_filename()
// Gives the filename for the created lexicons database.
{
  return gw_build_filename(directories_get_databases(), "lexicons.sql");
}


const gchar* lexicons_database_group_name ()
{
  return "lexicons";
}


const gchar * lexicon_strongs_greek_xml ()
{
  return "strongsgreekxml";
}


const gchar * lexicons_sql ()
{
  return "sql";
}


void lexicons_import (GKeyFile *keyfile)
{
  // See whether to import the lexicon files into the database.
  // Normally this happens once upon installation.
  // If it has been done already, and everything seems fine, bail out.
  bool import = false;
  unsigned int value;
  value = g_key_file_get_integer(keyfile, lexicons_database_group_name (), lexicon_strongs_greek_xml(), NULL);
  if (value != file_get_size (lexicons_get_strongs_greek_xml_filename())) {
    import = true;
  }
  value = g_key_file_get_integer(keyfile, lexicons_database_group_name (), lexicons_sql(), NULL);
  if (value != file_get_modification_time (lexicons_get_sql_filename())) {
    import = true;
  }
  if (!import) {
    return;
  }

  // Remove any previous database.
  unlink (lexicons_get_sql_filename().c_str());

  // Create the user database.
  sqlite3 *db;
  sqlite3_open(lexicons_get_sql_filename().c_str(), &db);
  sqlite3_exec(db, "create table strongsgreek (strongs integer, entry text);", NULL, NULL, NULL);
  sqlite3_close(db);

  // Import the lexicons into the database.
  lexicons_import_strongs_greek ();

  // Store the signatures.
  // If these signatures match next time, it won't create the database again.
  g_key_file_set_integer (keyfile, lexicons_database_group_name (), lexicon_strongs_greek_xml(), file_get_size (lexicons_get_strongs_greek_xml_filename()));
  g_key_file_set_integer (keyfile, lexicons_database_group_name (), lexicons_sql(), file_get_modification_time (lexicons_get_sql_filename()));  
}


void lexicons_add_text_to_definition (ustring& definition, const ustring& text)
// Adds "text" to the "definition", and tries to ensure good spacing.
{
  if (!definition.empty()) {
    ustring first_new_character = text.substr (0, 1);
    gunichar unichar = g_utf8_get_char(first_new_character.c_str());
    if (g_unichar_ispunct(unichar)) {
      // If the new text starts with a punctuation character, and the existing text ends with a punctuation character,
      // then the new punctuation overrides the old.
      ustring last_old_character = definition.substr (definition.length() - 1, 1);
      unichar = g_utf8_get_char (last_old_character.c_str());
      if (g_unichar_ispunct (unichar)) {
        definition.erase (definition.length() - 1, 1);
      }
    } else {
      // If the new text does not start with a punctuation character, 
      // and the old text does not end with an opening bracket, add a space.
      if (!g_str_has_suffix (definition.c_str(), "(")) {
        if (!g_str_has_suffix (definition.c_str(), " ")) {
          definition.append (" ");
        }
      }
    }
  }
  definition.append (text);
}


void lexicons_add_strongs_tag_to_definition (xmlTextReaderPtr reader, ustring& definition)
// This reads a Strong's reference tag and adds it to the definition.
{
  char *language = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "language");
  char *strongs = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "strongs");
  if ((language) && (strongs)) {
    ustring tag = "STRONGS";
    if (!strcmp(language, "GREEK"))
      tag.append ("G");
    else
      tag.append ("H");
    tag.append (strongs);
    lexicons_add_text_to_definition (definition, tag);
  }
  if (language) free (language);
  if (strongs) free (strongs);
}


void lexicons_import_strongs_greek ()
{
  // Show the progress. The lexicon has 5624 entries.
  ProgressWindow progresswindow ("Importing Strong's Greek Lexicon", false);
  progresswindow.set_iterate (0, 1, 5624);
  gchar * contents;
  g_file_get_contents(lexicons_get_strongs_greek_xml_filename().c_str(), &contents, NULL, NULL);
  if (!contents)
    return;

  // Open the database in fast mode.
  sqlite3 *db;
  sqlite3_open(lexicons_get_sql_filename().c_str(), &db);
  sqlite3_exec(db, "PRAGMA synchronous=OFF;", NULL, NULL, NULL);
  
  // Parse input.
  xmlParserInputBufferPtr inputbuffer;
  inputbuffer = xmlParserInputBufferCreateMem(contents, strlen (contents), XML_CHAR_ENCODING_NONE);
  xmlTextReaderPtr reader = xmlNewTextReader(inputbuffer, NULL);
  if (reader) {
    bool within_entry = false;
    unsigned int strongs_number = 0;
    ustring strongs_definition;
    while ((xmlTextReaderRead(reader) == 1)) {
      switch (xmlTextReaderNodeType(reader)) {
      case XML_READER_TYPE_ELEMENT:
        {
          xmlChar *element_name = xmlTextReaderName(reader);
          // Deal with an entry.
          if (!xmlStrcmp(element_name, BAD_CAST "entry")) {
            within_entry = true;
            char *attribute;
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "strongs");
            if (attribute) {
              strongs_number = convert_to_int (attribute);
              free(attribute);
            }
          }
          if (!xmlStrcmp(element_name, BAD_CAST "strongs")) {
            lexicons_add_text_to_definition (strongs_definition, "Strong's number");
          }
          if (!xmlStrcmp(element_name, BAD_CAST "greek")) {
            lexicons_add_text_to_definition (strongs_definition, "Greek");
            char * unicode = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "unicode");
            if (unicode) {
              lexicons_add_text_to_definition (strongs_definition, unicode);
              free(unicode);
            }
            char * transliteration = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "translit");
            if (transliteration) {
              lexicons_add_text_to_definition (strongs_definition, ";");
              lexicons_add_text_to_definition (strongs_definition, "transliteration");
              lexicons_add_text_to_definition (strongs_definition, transliteration);
              lexicons_add_text_to_definition (strongs_definition, ";");
              free(transliteration);
            }
          }
          if (!xmlStrcmp(element_name, BAD_CAST "pronunciation")) {
            char *pronunciation = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "strongs");
            if (pronunciation) {
              lexicons_add_text_to_definition (strongs_definition, "pronunciation");
              lexicons_add_text_to_definition (strongs_definition, pronunciation);
              lexicons_add_text_to_definition (strongs_definition, ";");
              free(pronunciation);
            }
          }
          if (!xmlStrcmp(element_name, BAD_CAST "strongsref")) {
            lexicons_add_strongs_tag_to_definition (reader, strongs_definition);
          }
          if (!xmlStrcmp(element_name, BAD_CAST "see")) {
            lexicons_add_text_to_definition (strongs_definition, "See also entry");
            lexicons_add_strongs_tag_to_definition (reader, strongs_definition);
            lexicons_add_text_to_definition (strongs_definition, ".");
          }
          break;
        }
      case XML_READER_TYPE_TEXT:
        {
          if (within_entry) {
            xmlChar *text = xmlTextReaderValue(reader);
            if (text) {
              lexicons_add_text_to_definition (strongs_definition, (const gchar *) text);
              xmlFree(text);
            }
          }
          break;
        }
      case XML_READER_TYPE_END_ELEMENT:
        {
          xmlChar *element_name = xmlTextReaderName(reader);
          if (!xmlStrcmp(element_name, BAD_CAST "entry")) {
            within_entry = false;
            progresswindow.iterate();
            char *sql;
            replace_text (strongs_definition, "\n", " ");
            replace_text (strongs_definition, "  ", " ");
            sql = g_strdup_printf("insert into strongsgreek values (%d, '%s');", strongs_number, double_apostrophy (strongs_definition).c_str());
            sqlite3_exec(db, sql, NULL, NULL, NULL);
            g_free(sql);
            strongs_definition.clear();
          }
          if (!xmlStrcmp(element_name, BAD_CAST "strongs")) {
            lexicons_add_text_to_definition (strongs_definition, ";");
          }
          break;
        }
      }
    }
  }
  if (reader)
    xmlFreeTextReader(reader);
  if (inputbuffer)
    xmlFreeParserInputBuffer(inputbuffer);

  // Close database.
  sqlite3_close(db);
  
  // Free xml data.    
  g_free(contents);
}


/*
void lexicons_get_strongs_data (const Reference& reference, vector <unsigned int>& strongs, vector <ustring>& phrases, bool include_unmarked)
// This gets the phrases and the strong's numbers for a verse.
// If to "include unmarked" data, this gets included too. Else it give the marked phrases only.
{
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    // Open the database.
    rc = sqlite3_open(kjv_get_sql_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    // Retrieve the full text.
    ustring text;
    {
      SqliteReader reader(0);
      char *sql;
      sql = g_strdup_printf("select text from text where book = %d and chapter = %d and verse = %d;", reference.book, reference.chapter, convert_to_int (reference.verse));
      rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
      g_free(sql);
      if (rc) {
        throw runtime_error(sqlite3_errmsg(db));
      }
      if (!reader.ustring0.empty()) {
        text = reader.ustring0[0];
      }
    }
    // Retrieve the Strong's data.
    if (!text.empty ()) {
      SqliteReader reader(0);
      char *sql;
      sql = g_strdup_printf("select start, end, number from strong where book = %d and chapter = %d and verse = %d order by start asc;", reference.book, reference.chapter, convert_to_int (reference.verse));
      rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
      g_free(sql);
      if (rc) {
        throw runtime_error(sqlite3_errmsg(db));
      }
      vector <size_t> start_positions;
      vector <size_t> end_positions;
      vector <unsigned int> numbers;
      for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
        start_positions.push_back (convert_to_int (reader.ustring0[i]));
        end_positions.push_back (convert_to_int (reader.ustring1[i]));
        numbers.push_back (convert_to_int (reader.ustring2[i]));
      }
      size_t last_end_position = 0;
      for (unsigned int i = 0; i < numbers.size(); i++) {
        if (include_unmarked) {
          if (start_positions[i] > last_end_position) {
            strongs.push_back (0);
            phrases.push_back (text.substr (last_end_position, start_positions[i] - last_end_position));
          }
        }
        strongs.push_back (numbers[i]);
        phrases.push_back (text.substr (start_positions[i], end_positions[i] - start_positions[i]));
        last_end_position = end_positions[i];
      }      
      if (include_unmarked) {
        if (text.length() > last_end_position) {
          strongs.push_back (0);
          phrases.push_back (text.substr (last_end_position, 1000));
        }
      }
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
}


vector <Reference> lexicons_get_strongs_verses (const Reference& reference, unsigned int strongs)
// Passing a Strong's number, and a Reference, this returns all the verses that contain this Strong's number.
// The Reference is used to find out whether to look for this Strong's number in the Old or New Testament.
{
  // Get the type of the book, e.g. whether Old or New Testament.
  BookType booktype = books_id_to_type (reference.book);

  // Store the references.
  vector <Reference> references;
  
  // Mine the data from the database.
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    SqliteReader reader(0);
    rc = sqlite3_open(kjv_get_sql_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    char *sql;
    sql = g_strdup_printf("select distinct book, chapter, verse from strong where number = %d;", strongs);
    rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
    g_free(sql);
    if (rc) {
      throw runtime_error(sqlite3_errmsg(db));
    }
    for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
      // Get the references, and store it only if it comes from the same Testament we need.
      Reference ref (convert_to_int (reader.ustring0[i]), convert_to_int (reader.ustring1[i]), reader.ustring2[i]);
      if (books_id_to_type (ref.book) == booktype) {
        references.push_back (ref);
      }
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);

  // Result.
  return references;
}


ustring lexicons_get_verse (const Reference& reference)
// Get the verse text from the KJV Bible.
{
  ustring text;
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    SqliteReader reader(0);
    rc = sqlite3_open(kjv_get_sql_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    char *sql;
    sql = g_strdup_printf("select text from text where book = %d and chapter = %d and verse = %d;", reference.book, reference.chapter, convert_to_int (reference.verse));
    rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
    g_free(sql);
    if (rc) {
      throw runtime_error(sqlite3_errmsg(db));
    }
    if (!reader.ustring0.empty()) {
      text = reader.ustring0[0];
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
  return text;
}

*/
