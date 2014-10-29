<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

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
page_access_level (Filter_Roles::MANAGER_LEVEL);


Assets_Page::header (Locale_Translate::_("Stylesheet"));
$view = new Assets_View (__FILE__);


$database_styles = Database_Styles::getInstance();
$session_logic = Session_Logic::getInstance ();


$name = $_GET['name'];
$view->view->name = Filter_Html::sanitize ($name);


$username = $session_logic->currentUser ();
$userlevel = $session_logic->currentLevel ();
$write = $database_styles->hasWriteAccess ($username, $name);
if ($userlevel >= Filter_Roles::ADMIN_LEVEL) $write = true;


if (isset($_POST['new'])) {
  $new = $_POST['entry'];
  if (in_array ($new, $database_styles->getMarkers ($name))) {
    Assets_Page::error (Locale_Translate::_("This style already exists"));
  } else {
    $database_styles->addMarker ($name, $new);
    Styles_Sheets::create_all ();
    Assets_Page::success (Locale_Translate::_("The style has been created"));
  }
}
if (isset ($_GET['new'])) {
  $dialog_entry = new Dialog_Entry (array ("name" => $name), Locale_Translate::_("Please enter the name for the new style"), "", "new", "");
  die;
}


if (isset($_POST['submit'])) {
  $data = $_POST['data'];
  if ($data != "") {
    if ($write) {
      $database_styles->deleteSheet ($name);
      $database_styles->importXml ($name, $data);
      Assets_Page::success (Locale_Translate::_("The stylesheet has been imported"));
    } else {
      Assets_Page::error (Locale_Translate::_("No write access to the stylesheet"));
    }
  }
}


@$delete = $_GET['delete'];
if ($delete != "") {
  if ($write) $database_styles->deleteMarker ($name, $delete);
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
