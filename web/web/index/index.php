<?php

// Page bootstrapping.
require_once ("../bootstrap/bootstrap.php");

Assets_Page::header ("Bibledit-Web");
$view = new Assets_View (__FILE__);
$view->render ("index.php");

Assets_Page::footer ();

?>
