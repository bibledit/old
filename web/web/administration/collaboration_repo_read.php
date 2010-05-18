<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);

$object = $_GET ['object'];
$smarty->assign ("object", $object);

$database_config_user = Database_Config_User::getInstance();
if (isset($_POST['url'])) {
  $url = $_POST['urlvalue'];
  $database_config_user->setRemoteRepositoryUrl ($object, $url);
}
$url = $database_config_user->getRemoteRepositoryUrl ($object);
$smarty->assign ("url", $url);

// In case the repository is secure, set up the secure keys.
$secure_key_directory = Filter_Git::git_config ($url);

$command = "git ls-remote $url 2>&1";
$smarty->assign ("command", $command);
ob_start ();
system ($command, &$exit_code);
$read_access_result = ob_get_contents ();
ob_end_clean();
$smarty->assign ("read_access_result", nl2br ($read_access_result));

if ($exit_code == 0) {
  $success_message = gettext ("Read access to the repository is successful.");
} else {
  $error_message = gettext ("Read access failed. Please retry it, possibly with another URL.");
}
@$smarty->assign ("success_message", $success_message);
@$smarty->assign ("error_message", $error_message);

$smarty->display("collaboration_repo_read.tpl");

// For security reasons, remove the private ssh key.
Filter_Git::git_un_config ($secure_key_directory);

?>
