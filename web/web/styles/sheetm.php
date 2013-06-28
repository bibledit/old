<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);

Assets_Page::header (gettext ("Stylesheet"));
$view = new Assets_View (__FILE__);
$database_styles = Database_Styles::getInstance();

$name = $_GET['name'];
$view->view->name = Filter_Html::sanitize ($name);

if (isset($_POST['submit'])) {
  $data = $_POST['data'];
  if ($data != "") {
    $database_styles->deleteSheet ($name);
    $database_styles->importXml ($name, $data);
  }
}

@$delete = $_GET['delete'];
if ($delete != "") {
  $database_styles->deleteMarker ($name, $delete);
}


$markers_names = $database_styles->getMarkersAndNames ($name);
foreach ($markers_names as $row) {
  $markers[] =  $row['marker'];
  $names[]=     $row['name'];
}
$view->view->markers = $markers;
$view->view->names = $names;

$xml = $database_styles->exportXml ($name);
$view->view->xml = $xml;

$view->render ("sheetm.php");

Assets_Page::footer ();

?>
