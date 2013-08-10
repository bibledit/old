<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);
include ("administration/credentials.php");



$database_resources = Database_Resources::getInstance ();
$database_offlineresources = Database_OfflineResources::getInstance ();


@$add = $_POST['entry'];
if (isset ($add)) {
  $database_resources->save ($add, "php script", "");
}


@$delete = $_GET['delete'];
if (isset ($delete)) {
  $database_resources->delete ($delete);
  $database_offlineresources->delete ($delete);
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
