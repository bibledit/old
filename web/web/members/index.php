<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (MEMBER_LEVEL);
Assets_Page::header (gettext ("Members"));
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("index.tpl");
Assets_Page::footer ();
?>
