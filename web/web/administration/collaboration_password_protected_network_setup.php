<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);

$smarty = new Smarty_Bibledit (__FILE__);

$which_git = new Filter_Which ("git");
$smarty->assign ("git", $which_git->available);

$object = $_GET ['object'];
$smarty->assign ("object", $object);

$database_config_user = Database_Config_User::getInstance();
if (isset($_POST['url'])) {
  $url = $_POST['urlvalue'];
  $database_config_user->setRemoteRepositoryUrl ($object, $url);
}
$url = $database_config_user->getRemoteRepositoryUrl ($object);
$smarty->assign ("url", $url);

$smarty->display("collaboration_password_protected_network_setup.tpl");
Assets_Page::footer ();
?>
