<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);


Assets_Page::header (gettext ("Patterns"));
$view = new Assets_View (__FILE__);
$database_config_general = Database_Config_General::getInstance();


if (isset($_POST['patterns'])) {
  $patterns = Filter_String::string2array ($_POST['patterns']);
  $database_config_general->setCheckingPatterns ($patterns);
  $view->view->success = gettext ("The patterns were saved");
}


$view->view->patterns = implode ("\n", $database_config_general->getCheckingPatterns ());
$view->render ("settingspatterns.php");
Assets_Page::footer ();


?>
