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


require_once ("../bootstrap/bootstrap.php");


// Security: The script runs from the cli SAPI only.
Filter_Cli::assert ();


// The Bible passed to the script.
$bible = Filter_Cli::argument (@$argv, 1);


$database_logs = Database_Logs::getInstance ();
$database_bibles = Database_Bibles::getInstance();
$database_config_bible = Database_Config_Bible::getInstance();


$send_receive = "send/receive:";


// Newer git versions would read their config from /root and then generate a fatal error.
// Fix that here by setting the $HOME variable to something git can read.
putenv ("HOME=" . dirname (__FILE__));


$database_logs->log ("Send/receive Bible" . " " . $bible, Filter_Roles::TRANSLATOR_LEVEL);


// The git directory for this object.
$directory = Filter_Git::git_directory ($bible);
$shelldirectory = escapeshellarg ($directory);


// Check that the repository directory is there.
if (!is_dir ($directory)) {
  $database_logs->log ("$send_receive Cannot send and receive because the git repository was not found in the filesystem.");
  die;
}


// Sync the repository with the database.
$success = Filter_Git::syncBible2Git ($bible, $directory);


// If the above does not succeed, then there is a serious problem.
// It means that the git repository no longer reflects the data in the database.
// This may lead to data corruption.
// Through the repository this would then be propagated to other the systems.
// The best thing to do is to remove the .git directory altogether so that it cannot propagate corrupt data.
if (!$success) {
  Filter_Rmdir::rmdir ("$directory/.git");
}


// Commit the new data to the repository.
// Log unusual things.
if ($success) {
  $logs = array ();
  $command = "cd $shelldirectory; git add . 2>&1";
  $logs [] = "$send_receive $command";
  unset ($result);
  exec ($command, $result, $exit_code);
  if ($exit_code != 0) $success = false;
  foreach ($result as $line) {
    $logs [] = "$send_receive $line";
  }
  if (!$success || (count ($result) > 0)) {
    foreach ($logs as $log) $database_logs->log ($log, Filter_Roles::TRANSLATOR_LEVEL);
  }
}


if ($success) {
  $logs = array ();
  $command = "cd $shelldirectory; git status 2>&1";
  $logs [] = "$send_receive $command";
  unset ($result);
  exec ($command, $result, $exit_code);
  if ($exit_code != 0) $success = false;
  foreach ($result as $line) {
    if ($line) $logs [] = "$send_receive $line";
  }
  if (!$success || (count ($result) > 4)) {
    foreach ($logs as $log) $database_logs->log ($log, Filter_Roles::TRANSLATOR_LEVEL);
  }
}


if ($success) {
  $logs = array ();
  $command = "cd $shelldirectory; git commit -a -m ChangesCommittedFromBibleditWeb 2>&1";
  $logs [] = "$send_receive $command";
  unset ($result);
  exec ($command, $result, $exit_code);
  if (($exit_code != 0) && ($exit_code != 1)) $success = false;
  foreach ($result as $line) {
    if ($line) $logs [] = "$send_receive $line";
  }
  if (!$success || (count ($result) > 4)) {
    foreach ($logs as $log) $database_logs->log ($log, Filter_Roles::TRANSLATOR_LEVEL);
  }
}


// Pull changes from the remote repository. 
// Record the pull messages to see which chapter has changes.
$pull_messages = array ();
if ($success) {
  $logs = array ();
  $conflict = false;
  $command = "cd $shelldirectory; git pull 2>&1";
  $logs [] = "$send_receive $command";
  unset ($result);
  exec ($command, $result, $exit_code);
  foreach ($result as $line) {
    $logs [] = "$send_receive $line";
    if (strstr ($line, "CONFLICT") !== false) $conflict = true;
    $pull_messages [] = $line;
  }
  if ($conflict) {
    $message = "Bibledit-Web will merge the conflicts.";
    $logs [] = "$send_receive $message";
    Filter_Conflict::run ($directory);
  }
  if (!$success || $conflict || (count ($result) > 1)) {
    foreach ($logs as $log) $database_logs->log ($log, Filter_Roles::TRANSLATOR_LEVEL);
  }
}


// Push our changes into the remote repository.
if ($success) {
  $logs = array ();
  $command = "cd $shelldirectory; git push 2>&1";
  $logs [] = "$send_receive $command";
  unset ($result);
  exec ($command, $result, $exit_code);
  if ($exit_code != 0) $success = false;
  foreach ($result as $line) {
    if (strstr ($line, "/.ssh") != false) continue;
    if ($line) $logs [] = "$send_receive $line";
  }
  if (!$success || (count ($result) > 1)) {
    foreach ($logs as $log) $database_logs->log ($log, Filter_Roles::TRANSLATOR_LEVEL);
  }
}


// Record the changes from the collaborators into the Bible database.
if ($success) {
  foreach ($pull_messages as $pull_message) {
    $book_chapter = Filter_Git::getPullPassage ($pull_message);
    if ($book_chapter) {
      $book = $book_chapter ['book'];
      $chapter = $book_chapter ['chapter'];
      Filter_Git::syncGitChapter2Bible ($directory, $bible, $book, $chapter);
    }
  }
}


// Done.
if (!$success) {
  $database_logs->log ("Failure during sending and receiving", Filter_Roles::TRANSLATOR_LEVEL);
}
$database_logs->log ("Ready sending and receiving Bible" . " " . $bible, Filter_Roles::TRANSLATOR_LEVEL);


?>
