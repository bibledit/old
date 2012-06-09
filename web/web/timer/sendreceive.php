<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2012 Teus Benschop.
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


require_once ("../bootstrap/bootstrap.php");
$database_logs = Database_Logs::getInstance ();
$database_logs->log (gettext ("Starting to send and receive the relevant Bibles and Consultation Notes."));


// Security: The script runs from the cli SAPI only.
if (php_sapi_name () != "cli") {
  $database_logs->log ("Fatal: This only runs through the cli Server API", true);
  die;
}



$database_logs->log (gettext ("Processing any data left over from previous actions."));
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

    $database_logs->log ("**********");
    if ($bible == "consultationnotes") {
      $database_logs->log (gettext ("Consultation Notes"));
    } else {
      $database_logs->log (gettext ("Bible") . ": " . $bible);
    }
    $database_logs->log (gettext ("Remote repository URL") . ": " . $remote_repository_url);

    // The git directory for this object.
    $directory = Filter_Git::git_directory ($bible);
    $database_logs->log (gettext ("Git repository directory") . ": " . $directory);
    $shelldirectory = escapeshellarg ($directory);
    
    // Continue to the next Bible if the repository directory is not there.
    if (!is_dir ($directory)) {
      $database_logs->log (gettext ("Cannot send and receive the data because the git repository was not found in the filesystem."));
      continue;
    }

    // Set up the secure shell keys in case these are needed.    
    $secure_key_directory = Filter_Git::git_config ($remote_repository_url);

    // Temporarily store the .git directory.
    $tempdirectory = tempnam (sys_get_temp_dir(), '');
    unlink ($tempdirectory);
    mkdir ($tempdirectory);
    $success = rename ("$directory/.git", "$tempdirectory/.git");
    if (!$success) {
      $database_logs->log(gettext ("Failed to temporarily store the .git directory"));
    }
    
    // Temporarily store the shared_dictionary. 
    // Disable error checking because it may not exist.
    @rename ("$directory/shared_dictionary", "$tempdirectory/shared_dictionary");

    // Completely remove all data from the git directory.
    Filter_Rmdir::rmdir ($directory);
    mkdir ($directory);

    // Move the .git directory back into place.
    $success = rename ("$tempdirectory/.git", "$directory/.git");
    if (!$success) {
      $database_logs->log(gettext ("Failed to restore the .git directory"));
    }
    
    // Move the shared_dictionary back into place. 
    // Disable error checking because it may not exist.
    @rename ("$tempdirectory/shared_dictionary", "$directory/shared_dictionary");

    // Store the data into the repository. Data that no longer exists will have been removed above.
    if ($bible == "consultationnotes") {
      $database_logs->log (gettext ("Transferring notes to file ..."));
      $success = Filter_Git::notesDatabase2filedata ($directory);
    } else {
      $database_logs->log (gettext ("Transferring Bible text to file ..."));
      $success = Filter_Git::bibleDatabase2filedata ($bible, $directory);
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
      $database_logs->log ($command);
      unset ($result);
      exec ($command, &$result, &$exit_code);
      if ($exit_code != 0) $success = false;
      foreach ($result as $line) {
        $database_logs->log ($line);
      }
      $message = "Exit code $exit_code";
      $database_logs->log ($message);
    }

    if ($success) {
      $command = "cd $shelldirectory; git status 2>&1";
      $database_logs->log ($command);
      unset ($result);
      exec ($command, &$result, &$exit_code);
      if ($exit_code != 0) $success = false;
      foreach ($result as $line) {
        $database_logs->log ($line);
      }
      $message = "Exit code $exit_code";
      $database_logs->log ($message);
    }  

    if ($success) {
      $command = "cd $shelldirectory; git commit -a -m sync 2>&1";
      $database_logs->log ($command);
      unset ($result);
      exec ($command, &$result, &$exit_code);
      if (($exit_code != 0) && ($exit_code != 1)) $success = false;
      foreach ($result as $line) {
        $database_logs->log ($line);
      }
      $message = "Exit code $exit_code";
      $database_logs->log ($message);
    }  

    // Pull changes from the remote repository.
    if ($success) {
      $command = "cd $shelldirectory; git pull 2>&1";
      $database_logs->log ($command);
      unset ($result);
      exec ($command, &$result, &$exit_code);
      if ($exit_code != 0) $success = false;
      foreach ($result as $line) {
        // Leave out messages like:
        //   Could not create directory '/var/www/.ssh'.
        //   Failed to add the host to the list of known hosts (/var/www/.ssh/known_hosts).
        // Such messages could confuse the user, and these are not really errors.
        if (strstr ($line, "/.ssh") != false) continue;
        $database_logs->log ($line);
        $database_git->insert ($directory, $line);
        if (strstr ($line, "CONFLICT") !== false) {
          $database_logs->log ($line);
          $message = gettext ("A conflict was found in the above book and chapter or consultation note. Please resolve this conflict manually. Open the chapter in the editor in USFM view, and select which of the two conflicting lines of text should be retained, and remove the other line, and the conflict markup. After that it is recommended to send and receive the Bibles again. This will remove the conflict from the repository.");
          $database_logs->log ($message);
          // Inform administrator about the conflict.
          $database_mail = Database_Mail::getInstance ();
          $database_users = Database_Users::getInstance ();
          $adminusername = $database_users->getAdministrator ();
          $subject = "Send/Receive conflict";
          $body = $line;
          $body .= $message;
          $database_mail->send ($adminusername, $subject, $body);
        }
      }
      $message = "Exit code $exit_code";
      $database_logs->log ($message);
    }  

    // Push our changes into the remote repository.
    if ($success) {
      $command = "cd $shelldirectory; git push 2>&1";
      $database_logs->log ($command);
      unset ($result);
      exec ($command, &$result, &$exit_code);
      if ($exit_code != 0) $success = false;
      foreach ($result as $line) {
        if (strstr ($line, "/.ssh") != false) continue;
        $database_logs->log ($line);
      }
      $message = "Exit code $exit_code";
      $database_logs->log ($message);
    }  

    if ($success) {
      $database_logs->log (gettext ("Moving the data that was changed into the database ..."));
      Filter_Git::filedata2database ();
    }

    // For security reasons, remove the secure shell keys.
    Filter_Git::git_un_config ($secure_key_directory);

    // Do a "git log" to provide information about the most recent commits.
    {
      $database_logs->log (gettext ("Listing the last few commits ..."));
      $command = "cd $shelldirectory; git log | head -n 24 2>&1";
      $database_logs->log ($command);
      unset ($result);
      exec ($command, &$result, &$exit_code);
      foreach ($result as $line) {
        $database_logs->log ($line);
      }
      $message = "Exit code $exit_code";
      $database_logs->log ($message);
    }  

    // Done.
    if (!$success) {
      $database_logs->log (gettext ("There was a failure"));
    }
    if ($bible == "consultationnotes") {
      $database_logs->log (gettext ("The Consultation Notes have been done."));
    } else {
      $database_logs->log(gettext ("This Bible has been done."));
    }
  }
}


$database_logs->log ("**********");
$database_logs->log (gettext ("Ready. All relevant Bibles, and Consultations Notes, have been sent and received."));

?>
