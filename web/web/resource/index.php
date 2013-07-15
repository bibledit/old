<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);
$header = new Assets_Header (gettext ("Resources"));
$header->jQueryOn ();
$header->run ();
$view = new Assets_View (__FILE__);
$view->render ("index.php");
Assets_Page::footer ();
?>
