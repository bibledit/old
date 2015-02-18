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
  int book = convert_to_int (request->query ["book"]);
  int chapter = convert_to_int (request->query ["chapter"]);
  int verse = convert_to_int (request->query ["verse"]);
  
  
  if (request->query.count ("goback")) {
    Navigation_Passage::goBack (request);
  }
  
  
  else if (request->query.count ("goforward")) {
    Navigation_Passage::goForward (request);
  }
  
  
  else if (request->query.count ("getbooks")) {
    return Navigation_Passage::getBooksFragment (request, bible);
  }
  
  
  else if (request->query.count ("applybook")) {
    string msg = request->query ["applybook"];
    if (msg.find ("cancel") == string::npos) {
      int book = convert_to_int (msg);
      if (book) Navigation_Passage::setBook (request, book);
    }
  }
  
  
  else if (request->query.count ("getchapters")) {
    return Navigation_Passage::getChaptersFragment (request, bible, book, chapter);
  }
  
  
  else if (request->query.count ("applychapter")) {
    string msg = request->query ["applychapter"];
    if (msg.find ("cancel") == string::npos) {
      int chapter = convert_to_int (msg);
      Navigation_Passage::setChapter (request, chapter);
    }
  }
  
  
  else if (request->query.count ("getverses")) {
    return Navigation_Passage::getVersesFragment (request, bible, convert_to_int (book), chapter, verse);
  }
  

  else if (request->query.count ("applyverse")) {
    string msg = request->query ["applyverse"];
    if (msg.find ("cancel") == string::npos) {
      int verse = convert_to_int (msg);
      Navigation_Passage::setVerse (request, verse);
    }
  }
  
  
  /*
  else if (request->query.count ("verse")) {
    int verse = convert_to_int (request->query ["verse"]);
    string book = request->query ["book"];
    if (number_in_string (book) != book) {
      book = convert_to_string (Database_Books::getIdFromEnglish (book));
    }
    int chapter = convert_to_int (request->query ["chapter"]);
    Navigation_Passage::setBookChapterVerse (webserver_request, convert_to_int (book), chapter, verse);
    return "";
  }
   */
  
  
  else if (request->query.count ("previousverse")) {
    Navigation_Passage::gotoPreviousVerse (webserver_request, bible);
  }
  
  
  else if (request->query.count ("nextverse")) {
    Navigation_Passage::gotoNextVerse (webserver_request, bible);
  }
  
  /*
  else if (request->query.count ("passage")) {
    string passage = request->query["passage"];
    Navigation_Passage::setPassage (webserver_request, bible, passage);
    return "";
  }
   */
  
  return Navigation_Passage::getNavigator (request, bible);
}

