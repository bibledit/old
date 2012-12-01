<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);
Assets_Page::header (gettext ("Typesetting"));
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("typesetting.tpl");
Assets_Page::footer ();
?>




