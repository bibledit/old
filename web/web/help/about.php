<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);
Assets_Page::header (gettext ("About"));
$smarty = new Smarty_Bibledit (__FILE__);
include ("version.php");
$smarty->assign ("version", $version);
$smarty->display("about.tpl");
Assets_Page::footer ();
?>
