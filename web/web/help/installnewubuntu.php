<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);
Assets_Page::header (gettext ("Installation"));
$view = new Assets_View (__FILE__);
$view->render ("installnewubuntu.php");
Assets_Page::footer ();
?>
