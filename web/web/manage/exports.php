<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
Assets_Page::header (gettext ("Exports"));
$smarty = new Smarty_Bibledit (__FILE__);
if (isset($_GET['generate'])) {
  $config_general = Database_Config_General::getInstance ();
  $config_general->setTimerExports (time ());
  $smarty->assign ("success", gettext ("The Bibles will be exported soon."));
}
$smarty->display("exports.tpl");
Assets_Page::footer ();
?>
