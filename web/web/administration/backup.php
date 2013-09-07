<?php

require_once ("../bootstrap/bootstrap.php");

page_access_level (ADMIN_LEVEL);

Assets_Page::header (gettext ("Backup"));

$view = new Assets_View (__FILE__);

$config_general = Database_Config_General::getInstance ();
$database_logs = Database_Logs::getInstance ();

if (isset ($_GET ['create'])) {
  $config_general->setTimerBackup (time ());
  $message = gettext ("Will create a backup within a minute");
  $view->view->success = $message;
  $database_logs->log ($message);
}


include ("database/credentials.php");
$view->view->file = Filter_Backup::file ();
$view->view->folder = Filter_Backup::folder ();
$view->view->database_host = $database_host;
$view->view->database_name = $database_name;
$view->view->database_user = $database_user;
$view->view->database_pass = $database_pass;
$view->render ("backup.php");

Assets_Page::footer ();

?>
