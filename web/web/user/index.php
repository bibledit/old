<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MEMBER_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);
$plugin_enumerator = Plugins_Enumerator::getInstance();
$plugins = $plugin_enumerator->getMenuPlugins ("user");
$smarty->assign ("plugins", $plugins);
$smarty->display("index.tpl");

?>
