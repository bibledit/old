/*
Copyright (©) 2003-2016 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include <bible/logic.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <filter/git.h>
#include <filter/diff.h>
#include <filter/usfm.h>
#include <filter/text.h>
#include <database/bibles.h>
#include <database/modifications.h>
#include <database/bibleactions.h>
#include <database/logs.h>
#include <database/versifications.h>
#include <database/books.h>
#include <database/config/general.h>
#include <database/cache.h>
#include <database/privileges.h>
#include <database/config/bible.h>
#include <config/logic.h>
#include <client/logic.h>
#include <demo/logic.h>
#include <sync/resources.h>
#include <sword/logic.h>
#include <resource/logic.h>
#include <search/logic.h>


void bible_logic_store_chapter (const string& bible, int book, int chapter, const string& usfm)
{
  Database_Bibles database_bibles;

  // Record data of the chapter to be stored prior to storing the new version.
  if (client_logic_client_enabled ()) {

    // Client stores Bible action.
    string oldusfm = database_bibles.getChapter (bible, book, chapter);
    Database_BibleActions database_bibleactions = Database_BibleActions ();
    database_bibleactions.record (bible, book, chapter, oldusfm);

  } else {

    // Server stores diff data.
    Database_Modifications database_modifications;
    database_modifications.storeTeamDiff (bible, book, chapter);

  }

  // Store the chapter in the database.
  database_bibles.storeChapter (bible, book, chapter, usfm);
}


void bible_logic_delete_chapter (const string& bible, int book, int chapter)
{
  Database_Bibles database_bibles;

  // Record data of the chapter to be deleted prior to deletion.
  if (client_logic_client_enabled ()) {

    // Client stores Bible action.
    string usfm = database_bibles.getChapter (bible, book, chapter);
    Database_BibleActions database_bibleactions = Database_BibleActions ();
    database_bibleactions.record (bible, book, chapter, usfm);

  } else {

    // Server stores diff data.
    Database_Modifications database_modifications;
    database_modifications.storeTeamDiff (bible, book, chapter);

  }

  // Delete the chapter from the database.
  database_bibles.deleteChapter (bible, book, chapter);
}


void bible_logic_delete_book (const string& bible, int book)
{
  Database_Bibles database_bibles;

  // Record data of the book to be deleted prior to deletion.
  if (client_logic_client_enabled ()) {

    // Client stores Bible actions.
    Database_BibleActions database_bibleactions = Database_BibleActions ();
    vector <int> chapters = database_bibles.getChapters (bible, book);
    for (auto & chapter : chapters) {
      string usfm = database_bibles.getChapter (bible, book, chapter);
      database_bibleactions.record (bible, book, chapter, usfm);
    }

  } else {

    // Server stores diff data.
    Database_Modifications database_modifications;
    database_modifications.storeTeamDiffBook (bible, book);

  }

  // Delete the book from the database.
  database_bibles.deleteBook (bible, book);
}


void bible_logic_delete_bible (const string& bible)
{
  Database_Bibles database_bibles;

  // Record data of the Bible to be deleted prior to deletion.
  if (client_logic_client_enabled ()) {

    // Client stores Bible actions.
    Database_BibleActions database_bibleactions = Database_BibleActions ();
    vector <int> books = database_bibles.getBooks (bible);
    for (auto book : books) {
      vector <int> chapters = database_bibles.getChapters (bible, book);
      for (auto chapter : chapters) {
        string usfm = database_bibles.getChapter (bible, book, chapter);
        database_bibleactions.record (bible, book, chapter, usfm);
      }
    }

  } else {

    // Server stores diff data.
    Database_Modifications database_modifications;
    database_modifications.storeTeamDiffBible (bible);
    
    // Possible git repository.
    string gitdirectory = filter_git_directory (bible);
    if (file_exists (gitdirectory)) {
      filter_url_rmdir (gitdirectory);
    }

  }

  // Delete the Bible from the database.
  database_bibles.deleteBible (bible);
  
  // Delete the search index.
  search_logic_delete_bible (bible);
  
  // Delete associated settings and privileges.
  Database_Privileges::removeBible (bible);
  Database_Config_Bible::remove (bible);
}


void bible_logic_import_resource (string bible, string resource)
{
  Database_Logs::log ("Starting to import resource " + resource + " into Bible " + bible);
  
  Database_Versifications database_versifications;
  Webserver_Request webserver_request;
  
  vector <int> books = database_versifications.getMaximumBooks ();
  for (auto & book : books) {
    
    string bookName = Database_Books::getEnglishFromId (book);

    vector <int> chapters = database_versifications.getMaximumChapters (book);
    for (auto & chapter : chapters) {

      string message = "Importing " + resource + " " + bookName + " chapter " + convert_to_string (chapter);
      Database_Logs::log (message, Filter_Roles::translator ());
      
      vector <string> usfm;
      
      if (chapter == 0) usfm.push_back ("\\id " + Database_Books::getUsfmFromId (book));
      
      if (chapter) {
        usfm.push_back ("\\c " + convert_to_string (chapter));
        usfm.push_back ("\\p");
      }

      vector <int> verses = database_versifications.getMaximumVerses (book, chapter);
      for (auto & verse : verses) {
        
        if (verse == 0) continue;

        // Fetch the text for the passage.
        bool server_is_installing_module = false;
        int wait_iterations = 0;
        string html;
        do {
          // Fetch this resource from the server.
          html = resource_logic_get_verse (&webserver_request, resource, book, chapter, verse);
          server_is_installing_module = (html == sword_logic_installing_module_text ());
          if (server_is_installing_module) {
            Database_Logs::log ("Waiting while Bibledit Cloud installs the requested SWORD module");
            this_thread::sleep_for (chrono::seconds (60));
            wait_iterations++;
          }
        } while (server_is_installing_module && (wait_iterations < 5));
        
        // Remove all html markup.
        html = filter_string_html2text (html);
        html = filter_string_str_replace ("\n", " ", html);

        // Add the verse to the USFM.
        usfm.push_back ("\\v " + convert_to_string (verse) + " " + filter_string_trim (html));
      }
      bible_logic_store_chapter (bible, book, chapter, filter_string_implode (usfm, "\n"));
    }
  }
  
  Database_Logs::log ("Completed importing resource " + resource + " into Bible " + bible);
}


// This logs the change in the Bible text.
void bible_logic_log_change (const string& bible, int book, int chapter, const string& usfm, const string & user, const string & summary)
{
#ifdef CLIENT_PREPARED

  (void) bible;
  (void) book;
  (void) chapter;
  (void) usfm;
  (void) user;
  (void) summary;
  
#else

  Database_Bibles database_bibles;
  string existing_usfm = database_bibles.getChapter (bible, book, chapter);
  
  int percentage = filter_diff_similarity (existing_usfm, usfm);
  percentage = 100 - percentage;
  string percentage_fragment;
  if (percentage) {
    percentage_fragment = " - " + convert_to_string (percentage) + "% change";
  }

  string stylesheet = Database_Config_Bible::getExportStylesheet (bible);

  vector <int> existing_verse_numbers = usfm_get_verse_numbers (existing_usfm);
  vector <int> verse_numbers = usfm_get_verse_numbers (usfm);
  vector <int> verses = existing_verse_numbers;
  verses.insert (verses.end (), verse_numbers.begin (), verse_numbers.end ());
  verses = array_unique (verses);
  sort (verses.begin (), verses.end ());

  vector <string> body;
  
  body.push_back ("Changes:");
  
  for (auto verse : verses) {
    string existing_verse_usfm = usfm_get_verse_text (existing_usfm, verse);
    string verse_usfm = usfm_get_verse_text (usfm, verse);
    if (existing_verse_usfm != verse_usfm) {
      Filter_Text filter_text_old = Filter_Text (bible);
      Filter_Text filter_text_new = Filter_Text (bible);
      filter_text_old.text_text = new Text_Text ();
      filter_text_new.text_text = new Text_Text ();
      filter_text_old.addUsfmCode (existing_verse_usfm);
      filter_text_new.addUsfmCode (verse_usfm);
      filter_text_old.run (stylesheet);
      filter_text_new.run (stylesheet);
      string old_text = filter_text_old.text_text->get ();
      string new_text = filter_text_new.text_text->get ();
      if (old_text != new_text) {
        body.push_back ("");
        body.push_back (filter_passage_display (book, chapter, convert_to_string (verse)));
        body.push_back ("Old: " + old_text);
        body.push_back ("New: " + new_text);
      }
    }
  }

  body.push_back ("");
  body.push_back ("Old USFM:");
  body.push_back (existing_usfm);

  body.push_back ("");
  body.push_back ("New USFM:");
  body.push_back (usfm);
  
  Database_Logs::log (user + " - " + summary + percentage_fragment, filter_string_implode (body, "\n"));

#endif
}
