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


#include <sync/bibles.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <tasks/logic.h>
#include <config/logic.h>
#include <database/config/general.h>
#include <database/config/bible.h>
#include <database/logs.h>
#include <database/bibles.h>
#include <client/logic.h>
#include <locale/translate.h>
#include <webserver/request.h>
#include <sync/logic.h>
#include <checksum/logic.h>
#include <access/bible.h>


string sync_bibles_url ()
{
  return "sync/bibles";
}


bool sync_bibles_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::guest ());
}


string sync_bibles (void * webserver_request) // Todo
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  Sync_Logic sync_logic = Sync_Logic (webserver_request);
  
  // Check on the credentials.
  if (!sync_logic.credentials_okay ()) return "";
  
  // Get the relevant parameters the client may have POSTed to us, the server.
  int action = convert_to_int (request->post ["a"]);
  string bible = request->post ["b"];
  int book = convert_to_int (request->post ["bk"]);
  int chapter = convert_to_int (request->post ["c"]);
  
  switch (action) {
    case Sync_Logic::bibles_get_total_checksum:
    {
      // The server reads the credentials from the client's user,
      // checks which Bibles this user has access to,
      // calculate the checksum of all chapters in those Bibles,
      // and returns this checksum to the client.
      string username = request->session_logic ()->currentUser ();
      vector <string> bibles = access_bible_bibles (request, username);
      string server_checksum = Checksum_Logic::getBibles (request, bibles);
      return server_checksum;
    }
    case Sync_Logic::bibles_get_bibles:
    {
      // The server reads the credentials from the client's user,
      // and responds with a list of Bibles this user has access to.
      string username = request->session_logic ()->currentUser ();
      vector <string> bibles = access_bible_bibles (request, username);
      string s_bibles = filter_string_implode (bibles, "\n");
      string checksum = Checksum_Logic::get (s_bibles);
      return checksum + "\n" + s_bibles;
    }
    case Sync_Logic::bibles_get_bible_checksum:
    {
      // The server responds with the checksum for the whole Bible.
      return Checksum_Logic::getBible (request, bible);
    }
    case Sync_Logic::bibles_get_books:
    {
      // The server responds with a checksum and then the list of books in the Bible.
      vector <int> server_books = request->database_bibles()->getBooks (bible);
      vector <string> v_server_books;
      for (auto book : server_books) v_server_books.push_back (to_string (book));
      string s_server_books = filter_string_implode (v_server_books, "\n");
      string checksum = Checksum_Logic::get (v_server_books);
      return checksum + "\n" + s_server_books;
    }
    case Sync_Logic::bibles_get_book_checksum:
    {
      // The server responds with the checksum of the whole book.
      return Checksum_Logic::getBook (request, bible, book);
    }
    case Sync_Logic::bibles_get_chapters:
    {
      // The server responds with the list of books in the Bible book.
      vector <int> server_chapters = request->database_bibles()->getChapters (bible, book);
      vector <string> v_server_chapters;
      for (auto & chapter : server_chapters) v_server_chapters.push_back (to_string (chapter));
      string s_server_chapters = filter_string_implode (v_server_chapters, "\n");
      string checksum = Checksum_Logic::get (v_server_chapters);
      return checksum + "\n" + s_server_chapters;
    }
    case Sync_Logic::bibles_get_chapter_checksum:
    {
      // The server responds with the checksum of the whole chapter.
      return Checksum_Logic::getChapter (request, bible, book, chapter);
    }
    case Sync_Logic::bibles_send_chapter: // Todo
    {
      return "";
    }
    case Sync_Logic::bibles_get_chapter:
    {
      // The server responds with the USFM of the chapter, prefixed by a checksum.
      string usfm = request->database_bibles()->getChapter (bible, book, chapter);
      string checksum = Checksum_Logic::get (usfm);
      return checksum + "\n" + usfm;
    }
  }
  
  // Bad request.
  // Delay a while to obstruct a flood of bad requests.
  this_thread::sleep_for (chrono::seconds (1));
  request->response_code = 400;
  return "";
}
