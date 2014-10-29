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


class Access_User
{


  // This function returns users assigned to the logged-in user.
  static public function assignees ()
  {
    $session_logic = Session_Logic::getInstance ();
    $database_users = Database_Users::getInstance ();
    $database_bibles = Database_Bibles::getInstance ();

    $myuser = $session_logic->currentUser ();
    $mylevel = $session_logic->currentLevel ();

    // Bibles the user has write access to.
    $mybibles = array ();
    $bibles = $database_bibles->getBibles ();
    foreach ($bibles as $bible) {
      if (Access_Bible::write ($bible, $myuser)) {
        $mybibles [] = $bible;
      }
    }

    // This holds the assignees.    
    $assignees = array ();

    // Process all users.
    $users = $database_users->getUsers ();
    sort ($users);
    foreach ($users as $user) {
      // Assignees should have a level less than or equal to $mylevel.
      $level = $database_users->getUserLevel ($user);
      if ($level <= $mylevel) {
        // Assignees should have access to $mybibles or no access to any Bible.
        // The admin has all users assigned.
        $userBibles = $database_users->getBibles4User ($user);
        $biblesInCommon = array_intersect ($userBibles, $mybibles);
        if (!empty ($biblesInCommon) || empty ($userBibles) || $mylevel >= Filter_Roles::ADMIN_LEVEL) {
          $assignees [] = $user;
        }
      }
    }
    
    return $assignees;
  }


}


?>
