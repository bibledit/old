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


class SendReceive_Logic
{


  static public function queuebible ($bible)
  {
    Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/sendreceive.php", $bible));
  }


  // If $control is true, it syncs.
  // If $control is numerical, it determines from the settings whether to sync.
  static public function queuesync ($control)
  {
    // Send / receive only works in Client mode.
    if (!Filter_Client::enabled ()) return;
    
    // Deal with a numerical minute to find out whether it's time to automatically sync.
    if (is_numeric ($control)) {
      $database_config_general = Database_Config_General::getInstance ();
      $repeat = $database_config_general->getRepeatSendReceive ();
      // Sync every hour.
      if ($repeat == 1) {
        $control = $control % 60;
        if ($control == 0) $control = true;
      }
      // Sync every five minutes.
      if ($repeat == 2) {
        $control = $control % 5;
        if ($control == 0) $control = true;
      }
    }
    
    // Send and receive: It is time now, or it is manual.
    if ($control === true) {
      // Only queue the sync tasks if none are running at the moment.
      if (SendReceive_Logic::syncqueued ()) {
        $database_logs = Database_Logs::getInstance ();
        $database_logs->log ("Not scheduling sync tasks, because the previous ones have not yet finished");
      } else {
        Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ .  "/sendnotes.php"));
        Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ .  "/sendbibles.php"));
        Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ .  "/sendsettings.php"));
        Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ .  "/externalresources.php"));
        Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ .  "/usfmresources.php"));
      }
    }
  }


  // Checks whether any of the sync tasks has been queued.
  // Returns the result as a boolean.
  public static function syncqueued ()
  {
    if (Tasks_Logic::queued ("sendnotes.php")) return true;
    if (Tasks_Logic::queued ("sendbibles.php")) return true;
    if (Tasks_Logic::queued ("sendsettings.php")) return true;
    if (Tasks_Logic::queued ("syncnotes.php")) return true;
    if (Tasks_Logic::queued ("syncbibles.php")) return true;
    if (Tasks_Logic::queued ("syncsettings.php")) return true;
    if (Tasks_Logic::queued ("externalresources.php")) return true;
    if (Tasks_Logic::queued ("usfmresources.php")) return true;
    return false;
  }
  

  static public function queueAll ($now)
  {
    $database_config_bible = Database_Config_Bible::getInstance();
    $database_bibles = Database_Bibles::getInstance();
    $database_config_general = Database_Config_General::getInstance ();

    $bibles = $database_bibles->getBibles ();
    foreach ($bibles as $bible) {
      if ($database_config_bible->getRemoteRepositoryUrl ($bible) != "") {
        if ($database_config_bible->getRepeatSendReceive ($bible) || $now) {
          self::queuebible ($bible);
        }
      }
    }
  }


}


?>
