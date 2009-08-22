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


#include "kjv.h"
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


ustring kjv_get_sword_xml_filename()
// Gives the filename for the Sword KJV xml file that comes with bibledit.
{
  return gw_build_filename(directories_get_package_data(), "swordkjv.xml");
}


ustring kjv_get_zefania_xml_filename()
// Gives the filename for the Zefania KJV xml file that comes with bibledit.
{
  return gw_build_filename(directories_get_package_data(), "zefaniakjv.xml");
}


ustring kjv_get_sql_filename()
// Gives the filename for the created KJV database.
{
  return gw_build_filename(directories_get_databases(), "kjv.sql");
}


const gchar* kjv_database_group_name ()
{
  return "kjv";
}


const gchar * sword_kjv_xml ()
{
  return "swordxml";
}


const gchar * zefania_kjv_xml ()
{
  return "zefaniaxml";
}


const gchar * sword_kjv_sql ()
{
  return "sql";
}


void kjv_import (GKeyFile *keyfile)
{
  // See whether to import the two .xml files into the database.
  // Normally this happens once upon installation.
  // If it has been done already, and everything seems fine, bail out.
  bool import = false;
  unsigned int value;
  value = g_key_file_get_integer(keyfile, kjv_database_group_name (), sword_kjv_xml(), NULL);
  if (value != file_get_size (kjv_get_sword_xml_filename())) {
    import = true;
  }
  value = g_key_file_get_integer(keyfile, kjv_database_group_name (), zefania_kjv_xml(), NULL);
  if (value != file_get_size (kjv_get_zefania_xml_filename())) {
    import = true;
  }
  value = g_key_file_get_integer(keyfile, kjv_database_group_name (), sword_kjv_sql(), NULL);
  if (value != file_get_modification_time (kjv_get_sql_filename())) {
    import = true;
  }
  if (!import) {
    return;
  }

  // Remove any previous database.
  unlink (kjv_get_sql_filename().c_str());

  // Create the user database.
  sqlite3 *db;
  sqlite3_open(kjv_get_sql_filename().c_str(), &db);
  sqlite3_exec(db, "create table text (book integer, chapter integer, verse integer, text text);", NULL, NULL, NULL);
  sqlite3_exec(db, "create table strong (book integer, chapter integer, verse integer, start integer, end integer, number integer);", NULL, NULL, NULL);
  sqlite3_exec(db, "create table morphology (book integer, chapter integer, verse integer, item integer, value text);", NULL, NULL, NULL);
  sqlite3_close(db);

  // Import text into the database.
  kjv_import_zefania ();  
  kjv_import_sword ();

  // Store the signatures.
  // If these signatures match next time, it won't create the database again.
  g_key_file_set_integer (keyfile, kjv_database_group_name (), sword_kjv_xml(), file_get_size (kjv_get_sword_xml_filename()));
  g_key_file_set_integer (keyfile, kjv_database_group_name (), zefania_kjv_xml(), file_get_size (kjv_get_zefania_xml_filename()));
  g_key_file_set_integer (keyfile, kjv_database_group_name (), sword_kjv_sql(), file_get_modification_time (kjv_get_sql_filename()));  
}


void kjv_import_sword ()
{
  // Show the progress. KJV has 31102 verses.
  ProgressWindow progresswindow ("Importing morphology into the King James Bible", false);
  progresswindow.set_iterate (0, 1, 31102);
  gchar * contents;
  g_file_get_contents(kjv_get_sword_xml_filename().c_str(), &contents, NULL, NULL);
  if (!contents)
    return;

  // Open the database in fast mode.
  sqlite3 *db;
  sqlite3_open(kjv_get_sql_filename().c_str(), &db);
  sqlite3_exec(db, "PRAGMA synchronous=OFF;", NULL, NULL, NULL);
  
  // Parse input.
  xmlParserInputBufferPtr inputbuffer;
  inputbuffer = xmlParserInputBufferCreateMem(contents, strlen (contents), XML_CHAR_ENCODING_NONE);
  xmlTextReaderPtr reader = xmlNewTextReader(inputbuffer, NULL);
  if (reader) {
    bool within_relevant_element = false;
    Reference reference (0, 0, "0");
    unsigned int total_items_count = 0;
    unsigned int current_items_count = 0;
    ustring raw_data;
    while ((xmlTextReaderRead(reader) == 1)) {
      switch (xmlTextReaderNodeType(reader)) {
      case XML_READER_TYPE_ELEMENT:
        {
          xmlChar *element_name = xmlTextReaderName(reader);
          // Deal with a verse element.
          if (!xmlStrcmp(element_name, BAD_CAST "verse")) {
            progresswindow.iterate();
            char *attribute;
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "osisID");
            if (attribute) {
              Parse parse (attribute, false, ".");
              if (parse.words.size() == 3) {
                reference.book = books_osis_to_id (parse.words[0]);
                reference.chapter = convert_to_int (parse.words[1]);
                reference.verse = parse.words[2];
              } else {
                gw_critical (attribute);
              }
              free(attribute);
            }
            total_items_count = 0;
          }
          // Deal with a w element.
          if (!xmlStrcmp(element_name, BAD_CAST "w")) {
            within_relevant_element = true;
            current_items_count = 0;
            raw_data.clear();
            char *attribute;
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "morph");
            if (attribute) {
              raw_data = attribute;
              free(attribute);
            }
          }
          // Deal with a transChange element.
          if (!xmlStrcmp(element_name, BAD_CAST "transChange")) {
            within_relevant_element = true;
          }
          break;
        }
      case XML_READER_TYPE_TEXT:
        {
          if (within_relevant_element) {
            xmlChar *text = xmlTextReaderValue(reader);
            if (text) {
              Parse parse ((const char *)text);
              xmlFree(text);
              current_items_count = parse.words.size();
              total_items_count += current_items_count;
            }
          }
          break;
        }
      case XML_READER_TYPE_END_ELEMENT:
        {
          xmlChar *element_name = xmlTextReaderName(reader);
          if (!xmlStrcmp(element_name, BAD_CAST "w")) {
            within_relevant_element = false;
            Parse parse (raw_data, false);
            for (unsigned int i = 0; i < parse.words.size(); i++) {
              Parse parse2 (parse.words[i], false, ":");
              if (parse2.words.size() == 2) {
                // The morphology is in the second parsed word.
                for (int i = current_items_count; i > 0; i--) {
                  char *sql;
                  sql = g_strdup_printf("insert into morphology values (%d, %d, %d, %d, '%s');", reference.book, reference.chapter, convert_to_int (reference.verse), total_items_count - i, double_apostrophy (parse2.words[1]).c_str());
                  sqlite3_exec(db, sql, NULL, NULL, NULL);
                  g_free(sql);
                }
              }
            }
          }
          // Deal with a transChange element.
          if (!xmlStrcmp(element_name, BAD_CAST "transChange")) {
            within_relevant_element = false;
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


void kjv_import_zefania ()
{
  // Show the progress. KJV has 31102 verses.
  ProgressWindow progresswindow ("Importing text and lemmata into the King James Bible", false);
  progresswindow.set_iterate (0, 1, 31102);
  gchar * contents;
  g_file_get_contents(kjv_get_zefania_xml_filename().c_str(), &contents, NULL, NULL);
  if (!contents)
    return;

  // Open the database in fast mode.
  sqlite3 *db;
  sqlite3_open(kjv_get_sql_filename().c_str(), &db);
  sqlite3_exec(db, "PRAGMA synchronous=OFF;", NULL, NULL, NULL);
  
  // Parse input.
  xmlParserInputBufferPtr inputbuffer;
  inputbuffer = xmlParserInputBufferCreateMem(contents, strlen (contents), XML_CHAR_ENCODING_NONE);
  xmlTextReaderPtr reader = xmlNewTextReader(inputbuffer, NULL);
  if (reader) {
    bool within_VERS_element = false;
    Reference reference (0, 0, "0");
    ustring verse_text;
    size_t strongs_start_position = 0;
    unsigned int strongs_number = 0;
    while ((xmlTextReaderRead(reader) == 1)) {
      switch (xmlTextReaderNodeType(reader)) {
      case XML_READER_TYPE_ELEMENT:
        {
          xmlChar *element_name = xmlTextReaderName(reader);
          // Deal with a Bible book.
          if (!xmlStrcmp(element_name, BAD_CAST "BIBLEBOOK")) {
            char *attribute;
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "bnumber");
            if (attribute) {
              reference.book = convert_to_int (attribute);
              free(attribute);
            }
          }
          // Deal with a chapter.
          if (!xmlStrcmp(element_name, BAD_CAST "CHAPTER")) {
            char *attribute;
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "cnumber");
            if (attribute) {
              reference.chapter = convert_to_int (attribute);
              free(attribute);
            }
          }
          // Deal with a "VERS" element.
          if (!xmlStrcmp(element_name, BAD_CAST "VERS")) {
            within_VERS_element = true;
            char *attribute;
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "vnumber");
            if (attribute) {
              reference.verse = attribute;
              free(attribute);
            }
          }
          // Deal with the "gr" element. It has the Strong's number.
          if (!xmlStrcmp(element_name, BAD_CAST "gr")) {
            char *attribute;
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "str");
            if (attribute) {
              // Store the position within the verse where the Strong's number starts.
              strongs_start_position = verse_text.length();
              // The Strong's number usually is a plain number, like so: "853". 
              // But at times it has an asterisk, like so: "*853".
              strongs_number = convert_to_int (number_in_string (attribute));
              free(attribute);
            }
          }
          break;
        }
      case XML_READER_TYPE_TEXT:
        {
          xmlChar *text = xmlTextReaderValue(reader);
          if (text) {
            if (within_VERS_element) {
              ustring verse = (const gchar *) text;
              // Correct the text in the xml file that starts with a space.
              if (verse_text.empty()) {
                if (verse.substr (0, 1) == " ") {
                  verse.erase (0, 1);
                }
              }
              verse_text.append (verse);
              // Remove double spaces.
              replace_text (verse_text, "  ", " ");
            }
            xmlFree(text);
          }
          break;
        }
      case XML_READER_TYPE_END_ELEMENT:
        {
          xmlChar *element_name = xmlTextReaderName(reader);
          if (!xmlStrcmp(element_name, BAD_CAST "VERS")) {
            within_VERS_element = false;
            // Store the verse text in the database.
            progresswindow.iterate();
            verse_text = trim (verse_text);
            char *sql;
            sql = g_strdup_printf("insert into text values (%d, %d, %d, '%s');", reference.book, reference.chapter, convert_to_int (reference.verse), double_apostrophy (verse_text).c_str());
            sqlite3_exec(db, sql, NULL, NULL, NULL);
            g_free(sql);
            verse_text.clear();
          }
          if (!xmlStrcmp(element_name, BAD_CAST "gr")) {
            // Store the position within the verse where the Strong's number ends.
            size_t strongs_end_position = verse_text.length();
            // Correct the space that at times is included within the text that the Strong's number refers to.
            if (strongs_end_position > strongs_start_position) {
              if (g_str_has_suffix (verse_text.c_str(), " ")) {
                strongs_end_position--;
              }
            }
            char *sql;
            sql = g_strdup_printf("insert into strong values (%d, %d, %d, %d, %d, %d);", reference.book, reference.chapter, convert_to_int (reference.verse), strongs_start_position, strongs_end_position, strongs_number);
            sqlite3_exec(db, sql, NULL, NULL, NULL);
            g_free(sql);
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


void kjv_get_strongs_data (const Reference& reference, vector <unsigned int>& strongs, vector <ustring>& phrases, bool include_unmarked)
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


vector <Reference> kjv_get_strongs_verses (const Reference& reference, unsigned int strongs)
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


ustring kjv_get_verse (const Reference& reference)
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


void kjv_get_lemmata_and_morphology (const Reference& reference, vector <ustring>& words,
                                     vector <unsigned int>& lemmata_positions, vector <unsigned int>& lemmata_values, 
																		 vector <unsigned int>& morphology_positions, vector <ustring>& morphology_values)
// Based on a "reference", it provides lemmata and morphology for a verse.
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

    // Retrieve the text and sort it out.
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
        Parse parse (reader.ustring0[0], false);
        words = parse.words;
      }
    }

    // Retrieve the Strong's lemmata.
    {
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
      size_t word_start = 0;
      for (unsigned int w = 0; w < words.size(); w++) {
        size_t word_end = word_start + words[w].length();
        for (unsigned int p = 0; p < start_positions.size(); p++) {
          unsigned int start_position = start_positions[p];
          unsigned int end_position = end_positions[p];
          if (word_start >= start_position) {
            if (word_end <= end_position + 1) {
              lemmata_positions.push_back (w);
              lemmata_values.push_back (numbers[p]);
            }
          }
        }
        word_start = word_end + 1; 
      }
    }

    // Retrieve the morphology.
    {
      SqliteReader reader(0);
      char *sql;
      sql = g_strdup_printf("select item, value from morphology where book = %d and chapter = %d and verse = %d order by item asc;", reference.book, reference.chapter, convert_to_int (reference.verse));
      rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
      g_free(sql);
      if (rc) {
        throw runtime_error(sqlite3_errmsg(db));
      }
      for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
        morphology_positions.push_back (convert_to_int (reader.ustring0[i]));
        morphology_values.push_back (reader.ustring1[i]);
      }
    }

  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
}


vector <Reference> kjv_search_text (ustring text)
// Searches the KJV for "text" and returns the references.
{
  // Show the progress. KJV has 31102 verses.
  ProgressWindow progresswindow ("Searching", false);
  progresswindow.set_iterate (0, 1, 31102);
  // Fold case of text to search for.
  text = text.casefold();
  vector <Reference> references;
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
    sql = g_strdup_printf("select book, chapter, verse, text from text;");
    rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
    g_free(sql);
    if (rc) {
      throw runtime_error(sqlite3_errmsg(db));
    }
    for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
      progresswindow.iterate();
      ustring verse = reader.ustring3[i].casefold();
      if (verse.find (text) != string::npos) {
        Reference reference (0);
        reference.book = convert_to_int (reader.ustring0[i]);
        reference.chapter = convert_to_int (reader.ustring1[i]);
        reference.verse = reader.ustring2[i];
        references.push_back (reference);
      }
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
  return references;
}


vector <Reference> kjv_search_strong (ustring strong)
// Searches the KJV for a Strong's number and returns the references.
{
  bool greek_lexicon = strong.substr (0, 1) == "G";
  vector <unsigned int> books;
  if (greek_lexicon)
    books = books_type_to_ids (btNewTestament);
  else
    books = books_type_to_ids (btOldTestament);
  Reference reference (books[0], 1, "1");
  unsigned int strongs = convert_to_int (number_in_string (strong));
  return kjv_get_strongs_verses (reference, strongs);
}

