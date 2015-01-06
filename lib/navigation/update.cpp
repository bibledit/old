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

#include <navigation/update.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <webserver/request.h>
#include <navigation/passage.h>
#include <database/books.h>


string navigation_update_url ()
{
  return "navigation/update";
}


bool navigation_update_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string navigation_update (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  
  string bible;
  if (request->query.count ("bible")) {
    bible = request->query ["bible"];
  } else {
    bible = request->database_config_user()->getBible ();
  }
  
  
  if (request->query.count ("goback")) {
    Navigation_Passage::goBack (request);
  }
  
  
  if (request->query.count ("goforward")) {
    Navigation_Passage::goForward (request);
  }
  
  
  if (request->query.count ("getchapters")) {
    string english = request->query ["book"];
    int book = Database_Books::getIdFromEnglish (english);
    return Navigation_Passage::getChaptersFragment (request, bible, book, -1);
  }
  
  
  if (request->query.count ("getverses")) {
    string book = request->query ["book"];
    if (number_in_string (book) != book) {
      book = convert_to_string (Database_Books::getIdFromEnglish (book));
    }
    int chapter = convert_to_int (request->query ["chapter"]);
    return Navigation_Passage::getVersesFragment (request, bible, convert_to_int (book), chapter, -1);
  }
  
  
  if (request->query.count ("verse")) {
    int verse = convert_to_int (request->query ["verse"]);
    string book = request->query ["book"];
    if (number_in_string (book) != book) {
      book = convert_to_string (Database_Books::getIdFromEnglish (book));
    }
    int chapter = convert_to_int (request->query ["chapter"]);
    Navigation_Passage::setBookChapterVerse (webserver_request, convert_to_int (book), chapter, verse);
    return "";
  }
  
  
  if (request->query.count ("previousverse")) {
    Navigation_Passage::gotoPreviousVerse (webserver_request, bible);
  }
  
  
  if (request->query.count ("nextverse")) {
    Navigation_Passage::gotoNextVerse (webserver_request, bible);
  }
  
  
  if (request->query.count ("passage")) {
    string passage = request->query["passage"];
    Navigation_Passage::setPassage (webserver_request, bible, passage);
    return "";
  }
  
  
  return Navigation_Passage::getNavigator (request, bible);
}

