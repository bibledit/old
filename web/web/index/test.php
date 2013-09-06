<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
$header = new Assets_Header (gettext ("Test"));
$header->jQueryOn ();
$header->run ();
$view = new Assets_View (__FILE__);
$view->render ("test.php");
Assets_Page::footer ();
?>
