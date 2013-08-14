<?php
/*
** Copyright (©) 2003-2013 Teus Benschop.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**  
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


$receive_send = "send/receive:";
include ("../session/levels.php");


require_once ("../bootstrap/bootstrap.php");
$database_logs = Database_Logs::getInstance ();
$database_logs->log ("$receive_send Starting to send and receive Bibles", TRANSLATOR_LEVEL);


// Security: The script runs from the cli SAPI only.
if (php_sapi_name () != "cli") {
  $database_logs->log ("$receive_send Fatal: This only runs through the cli Server API");
  die;
}


// Newer git versions would read their config from /root and then generate a fatal error.
// Fix that here by setting the $HOME variable to something git can read.
putenv ("HOME=" . dirname (__FILE__));


$database_logs->log ("$receive_send Processing any data left over from previous actions");
Filter_Git::filedata2database ();


$database_bibles = Database_Bibles::getInstance();
$database_config_user = Database_Config_User::getInstance();
$database_logs = Database_Logs::getInstance();
$database_git = Database_Git::getInstance();
$bibles = $database_bibles->getBibles();
// Add the Consultation Notes as well.
$bibles [] = "consultationnotes";
foreach ($bibles as $bible) {
  $remote_repository_url = $database_config_user->getRemoteRepositoryUrl ($bible);
  if ($remote_repository_url != "") {


    if ($bible == "consultationnotes") {
      //$database_logs->log ("$receive_send Consultation Notes");
    } else {
      $database_logs->log ("$receive_send Bible" . ": " . $bible, TRANSLATOR_LEVEL);
    }
    $database_logs->log ("$receive_send Remote repository URL: " . $remote_repository_url, ADMIN_LEVEL);


    // The git directory for this object.
    $directory = Filter_Git::git_directory ($bible);
    $database_logs->log ("$receive_send Git repository directory: " . $directory);
    $shelldirectory = escapeshellarg ($directory);

    
    // Continue to the next Bible if the repository directory is not there.
    if (!is_dir ($directory)) {
      $database_logs->log ("$receive_send Cannot send and receive the data because the git repository was not found in the filesystem.");
      continue;
    }


    // Sync the reposityr with the database. 
    if ($bible == "consultationnotes") {
      //$database_logs->log ("$receive_send Transferring notes to file.");
      //$success = Filter_Git::notesDatabase2filedata ($directory);
    } else {
      $database_logs->log ("$receive_send Syncing Bible text to the repository.", TRANSLATOR_LEVEL);
      $success = Filter_Git::syncBible2Git ($bible, $directory);
    }
    // If the above does not succeed, then there is a serious problem. 
    // It means that the repository does no longer reflect the data in the database.
    // This can lead to data loss. Through the repository this would then be propagated to other systems.
    // The best thing to do is to remove the .git directory altogether, 
    // so that it cannot propagate damaged data.
    // In that case Send / Receive would work again after it has been set up from scratch.
    if (!$success) {
      Filter_Rmdir::rmdir ("$directory/.git");
    }


    // Commit the new data to the repository.
    if ($success) {
      $command = "cd $shelldirectory; git add . 2>&1";
      $database_logs->log ("$receive_send $command");
      unset ($result);
      exec ($command, $result, $exit_code);
      if ($exit_code != 0) $success = false;
      foreach ($result as $line) {
        if ($line) $database_logs->log ("$receive_send $line");
      }
      $message = "Exit code $exit_code";
      $database_logs->log ("$receive_send $message");
    }


    if ($success) {
      $command = "cd $shelldirectory; git status 2>&1";
      $database_logs->log ("$receive_send $command");
      unset ($result);
      exec ($command, $result, $exit_code);
      if ($exit_code != 0) $success = false;
      foreach ($result as $line) {
        if ($line) $database_logs->log ("$receive_send $line", TRANSLATOR_LEVEL);
      }
      $message = "Exit code $exit_code";
      $database_logs->log ("$receive_send $message");
    }  


    if ($success) {
      $command = "cd $shelldirectory; git commit -a -m sync 2>&1";
      $database_logs->log ("$receive_send $command");
      unset ($result);
      exec ($command, $result, $exit_code);
      if (($exit_code != 0) && ($exit_code != 1)) $success = false;
      foreach ($result as $line) {
        if ($line) $database_logs->log ("$receive_send $line");
      }
      $message = "Exit code $exit_code";
      $database_logs->log ("$receive_send $message");
    }  


    // Pull changes from the remote repository.
    if ($success) {
      $command = "cd $shelldirectory; git pull 2>&1";
      $database_logs->log ("$receive_send $command");
      unset ($result);
      exec ($command, $result, $exit_code);
      if ($exit_code != 0) $success = false;
      foreach ($result as $line) {
        // Leave out messages like:
        //   Could not create directory '/var/www/.ssh'.
        //   Failed to add the host to the list of known hosts (/var/www/.ssh/known_hosts).
        // Such messages confuse the user, and are not real errors.
        if (strstr ($line, "/.ssh") != false) continue;
        $database_logs->log ("$receive_send $line", TRANSLATOR_LEVEL);
        $database_git->insert ($directory, $line);
        if (strstr ($line, "CONFLICT") !== false) {
          if ($line) $database_logs->log ("$receive_send $line", TRANSLATOR_LEVEL);
          $message = "A conflict was found in the above book and chapter or consultation note. Please resolve this conflict manually. Open the chapter in the editor in USFM view, and select which of the two conflicting lines of text should be retained, and remove the other line, and the conflict markup. After that it is recommended to send and receive the Bibles again. This will remove the conflict from the repository.";
          $database_logs->log ("$receive_send $message", TRANSLATOR_LEVEL);
          // Inform administrator about the conflict.
          $database_mail = Database_Mail::getInstance ();
          $database_users = Database_Users::getInstance ();
          $adminusername = $database_users->getAdministrator ();
          $subject = "Send/Receive" . ' ' . $line;
          $body = "<p>$line</p>";
          $body .= "<p>$message</p>";
          $database_mail->send ($adminusername, $subject, $body);
        }
      }
      $message = "Exit code $exit_code";
      $database_logs->log ("$receive_send $message");
    }  


    // Push our changes into the remote repository.
    if ($success) {
      $command = "cd $shelldirectory; git push 2>&1";
      $database_logs->log ("$receive_send $command");
      unset ($result);
      exec ($command, $result, $exit_code);
      if ($exit_code != 0) $success = false;
      foreach ($result as $line) {
        if (strstr ($line, "/.ssh") != false) continue;
        if ($line) $database_logs->log ("$receive_send $line", TRANSLATOR_LEVEL);
      }
      $message = "Exit code $exit_code";
      $database_logs->log ("$receive_send $message");
    }  


    if ($success) {
      $database_logs->log ("$receive_send Storing the changes in the database.");
      Filter_Git::filedata2database ();
    }


    // Done.
    if (!$success) {
      $database_logs->log ("$receive_send There was a failure", TRANSLATOR_LEVEL);
    }
    if ($bible == "consultationnotes") {
      //$database_logs->log ("$receive_send The Consultation Notes have been done.");
    } else {
      $database_logs->log ("$receive_send This Bible has been done.", TRANSLATOR_LEVEL);
    }
  }
}


$database_logs->log ("$receive_send Ready", TRANSLATOR_LEVEL);


?>
