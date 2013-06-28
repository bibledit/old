<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);
Assets_Page::header (gettext ("Daily Scrum Meeting"));
$view = new Assets_View (__FILE__);
$view->render ("daily-scrum-meeting.php");
Assets_Page::footer ();
?>
