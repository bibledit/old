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


#include <checks/index.h>
#include <assets/view.h>
#include <assets/page.h>
#include <filter/roles.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/md5.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <database/check.h>
#include <client/logic.h>
#include <demo/logic.h>
#include <sendreceive/logic.h>
#include <config/logic.h>
#include <access/bible.h>


string checks_index_url ()
{
  return "checks/index";
}


bool checks_index_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::translator ());
}


string checks_index (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  Database_Check database_check;

  
  string page;
  page = Assets_Page::header (translate ("Checks"), webserver_request);
  Assets_View view;
  
  
  if (request->query.count ("approve")) {
    int approve = convert_to_int (request->query["approve"]);
    database_check.approve (approve);
    view.set_variable ("success", translate("The entry was approved and suppressed."));
  }
  
                        
  if (request->query.count ("delete")) {
    int erase = convert_to_int (request->query["delete"]);
    database_check.erase (erase);
    view.set_variable ("success", translate("The entry was deleted for just now."));
  }

  
  // Get the Bibles the user has write-access to.
  vector <int> bibleIDs;
  vector <string> bibles = request->database_bibles()->getBibles ();
  for (auto bible : bibles) {
    if (access_bible_write (webserver_request, bible)) {
      int id = request->database_bibles()->getID (bible);
      bibleIDs.push_back (id);
    }
  }
  
  
  string resultblock;
  vector <Database_Check_Hit> hits = database_check.getHits ();
  for (auto hit : hits) {
    int bibleID = hit.bible;
    if (find (bibleIDs.begin(), bibleIDs.end (), bibleID) != bibleIDs.end ()) {
      int id = hit.rowid;
      string bible = filter_string_sanitize_html (request->database_bibles()->getName (bibleID));
      int book = hit.book;
      int chapter = hit.chapter;
      int verse = hit.verse;
      string link = filter_passage_link_for_opening_editor_at (book, chapter, convert_to_string (verse));
      string information = filter_string_sanitize_html (hit.data);
      resultblock.append ("<p>\n");
      resultblock.append ("<a href=\"index?approve=" + convert_to_string (id) + "\"> ✔ </a>\n");
      resultblock.append ("<a href=\"index?delete=" + convert_to_string (id) + "\"> ✗ </a>\n");
      resultblock.append (bible);
      resultblock.append (" ");
      resultblock.append (link);
      resultblock.append (" ");
      resultblock.append (information);
      resultblock.append ("</p>\n");
    }
  }
  view.set_variable ("resultblock", resultblock);


  page += view.render ("checks", "index");
  page += Assets_Page::footer ();
  return page;
}
