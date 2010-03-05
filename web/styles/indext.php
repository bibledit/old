<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (TRANSLATOR_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);
$database_config_user = Database_Config_User::getInstance();

if ($_GET['sheet'] != "") {
  $newsheet = $_GET['sheet'];
  $database_config_user->setStylesheet ($newsheet);
}

$stylesheet = $database_config_user->getStylesheet();
$smarty->assign ("stylesheet", $stylesheet);
$smarty->display ("indext.tpl");


?>
