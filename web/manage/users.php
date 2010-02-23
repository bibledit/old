<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);


if ($_GET['delete'] != "") {
  $name = $_GET['delete'];
  $confirm = $_GET['confirm'];
  if ($confirm != "") {
    $database = Database_Users::getInstance();
    $database->removeUser($name);
  } else {
    $dialog_yes = new Dialog_Yes (gettext ("Would you like to delete") . " $name?");
    die;
  }
}




$smarty = new Smarty_Bibledit (__FILE__);
$users = Session_Users::getInstance ();
for ($i = 0; $i < count ($users->levels); $i++) {
  $named_roles[] = $roles[$users->levels[$i]];
}
$smarty->assign ("usernames", $users->usernames);
$smarty->assign ("ids",       $users->ids);
$smarty->assign ("levels",    $named_roles);
$smarty->assign ("emails",    $users->emails);
$smarty->display("users.tpl");


?>
