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


  // Count running tasks.
  $runningTasksCount = 0;
  foreach ($actives as $offset => $task) {
    // Read the process list.
    // Previously it forked to start "ps", but this forking is expensive.
    // The current method reads the process list from the file system.
    // To make this work, it clears the file stat cache each time.
    $pid = $task ['pid'];
    clearstatcache ();
    if (file_exists ("/proc/$pid")) {
      // Process still running.
      $runningTasksCount++;
    } else {
      // Process has quit.
      // Log the output of the process, if there's any.
      $logfile = $task ['logfile'];
      if (file_exists ($logfile)) {
        $lines = file ($logfile);
        unlink ($logfile);
        $process = $task ['process'];
        if (count ($lines)) {
          $database_logs->log ("$process ($pid)");
          foreach ($lines as $line) {
            $database_logs->log ($line);
          }
        }
      }
      // Remove tasks from disk.
      $path = $task ['path'];
      @unlink ($path);
      // Destroy task.
      unset ($actives [$offset]);
    }
  }


  // When the maximum number of simultaneously running tasks has not yet been reached, start another task, if available.
  if ($runningTasksCount < 10) {
    clearstatcache ();
    $paths = scandir (dirname (__DIR__) . "/processes");
    $paths = Filter_Folders::cleanup ($paths);
    // Get the full paths to the files on disk.
    foreach ($paths as $offset => $path) {
      $paths [$offset] = dirname (__DIR__) . "/processes/$path";
    }
    // Find the first task which is not yet active.
    $active_paths = array ();
    foreach ($actives as $active) {
      $active_paths [] = $active ['path'];
    }
    foreach ($paths as $path) {
      if (!in_array ($path, $active_paths)) {
        // Get this process.
        $process = file_get_contents ($path);
        $process = trim ($process);
        // Log process output.
        $logfile = tempnam (sys_get_temp_dir (), "");
        // Start process.
        $command = "$process > $logfile 2> $logfile & echo $!";
        $pid = exec ($command);
        // If the process could be started, record it as active.
        $pid = trim ($pid);
        if ($pid != "") {
          $task = array ('path' => $path, 'process' => $process, 'pid' => $pid, 'logfile' => $logfile, 'time' => time ());
          $actives [] = $task;
        } else {
          // Process failed to start: Erase task from disk, and log the failure.
          unlink ($path);
          $database_logs->log ("Could not start process $process", Filter_Roles::ADMIN_LEVEL);
        }
        // Done.
        break;
      }
    }
  }


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
