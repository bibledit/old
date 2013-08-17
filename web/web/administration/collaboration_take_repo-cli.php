<?php

if (php_sapi_name () != "cli") return;

require_once ("../bootstrap/bootstrap.php");

error_reporting (E_ERROR);

$object = $argv[1];
$database_config_user = Database_Config_User::getInstance();
$url = $database_config_user->getRemoteRepositoryUrl ($object);
$directory = Filter_Git::git_directory ($object);


// Take action based on whether it deals with a Bible or with the Consultation Notes.
if ($object == "consultationnotes") {

  // Copy the data from the local cloned repository, and store it in Bibledit-Web's consultation notes,
  // overwriting anything that was there before.
  echo gettext ("Copying the data from the local cloned repository, and storing it in the Consultation Notes") . "\n";
  $notescounter = 0; // For progress counter.
  foreach (new DirectoryIterator ($directory) as $fileInfo) {
    if($fileInfo->isDot()) continue;
    if($fileInfo->isDir()) continue; // Exclude directories, e.g. the ".git" one.
    if (($notescounter % 1000) == 0) echo "$notescounter\n";
    $notescounter++;
    $identifier = $fileInfo->getFilename();
    // Simulate a line of output of command 'git pull'.
    Filter_Git::notesFiledata2database ($directory, "$identifier |    4 ++--");
  }

} else {

  // Copy the data from the local cloned repository, and store it in Bibledit-Web's Bible given in $object,
  // overwriting the whole Bible that was there before.
  echo gettext ("Copying the data from the local cloned repository, and storing it in the Bible") . "\n";
  Filter_Git::syncGit2Bible ($directory, $object);
  
}

exec ("sync");

$database_shell = Database_Shell::getInstance ();
$database_shell->stopProcess ("collaboration_take_repo", 0);

?>
