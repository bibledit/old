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


$header = new Assets_Header (Locale_Translate::_("Resources"));
$header->jQueryUIOn ("sortable");
$header->run ();
$view = new Assets_View (__FILE__);


$active_resources = $database_config_user->getActiveResources ();
$view->view->actives = $active_resources;


// The selectable resources are the available ones minus the active ones.
$available_resources = Resource_Logic::getNames ();
$selectable_resources = array_diff ($available_resources, $active_resources);
$view->view->selectables = $selectable_resources;


$view->render ("organize.php");
Assets_Page::footer ();


?>
