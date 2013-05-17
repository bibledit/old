<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);
Assets_Page::header (gettext ("Scrum-based translation"));
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("scrum-based-translation.tpl");
Assets_Page::footer ();
?>
