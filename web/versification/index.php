<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);
$database_versifications = Database_Versifications::getInstance();

if ($_GET['delete'] != "") {
  $name = $_GET['delete'];
  $confirm = $_GET['confirm'];
  if ($confirm != "") {
    $database_versifications->delete ($name);
  } else {
    $dialog_yes = new Dialog_Yes (gettext ("Would you like to delete this versification system?"));
    die;
  }
}

$systems = $database_versifications->getSystems();
$smarty->assign ("systems", $systems);
$smarty->display ("index.tpl");


?>
