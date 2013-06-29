<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (MEMBER_LEVEL);
Assets_Page::header (gettext ("My area"));
$view = new Assets_View (__FILE__);
$view->render ("index.php");
Assets_Page::footer ();
?>
