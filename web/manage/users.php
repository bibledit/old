<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);


if ($_GET['delete'] != "") {
  $name = $_GET['delete'];
  $confirm = $_GET['confirm'];
  if ($confirm == "") {
    $dialog_yes = new Dialog_Yes (NULL, gettext ("Would you like to delete") . " $name?", "delete");
    die;
  } else {
    $database = Database_Users::getInstance();
    $database->removeUser($name);
  }
}


$user =  $_GET['user'];
$level = $_GET['level'];
if (isset ($user) || isset ($level)) {
  if (($user == "") || ($level == "")) {
    $dialog_list = new Dialog_List (NULL, gettext ("Would you like to change the role given to user $user?"), "", "");
    for ($i = GUEST_LEVEL; $i <= ADMIN_LEVEL; $i++) {
      $database = Database_Users::getInstance ();
      $parameter = "?user=$user&level=$i";
      $dialog_list->add_row ($roles[$i], $parameter);
    }
    $dialog_list->run ();
    die;
  } else {
    $database = Database_Users::getInstance();
    $database->updateUserLevel($user, $level);
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
