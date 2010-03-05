<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);
$database_styles = Database_Styles::getInstance();


if ($_GET['delete'] != "") {
  $name = $_GET['delete'];
  $confirm = $_GET['confirm'];
  if ($confirm != "") {
    $database_styles->deleteSheet ($name);
  } else {
    $dialog_yes = new Dialog_Yes (gettext ("Would you like to delete this stylesheet?"));
    die;
  }
}


if (isset($_POST['new'])) {
  $name = $_POST['entry'];
  $database_styles->createSheet ($name);
}


if (isset ($_GET['new'])) {
  $dialog_entry = new Dialog_Entry ("", gettext ("Please enter the name for the new stylesheet"), "", "new", "");
  die;
}


$sheets = $database_styles->getSheets();
$smarty->assign ("sheets", $sheets);
$smarty->display ("indexm.tpl");


?>
