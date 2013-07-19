<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);
include ("administration/credentials.php");



$database_resources = Database_Resources::getInstance ();


@$add = $_POST['entry']; // Todo It creates a default command and default script.
if (isset ($add)) {
  $script = file_get_contents ("default.php");
  $database_resources->save ($add, "php script", $script);
}


@$delete = $_GET['delete'];
if (isset ($delete)) {
  $database_resources->delete ($delete); // Todo
}


@$resources = $_POST ['resources'];
if (isset ($resources)) {
  $resources = explode (",", $resources);
  // $database_config_user->setActiveResources ($resources); // Todo
}


$database_resources = Database_Resources::getInstance ();


$header = new Assets_Header (gettext ("Resources"));
$header->jQueryOn ();
$header->jQueryUIOn ("dialog");
$header->run ();
$view = new Assets_View (__FILE__);


$resources = $database_resources->getNames ();


$view->view->resources = $resources;
$view->render ("admin.php");
Assets_Page::footer ();


?>
