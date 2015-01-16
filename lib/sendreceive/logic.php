<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

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


require_once ("bootstrap/bootstrap");


class SendReceive_Logic
{


  static public function queuebible ($bible)
  {
    tasks_logic_queue (Tasks_Logic::PHP, array (__DIR__ . "/sendreceive", $bible));
  }


  // If $control is true, it syncs.
  // If $control is numerical, it determines from the settings whether to sync.
  static public function queuesync ($control)
  {
    // Send / receive only works in Client mode.
    if (!config_logic_client_enabled ()) return;
    
    // Deal with a numerical minute to find out whether it's time to automatically sync.
    if (is_numeric ($control)) {
      $database_config_general = Database_Config_General::getInstance ();
      $repeat = Database_Config_General::getRepeatSendReceive ();
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
        Database_Logs::log ("Not scheduling sync tasks, because the previous ones have not yet finished");
      } else {
        tasks_logic_queue (Tasks_Logic::PHP, array (__DIR__ .  "/sendnotes"));
        tasks_logic_queue (Tasks_Logic::PHP, array (__DIR__ .  "/sendbibles"));
        tasks_logic_queue (Tasks_Logic::PHP, array (__DIR__ .  "/sendsettings"));
        tasks_logic_queue (Tasks_Logic::PHP, array (__DIR__ .  "/externalresources"));
        tasks_logic_queue (Tasks_Logic::PHP, array (__DIR__ .  "/usfmresources"));
      }
    }
  }


  // Checks whether any of the sync tasks has been queued.
  // Returns the result as a boolean.
  public static function syncqueued ()
  {
    if (tasks_logic_queued ("sendnotes")) return true;
    if (tasks_logic_queued ("sendbibles")) return true;
    if (tasks_logic_queued ("sendsettings")) return true;
    if (tasks_logic_queued ("syncnotes")) return true;
    if (tasks_logic_queued ("syncbibles")) return true;
    if (tasks_logic_queued ("syncsettings")) return true;
    if (tasks_logic_queued ("externalresources")) return true;
    if (tasks_logic_queued ("usfmresources")) return true;
    return false;
  }
  

  static public function queueAll ($now)
  {
    $database_config_bible = Database_Config_Bible::getInstance();
    $database_bibles = Database_Bibles::getInstance();
    $database_config_general = Database_Config_General::getInstance ();

    $bibles = request->database_bibles()->getBibles ();
    for ($bibles as $bible) {
      if (Database_Config_Bible::getRemoteRepositoryUrl ($bible) != "") {
        if (Database_Config_Bible::getRepeatSendReceive ($bible) || $now) {
          self::queuebible ($bible);
        }
      }
    }
  }


}


?>
