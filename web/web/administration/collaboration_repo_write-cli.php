<?php

if (php_sapi_name () != "cli") return;

require_once ("../bootstrap/bootstrap.php");

$object = $argv[1];
$directory = $argv[2];

$database_config_user = Database_Config_User::getInstance();
$url = $database_config_user->getRemoteRepositoryUrl ($object);

// In case the repository is secure, set up the secure keys.
$secure_key_directory = Filter_Git::git_config ($url);

echo gettext ("Step: Adding a temporal file to the cloned repository") . "\n";

// Temporal file for trying write access.
$temporal_file_name = $directory . "/test_repository_writable";
file_put_contents ($temporal_file_name, "contents");

// Add this file.
$command = "cd $directory; git add . 2>&1";
echo "$command\n";
passthru ($command, &$exit_code);
if ($exit_code == 0) {
  echo gettext ("Ok: Local addition was made successfully.");
} else {
  echo gettext ("Error: Local addition failed.");
}
echo "\n";

echo gettext ("Step: Committing the above changes locally") . "\n";

// Commit the above locally.
$command = "cd $directory; git commit -a -m write-test 2>&1";
echo "$command\n";
passthru ($command, &$exit_code);
// Exit code can be 1 in case there was nothing to commit.
if (($exit_code == 0) || ($exit_code == 1)) {
  echo gettext ("Ok: Local commit was made successfully.");
} else {
  echo gettext ("Error: Local commit failed.");
}
echo "\n";

echo gettext ("Step: Pulling changes from the remote repository") . "\n";

// Pull changes.
// We cannot look at the exit code here in case the repository is empty,
// because in such cases the exit code is undefined.
$command = "cd $directory; git pull 2>&1";
echo "$command\n";
passthru ($command, &$exit_code);
echo gettext ("Ok: Changes were pulled from the repository successfully.");
echo "\n";

echo gettext ("Step: Pushing changes to the remote repository") . "\n";

// Push the changes to see if there is write access.
// Notice the --all switch needed when the remote repository is empty.
$command = "cd $directory; git push --all 2>&1";
echo "$command\n";
passthru ($command, &$exit_code);
if ($exit_code == 0) {
  echo gettext ("Ok: Changes were pushed to the repository successfully.");
} else {
  echo gettext ("Error: Pushing changes to the repository failed.");
}
echo "\n";

echo gettext ("Step: Removing the temporal file from the local repository") . "\n";

// Remove the temporal file from the cloned repository.
unlink ($temporal_file_name);
$command = "cd $directory; git commit -a -m write-test 2>&1";
echo "$command\n";
passthru ($command, &$exit_code);
if ($exit_code == 0) {
  echo gettext ("Ok: The temporal file was removed from the local repository successfully.");
} else {
  echo gettext ("Error: Removing the temporal file from the local repository failed.");
}
echo "\n";

echo gettext ("Step: Pushing the changes to the remote repository") . "\n";

// Push changes to the remote repository.
$command = "cd $directory; git push 2>&1";
echo "$command\n";
passthru ($command, &$exit_code);
if ($exit_code == 0) {
  echo gettext ("Ok: Changes were pushed to the remote repository successfully.");
} else {
  echo gettext ("Error: Pushing changes to the remote repository failed.");
}
echo "\n";

exec ("sync");

// For security reasons, remove the private ssh key.
Filter_Git::git_un_config ($secure_key_directory);

$database_shell = Database_Shell::getInstance ();
$database_shell->stopProcess ("collaboration_repo_write", 0);

?>