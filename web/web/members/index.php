<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (MEMBER_LEVEL);
Assets_Page::header (gettext ("Members"));
$smarty = new Smarty_Bibledit (__FILE__);
$plugin_enumerator = Plugins_Enumerator::getInstance();
$plugins = $plugin_enumerator->getMenuPlugins ("members");
$smarty->assign ("plugins", $plugins);
$smarty->display("index.tpl");
Assets_Page::footer ();
?>