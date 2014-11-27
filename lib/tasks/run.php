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


ignore_user_abort (true);
set_time_limit (0);
chdir (__DIR__);


require_once ("../bootstrap/bootstrap.php");


// Security.
Filter_Cli::assert ();


$database_logs = Database_Logs::getInstance ();


// Read the active tasks from disk.
$actives = array ();
$file = __DIR__ . "/tasks.tasks";
if (file_exists ($file)) {
  $contents = file_get_contents ($file);
  unlink ($file);
  $input = unserialize ($contents);
  if (is_array ($input)) {
    foreach ($input as $task) {
      if (!isset ($task ['path'])) continue;
      if (!isset ($task ['process'])) continue;
      if (!isset ($task ['logfile'])) continue;
      if (!isset ($task ['pid'])) continue;
      if (!isset ($task ['time'])) continue;
      $actives [] = $task;
    }
  }
  unset ($task);
  unset ($input);
  unset ($contents);
}


// Run for less than a minute stopping just before the full minute.
while ((time () % 60) < 57) {




  // Sleep for a short while to not overload the system.
  sleep (1);
}


// Kill tasks running for more than a few days.
$time = time ();
foreach ($actives as $offset => $task) {
  if ($time > ($task ['time']) + 259200) {
    $pid = $task ['pid'];
    posix_kill ($pid, SIGKILL);
    $process = $task ['process'];
    $database_logs = Database_Logs::getInstance ();
    $database_logs->log ("Killing stuck process $process with PID $pid", Filter_Roles::ADMIN_LEVEL);
    unset ($actives [$offset]);
  }
}


// Save any remaining tasks to disk.
// The next time the script runs it picks the tasks up again.
$actives = array_values ($actives);
$actives = serialize ($actives);
$path = __DIR__ . "/tasks.tasks";
file_put_contents ($path, $actives);


?>
