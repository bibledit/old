<?php

require_once ("../bootstrap/bootstrap.php");

page_access_level (ADMIN_LEVEL);

Assets_Page::header (gettext ("Backup"));

$smarty = new Smarty_Bibledit (__FILE__);

$config_general = Database_Config_General::getInstance ();
$database_logs = Database_Logs::getInstance ();

if (isset($_POST['submit'])) {
  $name = $_POST['name'];
  $name = filter_var ($name, FILTER_SANITIZE_URL);
  $config_general->setBackupFile ($name);
  $config_general->setTimerBackup (time ());
  if ($name == "") {
    $smarty->assign ("success", gettext ("Backups are disabled"));
  } else {
    $smarty->assign ("success", gettext ("The filename was saved and the backup will be created soon"));
    $database_logs->log (gettext ("Will create a backup within a minute"));
  }
}


include ("database/credentials.php");
$smarty->assign ("name", $config_general->getBackupFile ());
$smarty->assign ("file", Filter_Backup::file ());
$smarty->assign ("database_host", $database_host);
$smarty->assign ("database_name", $database_name);
$smarty->assign ("database_user", $database_user);
$smarty->assign ("database_pass", $database_pass);
$smarty->display("backup.tpl");

Assets_Page::footer ();

?>
