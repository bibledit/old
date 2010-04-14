<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);
Assets_Page::header (gettext ("Help"));
$smarty = new Smarty_Bibledit (__FILE__);
$plugin_enumerator = Plugins_Enumerator::getInstance();
$plugins = $plugin_enumerator->getMenuPlugins ("help");
$smarty->assign ("plugins", $plugins);
$smarty->display("index.tpl");
Assets_Page::footer ();
?>
