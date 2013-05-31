<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);
Assets_Page::header (gettext ("Sprint Backlog"));
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("sprint-backlog.tpl");
Assets_Page::footer ();
?>
