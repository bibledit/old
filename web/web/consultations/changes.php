<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);
Assets_Page::header (gettext ("Changes"));
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("changes.tpl");
Assets_Page::footer ();
?>
