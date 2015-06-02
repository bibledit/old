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


#include <sendreceive/logic.h>
#include <filter/url.h>
#include <tasks/logic.h>
#include <config/logic.h>
#include <database/config/general.h>
#include <database/config/bible.h>
#include <database/logs.h>
#include <database/bibles.h>
#include <client/logic.h>


void sendreceive_queue_bible (string bible)
{
  tasks_logic_queue (SENDRECEIVEBIBLES, {bible});
}


// If $minute is negative, it syncs.
// If $minute >=0, it determines from the settings whether to sync.
void sendreceive_queue_sync (int minute)
{
  // Send / receive only works in Client mode.
  if (!client_logic_client_enabled ()) return;
  
  // Deal with a numerical minute to find out whether it's time to automatically sync.
  if (minute >= 0) {
    int repeat = Database_Config_General::getRepeatSendReceive ();
    // Sync every hour.
    if (repeat == 1) {
      minute = minute % 60;
      if (minute == 0) minute = -1;
    }
    // Sync every five minutes.
    if (repeat == 2) {
      minute = minute % 5;
      if (minute == 0) minute = -1;
    }
  }
  
  // Send and receive: It is time now, or it is manual.
  if (minute < 0) {
    // Only queue the sync tasks if none are running at the moment.
    if (sendreceive_sync_queued ()) {
      Database_Logs::log ("Some sync actions are still running");
    } else {
      tasks_logic_queue (SYNCNOTES);
      tasks_logic_queue (SYNCBIBLES);
      tasks_logic_queue (SYNCSETTINGS);
      tasks_logic_queue (SYNCEXTERNALRESOURCES);
      tasks_logic_queue (SYNCUSFMRESOURCES);
    }
  }
}


// Checks whether any of the sync tasks has been queued.
// Returns the result as a boolean.
bool sendreceive_sync_queued ()
{
  if (tasks_logic_queued (SYNCNOTES)) return true;
  if (tasks_logic_queued (SYNCBIBLES)) return true;
  if (tasks_logic_queued (SYNCSETTINGS)) return true;
  if (tasks_logic_queued (SYNCEXTERNALRESOURCES)) return true;
  if (tasks_logic_queued (SYNCUSFMRESOURCES)) return true;
  return false;
}


void sendreceive_queue_all (bool now)
{
  Database_Bibles database_bibles = Database_Bibles ();
  vector <string> bibles = database_bibles.getBibles ();
  for (auto & bible : bibles) {
    if (Database_Config_Bible::getRemoteRepositoryUrl (bible) != "") {
      if (Database_Config_Bible::getRepeatSendReceive (bible) || now) {
        sendreceive_queue_bible (bible);
      }
    }
  }
}


