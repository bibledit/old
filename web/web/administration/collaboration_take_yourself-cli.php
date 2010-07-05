<?php

if (php_sapi_name () != "cli") return;

require_once ("../bootstrap/bootstrap.php");

$object = $argv[1];
$directory = $argv[2];
$database_config_user = Database_Config_User::getInstance();
$url = $database_config_user->getRemoteRepositoryUrl ($object);

// Set up the secure keys just in case the repository happens to be secure.
$secure_key_directory = Filter_Git::git_config ($url);

// Create new empty directory for the updated repository.
$newdirectory = tempnam (sys_get_temp_dir(), '');
unlink ($newdirectory);
mkdir ($newdirectory);

// Move the .git directory to a new one.
$renamed = rename ("$directory/.git", "$newdirectory/.git");
if (!$renamed) echo gettext ("Failed to rename the .git directory") . "\n";

// Put our data into the repository staging area.
echo gettext ("Step 1/3: Exporting the local data to the local repository") . "\n";
if ($object == "consultationnotes") {
  Filter_Git::notesDatabase2filedata ($newdirectory, true);
} else {
  Filter_Git::bibleDatabase2filedata ($object, $newdirectory, true);
}

// Add and commit the data.
$command = "cd $newdirectory; git add .";
echo "$command\n";
passthru ($command);
$command = "cd $newdirectory; git commit -a -m admin-sync";
echo "$command\n";
passthru ($command);

// Push data to remote repository.
echo gettext ("Step 2/3: Pushing the data to the remote repository") . "\n";
$command = "cd $newdirectory; git push 2>&1";
echo "$command\n";
exec ($command, &$output, &$exit_code);
if ($exit_code == 0) {
  echo gettext ("Your data was pushed to the remote repository successfully, overwriting any data that was there before.") . "\n";
} else {
  echo "Exit code $exit_code\n";
  foreach ($output as $line) {
    //echo "$line\n";
  }
  echo gettext ("Pushing your data to the remote repository failed.") . "\n";
}













// Store the .git repository for next use.
echo gettext ("Step 3/3: Storing the .git folder for next use") . "\n";
Filter_Git::repository2database ($newdirectory, $object, false);

// Be sure to sync in case somebody unplugs the USB flash drive before data was fully written to it.
exec ("sync");

// For security reasons, remove the private ssh key.
Filter_Git::git_un_config ($secure_key_directory);

$database_shell = Database_Shell::getInstance ();
$database_shell->stopProcess ("collaboration_take_yourself", 0);

?>
