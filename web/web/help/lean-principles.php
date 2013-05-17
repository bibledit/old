<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);
Assets_Page::header (gettext ("Lean principles"));
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("lean-principles.tpl");
Assets_Page::footer ();
?>
