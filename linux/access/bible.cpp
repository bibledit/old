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


#include <access/bible.h>
#include <webserver/request.h>
#include <database/config/bible.h>
#include <config/logic.h>
#include <client/logic.h>


// Returns true if the $user has read access to the $bible.
// If no $user is given, it takes the currently logged-in user.
bool access_bible_read (void * webserver_request, string & bible, string user)
{
  if (client_logic_client_enabled ()) {
    // Client: User has access to all Bibles.
    return true;
  }
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  if (user == "") {
    user = request->session_logic ()->currentUser ();
  }
  if (request->database_users ()->hasAccess2Bible (user, bible)) {
    return true;
  }
  if (Database_Config_Bible::getViewableByAllUsers (bible)) {
    return true;
  }
  return false;
}


// Returns true if the user has write access to the entire $bible.
// If no user is given, it takes the currently logged-in user.
// If the user has read-only access to even one book of the $bible,
// then the user is considered not to have write access to the entire $bible.
bool access_bible_write (void * webserver_request, string & bible, string user)
{
  if (client_logic_client_enabled ()) {
    // Client: User has access to all Bibles.
    return true;
  }
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  if (user == "") {
    user = request->session_logic ()->currentUser ();
  }
  if (!request->database_users ()->hasAccess2Bible (user, bible)) {
    return false;
  }
  bool readonly = request->database_users ()->hasReadOnlyAccess2Bible (user, bible);
  return !readonly;
}


// Returns true if the $user has write access to the $bible and the $book.
// If no $user is given, it takes the currently logged-in user.
bool access_bible_book_write (void * webserver_request, string user, const string & bible, int book)
{
  if (client_logic_client_enabled ()) {
    // Client: User has access to all Bibles.
    return true;
  }
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  if (user == "") {
    user = request->session_logic ()->currentUser ();
  }
  if (!request->database_users ()->hasAccess2Bible (user, bible)) {
    return false;
  }
  bool readonly = request->database_users ()->hasReadOnlyAccess2Book (user, bible, book);
  return !readonly;
}


// Returns an array of Bibles the user has read access to.
// If no user is given, it takes the currently logged-in user.
vector <string> access_bible_bibles (void * webserver_request, string user)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  vector <string> allbibles = request->database_bibles ()->getBibles ();
  vector <string> bibles;
  for (auto & bible : allbibles) {
    if (access_bible_read (webserver_request, bible, user)) {
      bibles.push_back (bible);
    }
  }
  return bibles;
}


// This function clamps bible.
// It returns bible if the currently logged-in user has access to it.
// Else it returns another accessible bible or "".
string access_bible_clamp (void * webserver_request, string bible)
{
  if (!access_bible_read (webserver_request, bible)) {
    bible = "";
    vector <string> bibles = access_bible_bibles (webserver_request);
    if (!bibles.empty ()) bible = bibles [0];
    Webserver_Request * request = (Webserver_Request *) webserver_request;
    request->database_config_user ()->setBible (bible);
  }
  return bible;
}

