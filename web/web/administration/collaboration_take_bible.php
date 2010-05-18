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

$directory = $_GET ['directory'];

// Create new empty directory for the updated repository.
$newdirectory = tempnam (sys_get_temp_dir(), '');
unlink ($newdirectory);
mkdir ($newdirectory);

// Move the .git directory to a new one.
rename ("$directory/.git", "$newdirectory/.git");

// Put our data into the repository staging area.
Filter_Git::bibleDatabase2filedata ($object, $newdirectory);

// Add and commit the data.
exec ("cd $newdirectory; git add .");
exec ("cd $newdirectory; git commit -a -m admin-sync");

// Push data to remote repository.
$command = "cd $newdirectory; git push 2>&1";
$smarty->assign ("command", $command);
exec ($command, &$result, &$exit_code);
$smarty->assign ("result", $result);
if ($exit_code == 0) {
  $success_message = gettext ("Your data was pushed to the remote repository successfully, overwriting any data that was there before.");
} else {
  $error_message = gettext ("Pushing your data to the remote repository failed.");
}
@$smarty->assign ("success_message", $success_message);
@$smarty->assign ("error_message", $error_message);

// Store the .git repository for next use.
Filter_Git::repository2database ($newdirectory, $object);

// Display the page.
$smarty->display("collaboration_take_bible.tpl");

// Be sure to sync in case somebody unplugs the USB flash drive before data was fully written to it.
exec ("sync");

// For security reasons, remove the private ssh key.
Filter_Git::git_un_config ($secure_key_directory);

?>
