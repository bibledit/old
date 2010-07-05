<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);
Assets_Page::header (gettext ("Installation"));
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("001ubuntu1004server.tpl");
Assets_Page::footer ();
?>





