<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
Assets_Page::header (gettext ("Settings"));
$smarty = new Smarty_Bibledit (__FILE__);
if (isset($_GET['run'])) {
  $database_config_general = Database_Config_General::getInstance();
  $database_config_general->setTimerChecks (time ());
  $smarty->assign ("success", gettext ("Will run the checks within a minute. See the logbook for progress."));
  $database_logs = Database_Logs::getInstance ();
  $database_logs->log (gettext ("Will run the checks within a minute"));
}
$smarty->display("settings.tpl");
Assets_Page::footer ();
?>
