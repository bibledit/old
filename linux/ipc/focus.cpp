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


#include <ipc/focus.h>
#include <filter/string.h>
#include <filter/usfm.h>
#include <database/versifications.h>
#include <database/navigation.h>
#include <webserver/request.h>


// Sets the focus.
void Ipc_Focus::set (void * webserver_request, int book, int chapter, int verse)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  bool set = false;
  if (book != getBook (request)) set = true;
  if (chapter != getChapter (request)) set = true;
  if (verse != getVerse (request)) set = true;
  
  if (set) {
    string user = request->session_logic()->currentUser ();
    Database_Ipc database_ipc = Database_Ipc (request);
    database_ipc.storeMessage (user, "", "focus", convert_to_string (book) + "." + convert_to_string (chapter) + "." + convert_to_string (verse));
    request->database_config_user()->setFocusedBook (book);
    request->database_config_user()->setFocusedChapter (chapter);
    request->database_config_user()->setFocusedVerse (verse);
  }
}


// Gets the focused book.
int Ipc_Focus::getBook (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  int book = request->database_config_user()->getFocusedBook ();
  return book;
}


// Gets the focused chapter.
int Ipc_Focus::getChapter (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  int chapter = request->database_config_user()->getFocusedChapter ();
  return chapter;
}


// Gets the focused verse.
int Ipc_Focus::getVerse (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  int verse = request->database_config_user()->getFocusedVerse ();
  return verse;
}
