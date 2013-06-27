<?php

// Page bootstrapping.
require_once ("../bootstrap/bootstrap.php");

Assets_Page::header ("Bibledit-Web");
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("index.tpl");

Assets_Page::footer ();

?>
