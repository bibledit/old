<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);

$bible = $_GET ['bible'];
$smarty->assign ("bible", $bible);

$database_config_user = Database_Config_User::getInstance();
$url = $database_config_user->getRemoteRepositoryUrl ($bible);
$smarty->assign ("url", $url);

// In case the repository is secure, set up the secure keys.
$secure_key_directory = Filter_Git::git_config ($url);

$directory = $_GET ['directory'];
$smarty->assign ("directory", $directory);

// Temporal file for trying write access.
$temporal_file_name = $directory . "/test_repository_writable";
file_put_contents ($temporal_file_name, "contents");

// Add this file.
$command1 = "cd $directory; git add . 2>&1";
$smarty->assign ("command1", $command1);
exec ($command1, &$result1, &$exit_code);
$smarty->assign ("result1", $result1);
if ($exit_code == 0) {
  $success_message1 = gettext ("Local addition was made successfully.");
} else {
  $error_message1 = gettext ("Local addition failed.");
}
$smarty->assign ("success_message1", $success_message1);
$smarty->assign ("error_message1", $error_message1);

// Commit the above locally.
$command2 = "cd $directory; git commit -a -m write-test 2>&1";
$smarty->assign ("command2", $command2);
exec ($command2, &$result2, &$exit_code);
$smarty->assign ("result2", $result2);
// Exit code can be 1 in case there was nothing to commit.
if (($exit_code == 0) || ($exit_code == 1)) {
  $success_message2 = gettext ("Local commit was made successfully.");
} else {
  $error_message2 = gettext ("Local commit failed.");
}
$smarty->assign ("success_message2", $success_message2);
$smarty->assign ("error_message2", $error_message2);

// Pull changes.
$command3 = "cd $directory; git pull 2>&1";
$smarty->assign ("command3", $command3);
exec ($command3, &$result3, &$exit_code);
$smarty->assign ("result3", $result3);
if ($exit_code == 0) {
  $success_message3 = gettext ("Changes were pulled from the repository successfully.");
} else {
  $error_message3 = gettext ("Pulling changes from the repository failed.");
}
$smarty->assign ("success_message3", $success_message3);
$smarty->assign ("error_message3", $error_message3);

// Push the changes to see if there is write access.
$command4 = "cd $directory; git push 2>&1";
$smarty->assign ("command4", $command4);
exec ($command4, &$result4, &$exit_code);
$smarty->assign ("result4", $result4);
if ($exit_code == 0) {
  $success_message4 = gettext ("Changes were pushed to the repository successfully.");
} else {
  $error_message4 = gettext ("Pushing changes to the repository failed.");
}
$smarty->assign ("success_message4", $success_message4);
$smarty->assign ("error_message4", $error_message4);

// Remove the temporal file from the cloned repository.
unlink ($temporal_file_name);
$command5 = "cd $directory; git commit -a -m write-test 2>&1";
$smarty->assign ("command5", $command5);
exec ($command5, &$result5, &$exit_code);
$smarty->assign ("result5", $result5);
if ($exit_code == 0) {
  $success_message5 = gettext ("Changes were committed to the repository successfully.");
} else {
  $error_message5 = gettext ("Committing changes to the repository failed.");
}
$smarty->assign ("success_message5", $success_message5);
$smarty->assign ("error_message5", $error_message5);

// Push changes to the remote repository.
$command6 = "cd $directory; git push 2>&1";
$smarty->assign ("command6", $command6);
exec ($command6, &$result6, &$exit_code);
$smarty->assign ("result6", $result6);
if ($exit_code == 0) {
  $success_message6 = gettext ("Changes were pushed to the remote repository successfully.");
} else {
  $error_message6 = gettext ("Pushing changes to the remote repository failed.");
}
$smarty->assign ("success_message6", $success_message6);
$smarty->assign ("error_message6", $error_message6);

$smarty->display("collaboration_repo_write.tpl");

exec ("sync");

// For security reasons, remove the private ssh key.
Filter_Git::git_un_config ($secure_key_directory);

?>
