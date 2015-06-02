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


#include <bible/css.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/customcss.h>
#include <webserver/request.h>
#include <database/config/bible.h>
#include <database/books.h>
#include <locale/translate.h>
#include <access/bible.h>
#include <fonts/logic.h>


string bible_css_url ()
{
  return "bible/css";
}


bool bible_css_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::translator ());
}


string bible_css (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  string page;
  
  Assets_Header header = Assets_Header (translate("Font and text direction"), request);
  header.jQueryUIOn ("sortable");
  page = header.run ();
  
  Assets_View view = Assets_View ();
  
  // The name of the Bible.
  string bible = access_bible_clamp (request, request->query ["bible"]);
  view.set_variable ("bible", filter_string_sanitize_html (bible));
  
  // Data submission.
  if (request->post.count ("submit")) {
    
    string font = request->post ["font"];
    font = filter_string_trim (font);
    Database_Config_Bible::setTextFont (bible, font);
    
    string s_direction = request->post ["direction"];
    int i_direction = Filter_CustomCSS::directionValue (s_direction);
    
    string s_mode = request->post ["mode"];
    int i_mode = Filter_CustomCSS::writingModeValue (s_mode);
    
    Database_Config_Bible::setTextDirection (bible, i_mode * 10 + i_direction);
    
    page += Assets_Page::error ("The information was saved.");
    
  }
  
  string font = Database_Config_Bible::getTextFont (bible);
  view.set_variable ("font", font);

  int direction = Database_Config_Bible::getTextDirection (bible);
  
  view.set_variable ("direction_none", Filter_CustomCSS::directionUnspecified (direction));
  view.set_variable ("direction_ltr", Filter_CustomCSS::directionLeftToRight (direction));
  view.set_variable ("direction_rtl", Filter_CustomCSS::directionRightToLeft (direction));
  
  view.set_variable ("mode_none", Filter_CustomCSS::writingModeUnspecified (direction));
  view.set_variable ("mode_tblr", Filter_CustomCSS::writingModeTopBottomLeftRight (direction));
  view.set_variable ("mode_tbrl", Filter_CustomCSS::writingModeTopBottomRightLeft (direction));
  view.set_variable ("mode_btlr", Filter_CustomCSS::writingModeBottomTopLeftRight (direction));
  view.set_variable ("mode_btrl", Filter_CustomCSS::writingModeBottomTopRightLeft (direction));
  
  string cls = Filter_CustomCSS::getClass (bible);
  view.set_variable ("custom_class", cls);
  view.set_variable ("custom_css", Filter_CustomCSS::getCss (cls, Fonts_Logic::getFontPath (font), direction));

  page += view.render ("bible", "css");
  
  page += Assets_Page::footer ();
  
  return page;
}
