<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);

/**
* If a user is to be deleted, this gets passed as a GET request to this page.
* Do that first, then proceed to the actual page.
*/
if (isset ($_GET['delete'])) {
  $database = Session_Database::getInstance();
  $database->removeUser($_GET['delete']);
}

/**
* Normal page display.
*/
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
