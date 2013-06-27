<?php
    
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);

$session_logic = Session_Logic::getInstance ();
$session_logic->logout ();

Assets_Page::header (gettext ("Logout"));
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("logout.tpl");
Assets_Page::footer ();
?>
