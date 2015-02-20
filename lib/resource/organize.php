<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

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


require_once ("../bootstrap/bootstrap");
page_access_level (Filter_Roles::consultant ());


$database_config_user = Database_Config_User::getInstance ();


@$add = request->query['add'];
if (isset ($add)) {
  $resources = request->database_config_user()->getActiveResources ();
  $resources [] = $add;
  request->database_config_user()->setActiveResources ($resources);
}


@$remove = request->query['remove'];
if (isset ($remove)) {
  $resources = request->database_config_user()->getActiveResources ();
  $key = array_search ($remove, $resources);
  unset ($resources[$key]);
  request->database_config_user()->setActiveResources ($resources);
}


@$resources = request->post ['resources'];
if (isset ($resources)) {
  $resources = explode (",", $resources);
  request->database_config_user()->setActiveResources ($resources);
}


$header = new Assets_Header (translate("Resources"));
$header->jQueryUIOn ("sortable");
$header->run ();
$view = new Assets_View (__FILE__);


$active_resources = request->database_config_user()->getActiveResources ();
$view.set_variable ("actives = $active_resources;


// The selectable resources are the available ones minus the active ones.
$available_resources = Resource_Logic::getNames ();
$selectable_resources = filter_string_array_diff ($available_resources, $active_resources);
$view.set_variable ("selectables = $selectable_resources;


$view->render ("organize");
Assets_Page::footer ();


?>
