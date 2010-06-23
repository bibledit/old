<?php

if (php_sapi_name () != "cli") return;

require_once ("../bootstrap/bootstrap.php");

$object = $argv[1];
$directory = $argv[2];

$database_config_user = Database_Config_User::getInstance();
$url = $database_config_user->getRemoteRepositoryUrl ($object);

// Set up the secure keys just in case the repository happens to be secure.
$secure_key_directory = Filter_Git::git_config ($url);

// Take action based on whether it deals with a Bible or with the Consultation Notes.
if ($object == "consultationnotes") {

  // Copy the data from the local cloned repository, and store it in Bibledit-Web's consultation notes,
  // overwriting anything that was there before.
  echo gettext ("Step 1/2: Copying the data from the local cloned repository, and storing it in the Consultation Notes") . "\n";
  Filter_Git::notesFiledata2database ($directory, true);

} else {

  // Copy the data from the local cloned repository, and store it in Bibledit-Web's Bible given in $object,
  // overwriting the whole Bible that was there before.
  echo gettext ("Step 1/2: Copying the data from the local cloned repository, and storing it in the Bible") . "\n";
  Filter_Git::bibleFiledata2database ($directory, $object, true);
  
}

// Store the git repository in the .git directory into Bibledit-Web's database,
// keeping it there for the next Send/Receive action.
echo gettext ("Step 2/2: Storing the git repository for the next Send/Receive action") . "\n";
Filter_Git::repository2database ($directory, $object, true);

// For security reasons, remove the private ssh key.
Filter_Git::git_un_config ($secure_key_directory);

exec ("sync");

$database_shell = Database_Shell::getInstance ();
$database_shell->stopProcess ("collaboration_take_repo", 0);

?>
