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


  
// Deal with the changes in the Bible made per user.
// Deal with notifications for the daily changes in the Bibles.
// This takes a few minutes on a production machine with two Bibles and changes in several chapters.
if (!$client) {
  if (($hour == 0) && ($minute == 20)) {
    Changes_Logic::start ();
  }
}


// Run the checks on the Bibles.
// This takes 15 minutes on a production machine with two Bibles.
if (!$client) {
  if (($hour == 0) && ($minute == 30)) {
    Checks_Logic::startAll ();
  }
}


// Sprint burndown.
// It runs every hour.
// The script itself determines what to do at which hour of the day or day of the week or day of the month.
if (!$client) {
  if ($minute == 5) {
    Sprint_Logic::burndown ("", false);
  }
}


// Export the Bibles to the various output formats.
// This may take an hour on a production machine.
if (($hour == 1) && ($minute == 10)) {
  Export_Logic::scheduleAll ();
}


// Email statistics to the users.
if (!$client) {
  if (($hour == 3) && ($minute == 0)) {
    tasks_logic_queue (Tasks_Logic::PHP, array (__DIR__ . "/statistics"));
  }
}




?>
