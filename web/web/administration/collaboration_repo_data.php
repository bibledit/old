<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);
$object = $_GET ['object'];
$smarty->assign ("object", $object);
$database_config_user = Database_Config_User::getInstance();
$url = $database_config_user->getRemoteRepositoryUrl ($object);
$directory = $_GET ['directory'];
$smarty->assign ("directory", $directory);
$smarty->display("collaboration_repo_data.tpl");
?>
