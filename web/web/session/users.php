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



require_once ("bootstrap/bootstrap.php");



class Session_Users
{

  public $usernames;
  public $ids;
  public $levels;
  public $emails;
  
  private static $instance;

  private function __construct() {
    // Get the level (= role) of the person that works on this page.
    // This is to ensure that no users of a higher level can be listed.
    // E.g. if the team manager goes to this page to manage users, the site admin will not be displayed.
    $session_logic = Session_Logic::getInstance ();
    $mylevel = $session_logic->currentLevel ();

    $server = Database_Instance::getInstance ();
    $query = "SELECT * FROM users ORDER BY level DESC, username ASC;";
    $result = $server->runQuery ($query);
    while ($row = $result->fetch_assoc()) {
      $level = $row["level"];
      if ($level <= $mylevel) {
        $this->usernames[] = $row["username"];
        $this->ids[]       = $row["id"];
        $this->levels[]    = $level;
        $this->emails[]    = $row["email"];
      }
    }
  } 

  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Session_Users();
    }
    return self::$instance;
  }
}



?>
