<?php

if (php_sapi_name () != "cli") return;

require_once ("../bootstrap/bootstrap.php");

$object = $argv[1];
$directory = $argv[2];

$database_config_user = Database_Config_User::getInstance();
$url = $database_config_user->getRemoteRepositoryUrl ($object);

// In case the repository is secure, set up the secure keys.
$secure_key_directory = Filter_Git::git_config ($url);

$command = "cd $directory; git clone $url .";
echo "$command\n";
passthru ($command, &$exit_code);
if ($exit_code == 0) {
  echo gettext ("Ok: The repository was cloned successfully.");
} else {
  echo gettext ("Error: The repository could not be cloned.");
}
echo "\n";

// Switch rename detection off. 
// This is necessary for the consultation notes, since git has been seen to "detect" spurious renames.
exec ("cd $directory; git config diff.renamelimit 0");

// Newer versions of git ask the user to set the default pushing method.
exec ("cd $directory; git config push.default matching");

// On the XO machine, the mail name and address were not set properly; therefore these are set manually.
// Also, since it runs from the web server, it is likely to be set to an irrelevant name and address.
$database_config_general = Database_Config_General::getInstance();
$mail_name = $database_config_general->getSiteMailName();
exec ("cd $directory; git config user.name \"$mail_name\"");
$mail_address = $database_config_general->getSiteMailAddress();
exec ("cd $directory; git config user.email \"$mail_address\"");

// For security reasons, remove the private ssh key.
Filter_Git::git_un_config ($secure_key_directory);

exec ("sync");

$database_shell = Database_Shell::getInstance ();
$database_shell->stopProcess ("collaboration_repo_clone", 0);

?>
