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


#include <lexicon/index.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/url.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <ipc/focus.h>
#include <database/kjv.h>


string lexicon_index_url ()
{
  return "lexicon/index";
}


bool lexicon_index_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string lexicon_index (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  
  Database_Kjv database_kjv;
  
  
  Assets_Header header = Assets_Header (translate ("Lexicon"), request);
  header.setNavigator ();
  string page = header.run ();
  Assets_View view = Assets_View ();

  
  int book = Ipc_Focus::getBook (request);
  int chapter = Ipc_Focus::getChapter (request);
  int verse = Ipc_Focus::getVerse (request);

  
  string englishblock;
  vector <Database_Kjv_Item> kjv_items = database_kjv.getVerse (book, chapter, verse);
  for (auto & item : kjv_items) {
    if (!englishblock.empty ()) englishblock.append (" ");
    string href = filter_url_build_http_query ("strong", "s", item.strong);
    string title = "Strong's " + item.strong;
    englishblock.append ("<a href=\"" + href + "\" title =\"" + title + "\">" + item.english + "</a>");
  }
  view.set_variable ("englishblock", englishblock);


  page += view.render ("lexicon", "index");
  page += Assets_Page::footer ();
  return page;
}
