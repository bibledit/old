<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
Assets_Page::header (gettext ("Checks"));
$smarty = new Smarty_Bibledit (__FILE__);


$results = array ("Result 1", "Result 2", "...", "Result n");
$smarty->assign ("results", $results);


$smarty->display("index.tpl");
Assets_Page::footer ();
?>
