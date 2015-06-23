/*
 Copyright (©) 2003-2015 Teus Benschop.
 
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


#include <demo/logic.h>
#include <filter/md5.h>
#include <filter/roles.h>
#include <filter/usfm.h>
#include <filter/url.h>
#include <filter/string.h>
#include <config/logic.h>
#include <config.h>
#include <database/config/general.h>
#include <database/config/bible.h>
#include <database/logs.h>
#include <database/notes.h>
#include <locale/translate.h>
#include <client/logic.h>
#include <styles/logic.h>
#include <styles/sheets.h>
#include <locale/logic.h>
#include <bible/logic.h>


// Returns true for correct credentials for a demo installation.
// Else returns false.
bool demo_acl (string user, string pass)
{
  if (strcmp (DEMO, "yes") == 0) {
    if (user == session_admin_credentials ()) {
      if ((pass == session_admin_credentials ()) || (pass == md5 (session_admin_credentials ()))) {
        return true;
      }
    }
  }
  return false;
}


// Returns the address of the current demo server.
string demo_address ()
{
  return "http://bibledit.org";
}


int demo_port ()
{
  return 8080;
}


// Returns a warning in case the client is connected to the open demo server.
string demo_client_warning ()
{
  string warning;
  if (client_logic_client_enabled ()) {
    string address = Database_Config_General::getServerAddress ();
    if (address == demo_address ()) {
      int port = Database_Config_General::getServerPort ();
      if (port == demo_port ()) {
        warning.append (translate("You are connected to a public demo of Bibledit Cloud."));
        warning.append (" ");
        warning.append (translate("Everybody can modify the data on that server."));
        warning.append (" ");
        warning.append (translate("After send and receive your data will reflect the data on the server."));
      }
    }
  }
  return warning;
}


// Cleans and resets the data in the Bibledit installation.
void demo_clean_data ()
{
  Database_Logs::log ("Cleaning up the demo data");

  
  Webserver_Request request;
  
  
  // Set user to the demo credentials (admin) as this is the user who is always logged-in in a demo installation.
  request.session_logic ()->setUsername (session_admin_credentials ());
  
  
  // Delete the "Standard" stylesheet and re-create it.
  // Delete empty sheet that may have been there.
  request.database_styles()->deleteSheet (styles_logic_standard_sheet ());
  request.database_styles()->revokeWriteAccess ("", styles_logic_standard_sheet ());
  request.database_styles()->deleteSheet ("");
  request.database_styles()->createSheet (styles_logic_standard_sheet ());
  request.database_styles()->grantWriteAccess (session_admin_credentials (), styles_logic_standard_sheet ());
  styles_sheets_create_all ();
  
  
  // Set the export stylesheet to "Standard" for all Bibles and the admin.
  vector <string> bibles = request.database_bibles()->getBibles ();
  for (auto & bible : bibles) {
    Database_Config_Bible::setExportStylesheet (bible, styles_logic_standard_sheet ());
  }
  request.database_config_user()->setStylesheet (styles_logic_standard_sheet ());
  
  
  // Set the site language to "Default"
  Database_Config_General::setSiteLanguage ("");
  
  
  // Ensure the default users are there.
  map <string, int> users = {
    make_pair ("guest", Filter_Roles::guest ()),
    make_pair ("member", Filter_Roles::member ()),
    make_pair ("consultant", Filter_Roles::consultant ()),
    make_pair ("translator", Filter_Roles::translator ()),
    make_pair ("manager", Filter_Roles::manager ()),
    make_pair (session_admin_credentials (), Filter_Roles::admin ())
  };
  for (auto & element : users) {
    if (!request.database_users ()->usernameExists (element.first)) {
      request.database_users ()->addNewUser(element.first, element.first, element.second, "");
    }
    request.database_users ()->updateUserLevel (element.first, element.second);
  }

  
  // Create / update sample Bible.
  demo_create_sample_bible (&request);


  // Clean out nearly empty chapters from the Bibles.
  bibles = request.database_bibles()->getBibles ();
  for (auto bible : bibles) {
    vector <int> books = request.database_bibles()->getBooks (bible);
    for (auto book : books) {
      vector <int> chapters = request.database_bibles()->getChapters (bible, book);
      for (auto chapter : chapters) {
        // Remove chapters, other than 0, that are rather short, as these chapters likely contain no text, but USFM markers only.
        if (chapter == 0) continue;
        string usfm = request.database_bibles()->getChapter (bible, book, chapter);
        if (usfm.length () < 200) {
          Database_Logs::log ("Deleting a demo chapter because it does not contain enough text: " + convert_to_string (book) + ":" + convert_to_string (chapter));
          Bible_Logic::deleteChapter (bible, book, chapter);
        }
      }
      // If a book contains chapter 0 only, remove that entire book.
      chapters = request.database_bibles()->getChapters (bible, book);
      if ((chapters.size () == 1) && (chapters [0] == 0)) {
        Database_Logs::log ("Deleting a demo book because it is empty");
        Bible_Logic::deleteBook (bible, book);
      }
    }
    // If a Bible contains no books, remove that Bible.
    books = request.database_bibles()->getBooks (bible);
    if (books.empty ()) {
      Database_Logs::log ("Deleting a demo Bible because it is empty");
      Bible_Logic::deleteBible (bible);
    }
  }
  
  
  // Create sample notes.
  demo_create_sample_notes (&request);
}


// The name of the sample Bible.
string demo_sample_bible_name ()
{
  return "Bibledit Sample Bible";
}


// Creates a sample Bible.
void demo_create_sample_bible (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  // Ensure the sample Bible exists.
  request->database_bibles()->createBible (demo_sample_bible_name ());
  
  // Store some text into the sample Bible.
  string directory = filter_url_create_root_path ("demo");
  vector <string> files = filter_url_scandir (directory);
  for (auto file : files) {
    if (filter_url_get_extension (file) == "usfm") {
      file = filter_url_create_path (directory, file);
      string usfm = filter_url_file_get_contents (file);
      vector <BookChapterData> book_chapter_data = usfm_import (usfm, styles_logic_standard_sheet ());
      for (auto data : book_chapter_data) {
        Bible_Logic::storeChapter (demo_sample_bible_name (), data.book, data.chapter, data.data);
      }
    }
  }
  
  // Set the sample Bible to viewable by all users.
  Database_Config_Bible::setViewableByAllUsers (demo_sample_bible_name (), true);
}


// Create sample notes.
void demo_create_sample_notes (void * webserver_request)
{
  Database_Notes database_notes = Database_Notes (webserver_request);
  vector <int> identifiers = database_notes.getIdentifiers ();
  if (identifiers.size () < 10) {
    for (size_t i = 1; i <= 10; i++) {
      database_notes.storeNewNote (demo_sample_bible_name (), i, i, i, "Sample Note " + convert_to_string (i), "Sample Contents for note " + convert_to_string (i), false);
    }
  }
}
