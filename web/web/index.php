<?php

  // Page bootstrapping.
  require_once ("bootstrap/bootstrap.php");

  $smarty = new Smarty_Bibledit (__FILE__);
  $smarty->display("index.tpl");

?>
