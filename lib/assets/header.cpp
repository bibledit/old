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
#include <menu/main.h>
#include <menu/user.h>
#include <locale/translate.h>
#include <config.h>


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


void Assets_Header::jQueryUIOn (string subset)
{
  includeJQueryUI = true;
  // Setting a subset makes a big difference to the size of the JavaScript
  // the page has to load, and therefore it makes a big difference for the
  // load time of the page.
  // Subsets can be created on the download builder at http://jqueryui.com.
  if (subset != "") {
    JQueryUISubset = subset + ".";
  }
}


// Adds logic to the header allowing to redirect the user to the original
// URL after successful login.
void Assets_Header::setLogin () // C++Port
{
  /* 
    $request = $_SERVER['REQUEST_URI'];
    $this->view->view->request = $request; Update to this->request.
   */
}


// Display the passage navigator.
void Assets_Header::setNavigator () // C++Port
{
  /* 
    $this->displayNavigator = true;
   */
}


// Display the user's basic stylesheet.css.
void Assets_Header::setStylesheet () // C++Port
{
  /* 
    $database_config_user = Database_Config_User::getInstance ();
    $stylesheet = $database_config_user->getStylesheet ();
    $this->includedStylesheet = $stylesheet;
   */
}


// Display the user's editor stylesheet.css.
void Assets_Header::setEditorStylesheet ()
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  string stylesheet = request->database_config_user()->getStylesheet ();
  includedEditorStylesheet = stylesheet;
}


void Assets_Header::setSearchQuery (string query) // C++Port
{
  query = ""; // Temporal.
  /* 
    $this->view->view->search_query = $query;
   */
}


// Whether to display the topbar.
bool Assets_Header::displayTopbar () // C++Port
{
  /*
    // If the topbar is in the query, it means: don't display it.
    if (isset ($_GET ['topbar'])) {
      unset ($_GET ['topbar']);
      return false;      
    }
*/
  // Display the topbar.
  return true;
}


// Sets the page to refresh after "seconds".
void Assets_Header::refresh (int seconds)
{
  string headline = "<META HTTP-EQUIV=\"refresh\" CONTENT=\"" + convert_to_string (seconds) + "\">";
  headLines.push_back (headline);
}


// Runs the header.
string Assets_Header::run ()
{
  string page;
  
  // Include the Bibledit version number in the stylesheet URL to refresh the browser's cache after a Bibledit upgrade.
  view->set_variable("VERSION", VERSION);

  if (includeJQueryUI) {
    view->enable_zone ("include_jquery_ui");  // C++Port
    view->set_variable ("include_jquery_ui_subset", JQueryUISubset);
  }

  string headlines;
  for (auto & headline : headLines) {
    if (!headlines.empty ()) headlines.append ("\n");
    headlines.append (headline);
  }
  view->set_variable ("head_lines", headlines);

  if (displayTopbar ()) {
    view->enable_zone ("display_topbar");
    Menu_Main menu_main = Menu_Main (webserver_request);
    view->set_variable ("mainmenu", menu_main.create ());
    view->enable_zone ("user_full");
    Menu_User menu_user = Menu_User (webserver_request);
    view->set_variable ("usermenu", menu_user.create (loginrequest));
    if (((Webserver_Request *) webserver_request)->session_logic ()->currentLevel () >= 2) {
      view->enable_zone ("display_search");
      view->set_variable ("search", gettext ("Search"));
      view->set_variable ("searching", gettext ("Searching"));
    }
  /*
    $this->view->view->display_navigator = $this->displayNavigator; // C++Port
    if ($this->view->view->display_navigator) {
      $database_config_user = Database_Config_User::getInstance ();
      $bible = access_bible_clamp ($database_config_user->getBible ());
      $this->view->view->navigationHtml = Navigation_Passage::getContainer ();
      $this->view->view->navigationCode = Navigation_Passage::code ($bible, true);
    }
    $this->view->view->included_stylesheet = $this->includedStylesheet;
  */
    if (!includedEditorStylesheet.empty ()) {
      view->enable_zone ("include_editor_stylesheet");
      view->set_variable ("included_editor_stylesheet", includedEditorStylesheet);
    }
  }
  page += view->render("assets", "xhtml_start");
  page += view->render("assets", "header");

  return page;
}
