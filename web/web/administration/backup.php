<?php

require_once ("../bootstrap/bootstrap.php");

page_access_level (ADMIN_LEVEL);

Assets_Page::header (gettext ("Backup"));

$smarty = new Smarty_Bibledit (__FILE__);

$config_general = Database_Config_General::getInstance ();

if (isset($_POST['submit'])) {
  $name = $_POST['name'];
  $name = filter_var ($name, FILTER_SANITIZE_URL);
  $config_general->setBackupFile ($name);
  $config_general->setTimerBackup (time ());
  if ($name == "") {
    $smarty->assign ("success", gettext ("Backups are disabled"));
  } else {
    $smarty->assign ("success", gettext ("The filename was saved and the backup will be created soon"));
  }
}

$smarty->assign ("name", $config_general->getBackupFile ());
$smarty->assign ("url", Filter_Backup::url ());
$smarty->display("backup.tpl");

Assets_Page::footer ();

?>
