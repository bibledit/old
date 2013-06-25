<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);
$database_versifications = Database_Versifications::getInstance();


if (isset ($_GET['delete'])) {
  $name = $_GET['delete'];
  $confirm = $_GET['confirm'];
  if ($confirm != "") {
    $database_versifications->delete ($name);
  } else {
    $dialog_yes = new Dialog_Yes (NULL, gettext ("Would you like to delete this versification system?"), "delete");
    die;
  }
}


if (isset($_POST['new'])) {
  $name = $_POST['entry'];
  $database_versifications->create ($name);
}


if (isset ($_GET['new'])) {
  $dialog_entry = new Dialog_Entry ("", gettext ("Please enter the name for the new versification system"), "", "new", "");
  die;
}


$systems = $database_versifications->getSystems();
$smarty->assign ("systems", $systems);
$smarty->display ("index.tpl");
Assets_Page::footer ();


?>
