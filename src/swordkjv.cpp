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


#include "swordkjv.h"
#include <gtk/gtk.h>
#include "gwrappers.h"
#include "progresswindow.h"
#include <libxml/xmlreader.h>
#include "utilities.h"
#include "books.h"
#include "tiny_utilities.h"
#include "directories.h"
#include "unixwrappers.h"
#include <sqlite3.h>
#include "sqlite_reader.h"


ustring sword_kjv_get_user_filename()
// Gives the filename for the user-created Sword KJV database.
{
  return gw_build_filename(directories_get_templates_user(), "swordkjv.sql");
}


ustring sword_kjv_get_package_filename()
// Gives the filename for the Sword KJV database that comes with bibledit.
{
  return gw_build_filename(directories_get_package_data(), "swordkjv.sql");
}


ustring sword_kjv_get_filename ()
// Gives the package filename, or the user file if it's there.
{
  ustring filename = sword_kjv_get_user_filename();
  if (!g_file_test (filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    filename = sword_kjv_get_package_filename();
  }
  return filename;
}


void sword_kjv_ensure_user_database()
{
  // Remove any previous one.
  unlink (sword_kjv_get_user_filename().c_str());

  // Create a new database.
  sqlite3 *db;
  sqlite3_open(sword_kjv_get_user_filename().c_str(), &db);
  sqlite3_exec(db, "create table baretext (book integer, chapter integer, verse integer, text text);", NULL, NULL, NULL);
  sqlite3_exec(db, "create table richtext (book integer, chapter integer, verse integer, text text);", NULL, NULL, NULL);
  sqlite3_close(db);
}


ustring sword_kjv_html_entry_url ()
{
  return "sword_kjv_entry";
}


ustring sword_kjv_import_url ()
{
  return "sword_kjv_import";
}


ustring sword_kjv_delete_url ()
{
  return "sword_kjv_delete";
}


void import_sword_kjv_home_entry (HtmlWriter2& htmlwriter)
{
  htmlwriter.paragraph_open ();
  htmlwriter.hyperlink_add (sword_kjv_html_entry_url (), "KJV Bible from the Sword library");
  htmlwriter.paragraph_close ();
}


void import_sword_kjv_detailed_page (HtmlWriter2& htmlwriter)
{
  htmlwriter.heading_open (3);
  htmlwriter.text_add ("KJV Bible from Sword library");
  htmlwriter.heading_close ();
  htmlwriter.paragraph_open();
  htmlwriter.text_add ("The King James version from the Sword library has parsings and other information that is used to assist the translator. Bibledit comes with this data already loaded. If a new version is available in the Sword library, it can be imported here and used subsequently.");
  htmlwriter.paragraph_close();
  htmlwriter.paragraph_open();
  htmlwriter.hyperlink_add (sword_kjv_import_url (), "Import it from the library");
  htmlwriter.paragraph_close();
  htmlwriter.paragraph_open();
  htmlwriter.hyperlink_add (sword_kjv_delete_url (), "Revert to the one that came with Bibledit");
  htmlwriter.paragraph_close();
}


vector <ustring> sword_kjv_import ()
{
  vector <ustring> messages;
  
  // Check whether program mod2osis is available.
  if (!gw_find_program_in_path ("mod2osis")) {
    messages.push_back ("Program mod2osis is needed to do the import, but it is not available on this system. It would normally be available in a packaged called libsword-dev or similar.");
    return messages;
  }

  // Progress reporting.
  ProgressWindow progresswindow ("Import", false);
  progresswindow.set_text ("Reading");
  progresswindow.set_iterate (0, 1, 65000);
  
  // Read the KJV Sword module.
  ustring osis;
  FILE *stream = popen("mod2osis KJV", "r");
  char buf[8192];
  while (fgets(buf, sizeof(buf), stream)) {
    osis.append(buf);
    progresswindow.iterate ();
  }
  int result = pclose(stream);
  if (result != 0) {
    messages.push_back ("Something didn't work out during import. The system log will give more information. Is the KJV module available in the Sword library?");
    return messages;
  }

  // Progress. KJV has 31102 verses.
  progresswindow.set_text ("Writing");
  progresswindow.set_iterate (0, 1, 31102);
  
  // Open the user database.
  sword_kjv_ensure_user_database();
  sqlite3 *db;
  sqlite3_open(sword_kjv_get_filename().c_str(), &db);
  sqlite3_exec(db, "PRAGMA synchronous=OFF;", NULL, NULL, NULL);

  // Parse input.
  xmlParserInputBufferPtr inputbuffer;
  inputbuffer = xmlParserInputBufferCreateMem(osis.c_str(), osis.length(), XML_CHAR_ENCODING_NONE);
  xmlTextReaderPtr reader = xmlNewTextReader(inputbuffer, NULL);
  if (reader) {
    bool within_verse_element = false;
    bool within_w_element = false;
    ustring bare_verse_text;
    ustring rich_verse_text;
    vector <ustring> strongs;
    Reference reference (0, 0, "0");
    while ((xmlTextReaderRead(reader) == 1)) {
      switch (xmlTextReaderNodeType(reader)) {
      case XML_READER_TYPE_ELEMENT:
        {
          xmlChar *element_name = xmlTextReaderName(reader);
          // Deal with a verse element.
          if (!xmlStrcmp(element_name, BAD_CAST "verse")) {
            within_verse_element = true;
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
          }
          // Deal with a w element.
          if (!xmlStrcmp(element_name, BAD_CAST "w")) {
            within_w_element = true;
            char *attribute;
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "lemma");
            if (attribute) {
              Parse parse (attribute, false);
              for (unsigned int i = 0; i < parse.words.size(); i++) {
                ustring strong = parse.words[i];
                if (strong.find ("strong:") == 0) {
                  strong.erase (0, 8);
                  strongs.push_back (strong);
                }
              }
              free(attribute);
            }
          }
          break;
        }
      case XML_READER_TYPE_TEXT:
        {
          xmlChar *text = xmlTextReaderValue(reader);
          if (text) {
            if (within_verse_element) {
              if (!g_ascii_ispunct (*text)) {
                if (!bare_verse_text.empty())
                  bare_verse_text.append (" ");
                bare_verse_text.append ((const char *)text);
                if (!rich_verse_text.empty()) 
                  rich_verse_text.append (" ");
              }
              for (unsigned int i = 0; i < strongs.size(); i++) {
                rich_verse_text.append ("(" + strongs[i] + ")");
              }
              strongs.clear();
              rich_verse_text.append ((const char *)text);
            }
            xmlFree(text);
          }
          break;
        }
      case XML_READER_TYPE_END_ELEMENT:
        {
          xmlChar *element_name = xmlTextReaderName(reader);
          if (!xmlStrcmp(element_name, BAD_CAST "verse")) {
            within_verse_element = false;
            // Store the verse texts in the database.
            progresswindow.iterate();
            char *sql;
            bare_verse_text = bare_verse_text.casefold();
            bare_verse_text = trim (bare_verse_text);
            replace_text (bare_verse_text, "  ", " ");
            sql = g_strdup_printf("insert into baretext values (%d, %d, %d, '%s');", reference.book, reference.chapter, convert_to_int (reference.verse), double_apostrophy (bare_verse_text).c_str());
            sqlite3_exec(db, sql, NULL, NULL, NULL);
            g_free(sql);
            bare_verse_text.clear();
            replace_text (rich_verse_text, "  ", " ");
            rich_verse_text = trim (rich_verse_text);
            sql = g_strdup_printf("insert into richtext values (%d, %d, %d, '%s');", reference.book, reference.chapter, convert_to_int (reference.verse), double_apostrophy (rich_verse_text).c_str());
            sqlite3_exec(db, sql, NULL, NULL, NULL);
            g_free(sql);
            rich_verse_text.clear();
          }
          if (!xmlStrcmp(element_name, BAD_CAST "w")) {
            within_w_element = false;
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
  
  // Give the okay message.  
  messages.push_back ("The KJV Bible was successfully imported. Full changes take effect after reboot.");
  return messages;
}


vector <ustring> sword_kjv_delete ()
{
  unlink (sword_kjv_get_user_filename().c_str());
  vector <ustring> messages;
  messages.push_back ("The system has now reverted to using the KJV Bible as it came with Bibledit.");
  return messages;
}


void sword_kjv_action_result_page (const vector <ustring>& messages, HtmlWriter2& htmlwriter)
{
  for (unsigned int i = 0; i < messages.size(); i++) {
    htmlwriter.paragraph_open();
    htmlwriter.text_add (messages[i]);
    htmlwriter.paragraph_close();
  } 
}


void sword_kjv_internal_dissect_text (ustring text, vector <unsigned int>& strongs, vector <ustring>& phrases)
// Extracts the phrases and strong's numbers from a verse text.
// A raw verse would look like this:
// (07225)In the beginning (0430)God (0853)(01254)created (08064)the heaven (0853)and (0776)the earth.
{
  phrases.clear();
  strongs.clear();
  size_t pos = text.find ("(");
  while (pos != string::npos) {
    // Handle the unlikely case that the text does not start with a Strong's number.
    if (pos > 0) {
      gw_warning (text);
      ustring phrase = text.substr (0, pos);
      text.erase (0, pos);
      phrases.push_back (phrase);
      strongs.push_back (0);
    }
    // Look for the Strong's number.
    unsigned int strong = 0;
    pos = text.find (")");
    if (pos != string::npos) {
      ustring number = text.substr (1, pos - 1);
      strong = convert_to_int (number);
      text.erase (0, pos + 1);
    }
    // Look for the phrase.
    pos = text.find ("(");
    ustring phrase (text);
    if (pos != string::npos) {
      phrase = text.substr (0, pos);
      phrase = trim (phrase);
      text.erase (0, pos);
    }
    // Store the data found.
    phrases.push_back (phrase);
    strongs.push_back (strong);
    // Next iteration.
    pos = text.find ("(");
  }
}


void sword_kjv_internal_clean_text (ustring& text)
// Clens the Strong's numbers of of the text.
// A raw verse would look like this:
// (07225)In the beginning (0430)God (0853)(01254)created (08064)the heaven (0853)and (0776)the earth.
{
  size_t pos1 = text.find ("(");
  while (pos1 != string::npos) {
    size_t pos2 = text.find (")");
    if (pos2 != string::npos) {
      text.erase (pos1, pos2 - pos1 + 1);
    }
    pos1 = text.find ("(");
  }
}


void sword_kjv_get_strongs_data (const Reference& reference, vector <unsigned int>& strongs, vector <ustring>& phrases)
// This gets the phrases and the strong's numbers for a verse.
{
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    SqliteReader reader(0);
    rc = sqlite3_open(sword_kjv_get_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    char *sql;
    sql = g_strdup_printf("select text from richtext where book = %d and chapter = %d and verse = %d;", reference.book, reference.chapter, convert_to_int (reference.verse));
    rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
    g_free(sql);
    if (rc) {
      throw runtime_error(sqlite3_errmsg(db));
    }
    if (!reader.ustring0.empty()) {
      sword_kjv_internal_dissect_text (reader.ustring0[0], strongs, phrases);
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
}


vector <Reference> sword_kjv_get_strongs_verses (const Reference& reference, unsigned int strongs)
// Passing a Strong's number, and a Reference, this returns all the verses that contain this Strong's number.
// The Reference solely is used to find out whether to look for this Strong's number in the Old or New Testament.
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
    rc = sqlite3_open(sword_kjv_get_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    char *sql;
    sql = g_strdup_printf("select book, chapter, verse from richtext where text glob ('*(%d)*');", strongs);
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


ustring sword_kjv_get_verse (const Reference& reference)
// Get the verse text from the Sword KJV Bible.
{
  ustring text;
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    SqliteReader reader(0);
    rc = sqlite3_open(sword_kjv_get_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    char *sql;
    sql = g_strdup_printf("select text from richtext where book = %d and chapter = %d and verse = %d;", reference.book, reference.chapter, convert_to_int (reference.verse));
    rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
    g_free(sql);
    if (rc) {
      throw runtime_error(sqlite3_errmsg(db));
    }
    if (!reader.ustring0.empty()) {
      text = reader.ustring0[0];
      sword_kjv_internal_clean_text (text);
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
  return text;
}
