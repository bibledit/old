<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);

$object = $_GET ['object'];
$smarty->assign ("object", $object);

$database_config_user = Database_Config_User::getInstance();
$url = $database_config_user->getRemoteRepositoryUrl ($object);
$smarty->assign ("url", $url);

// In case the repository is secure, set up the secure keys.
$secure_key_directory = Filter_Git::git_config ($url);

$directory = tempnam (sys_get_temp_dir(), '');
unlink ($directory);
mkdir ($directory);
$smarty->assign ("directory", $directory);

$command = "cd $directory; git clone $url . 2>&1";
$smarty->assign ("command", $command);

exec ($command, &$clone_result, &$exit_code);
$smarty->assign ("clone_result", $clone_result);

if ($exit_code == 0) {
  $success_message = gettext ("The repository was cloned successfully.");
} else {
  $error_message = gettext ("The repository could not be cloned.");
}
@$smarty->assign ("success_message", $success_message);
@$smarty->assign ("error_message", $error_message);

// Switch rename detection off. 
// This is necessary for the consultation notes, since git has been seen to "detect" spurious renames.
exec ("cd $directory; git config --global diff.renamelimit 0");

// Newer versions of git ask the user to set the default pushing method.
exec ("cd $directory; git config push.default matching");

// On the XO machine, the mail name and address were not set properly; therefore these are set manually.
// Also, since it runs from the web server, it is likely to be set to an irrelevant name and address.
$database_config_general = Database_Config_General::getInstance();
$mail_name = $database_config_general->getSiteMailName();
exec ("cd $directory; git config user.name \"$mail_name\"");
$mail_address = $database_config_general->getSiteMailAddress();
exec ("cd $directory; git config user.email \"$mail_address\"");

$smarty->display("collaboration_repo_clone.tpl");

exec ("sync");

// For security reasons, remove the private ssh key.
Filter_Git::git_un_config ($secure_key_directory);

?>
