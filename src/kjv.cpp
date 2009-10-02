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
#include "sourcelanguage.h"


void kjv_home_entry (HtmlWriter2& htmlwriter)
{
  htmlwriter.paragraph_open ();
  htmlwriter.hyperlink_add (kjv_html_entry_url (), "Sword King James Bible with Strong's lemmata");
  htmlwriter.paragraph_close ();
}


ustring kjv_html_entry_url ()
{
  return "kjv_entry";
}


void kjv_detailed_page (HtmlWriter2& htmlwriter)
{
  htmlwriter.heading_open (3);
  htmlwriter.text_add ("Sword King James Bible with Strong's lemmata");
  htmlwriter.heading_close ();
  htmlwriter.paragraph_open();
  htmlwriter.text_add ("The King James Bible from the Sword project is useful for its Strong's numbers tagged in the text. Bibledit uses this to find related verses, among other things. Importing consists of two steps: exporting it from Sword and creating a database out of it.");
  htmlwriter.paragraph_close();
  htmlwriter.heading_open (4);
  htmlwriter.text_add ("1. Exporting the text from Sword");
  htmlwriter.heading_close ();
  htmlwriter.paragraph_open();
  htmlwriter.text_add ("To export the text from the Sword library, it is supposed that the KJV Bible has been installed. Open a terminal, and type the following command:");
  htmlwriter.paragraph_close();
  htmlwriter.paragraph_open();
  htmlwriter.text_add ("mod2osis KJV > kjv.txt");
  htmlwriter.paragraph_close();
  htmlwriter.paragraph_open();
  htmlwriter.text_add ("This will take a while. It should give no errors. Once it finishes there will be a file called kjv.txt in the home directory.");
  htmlwriter.paragraph_close();
  htmlwriter.heading_open (4);
  htmlwriter.text_add ("2. Creating the database");
  htmlwriter.heading_close ();
  htmlwriter.paragraph_open();
  htmlwriter.hyperlink_add (kjv_create_database_url (), "Create database");
  htmlwriter.paragraph_close();
  htmlwriter.paragraph_open();
  htmlwriter.text_add ("Creating the database should take a while. Once done there will be a file called kjv.sql in the home directory. This file should be added to the bibledit package, replacing the one it currently has.");
  htmlwriter.paragraph_close();
}


ustring kjv_create_database_url ()
{
  return "kjv_create_db";
}


void kjv_action_page (HtmlWriter2& htmlwriter)
{
  htmlwriter.heading_open (3);
  htmlwriter.text_add ("Database creation from King James Bible exported from the Sword library");
  htmlwriter.heading_close ();

  vector <ustring> messages;
  bool keep_going = true;

  // Locate the kjv.txt file.
  ustring kjv_txt_file = gw_build_filename (g_get_home_dir (), "kjv.txt");
  messages.push_back ("Looking for file " + kjv_txt_file);
  if (!g_file_test (kjv_txt_file.c_str(), G_FILE_TEST_IS_REGULAR)) {
    kjv_txt_file.clear();
  }
  if (kjv_txt_file.empty()) {
    messages.push_back ("Can't find the input file");
    keep_going = false;
  }
  if (keep_going) {
    messages.push_back ("Using file " + kjv_txt_file);
  }

  // The database.
  ustring kjv_sql_file = gw_build_filename (g_get_home_dir (), "kjv.sql");

  // Importing data into the database.
  if (keep_going) {
    messages.push_back ("Importing data into the database at " + kjv_sql_file);
    kjv_import_sword (kjv_txt_file, kjv_sql_file);
  }

  // Write accumulated messages.
  htmlwriter.heading_open (3);
  if (keep_going) {
    htmlwriter.text_add ("Success! The database was created");
  } else {
    htmlwriter.text_add ("Error!");
  }
  htmlwriter.heading_close ();
  if (keep_going) {
    htmlwriter.paragraph_open ();
    htmlwriter.text_add ("To use the database, copy the file kjv.sql into the Bibledit package and re-install.");
    htmlwriter.paragraph_close ();
  }
  for (unsigned int i = 0; i < messages.size(); i++) {
    htmlwriter.paragraph_open ();
    htmlwriter.text_add (messages[i]);
    htmlwriter.paragraph_close ();
  }  

  // Write OK.
  htmlwriter.paragraph_open ();
  htmlwriter.hyperlink_add ("", "Ok");
  htmlwriter.paragraph_close ();
}


ustring kjv_sql_filename ()
{
  return gw_build_filename(directories_get_package_data(), "kjv.sql");
}


void kjv_import_sword (const ustring& textfile, const ustring& database) // Todo
{
  // Show the progress. KJV has 31102 verses.
  ProgressWindow progresswindow ("Importing King James Bible", false);
  progresswindow.set_iterate (0, 1, 31102);
  gchar * contents;
  g_file_get_contents(textfile.c_str(), &contents, NULL, NULL);
  if (!contents)
    return;

  // Create the database, put it in fast mode.
  unlink (database.c_str());
  sqlite3 *db;
  sqlite3_open(database.c_str(), &db);
  sqlite3_exec(db, "create table kjv (book integer, chapter integer, verse integer, item integer, fragment text, lemma text);", NULL, NULL, NULL);
  sqlite3_exec(db, "PRAGMA synchronous=OFF;", NULL, NULL, NULL);

  // Parse input.
  xmlParserInputBufferPtr inputbuffer;
  inputbuffer = xmlParserInputBufferCreateMem(contents, strlen (contents), XML_CHAR_ENCODING_NONE);
  xmlTextReaderPtr reader = xmlNewTextReader(inputbuffer, NULL);
  if (reader) {
    bool within_relevant_element = false;
    Reference reference (0, 0, "0");
    unsigned int item_number = 0;
    ustring textfragment;
    ustring lemmata;
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
            item_number = 0;
          }
          // Deal with a w element.
          if (!xmlStrcmp(element_name, BAD_CAST "w")) {
            within_relevant_element = true;
            item_number++;
            textfragment.clear();
            lemmata.clear();
            char *attribute;
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "lemma");
            if (attribute) {
              lemmata = attribute;
              free(attribute);
            }
          }
          break;
        }
      case XML_READER_TYPE_TEXT:
        {
          if (within_relevant_element) {
            xmlChar *text = xmlTextReaderValue(reader);
            if (text) {
              textfragment = (const char *)text;
              xmlFree(text);
              textfragment = textfragment.casefold();
            }
          }
          break;
        }
      case XML_READER_TYPE_END_ELEMENT:
        {
          xmlChar *element_name = xmlTextReaderName(reader);
          if (!xmlStrcmp(element_name, BAD_CAST "w")) {
            replace_text (lemmata, "strong:", "");
            char *sql;
            sql = g_strdup_printf("insert into kjv values (%d, %d, %d, %d, '%s', '%s');", 
                                  reference.book, reference.chapter, convert_to_int (reference.verse), 
                                  item_number, 
                                  double_apostrophy (textfragment).c_str(), lemmata.c_str());
            sqlite3_exec(db, sql, NULL, NULL, NULL);
            g_free(sql);
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
  /*
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
    unsigned int total_items_count = 0;
    unsigned int current_items_count = 0;
    ustring verse_text;
    unsigned int strongs_number = 0;
    bool add_italics = false;
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
            total_items_count = 0;
          }
          // Deal with the "gr" element. It has the Strong's number.
          if (!xmlStrcmp(element_name, BAD_CAST "gr")) {
            char *attribute;
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "str");
            if (attribute) {
              // The Strong's number usually is a plain number, like so: "853". 
              // But at times it has an asterisk, like so: "*853".
              strongs_number = convert_to_int (number_in_string (attribute));
              free(attribute);
            }
          }
          // Deal with the "STYLE" element. It (usually) indicates italics.
          if (!xmlStrcmp(element_name, BAD_CAST "STYLE")) {
            add_italics = true;
          }
          break;
        }
      case XML_READER_TYPE_TEXT:
        {
          xmlChar *text = xmlTextReaderValue(reader);
          if (text) {
            if (within_VERS_element) {
              ustring text2 = (const gchar *) text;
              Parse parse (trim (text2), false);
              current_items_count = parse.words.size();
              for (unsigned int i = 0; i < parse.words.size(); i++) {
                if (!verse_text.empty()) {
                  gunichar unichar = g_utf8_get_char(parse.words[i].substr (0, 1).c_str());
                  if (g_unichar_ispunct(unichar)) {
                    current_items_count--;
                  } else {
                    verse_text.append (" ");
                  }
                }
                if (i == 0) 
                  if (add_italics)
                    verse_text.append ("[");
                verse_text.append (parse.words[i]);
              }
              total_items_count += current_items_count;
              if (add_italics) {
                verse_text.append ("]");
                add_italics = false;
              }
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
            unsigned int start_position = total_items_count - current_items_count;
            unsigned int end_position = start_position + current_items_count;
            for (unsigned int i = start_position; i < end_position; i++) {
              char *sql;
              sql = g_strdup_printf("insert into lemmata values (%d, %d, %d, %d, '%s');", reference.book, reference.chapter, convert_to_int (reference.verse), i, convert_to_string (strongs_number).c_str());
              sqlite3_exec(db, sql, NULL, NULL, NULL);
              g_free(sql);
            }
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
  */
}


void kjv_get_strongs_data (const Reference& reference, vector <unsigned int>& strongs, vector <ustring>& words)
// This gets the words and their applicable Strong's numbers for a verse.
{
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    // Open the database.
    rc = sqlite3_open(kjv_sql_filename().c_str(), &db);
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
    // Parse the text.
    Parse parse (text, false);
    // Retrieve the Strong's data.
    if (!text.empty ()) {
      SqliteReader reader(0);
      char *sql;
      sql = g_strdup_printf("select item, value from lemmata where book = %d and chapter = %d and verse = %d order by item asc;", reference.book, reference.chapter, convert_to_int (reference.verse));
      rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
      g_free(sql);
      if (rc) {
        throw runtime_error(sqlite3_errmsg(db));
      }
      for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
        unsigned int item = convert_to_int (reader.ustring0[i]);
        unsigned int number = convert_to_int (reader.ustring1[i]);
        if (item < parse.words.size()) {
          strongs.push_back (number);
          words.push_back (parse.words[item]);
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
    rc = sqlite3_open(kjv_sql_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    char *sql;
    sql = g_strdup_printf("select distinct book, chapter, verse from lemmata where value = '%s';", convert_to_string (strongs).c_str());
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
    rc = sqlite3_open(kjv_sql_filename().c_str(), &db);
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
    rc = sqlite3_open(kjv_sql_filename().c_str(), &db);
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

