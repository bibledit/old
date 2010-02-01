<?php
    
require_once ("../bootstrap/bootstrap.php");

$session_logic = Session_Logic::getInstance ();
$session_logic->logout ();

$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("logout.tpl");

?>