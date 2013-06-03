<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
Assets_Page::header (gettext ("Changes"));
$smarty = new Smarty_Bibledit (__FILE__);


$database_users = Database_Users::getInstance ();
$database_changes = Database_Changes::getInstance ();
$config_general = Database_Config_General::getInstance ();
$database_logs = Database_Logs::getInstance ();


if (isset($_GET['generate'])) {
  $config_general->setTimerDiff (time ());
  $smarty->assign ("success", gettext ("The lists of changes will be generated within a minute."));
  $database_logs->log ("The lists of changes will be generated within a minute");
}


@$clear = $_GET['clear'];
if (isset ($clear)) {
  $database_changes->clearUser ($clear);
  $smarty->assign ("success", gettext ("The change notifications were cleared for the user."));
}


$users = $database_users->getUsers ();
$pendingUsers = array ();
$pendingCount = array ();
foreach ($users as $user) {
  $ids = $database_changes->getIdentifiers ($user);
  $count = count ($ids);
  if ($count > 0) {
    $pendingUsers [] = $user;
    $pendingCount [] = $count;
  }
}


$smarty->assign ("users", $pendingUsers);
$smarty->assign ("count", $pendingCount);


$smarty->display("changes.tpl");
Assets_Page::footer ();


?>
