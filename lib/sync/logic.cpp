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


#include <sync/logic.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/md5.h>
#include <filter/roles.h>
#include <database/notes.h>
#include <database/noteactions.h>
#include <database/mail.h>
#include <database/logs.h>
#include <database/config/general.h>
#include <config/logic.h>
#include <trash/handler.h>


Sync_Logic::Sync_Logic (void * webserver_request_in)
{
  webserver_request = webserver_request_in;
}


Sync_Logic::~Sync_Logic ()
{
}


bool Sync_Logic::credentials_okay ()
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  // Get the credentials the client POSTed to the us, the server.
  string username = hex2bin (request->post ["u"]);
  string password = request->post ["p"];
  int level = convert_to_int (request->post ["l"]);
  
  // Check all credentials.
  bool user_ok = request->database_users ()->usernameExists (username);
  if (!user_ok) Database_Logs::log ("Non existing user: " + username, Filter_Roles::manager ());
  bool pass_ok = (password == request->database_users ()->getmd5 (username));
  if (!pass_ok) Database_Logs::log ("Incorrect password: " + password, Filter_Roles::manager ());
  bool level_ok = (level == request->database_users ()->getUserLevel (username));
  if (!level_ok) Database_Logs::log ("Incorrect role: " + Filter_Roles::text (level), Filter_Roles::manager ());
  if (!user_ok || !pass_ok || !level_ok) {
    // Unauthorized.
    // Delay a while to obstruct a flood of requests with invalid credentials.
    this_thread::sleep_for (chrono::seconds (1));
    request->response_code = 401;
    return false;
  }
  
  // Set username in session.
  request->session_logic ()->setUsername (username);
  
  // OK.
  return true;
}


// Calculates the checksum of the array of note identifiers.
string Sync_Logic::checksum (const vector <int> & identifiers)
{
  Database_Notes database_notes = Database_Notes (webserver_request);
  vector <string> checksums;
  for (const auto & identifier : identifiers) {
    checksums.push_back (database_notes.getChecksum (identifier));
  }
  string checksum = filter_string_implode (checksums, "");
  checksum = md5 (checksum);
  return checksum;
}


// This function takes a start-ing note identifier,
// and an end-ing note identifier.
// It divides this range of identifier into ten 
// smaller ranges.
// It returns an array of array (start, end) with the new ranges 
// for the note identifiers.
vector <Sync_Logic_Range> Sync_Logic::create_range (int start, int end)
{
  int range = end - start;
  range = round (range / 10);
  vector <Sync_Logic_Range> ranges;
  for (unsigned int i = 0; i <= 9; i++) {
    int first = start + (i * range);
    int last = start + ((i + 1) * range) - 1;
    if (i == 9) {
      last = end;
    }
    Sync_Logic_Range item;
    item.low = first;
    item.high = last;
    ranges.push_back (item);
  }
  return ranges;
}


// Sends a post request to the url.
// On failure it retries a few times.
// It returns the server's response, or an empty string on failure.
string Sync_Logic::post (map <string, string> & post, const string& url, string & error)
{
  error.clear ();
  // After failure, retry a few times more.
  int retry = 0;
  while (++retry <= 3) {
    string response = filter_url_http_post (url, post, error);
    if (error.empty ()) {
      // Success: Return response.
      return response;
    } else {
      // Log failure.
      Database_Logs::log (error, Filter_Roles::translator ());
    }
  }
  // Too many failures: Give up.
  return "";
}


// Calculates the checksum of all settings that to be kept in sync between server and client.
string Sync_Logic::settings_checksum ()
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  string checksum;
  checksum.append (request->database_config_user()->getWorkbenchURLs ());
  checksum.append (request->database_config_user()->getWorkbenchWidths ());
  checksum.append (request->database_config_user()->getWorkbenchHeights ());
  return md5 (checksum);
}


/* C++Port


// Calculates the checksum of all offline resources.
static public function offline_resources_checksum ()
{
  checksum = "";
  database_offlineresources = Database_OfflineResources::getInstance ();
  names = database_offlineresources.names ();
  for (names as name) {
    checksum += self::offline_resource_checksum (name);
  }
  checksum = md5 (checksum);
  return checksum;
}


// Calculates the checksum of offline resource name.
static public function offline_resource_checksum (name)
{
  checksum = array ();
  database_offlineresources = Database_OfflineResources::getInstance ();
  files = database_offlineresources.files (name);
  for (files as file) {
    checksum [] = name;
    checksum [] = self::offline_resource_file_checksum (name, file);
  }
  checksum = implode ("", checksum);
  checksum = md5 (checksum);
  return checksum;    
}


// Calculates the checksum of offline resource name the file.
static public function offline_resource_file_checksum (name, file)
{
  database_offlineresources = Database_OfflineResources::getInstance ();
  checksum = database_offlineresources.size (name, file);
  return checksum;
}


// Calculates the checksum of all USFM resources.
static public function usfm_resources_checksum ()
{
  checksum = array ();
  database_usfmresources = Database_UsfmResources::getInstance ();
  resources = database_usfmresources.getResources ();
  for (resources as resource) {
    checksum [] = self::usfm_resource_checksum (resource);
  }
  checksum = implode ("", checksum);
  checksum = md5 (checksum);
  return checksum;    
}


// Calculates the checksum of USFM resource name.
static public function usfm_resource_checksum (name)
{
  checksum = array ();
  database_usfmresources = Database_UsfmResources::getInstance ();
  books = database_usfmresources.getBooks (name);
  for (books as book) {
    checksum [] = book;
    checksum [] = self::usfm_resource_book_checksum (name, book);
  }
  checksum = implode ("", checksum);
  checksum = md5 (checksum);
  return checksum;    
}


// Calculates the checksum of USFM resource name book.
static public function usfm_resource_book_checksum (name, book)
{
  checksum = array ();
  database_usfmresources = Database_UsfmResources::getInstance ();
  chapters = database_usfmresources.getChapters (name, book);
  for (chapters as chapter) {
    checksum [] = chapter;
    checksum [] = self::usfm_resource_chapter_checksum (name, book, chapter);
  }
  checksum = implode ("", checksum);
  checksum = md5 (checksum);
  return checksum;    
}


// Calculates the checksum of USFM resource name book chapter.
static public function usfm_resource_chapter_checksum (name, book, chapter)
{
  database_usfmresources = Database_UsfmResources::getInstance ();
  checksum = database_usfmresources.getSize (name, book, chapter);
  return checksum;
}

*/

