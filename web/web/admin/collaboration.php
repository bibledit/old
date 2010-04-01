<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);
$plugin_enumerator = Plugins_Enumerator::getInstance();
$plugins = $plugin_enumerator->getMenuPlugins ("administration");
$smarty->display("collaboration.tpl");
?>
