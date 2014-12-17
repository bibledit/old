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


#include <timer/index.h>
#include <database/logs.h>
#include <config/globals.h>
#include <filter/string.h>
#include <tasks/logic.h>
#include <tasks/run.h>
#include <config/logic.h>


void timer_index () 
{
  bool client = config_logic_client_enabled ();
  int previous_second = -1;
  int previous_minute = -1;
  while (config_globals_running) {

    try {

      // Wait shortly.
      this_thread::sleep_for (chrono::milliseconds (100));
      
      // The current time.
      int seconds_since_epoch = filter_string_date_seconds_since_epoch ();
      int second = filter_string_date_numerical_second (seconds_since_epoch);
      int minute = filter_string_date_numerical_minute (seconds_since_epoch);
      int hour = filter_string_date_numerical_hour (seconds_since_epoch);
      
      // Run once per second.
      if (second == previous_second) continue;
      previous_second = second;

      // At midnight, for five minutes, do nothing to allow time for external backup
      // without corrupting the SQLite databases due to simultaneous access by
      // Bibledit-Web and the backup program.
      if ((hour == 0) && (minute <= 5)) continue;

      // Every second: Deal with queued and/or active tasks.
      tasks_run_check ();
      
      // Run the part below once per minute.
      if (minute == previous_minute) continue;
      previous_minute = minute;
      
      // Every minute send out queued email, except in client mode.
      if (!client) {
        tasks_logic_queue (SENDEMAIL);
      }

      // Check for new mail every five minutes.
      // Do not check more often with gmail else the account may be shut down.
      if (!client && ((minute % 5) == 0)) tasks_logic_queue (RECEIVEEMAIL);

      // At the sixth minute after midnight, after the backup silence, and any hour after, rotate the journal.
      if (minute == 6) tasks_logic_queue (ROTATEJOURNAL);
      
    } catch (exception & e) {
      Database_Logs::log (e.what ());
    } catch (exception * e) {
      Database_Logs::log (e->what ());
    } catch (...) {
      Database_Logs::log ("A general internal error occurred in the timers");
    }

  }
}

