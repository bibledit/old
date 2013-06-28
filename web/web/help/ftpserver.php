<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);
Assets_Page::header (gettext ("FTP server"));
$view = new Assets_View (__FILE__);
$view->render ("ftpserver.php");
Assets_Page::footer ();
?>
