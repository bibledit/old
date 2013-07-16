<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);


$database_config_user = Database_Config_User::getInstance ();


@$add = $_GET['add'];
if (isset ($add)) {
  $resources = $database_config_user->getActiveResources ();
  $resources [] = $add;
  $database_config_user->setActiveResources ($resources);
}


$database_bibles = Database_Bibles::getInstance ();


$header = new Assets_Header (gettext ("Resources"));
$header->jQueryOn ();
$header->run ();
$view = new Assets_View (__FILE__);


$view->view->actives = $database_config_user->getActiveResources ();
$view->view->bibles = $database_bibles->getBibles ();
$view->render ("settings.php");
Assets_Page::footer ();


?>
