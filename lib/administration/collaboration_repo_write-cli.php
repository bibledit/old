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

Filter_Cli::assert ();

$object = Filter_Cli::argument (@$argv, 1);
$directory = Filter_Cli::argument (@$argv, 2);
$escapedDir = escapeshellarg ($directory);

$database_config_bible = Database_Config_Bible::getInstance();
$url = $database_config_bible->getRemoteRepositoryUrl ($object);

echo Locale_Translate::_("Step: Adding a temporal file to the cloned repository") . "\n";

// Temporal file for trying write access.
$temporal_file_name = $directory . "/test_repository_writable";
file_put_contents ($temporal_file_name, "contents");

// Add this file.
$command = "cd $escapedDir; git add --all . 2>&1";
echo "$command\n";
passthru ($command, $exit_code);
if ($exit_code == 0) {
  echo Locale_Translate::_("Ok: Local addition was made successfully.");
} else {
  echo Locale_Translate::_("Error: Local addition failed.");
}
echo "\n";

echo Locale_Translate::_("Step: Committing the above changes locally") . "\n";

// Commit the above locally.
$command = "cd $escapedDir; git commit -a -m write-test 2>&1";
echo "$command\n";
passthru ($command, $exit_code);
// Exit code can be 1 in case there was nothing to commit.
if (($exit_code == 0) || ($exit_code == 1)) {
  echo Locale_Translate::_("Ok: Local commit was made successfully.");
} else {
  echo Locale_Translate::_("Error: Local commit failed.");
}
echo "\n";

echo Locale_Translate::_("Step: Pulling changes from the remote repository") . "\n";

// Pull changes.
// We cannot look at the exit code here in case the repository is empty,
// because in such cases the exit code is undefined.
$command = "cd $escapedDir; git pull 2>&1";
echo "$command\n";
passthru ($command, $exit_code);
echo Locale_Translate::_("Ok: Changes were pulled from the repository successfully.");
echo "\n";

echo Locale_Translate::_("Step: Pushing changes to the remote repository") . "\n";

// Push the changes to see if there is write access.
// Notice the --all switch needed when the remote repository is empty.
$command = "cd $escapedDir; git push --all 2>&1";
echo "$command\n";
passthru ($command, $exit_code);
if ($exit_code == 0) {
  echo Locale_Translate::_("Ok: Changes were pushed to the repository successfully.");
} else {
  echo Locale_Translate::_("Error: Pushing changes to the repository failed.");
}
echo "\n";

echo Locale_Translate::_("Step: Removing the temporal file from the local repository") . "\n";

// Remove the temporal file from the cloned repository.
unlink ($temporal_file_name);
$command = "cd $escapedDir; git commit -a -m write-test 2>&1";
echo "$command\n";
passthru ($command, $exit_code);
if ($exit_code == 0) {
  echo Locale_Translate::_("Ok: The temporal file was removed from the local repository successfully.");
} else {
  echo Locale_Translate::_("Error: Removing the temporal file from the local repository failed.");
}
echo "\n";

echo Locale_Translate::_("Step: Pushing the changes to the remote repository") . "\n";

// Push changes to the remote repository.
$command = "cd $escapedDir; git push 2>&1";
echo "$command\n";
passthru ($command, $exit_code);
if ($exit_code == 0) {
  echo Locale_Translate::_("Ok: Changes were pushed to the remote repository successfully.");
} else {
  echo Locale_Translate::_("Error: Pushing changes to the remote repository failed.");
}
echo "\n";

exec ("sync");

$database_shell = Database_Shell::getInstance ();
$database_shell->stopProcess ("collaboration_repo_write", 0);

?>
