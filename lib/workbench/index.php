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
page_access_level (Filter_Roles::CONSULTANT_LEVEL);


@$bench = $_GET ['bench'];
if (isset ($bench)) {
  $database_config_user = Database_Config_User::getInstance ();
  $workbenches = Workbench_Logic::getWorkbenches ();
  $workbench = $workbenches [$bench];
  $database_config_user->setActiveWorkbench ($workbench);
}


$header = new Assets_Header (Locale_Translate::_("Workbench"));
$header->setNavigator ();
$header->run ();


$view = new Assets_View (__FILE__);


$urls = Workbench_Logic::getURLs (true);
$widths = Workbench_Logic::getWidths ();
foreach ($urls as $key => $url) {
  $row = intval ($key / 5) + 1;
  $column = $key % 5 + 1;
  $variable = "url" . $row . $column;
  $view->view->$variable = $url;
  $variable = "width" . $row . $column;
  $view->view->$variable = $widths [$key];
}


$heights = Workbench_Logic::getHeights ();
foreach ($heights as $key => $height) {
  $row = $key + 1;
  $variable = "height" . $row;
  $view->view->$variable = $height;
}


$view->render ("index.php");


Assets_Page::footer ();


?>
