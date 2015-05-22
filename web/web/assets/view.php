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


class Assets_View
{

  public $view;


  // Wrapper around Zend_View, adapted for Bibledit-Web.
  public function __construct ($calling_file_php)
  {
    $this->view = new Zend_View (array ("basePath" => dirname ($calling_file_php)));

    // Modifier for the links in the headers.
    $bibledit_root_folder = Bootstrap::getInstance ()->bibledit_root_folder;
    $file_path = dirname ($calling_file_php);
    $header_path_modifier = "";
    $iterations_count = 0;
    while (($file_path != $bibledit_root_folder) && ($iterations_count < 10)) {
      $file_path = dirname ($file_path);
      $iterations_count++;
      $header_path_modifier .= "../";
    }
    $this->view->header_path_modifier = $header_path_modifier;
  }



  // This function sets session variables, then calls the Zend_View render function.
  // Setting the session variables in Zend_View is postponed to the very last moment,
  // since these could change during the course of the calling .php file.
  public function render ($script)
  {
    $session_logic = Session_Logic::getInstance ();
    if ($session_logic->loggedIn ()) {
      $this->view->user = $session_logic->currentUser ();
      $this->view->level = $session_logic->currentLevel (true);
    }
    echo $this->view->render($script);
  }

}


?>
