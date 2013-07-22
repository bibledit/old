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


$header = new Assets_Header (gettext ("Resources"));
$header->jQueryOn ();
$header->jQueryUIOn ("sortable");
$header->run ();
$view = new Assets_View (__FILE__);


$active_resources = $database_config_user->getActiveResources ();
$view->view->actives = $active_resources;


// Active resources are no longer listed as available.


$database_bibles = Database_Bibles::getInstance ();
$available_bibles = $database_bibles->getBibles ();
$available_bibles = array_diff ($available_bibles, $active_resources);
$view->view->bibles = $available_bibles;


$database_usfmresources = Database_UsfmResources::getInstance ();
$available_usfm_resources = $database_usfmresources->getResources ();
$available_usfm_resources = array_diff ($available_usfm_resources, $active_resources);
$view->view->usfms = $available_usfm_resources;


$database_resources = Database_Resources::getInstance ();
$available_external_resources = $database_resources->getNames ();
$available_external_resources = array_diff ($available_external_resources, $active_resources);
$view->view->externals = $available_external_resources;



$view->render ("settings.php");
Assets_Page::footer ();


?>
