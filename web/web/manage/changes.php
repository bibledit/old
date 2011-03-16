<?php

require_once ("../bootstrap/bootstrap.php");

page_access_level (MANAGER_LEVEL);

Assets_Page::header (gettext ("Changes"));

$smarty = new Smarty_Bibledit (__FILE__);

if (isset($_GET['generate'])) {
  $config_general = Database_Config_General::getInstance ();
  $config_general->setTimerDiff (time ());
  $smarty->assign ("success", gettext ("The lists of changes will be generated soon."));
}

$smarty->display("changes.tpl");

Assets_Page::footer ();

?>
