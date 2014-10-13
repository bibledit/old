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


// Deals with the consultation notes stuff.


class Ipc_Notes
{
  private static $instance;
  private function __construct() {
  }
  public static function getInstance()
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Ipc_Notes ();
    }
    return self::$instance;
  }


  public function open ($identifier)
  {
    $session_logic = Session_Logic::getInstance ();
    $user = $session_logic->currentUser ();
    $database_ipc = Database_Ipc::getInstance();
    $database_ipc->storeMessage ($user, "", "opennote", $identifier);
  }


  public function get ()
  {
    $session_logic = Session_Logic::getInstance ();
    $user = $session_logic->currentUser ();
    $database_ipc = Database_Ipc::getInstance();
    $data = $database_ipc->getNote ($user);
    if ($data == NULL) return NULL;
    $identifier = $data ['message'];
    return $identifier;
  }


  public function delete ()
  {
    $session_logic = Session_Logic::getInstance ();
    $user = $session_logic->currentUser ();
    $database_ipc = Database_Ipc::getInstance();
    $data = $database_ipc->getNote ($user);
    $counter = 0;
    while ($data && ($counter < 100)) {
      $id = $data ['id'];
      $database_ipc->deleteMessage ($id);
      $data = $database_ipc->getNote ($user);
      $counter++;
    }
  }


  // If a parameter is passed, it sets the alive status of the notes editor.
  // If no parameter is passed, it returns the alive status.
  public function alive ($alive = false)
  {
    $session_logic = Session_Logic::getInstance ();
    $user = $session_logic->currentUser ();
    $database_ipc = Database_Ipc::getInstance ();
    if (func_num_args () == 0) {
      $alive = $database_ipc->getNotesAlive ();
      return $alive;
    } else {
      $alive = Filter_Bool::int ($alive);
      $database_ipc->storeMessage ($user, "", "notesalive", $alive);
    }
  }


}


?>
