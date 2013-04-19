<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (TRANSLATOR_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);
$database_config_user = Database_Config_User::getInstance();
$bible = $database_config_user->getBible();
$smarty->assign ("bible", $bible);
$smarty->display("index.tpl");
?>
