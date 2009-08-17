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
#include "compress.h"


ustring lexicons_get_strongs_greek_xml_filename()
// Gives the filename for the Strong's Greek lexicon XML file that comes with bibledit.
{
  return gw_build_filename(directories_get_package_data(), "strongsgreek.xml");
}


ustring lexicons_get_strongs_hebrew_zip_filename()
// Gives the filename for the Strong's Hebrew zipped lexicon that comes with bibledit.
{
  return gw_build_filename(directories_get_package_data(), "heb_strongs.zip");
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


const gchar * lexicon_strongs_hebrew_zip ()
{
  return "hebstrongszip";
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
  value = g_key_file_get_integer(keyfile, lexicons_database_group_name (), lexicon_strongs_hebrew_zip(), NULL);
  if (value != file_get_size (lexicons_get_strongs_hebrew_zip_filename())) {
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
  sqlite3_exec(db, "create table strongshebrew (strongs integer, entry text);", NULL, NULL, NULL);
  sqlite3_close(db);

  // Import the lexicons into the database.
  lexicons_import_strongs_greek ();
  lexicons_import_strongs_hebrew ();

  // Store the signatures.
  // If these signatures match next time, it won't create the database again.
  g_key_file_set_integer (keyfile, lexicons_database_group_name (), lexicon_strongs_greek_xml(), file_get_size (lexicons_get_strongs_greek_xml_filename()));
  g_key_file_set_integer (keyfile, lexicons_database_group_name (), lexicon_strongs_hebrew_zip(), file_get_size (lexicons_get_strongs_hebrew_zip_filename()));
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


void lexicons_add_strongs_tag_to_definition (const ustring& link, ustring& definition)
// This adds a Strong's reference tag to the definition.
{
  ustring tag = "STRONGS" + link;
  lexicons_add_text_to_definition (definition, tag);
}


void lexicons_add_strongs_tag_to_definition (xmlTextReaderPtr reader, ustring& definition)
// This reads a Strong's reference tag and adds it to the definition.
{
  char *language = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "language");
  char *strongs = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "strongs");
  if ((language) && (strongs)) {
    ustring tag;
    if (!strcmp(language, "GREEK"))
      tag.append ("G");
    else
      tag.append ("H");
    tag.append (strongs);
    lexicons_add_strongs_tag_to_definition (tag, definition);
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


void lexicons_import_strongs_hebrew ()
{
  // Open the database in fast mode.
  sqlite3 *db;
  sqlite3_open(lexicons_get_sql_filename().c_str(), &db);
  sqlite3_exec(db, "PRAGMA synchronous=OFF;", NULL, NULL, NULL);
  
  // Unpack the zipped archive.
  ustring lexicon_directory = gw_build_filename (directories_get_temp (), "lexicon");
  gw_mkdir_with_parents (lexicon_directory);
  uncompress (lexicons_get_strongs_hebrew_zip_filename (), lexicon_directory);

  // Show the progress. The lexicon consists of 87 separate files.
  ProgressWindow progresswindow ("Importing Strong's Hebrew Lexicon", false);
  progresswindow.set_iterate (0, 1, 87);

  // Go through each file.
  for (unsigned int i = 0; i < 87; i++) {
    progresswindow.iterate ();
    ustring filename = gw_build_filename (lexicon_directory, "heb" + convert_to_string (i) + ".xml");
    gw_message ("Processing file " + filename);

    // File number 63 has an xml error, and this is corrected here.
    if (i == 63) {
      ReadText rt (filename, true, false);
      for (unsigned int i = 0; i < rt.lines.size(); i++) {
        if (rt.lines[i].find ("H6401") != string::npos)
          rt.lines[i].clear();
      }
      write_lines (filename, rt.lines);
    }

    // Read the file.
    gchar * contents;
    g_file_get_contents(filename.c_str(), &contents, NULL, NULL);
    if (!contents)
      return;

    // Parse input.
    xmlParserInputBufferPtr inputbuffer;
    inputbuffer = xmlParserInputBufferCreateMem(contents, strlen (contents), XML_CHAR_ENCODING_NONE);
    xmlTextReaderPtr reader = xmlNewTextReader(inputbuffer, NULL);
    if (reader) {
      bool within_item = false;
      bool within_strong_id = false;
      bool within_link = false;
      unsigned int strongs_number = 0;
      ustring strongs_definition;
      while ((xmlTextReaderRead(reader) == 1)) {
        switch (xmlTextReaderNodeType(reader)) {
        case XML_READER_TYPE_ELEMENT:
          {
            xmlChar *element_name = xmlTextReaderName(reader);
            // Deal with an item.
            if (!xmlStrcmp(element_name, BAD_CAST "item")) {
              within_item = true;
              char *attribute;
              attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
              if (attribute) {
                strongs_number = convert_to_int (number_in_string (attribute));
                free(attribute);
              }
            }
            if (!xmlStrcmp(element_name, BAD_CAST "strong_id")) {
              within_strong_id = true;
              lexicons_add_text_to_definition (strongs_definition, "Strong's number");
            }
            if (!xmlStrcmp(element_name, BAD_CAST "title")) {
              lexicons_add_text_to_definition (strongs_definition, "Hebrew");
            }
            if (!xmlStrcmp(element_name, BAD_CAST "transliteration")) {
              lexicons_add_text_to_definition (strongs_definition, "transliteration ");
            }
            if (!xmlStrcmp(element_name, BAD_CAST "pronunciation")) {
              lexicons_add_text_to_definition (strongs_definition, "pronunciation ");
            }
            if (!xmlStrcmp(element_name, BAD_CAST "link")) {
              within_link = true;
              char *attribute;
              attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "target");
              if (attribute) {
                lexicons_add_strongs_tag_to_definition (attribute, strongs_definition);
                free(attribute);
              }
            }
            break;
          }
        case XML_READER_TYPE_TEXT:
          {
            if (within_link)
              break;
            if (within_item) {
              xmlChar *text = xmlTextReaderValue(reader);
              if (text) {
                ustring text2 ((const gchar *) text);
                if (within_strong_id) {
                  text2 = number_in_string (text2);
                }
                lexicons_add_text_to_definition (strongs_definition, text2);
                xmlFree(text);
              }
            }
            break;
          }
        case XML_READER_TYPE_END_ELEMENT:
          {
            xmlChar *element_name = xmlTextReaderName(reader);
            if (!xmlStrcmp(element_name, BAD_CAST "item")) {
              within_item = false;
              char *sql;
              replace_text (strongs_definition, "\n", " ");
              replace_text (strongs_definition, "  ", " ");
              sql = g_strdup_printf("insert into strongshebrew values (%d, '%s');", strongs_number, double_apostrophy (strongs_definition).c_str());
              sqlite3_exec(db, sql, NULL, NULL, NULL);
              g_free(sql);
              strongs_definition.clear();
            }
            if (!xmlStrcmp(element_name, BAD_CAST "strong_id")) {
              within_strong_id = false;
              lexicons_add_text_to_definition (strongs_definition, ";");
            }
            if (!xmlStrcmp(element_name, BAD_CAST "title")) {
              lexicons_add_text_to_definition (strongs_definition, ";");
            }
            if (!xmlStrcmp(element_name, BAD_CAST "transliteration")) {
              lexicons_add_text_to_definition (strongs_definition, ";");
            }
            if (!xmlStrcmp(element_name, BAD_CAST "pronunciation")) {
              lexicons_add_text_to_definition (strongs_definition, ". ");
            }
            if (!xmlStrcmp(element_name, BAD_CAST "link")) {
              within_link = false;
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

    // Free xml data.    
    g_free(contents);
  }

  // Close database.
  sqlite3_close(db);
    
  // Clean out the temporal data.
  unix_rmdir (lexicon_directory);
}


ustring lexicons_get_definition (bool greek_lexicon, unsigned int strongs_number)
// Get the definition from the lexicons.
// greek_lexicon: If true take the definition from the Greek lexicon, else from the Hebrew one.
// strongs_number: Strong's number.
{
  ustring definition;
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    SqliteReader reader(0);
    rc = sqlite3_open(lexicons_get_sql_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    char *sql;
    const gchar* table = NULL;
    if (greek_lexicon)
      table = "strongsgreek";
    else
      table = "strongshebrew";
    sql = g_strdup_printf("select entry from '%s' where strongs = %d;", table, strongs_number);
    rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
    g_free(sql);
    if (rc) {
      throw runtime_error(sqlite3_errmsg(db));
    }
    if (!reader.ustring0.empty()) {
      definition = reader.ustring0[0];
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
  return definition;
}


