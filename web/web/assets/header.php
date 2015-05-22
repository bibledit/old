<?php
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

class Assets_Header
{
  private static $instance;
  private $view;
  private $includeJQueryUI = false;
  private $JQueryUISubset = "";
  private $headLines = array ();
  private $displayNavigator = false;
  private $includedStylesheet = false;
  private $includedEditorStylesheet = false;

  public function __construct ($title)
  {
    $this->view = new Assets_View (__FILE__);
    $this->view->view->title = $title;
  }

  public function jQueryUIOn ($subset = "")
  {
    $this->includeJQueryUI = true;
    // Setting a subset makes a big difference in the size of the JavaScript
    // the page has to load, and therefore it makes a big difference in the
    // load time of the page.
    // Subsets can be created on the download builder at http://jqueryui.com.
    if ($subset != "") {
      $this->JQueryUISubset = $subset . ".";
    }
  }

  // Adds an extra line in the <head>.
  public function addHeadLine ($line)
  {
    $this->headLines [] = $line;
  }

  // Adds an 'onload' statement to the <body> html tag.
  // $code could be, e.g.: onload="document.form.user.focus();"
  public function setBodyOnload ($code)
  {
    $this->view->view->onLoadStatement = $code;
  }

  // Adds logic to the header allowing to redirect the user to the original
  // URL after successful login.
  public function setLogin ()
  {
    $request = $_SERVER['REQUEST_URI'];
    $this->view->view->request = $request;
  }


  // Display the passage navigator.
  public function setNavigator ()
  {
    $this->displayNavigator = true;
  }

  // Display the user's basic stylesheet.css.
  public function setStylesheet ()
  {
    $database_config_user = Database_Config_User::getInstance ();
    $stylesheet = $database_config_user->getStylesheet ();
    $this->includedStylesheet = $stylesheet;
  }

  // Display the user's editor stylesheet.css.
  public function setEditorStylesheet ()
  {
    $database_config_user = Database_Config_User::getInstance ();
    $stylesheet = $database_config_user->getStylesheet ();
    $this->includedEditorStylesheet = $stylesheet;
  }

  public function setSearchQuery ($query)
  {
    $this->view->view->search_query = $query;
  }
  
  // Runs the header.
  public function run ()
  {
    $this->view->view->include_jquery_ui = $this->includeJQueryUI;
    $this->view->view->include_jquery_ui_subset = $this->JQueryUISubset;
    $this->view->view->head_lines = $this->headLines;
    $this->view->view->display_topbar = $this->displayTopbar ();
    if ($this->view->view->display_topbar) {
      $menu_main = new Menu_Main ();
      $this->view->view->mainmenu = $menu_main->create ();
      $menu_user = new Menu_User ();
      $this->view->view->usermenu = $menu_user->create ();
    }
    $this->view->view->display_navigator = $this->displayNavigator;
    if ($this->view->view->display_navigator) {
      $database_config_user = Database_Config_User::getInstance ();
      $bible = Access_Bible::clamp ($database_config_user->getBible ());
      $this->view->view->navigationHtml = Navigation_Passage::getContainer ();
      $this->view->view->navigationCode = Navigation_Passage::code ($bible, true);
    }
    $this->view->view->included_stylesheet = $this->includedStylesheet;
    $this->view->view->included_editor_stylesheet = $this->includedEditorStylesheet;
    $this->view->render ("xhtml_start.php");
    $this->view->render ("header.php");
  }
  
  // Whether to display the topbar.
  private function displayTopbar ()
  {
    // If the topbar is in the query, it means: don't display it.
    if (isset ($_GET ['topbar'])) {
      unset ($_GET ['topbar']);
      return false;      
    }
    // Display the topbar.
    return true;
  }

}


?>
