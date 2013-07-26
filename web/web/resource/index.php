<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);
$header = new Assets_Header (gettext ("Resources"));
$header->jQueryOn ();
$header->run ();
$view = new Assets_View (__FILE__);

$database_config_user = Database_Config_User::getInstance ();
$resources = $database_config_user->getActiveResources ();
$view->view->resources = $resources;

$resource_count = count ($resources);
$script = "var resourceCount = $resource_count;";
$view->view->script = $script;

$view->view->navigationHtml = Navigation_Logic::getContainer ();

$database_config_user = Database_Config_User::getInstance (); // Todo temporal.
$bible = $database_config_user->getBible (); // Todo temporal
$view->view->navigationCode = Navigation_Logic::code ($bible); // Todo update this to "".

$view->render ("index.php");
Assets_Page::footer ();
?>
