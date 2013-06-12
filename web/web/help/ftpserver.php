<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);
Assets_Page::header (gettext ("FTP server"));
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("ftpserver.tpl");
Assets_Page::footer ();
?>
