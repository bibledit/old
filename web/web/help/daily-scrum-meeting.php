<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);
Assets_Page::header (gettext ("Daily Scrum Meeting"));
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("daily-scrum-meeting.tpl");
Assets_Page::footer ();
?>
