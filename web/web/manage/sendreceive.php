<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
Assets_Page::header (gettext ("Send/Receive"));
$view = new Assets_View (__FILE__);
if (isset($_GET['run'])) {
  $database_config_general = Database_Config_General::getInstance();
  $database_config_general->setTimerSendReceive (time ());
  $view->view->success = gettext ("Will send and receive Bibles within a minute. See the logbook for progress.");
  $database_logs = Database_Logs::getInstance ();
  $database_logs->log (gettext ("Will send and receive Bibles within a minute"));
}
$view->render ("sendreceive.php");
Assets_Page::footer ();
?>
