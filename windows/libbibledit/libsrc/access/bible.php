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


require_once ("bootstrap/bootstrap.php");


class Access_Bible
{


  // Returns true if the $user has read access to the $bible.
  // If no $user is given, it takes the currently logged-in user.
  static public function read ($bible, $user = "")
  {
    // Client: User has access to all Bibles.
    if (Filter_Client::enabled ()) {
      return true;
    }
    if ($user == "") {
      $session_logic = Session_Logic::getInstance ();
      $user = $session_logic->currentUser ();
    }
    $database_users = Database_Users::getInstance ();
    if ($database_users->hasAccess2Bible ($user, $bible)) {
      return true;
    }
    $database_config_bible = Database_Config_Bible::getInstance ();
    if ($database_config_bible->getViewableByAllUsers ($bible)) {
      return true;
    }
    return false;
  }


  // Returns true if the $user has write access to the $bible.
  // If no $user is given, it takes the currently logged-in user.
  static public function write ($bible, $user = "")
  {
    // Client: User has access to all Bibles.
    if (Filter_Client::enabled ()) {
      return true;
    }
    if ($user == "") {
      $session_logic = Session_Logic::getInstance ();
      $user = $session_logic->currentUser ();
    }
    $database_users = Database_Users::getInstance ();
    if (!$database_users->hasAccess2Bible ($user, $bible)) {
      return false;
    }
    $readonly = $database_users->hasReadOnlyAccess2Bible ($user, $bible);
    return !$readonly;
  }


  // Returns an array of Bibles the $user has read access to.
  // If no $user is given, it takes the currently logged-in user.
  static public function bibles ($user = "")
  {
    $database_bibles = Database_Bibles::getInstance ();
    $bibles = $database_bibles->getBibles ();
    foreach ($bibles as $offset => $bible) {
      if (!self::read ($bible, $user)) {
        unset ($bibles [$offset]);
      }
    }
    $bibles = array_values ($bibles);
    return $bibles;
  }
  
  
  // This function clamps $bible.
  // It returns $bible if the currently logged-in user has access to it.
  // Else it returns another accessible $bible or "".
  static public function clamp ($bible)
  {
    if (!self::read ($bible)) {
      $bible = "";
      $bibles = self::bibles ();
      if (count ($bibles)) {
        $bible = $bibles [0];
      }
      $database_config_user = Database_Config_User::getInstance ();
      $database_config_user->setBible ($bible);
    }
    return $bible;
  }


}


?>
