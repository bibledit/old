<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);
Assets_Page::header (gettext ("About"));
$view = new Assets_View (__FILE__);
include ("version.php");
$view->view->version = $version;
$view->render ("about.php");
Assets_Page::footer ();
?>
