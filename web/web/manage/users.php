<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);

$database_users = Database_Users::getInstance();

// New user creation.
if (isset ($_GET['new'])) {
  $dialog_entry = new Dialog_Entry ("", gettext ("Please enter a name for the new user"), "", "new", "");
  die;
}
if (isset($_POST['new'])) {
  $user = $_POST['entry'];
  if ($database_users->usernameExists ($user)) {
    Assets_Page::error (gettext ("User already exists"));
  } else {
    include ("session/levels.php");
    $database_users->addNewUser($user, $user, MEMBER_LEVEL, "");
    Assets_Page::success (gettext ("User created"));
  }
}

if (isset ($_GET['delete'])) {
  $name = $_GET['delete'];
  @$confirm = $_GET['confirm'];
  if ($confirm == "") {
    $dialog_yes = new Dialog_Yes (NULL, gettext ("Would you like to delete") . " $name?", "delete");
    die;
  } else {
    $database_users->removeUser($name);
  }
}

@$user =  $_GET['user'];
@$level = $_GET['level'];
if (isset ($user) || isset ($level)) {
  if (($user == "") || ($level == "")) {
    $dialog_list = new Dialog_List (NULL, gettext ("Would you like to change the role given to user $user?"), "", "");
    for ($i = GUEST_LEVEL; $i <= ADMIN_LEVEL; $i++) {
      $parameter = "?user=$user&level=$i";
      $dialog_list->add_row ($roles[$i], $parameter);
    }
    $dialog_list->run ();
    die;
  } else {
    $database_users->updateUserLevel($user, $level);
    $database_mail = Database_Mail::getInstance ();
    $database_mail->send ($user, gettext ("Access level change"), gettext ("Your access level was updated"));
  }
}

Assets_Page::header (gettext ("Users"));
$smarty = new Smarty_Bibledit (__FILE__);
$users = Session_Users::getInstance ();
for ($i = 0; $i < count ($users->levels); $i++) {
  $named_roles[] = $roles[$users->levels[$i]];
}
$smarty->assign ("usernames", $users->usernames);
$smarty->assign ("levels",    $named_roles);
$smarty->assign ("emails",    $users->emails);
$smarty->display("users.tpl");
Assets_Page::footer ();


?>
