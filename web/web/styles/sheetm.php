<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
  
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


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
