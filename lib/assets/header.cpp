/*
Copyright (©) 2003-2014 Teus Benschop.

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
#include <cstdlib>
#include <iostream>
#include <filter/url.h>
#include <config/globals.h>
#include <config.h>
#include <menu/main.h>


using namespace std;


Assets_Header::Assets_Header (string title)
{
  includeJQueryUI = false;
  displayNavigator = false;
  includedStylesheet = false;
  includedEditorStylesheet = false;
  view = new Assets_View (__FILE__);
  view->set_variable ("title", title);
}


Assets_Header::~Assets_Header ()
{
  delete view;
}


void Assets_Header::jQueryUIOn (string subset)
{
  subset = ""; // Temporal.
  /* C++Port
    $this->includeJQueryUI = true;
    // Setting a subset makes a big difference in the size of the JavaScript
    // the page has to load, and therefore it makes a big difference in the
    // load time of the page.
    // Subsets can be created on the download builder at http://jqueryui.com.
    if ($subset != "") {
      $this->JQueryUISubset = $subset . ".";
    }
   */
}


// Adds an extra line in the <head>.
void Assets_Header::addHeadLine (string line)
{
  line = ""; // Temporal.
  /* C++Port
    $this->headLines [] = $line;
   */
}


// Adds an 'onload' statement to the <body> html tag.
// Variable "code" could be, e.g.: onload="document.form.user.focus();"
void Assets_Header::setBodyOnload (string code)
{
  code = ""; // Temporal.
  /* C++Port
    $this->view->view->onLoadStatement = $code;
   */
}


// Adds logic to the header allowing to redirect the user to the original
// URL after successful login.
void Assets_Header::setLogin ()
{
  /* C++Port
    $request = $_SERVER['REQUEST_URI'];
    $this->view->view->request = $request;
   */
}


// Display the passage navigator.
void Assets_Header::setNavigator ()
{
  /* C++Port
    $this->displayNavigator = true;
   */
}


// Display the user's basic stylesheet.css.
void Assets_Header::setStylesheet ()
{
  /* C++Port
    $database_config_user = Database_Config_User::getInstance ();
    $stylesheet = $database_config_user->getStylesheet ();
    $this->includedStylesheet = $stylesheet;
   */
}


// Display the user's editor stylesheet.css.
void Assets_Header::setEditorStylesheet ()
{
  /* C++Port
    $database_config_user = Database_Config_User::getInstance ();
    $stylesheet = $database_config_user->getStylesheet ();
    $this->includedEditorStylesheet = $stylesheet;
   */
}


void Assets_Header::setSearchQuery (string query)
{
  query = ""; // Temporal.
  /* C++Port
    $this->view->view->search_query = $query;
   */
}


// Whether to display the topbar.
bool Assets_Header::displayTopbar ()
{
  /* C++Port
    // If the topbar is in the query, it means: don't display it.
    if (isset ($_GET ['topbar'])) {
      unset ($_GET ['topbar']);
      return false;      
    }
*/
  // Display the topbar.
  return true;
}


// Runs the header.
string Assets_Header::run ()
{
  string page;
  
  // Include the Bibledit version number in the stylesheet URL to refresh the browser's cache after a Bibledit upgrade.
  view->set_variable ("VERSION", VERSION);

  /* C++Port
    $this->view->view->include_jquery_ui = $this->includeJQueryUI;
    $this->view->view->include_jquery_ui_subset = $this->JQueryUISubset;
    $this->view->view->head_lines = $this->headLines;
  */

  if (displayTopbar ()) { // Todo working here.
    view->enable_zone ("display_topbar");
    Menu_Main menu_main = Menu_Main (); // Todo port main menu to C++
    view->set_variable ("mainmenu", menu_main.create ());
    view->enable_zone ("display_user"); // Todo implement and test it.
    /*
    $menu_user = new Menu_User (); // Todo port user menu to C++
    $this->view->view->usermenu = $menu_user->create ();
    */
  /* C++Port
    $this->view->view->display_navigator = $this->displayNavigator;
    if ($this->view->view->display_navigator) {
      $database_config_user = Database_Config_User::getInstance ();
      $bible = Access_Bible::clamp ($database_config_user->getBible ());
      $this->view->view->navigationHtml = Navigation_Passage::getContainer ();
      $this->view->view->navigationCode = Navigation_Passage::code ($bible, true);
    }
    $this->view->view->included_stylesheet = $this->includedStylesheet;
    $this->view->view->included_editor_stylesheet = $this->includedEditorStylesheet;
  */
  }
  page += view->render ("xhtml_start.html");
  page += view->render ("header.html");

  return page;
}
