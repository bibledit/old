<?php

if (php_sapi_name () != "cli") return;

require_once ("../bootstrap/bootstrap.php");

echo (gettext ("Starting to send and receive the relevant Bibles and Consultation Notes.") . "\n");

echo (gettext ("Processing any data left over from previous actions.") . "\n");
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

    echo "**********\n";
    if ($bible == "consultationnotes") {
      echo gettext ("Consultation Notes") . "\n";
    } else {
      echo gettext ("Bible") . ": " . $bible . "\n";
    }
    echo gettext ("Remote repository URL") . ": " . $remote_repository_url . "\n";

    // The git directory for this object.
    $directory = Filter_Git::git_directory ($bible);
    echo gettext ("Git repository directory") . ": " . $directory . "\n";
    $shelldirectory = escapeshellarg ($directory);
    
    // Continue to the next Bible if the repository directory is not there.
    if (!is_dir ($directory)) {
      echo gettext ("Cannot send and receive the data because the git repository was not found in the filesystem.") . "\n";
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
      echo gettext ("Failed to temporarily store the .git directory") . "\n";
    }
    
    // Temporarily store the shared_dictionary. 
    // Do not check on errors, because it may not exist.
    $success = rename ("$directory/shared_dictionary", "$tempdirectory/shared_dictionary");

    // Completely remove all data from the git directory.
    Filter_Rmdir::rmdir ($directory);
    mkdir ($directory);

    // Move the .git directory back into place.
    $success = rename ("$tempdirectory/.git", "$directory/.git");
    if (!$success) {
      echo gettext ("Failed to restore the .git directory") . "\n";
    }
    
    // Move the shared_dictionary back into place. It may not exist.
    $success = rename ("$tempdirectory/shared_dictionary", "$directory/shared_dictionary");

    // Store the data into the repository. Data that no longer exists will have been removed above.
    if ($bible == "consultationnotes") {
      echo gettext ("Transferring notes to file ...") . "\n";
      $success = Filter_Git::notesDatabase2filedata ($directory);
    } else {
      echo gettext ("Transferring Bible text to file ...") . "\n";
      $success = Filter_Git::bibleDatabase2filedata ($bible, $directory);
    }
    // If the above does not succeed, then there is a serious problem. 
    // It means that the repository does no longer reflect the data in the database.
    // This can lead to data loss. Through the repository this would then be propagated to other system.
    // The best thing to do is to remove the .git directory, so that it cannot propagate damaged data.
    // Send / Receive would only work again after it has been set up from scratch.
    if (!$success) {
      Filter_Rmdir::rmdir ("$directory/.git");
    }

    // Commit the new data to the repository.
    if ($success) {
      $command = "cd $shelldirectory; git add . 2>&1";
      echo "$command\n";
      $database_logs->log ($command);
      unset ($result);
      exec ($command, &$result, &$exit_code);
      if ($exit_code != 0) $success = false;
      foreach ($result as $line) {
        $database_logs->log ($line);
      }
      $message = "Exit code $exit_code";
      echo "$message\n";
      $database_logs->log ($message);
    }

    if ($success) {
      $command = "cd $shelldirectory; git status 2>&1";
      echo "$command\n";
      $database_logs->log ($command);
      unset ($result);
      exec ($command, &$result, &$exit_code);
      if ($exit_code != 0) $success = false;
      foreach ($result as $line) {
        $database_logs->log ($line);
      }
      $message = "Exit code $exit_code";
      echo "$message\n";
      $database_logs->log ($message);
    }  

    if ($success) {
      $command = "cd $shelldirectory; git commit -a -m sync 2>&1";
      echo "$command\n";
      $database_logs->log ($command);
      unset ($result);
      exec ($command, &$result, &$exit_code);
      if (($exit_code != 0) && ($exit_code != 1)) $success = false;
      foreach ($result as $line) {
        $database_logs->log ($line);
      }
      $message = "Exit code $exit_code";
      echo "$message\n";
      $database_logs->log ($message);
    }  

    // Pull changes from the remote repository.
    if ($success) {
      $command = "cd $shelldirectory; git pull 2>&1";
      echo "$command\n";
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
          echo "$line\n";
          $message = gettext ("A conflict was found in the above book and chapter or consultation note. Please resolve this conflict manually. Open the chapter in the editor in USFM view, and select which of the two conflicting lines of text should be retained, and remove the other line, and the conflict markup. After that it is recommended to send and receive the Bibles again. This will remove the conflict from the repository.");
          echo  "$message\n";
          $database_logs->log ($message);
        }
      }
      $message = "Exit code $exit_code";
      echo "$message\n";
      $database_logs->log ($message);
    }  

    // Push our changes into the remote repository.
    if ($success) {
      $command = "cd $shelldirectory; git push 2>&1";
      echo "$command\n";
      $database_logs->log ($command);
      unset ($result);
      exec ($command, &$result, &$exit_code);
      if ($exit_code != 0) $success = false;
      foreach ($result as $line) {
        if (strstr ($line, "/.ssh") != false) continue;
        $database_logs->log ($line);
      }
      $message = "Exit code $exit_code";
      echo "$message\n";
      $database_logs->log ($message);
    }  

    if ($success) {
      echo gettext ("Moving the data that was changed into the database ...") . "\n";
      Filter_Git::filedata2database ();
    }

    // For security reasons, remove the secure shell keys.
    Filter_Git::git_un_config ($secure_key_directory);

    // Do a "git log" to provide information about the most recent commits.
    {
      echo gettext ("Listing the last few commits ...") . "\n";
      $command = "cd $shelldirectory; git log | head -n 24 2>&1";
      echo "$command\n";
      $database_logs->log ($command);
      unset ($result);
      exec ($command, &$result, &$exit_code);
      foreach ($result as $line) {
        $database_logs->log ($line);
      }
      $message = "Exit code $exit_code";
      echo "$message\n";
      $database_logs->log ($message);
    }  

    // Done.
    if (!$success) {
      echo gettext ("There was a failure") . "\n";
    }
    if ($bible == "consultationnotes") {
      echo gettext ("The Consultation Notes have been done.") . "\n";
    } else {
      echo gettext ("This Bible has been done.") . "\n";
    }
  }
}

echo "**********\n";
echo gettext ("Ready. All relevant Bibles, and Consultations Notes, have been done.") . "\n";

$database_shell = Database_Shell::getInstance ();
$database_shell->stopProcess ("sendreceive", 0);

?>
