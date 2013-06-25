<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);


Assets_Page::header (gettext ("Patterns"));
$smarty = new Smarty_Bibledit (__FILE__);
$database_config_general = Database_Config_General::getInstance();


if (isset($_POST['patterns'])) {
  $patterns = Filter_String::string2array ($_POST['patterns']);
  $database_config_general->setCheckingPatterns ($patterns);
  $smarty->assign ("success", gettext ("The patterns were saved"));
}


$smarty->assign ("patterns", implode ("\n", $database_config_general->getCheckingPatterns ()));
$smarty->display("settingspatterns.tpl");
Assets_Page::footer ();


?>
