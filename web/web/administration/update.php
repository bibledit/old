<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
Assets_Page::header (gettext ("Software update"));
$smarty = new Smarty_Bibledit (__FILE__);
include ("version.php");
$smarty->assign ("version", $version);
$smarty->display("update.tpl");
Assets_Page::footer ();
?>
