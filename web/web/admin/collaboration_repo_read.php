<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);

$bible = $_GET ['bible'];
$smarty->assign ("bible", $bible);

$database_config_user = Database_Config_User::getInstance();
if (isset($_POST['url'])) {
  $url = $_POST['urlvalue'];
  $database_config_user->setRemoteRepositoryUrl ($bible, $url);
}
$url = $database_config_user->getRemoteRepositoryUrl ($bible);
$smarty->assign ("url", $url);

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
$smarty->assign ("success_message", $success_message);
$smarty->assign ("error_message", $error_message);

$smarty->display("collaboration_repo_read.tpl");
?>
