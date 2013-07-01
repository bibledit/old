<?php

require_once ("../bootstrap/bootstrap.php");

page_access_level (ADMIN_LEVEL);

Assets_Page::header (gettext ("Backup"));

$view = new Assets_View (__FILE__);

$config_general = Database_Config_General::getInstance ();
$database_logs = Database_Logs::getInstance ();

if (isset($_POST['submit'])) {
  $name = $_POST['name'];
  $name = filter_var ($name, FILTER_SANITIZE_URL);
  $config_general->setBackupFile ($name);
  $config_general->setTimerBackup (time ());
  if ($name == "") {
    $view->view->success = gettext ("Backups are disabled");
  } else {
    $view->view->success = gettext ("The filename was saved and the backup will be created within a minute");
    $database_logs->log (gettext ("Will create a backup within a minute"));
  }
}


include ("database/credentials.php");
$view->view->name = $config_general->getBackupFile ();
$view->view->file = Filter_Backup::file ();
$view->view->database_host = $database_host;
$view->view->database_name = $database_name;
$view->view->database_user = $database_user;
$view->view->database_pass = $database_pass;
$view->render ("backup.php");

Assets_Page::footer ();

?>
