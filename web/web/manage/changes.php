<?php

require_once ("../bootstrap/bootstrap.php");

page_access_level (MANAGER_LEVEL);

Assets_Page::header (gettext ("Changes")); // Todo working here.

$smarty = new Smarty_Bibledit (__FILE__);


if (isset($_GET['generate'])) {
  $config_general = Database_Config_General::getInstance ();
//  $config_general->setTimerBackup (time ()); // Todo
  $smarty->assign ("success", gettext ("The lists of changes will be generated soon."));
}

$smarty->display("changes.tpl");

Assets_Page::footer ();

?>
