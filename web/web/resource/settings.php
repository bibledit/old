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


@$remove = $_GET['remove'];
if (isset ($remove)) {
  $resources = $database_config_user->getActiveResources ();
  $key = array_search ($remove, $resources);
  unset ($resources[$key]);
  $database_config_user->setActiveResources ($resources);
}


@$resources = $_POST ['resources'];
if (isset ($resources)) {
  $resources = explode (",", $resources);
  $database_config_user->setActiveResources ($resources);
}


$database_bibles = Database_Bibles::getInstance ();


$header = new Assets_Header (gettext ("Resources"));
$header->jQueryOn ();
$header->jQueryUIOn ();
$header->run ();
$view = new Assets_View (__FILE__);


$active_resources = $database_config_user->getActiveResources ();
$available_bibles = $database_bibles->getBibles ();
$available_bibles = array_diff ($available_bibles, $active_resources);


$view->view->actives = $active_resources;
$view->view->bibles = $available_bibles;
$view->render ("settings.php");
Assets_Page::footer ();


?>
