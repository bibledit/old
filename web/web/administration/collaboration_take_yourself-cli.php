<?php

if (php_sapi_name () != "cli") return;

require_once ("../bootstrap/bootstrap.php");

$object = $argv[1];
$database_config_user = Database_Config_User::getInstance();
$url = $database_config_user->getRemoteRepositoryUrl ($object);
$directory = Filter_Git::git_directory ($object);

// Set up the secure keys just in case the repository happens to be secure.
$secure_key_directory = Filter_Git::git_config ($url);

// Temporarily store the .git directory Todo
$tempdirectory = tempnam (sys_get_temp_dir(), '');
unlink ($tempdirectory);
mkdir ($tempdirectory);
$renamed = rename ("$directory/.git", "$tempdirectory/.git");
if (!$renamed) {
  $message = gettext ("Failed to temporarily store the .git directory");
  echo "$message\n";
}

// Completely remove all data from the git directory.
Filter_Rmdir::rmdir ($directory);
mkdir ($directory);

// Move the .git directory back into place. Todo
$renamed = rename ("$tempdirectory/.git", "$directory/.git");
if (!$renamed) {
  $message = gettext ("Failed to restore the .git directory");
  echo "$message\n";
}

// Put our data into the repository staging area.
$message = gettext ("Step 1/2: Exporting the local data to the local repository");
echo "$message\n";
if ($object == "consultationnotes") {
  Filter_Git::notesDatabase2filedata ($directory, true);
} else {
  Filter_Git::bibleDatabase2filedata ($object, $directory, true);
}

// Directory made fit for use by the shell.
$shelldirectory = escapeshellarg ($directory);

// Add and commit the data.
$command = "cd $shelldirectory; git add .";
echo "$command\n";
exec ($command, &$output, &$exit_code);
echo "Exit code $exit_code\n";
if ($exit_code != 0) {
  foreach ($output as $line) {
    echo "$line\n";
  }
}
$command = "cd $shelldirectory; git commit -a -m admin-sync";
echo "$command\n";
exec ($command, &$output, &$exit_code);
echo "Exit code $exit_code\n";
if ($exit_code != 0) {
  foreach ($output as $line) {
    echo "$line\n";
  }
}

// Push data to remote repository.
echo gettext ("Step 2/2: Pushing the data to the remote repository") . "\n";
$command = "cd $shelldirectory; git push 2>&1";
echo "$command\n";
exec ($command, &$output, &$exit_code);
if ($exit_code == 0) {
  echo gettext ("Your data was pushed to the remote repository successfully, overwriting any data that was there before.") . "\n";
} else {
  echo "Exit code $exit_code\n";
  foreach ($output as $line) {
    echo "$line\n";
  }
  echo gettext ("Pushing your data to the remote repository failed.") . "\n";
}

// Be sure to sync in case somebody unplugs the USB flash drive before data was fully written to it.
exec ("sync");

// For security reasons, remove the private ssh key.
Filter_Git::git_un_config ($secure_key_directory);

$database_shell = Database_Shell::getInstance ();
$database_shell->stopProcess ("collaboration_take_yourself", 0);

?>
