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


// Deals with the passage focus stuff.
class Ipc_Focus
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance()
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Ipc_Focus();
    }
    return self::$instance;
  }


  // Sets the focus.
  function set ($book, $chapter, $verse)
  {
    $session_logic = Session_Logic::getInstance ();
    $user = $session_logic->currentUser ();
    $database_ipc = Database_Ipc::getInstance();
    $database_ipc->storeMessage ($user, "", "focus", "$book.$chapter.$verse");
  }


  // Gets the focused book.
  function getBook ()
  {
    $database_ipc = Database_Ipc::getInstance();
    $message = $database_ipc->getFocus ();
    $message = explode (".", $message);
    return $message[0];
  }


  // Gets the focused chapter.
  function getChapter ()
  {
    $database_ipc = Database_Ipc::getInstance();
    $message = $database_ipc->getFocus ();
    $message = explode (".", $message);
    return $message[1];
  }
  
  
  // Gets the focused verse.
  function getVerse ()
  {
    $database_ipc = Database_Ipc::getInstance();
    $message = $database_ipc->getFocus ();
    $message = explode (".", $message);
    return $message[2];
  }


}


?>
