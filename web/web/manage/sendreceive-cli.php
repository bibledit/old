<?php

if (php_sapi_name () != "cli") return;

require_once ("../bootstrap/bootstrap.php");

echo (gettext ("Starting to send and receive the relevant Bibles and Project Notes.") . "\n");

$database_bibles = Database_Bibles::getInstance();
$database_config_user = Database_Config_User::getInstance();
$database_repositories = Database_Repositories::getInstance();
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
    $dot_git = $database_repositories->getRepository ($bible, 0);
    if ($dot_git == "") {
      echo gettext ("Cannot send and receive this Bible because the git repository was not found in the database.") . "\n";
      continue;
    }

    $secure_key_directory = Filter_Git::git_config ($remote_repository_url);

    $directory = tempnam (sys_get_temp_dir(), '');
    unlink ($directory);
    mkdir ($directory);
    echo gettext ("Working directory") . ": " . $directory . "\n";
    if ($bible == "consultationnotes") {
      echo gettext ("Transferring notes to file ...") . "\n";
      Filter_Git::notesDatabase2filedata ($directory);
    } else {
      echo gettext ("Transferring Bible text to file ...") . "\n";
      Filter_Git::bibleDatabase2filedata ($bible, $directory);
    }
    echo gettext ("Restoring the previously saved git repository ...") . "\n";
    Filter_Git::database2repository ($bible, $directory);

    $command = "cd $directory; git add . 2>&1";
    echo "$command\n";
    unset ($result);
    passthru ($command, &$exit_code);
    
    $command = "cd $directory; git status 2>&1";
    echo "$command\n";
    unset ($result);
    passthru ($command, &$exit_code);

    $command = "cd $directory; git commit -a -m sync 2>&1";
    echo "$command\n";
    unset ($result);
    passthru ($command, &$exit_code);
    
    $command = "cd $directory; git pull 2>&1";
    echo "$command\n";
    unset ($result);
    exec ($command, &$result, &$exit_code);
    foreach ($result as $line) {
      // Leave out messages like:
      //   Could not create directory '/var/www/.ssh'.
      //   Failed to add the host to the list of known hosts (/var/www/.ssh/known_hosts).
      // Such messages could confuse the user, and these are not really errors.
      if (strstr ($line, "/.ssh") != false) continue;
      echo "$line\n";
      if (strstr ($line, "CONFLICT") !== false) {
        echo gettext ("A conflict was found in the above book and chapter or consultation note. Please resolve this conflict manually. Open the chapter in the editor in USFM view, and select which of the two conflicting lines of text should be retained, and remove the other line, and the conflict markup. After that it is recommended to send and receive the Bibles again. This will remove the conflict from the repository.") . "\n";
      }
    }

    $command = "cd $directory; git push 2>&1";
    echo "$command\n";
    unset ($result);
    exec ($command, &$result, &$exit_code);
    foreach ($result as $line) {
      if (strstr ($line, "/.ssh") != false) continue;
      echo "$line\n";
    }

    if ($bible == "consultationnotes") {
      echo gettext ("Moving the notes from file into the database ...") . "\n";
      Filter_Git::notesFiledata2database ($directory);
    } else {
      echo gettext ("Moving the Bible data from file into the database ...") . "\n";
      Filter_Git::bibleFiledata2database ($directory, $bible);
    }
    echo gettext ("Saving the git repository for next use ...") . "\n";
    Filter_Git::repository2database ($directory, $bible);

    Filter_Git::git_un_config ($secure_key_directory);

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
