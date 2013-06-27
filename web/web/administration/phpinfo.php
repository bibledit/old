<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
Assets_Page::header (gettext ("PHP Info"));
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("phpinfo.tpl");
phpinfo ();
Assets_Page::footer ();
?>
