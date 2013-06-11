<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);
Assets_Page::header (gettext ("Installation"));
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("installation.tpl");
Assets_Page::footer ();
// http://www.crosswire.org/wiki/Creating_and_Maintaining_a_Module_Repository
?>
