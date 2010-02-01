<?php

  // This displays information to the user when the database cannot be accessed.
  require_once("smarty/bibledit_smarty.php");
  $smarty = new Bibledit_Smarty (__FILE__, "../");
  $smarty->assign("database_error", $mysqli->error);
  $smarty->display("no_database_creation.tpl");

?>
