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


#include <assets/header.h>
#include <filter/url.h>
#include <filter/string.h>
#include <config/globals.h>
#include <locale/translate.h>
#include <config/logic.h>
#include <access/bible.h>
#include <navigation/passage.h>
#include <menu/logic.h>


Assets_Header::Assets_Header (string title, void * webserver_request_in)
{
  includeJQueryUI = false;
  displayNavigator = false;
  webserver_request = webserver_request_in;
  view = new Assets_View ();
  view->set_variable ("title", title);
}


Assets_Header::~Assets_Header ()
{
  delete view;
}


void Assets_Header::jQueryUIOn ()
{
  includeJQueryUI = true;
}


// Display the passage navigator.
void Assets_Header::setNavigator ()
{
  displayNavigator = true;
}


// Display the user's basic stylesheet.css.
void Assets_Header::setStylesheet ()
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  string stylesheet = request->database_config_user()->getStylesheet ();
  includedStylesheet = stylesheet;
}


// Display the user's editor stylesheet.css.
void Assets_Header::setEditorStylesheet ()
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  string stylesheet = request->database_config_user()->getStylesheet ();
  includedEditorStylesheet = stylesheet;
}


// Whether to display the topbar.
bool Assets_Header::displayTopbar ()
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  // If the topbar is in the query: Don't display the top bar.
  if (request->query.count ("topbar")) {
    return false;
  }
  // Display the topbar.
  return true;
}


// Sets the page to refresh after "seconds".
void Assets_Header::refresh (int seconds, string url)
{
  string content = convert_to_string (seconds);
  if (!url.empty ()) content.append (";URL=" + url);
  string headline = "<META HTTP-EQUIV=\"refresh\" CONTENT=\"" + content + "\">";
  headLines.push_back (headline);
}


// Adds a menu item to the fading menu.
void Assets_Header::setFadingMenu (string html) // Todo
{
  fadingmenu = html;
}


// Runs the header.
string Assets_Header::run ()
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  string page;
  
  // Include the Bibledit version number in the stylesheet and javascript URL
  // to refresh the browser's cache after a Bibledit upgrade.
  view->set_variable("VERSION", config_logic_version ());

  if (includeJQueryUI) {
    view->enable_zone ("include_jquery_ui");
  }

  string headlines;
  for (auto & headline : headLines) {
    if (!headlines.empty ()) headlines.append ("\n");
    headlines.append (headline);
  }
  view->set_variable ("head_lines", headlines);

  if (!includedStylesheet.empty ()) {
    view->enable_zone ("include_stylesheet");
    view->set_variable ("included_stylesheet", includedStylesheet);
  }
  if (!includedEditorStylesheet.empty ()) {
    view->enable_zone ("include_editor_stylesheet");
    view->set_variable ("included_editor_stylesheet", includedEditorStylesheet);
  }
  
  if (displayTopbar ()) {
    view->enable_zone ("display_topbar");
    
    // The start button to be displayed only when there's no menu.
    bool start_button = true;
    
    string menublock;
    string item = request->query ["item"];
    if (item == "main") {
      menublock = menu_logic_main_categories (webserver_request);
      start_button = false;
    } else if (item == "translate") {
      menublock = menu_logic_translate_category (webserver_request);
    } else if (item == "search") {
      menublock = menu_logic_search_category (webserver_request);
    } else if (item == "tools") {
      menublock = menu_logic_tools_category (webserver_request);
    } else if (item == "settings") {
      menublock = menu_logic_settings_category (webserver_request);
    } else if (item == "help") {
      menublock = menu_logic_help_category (webserver_request);
    }
    view->set_variable ("mainmenu", menublock);
    
    if (start_button) {
      view->enable_zone ("start_button");
    }
    
    if (!fadingmenu.empty ()) {
      view->enable_zone ("fading_menu");
      view->set_variable ("fadingmenu", fadingmenu);
      fadingmenu.clear ();
    }

    if (displayNavigator) {
      view->enable_zone ("display_navigator");
      string bible = access_bible_clamp (request, request->database_config_user()->getBible ());
      view->set_variable ("navigation_code", Navigation_Passage::code (bible, true));
    }
  }

  vector <string> embedded_css;
  int fontsize = request->database_config_user ()->getGeneralFontSize ();
  if (fontsize != 100) {
    embedded_css.push_back ("body { font-size: " + convert_to_string (fontsize) + "%; }");
  }
  fontsize = request->database_config_user ()->getMenuFontSize ();
  if (fontsize != 100) {
    embedded_css.push_back (".menu { font-size: " + convert_to_string (fontsize) + "%; }");
  }
  fontsize = request->database_config_user ()->getHebrewFontSize ();
  if (fontsize != 100) {
    embedded_css.push_back (".hebrew { font-size: " + convert_to_string (fontsize) + "%; }");
  }
  fontsize = request->database_config_user ()->getGreekFontSize ();
  if (fontsize != 100) {
    embedded_css.push_back (".greek { font-size: " + convert_to_string (fontsize) + "%; }");
  }
  if (!embedded_css.empty ()) {
    view->set_variable ("embedded_css", filter_string_implode (embedded_css, "\n"));
  }
  
  page += view->render("assets", "xhtml_start");
  page += view->render("assets", "header");

  return page;
}
