<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);
Assets_Page::header (gettext ("Installation"));
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("06ubuntu1110.tpl");
Assets_Page::footer ();
?>





