<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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
  private $includeJQuery = false;
  private $includeJQueryUI = false;
  private $JQueryUISubset = "";
  private $headLines = array ();

  public function __construct ($title) 
  {
    $this->view = new Assets_View (__FILE__);
    $this->view->view->title = $title;
  }

  public function jQueryOn ()
  {
    $this->includeJQuery = true;
  }
  
  public function jQueryUIOn ($subset = "")
  {
    $this->jQueryOn ();
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
    
  // Runs the header.
  public function run ()
  {
    $this->view->view->include_jquery = $this->includeJQuery;
    $this->view->view->include_jquery_ui = $this->includeJQueryUI;
    $this->view->view->include_jquery_ui_subset = $this->JQueryUISubset;
    $this->view->view->head_lines = $this->headLines;
    $this->view->render ("xhtml_start.php");
    $this->view->render ("header_full.php");
    
  }

}


?>
