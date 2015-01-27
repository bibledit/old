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


#include <sendreceive/sendreceive.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <filter/git.h>
#include <database/config/general.h>
#include <database/config/bible.h>
#include <database/logs.h>
#include <webserver/request.h>


void sendreceive_sendreceive (string bible)
{
  // Check on Bible.
  if (bible.empty ()) {
    Database_Logs::log ("No Bible to send and receive", Filter_Roles::translator ());
    return;
  }
  

  // The git repository directory for this object.
  string directory = filter_git_directory (bible);
  

  // Check that the repository directory is there.
  if (!file_exists (directory)) {
    Database_Logs::log ("Cannot send and receive because the local git repository was not found.");
    return;
  }
  
  
  Database_Logs::log ("Send/receive Bible " + bible, Filter_Roles::translator ());
  Webserver_Request request;
  bool success = true;
  string error;
  
  
  // Synchronize the Bible from the database to the local git repository.
  filter_git_sync_bible_to_git (&request, bible, directory);
  

  // Commit the new data to the repository.
  // Log any errors.
  if (success) {
    success = filter_git_add_remove_all (directory, error);
    if (!success) {
      Database_Logs::log (error, Filter_Roles::translator ());
    }
  }
  
  

  
  /* Todo
  
  
  if ($success) {
    $logs = array ();
    $command = "cd $shelldirectory; git status 2>&1";
    $logs [] = "$send_receive $command";
    unset ($result);
    exec ($command, $result, $exit_code);
    if ($exit_code != 0) $success = false;
    for ($result as $line) {
      if ($line) $logs [] = "$send_receive $line";
    }
    if (!$success || (count ($result) > 4)) {
      for ($logs as $log) Database_Logs::log ($log, Filter_Roles::translator ());
    }
  }
  
  
  if ($success) {
    $logs = array ();
    $command = "cd $shelldirectory; git commit -a -m ChangesCommittedFromBibleditWeb 2>&1";
    $logs [] = "$send_receive $command";
    unset ($result);
    exec ($command, $result, $exit_code);
    if (($exit_code != 0) && ($exit_code != 1)) $success = false;
    for ($result as $line) {
      if ($line) $logs [] = "$send_receive $line";
    }
    if (!$success || (count ($result) > 4)) {
      for ($logs as $log) Database_Logs::log ($log, Filter_Roles::translator ());
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
    for ($result as $line) {
      $logs [] = "$send_receive $line";
      if (strstr ($line, "CONFLICT") !== false) $conflict = true;
      $pull_messages [] = $line;
    }
    if ($conflict) {
      $message = "Bibledit will merge the conflicts.";
      $logs [] = "$send_receive $message";
      Filter_Conflict::run ($directory);
    }
    if (!$success || $conflict || (count ($result) > 1)) {
      for ($logs as $log) Database_Logs::log ($log, Filter_Roles::translator ());
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
    for ($result as $line) {
      if (strstr ($line, "/.ssh") != false) continue;
      if ($line) $logs [] = "$send_receive $line";
    }
    if (!$success || (count ($result) > 1)) {
      for ($logs as $log) Database_Logs::log ($log, Filter_Roles::translator ());
    }
  }
  
  
  // Record the changes from the collaborators into the Bible database.
  if ($success) {
    for ($pull_messages as $pull_message) {
      $book_chapter = filter_git_get_pull_passage ($pull_message);
      if ($book_chapter) {
        $book = $book_chapter ['book'];
        $chapter = $book_chapter ['chapter'];
        filter_git_sync_git_chapter_to_bible ($directory, bible, book, chapter);
      }
    }
  }
  
  
  // Done.
  if (!$success) {
    Database_Logs::log ("Failure during sending and receiving", Filter_Roles::translator ());
  }
  Database_Logs::log ("Ready sending and receiving Bible" . " " . $bible, Filter_Roles::translator ());
*/

}
