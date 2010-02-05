<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->assign ("question", $_GET['question']);
$smarty->assign ("yes", $_GET['yes']);
$smarty->assign ("no", $_GET['no']);
$smarty->display("confirm.tpl");
?>
