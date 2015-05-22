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


/*

The purpose of this class is to partly simulate a Trash.
This is how it works:
It gets information from the object to delete.
It converts this information to plain text.
It adds information as to who did what.
It logs the event.
The user can consult the log later, and partially reconstruct the original item.

*/

class Trash_Handler
{

  // Singleton instance.
  private static $instance;
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Trash_Handler ();
    }
    return self::$instance;
  }


  public function changeNotification ($id)
  {
    $database_modifications = Database_Modifications::getInstance ();
    $database_logs = Database_Logs::getInstance ();
    $passage = $database_modifications->getNotificationPassage ($id);
    $passageText = Filter_Books::passagesDisplayInline (array (array ($passage['book'], $passage['chapter'], $passage['verse'])));
    $modification = $database_modifications->getNotificationModification ($id);
    $session_logic = Session_Logic::getInstance ();
    $username = $session_logic->currentUser ();
    $database_logs->log ("$username removed change notification $passageText: $modification");
  }


  public function consultationNote ($id)
  {
    $database_notes = Database_Notes::getInstance ();
    $database_logs = Database_Logs::getInstance ();
    $passage = $database_notes->getPassages ($id);
    $passageText = Filter_Books::passagesDisplayInline ($passage);
    $summary = $database_notes->getSummary ($id);
    $contents = $database_notes->getContents ($id);
    $contents = Filter_Html::html2text ($contents);
    $session_logic = Session_Logic::getInstance ();
    $username = $session_logic->currentUser ();
    $database_logs->log ("$username deleted / marked for deletion consultation note $passageText | $summary | $contents");
  }



}


?>
