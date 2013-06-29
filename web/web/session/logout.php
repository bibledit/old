<?php
    
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);

$session_logic = Session_Logic::getInstance ();
$session_logic->logout ();

Assets_Page::header (gettext ("Logout"));
$view = new Assets_View (__FILE__);
$view->render ("logout.php");
Assets_Page::footer ();
?>
