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


#include <fonts/index.h>
#include <assets/view.h>
#include <assets/page.h>
#include <dialog/entry.h>
#include <dialog/list.h>
#include <filter/roles.h>
#include <filter/url.h>
#include <filter/string.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <fonts/logic.h>
#include <database/config/bible.h>


string fonts_index_url ()
{
  return "fonts/index";
}


bool fonts_index_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::manager ());
}


string fonts_index (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  string page;
  page = Assets_Page::header (translate ("Fonts"), webserver_request, "");
  Assets_View view = Assets_View ();
  string success;

  
  // Delete a font if it is not in use.
  if (request->query.count ("delete")) {
    string del = request->query ["delete"];
    string font = filter_url_basename (del);
    bool font_in_use = false;
    vector <string> bibles = request->database_bibles ()->getBibles ();
    for (auto & bible : bibles) {
      if (font == Database_Config_Bible::getTextFont (bible)) font_in_use = true;
    }
    if (!font_in_use) {
      Fonts_Logic::erase (font);
    } else {
      page += Assets_Page::error (translate("The font could not be deleted because it is in use."));
    }
  }
  

  // Upload a font.
  // It may take time in case the file is large or the network is slow.
  if (request->post.count ("upload")) {
    string filename = request->post ["filename"];
    string path = filter_url_create_root_path ("fonts", filename);
    string data = request->post ["data"];
    filter_url_file_put_contents (path, data);
    success = translate("The font has been uploaded.");
  }
  

  vector <string> fonts = Fonts_Logic::getFonts ();
  
  vector <string> fontsblock;
  for (auto & font : fonts) {
    fontsblock.push_back ("<p>");
    fontsblock.push_back ("<a href=\"?delete=" + font+ "\" title=\"" + translate("Delete font") + "\"> ✗ </a>");
    fontsblock.push_back (font);
    fontsblock.push_back ("</p>");
  }
  
  view.set_variable ("fontsblock", filter_string_implode (fontsblock, "\n"));
  view.set_variable ("success", success);
  
  page += view.render ("fonts", "index");
  
  page += Assets_Page::footer ();
  
  return page;
}
