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
page_access_level (Filter_Roles::ADMIN_LEVEL);


$database_resources = Database_Resources::getInstance ();
$database_offlineresources = Database_OfflineResources::getInstance ();


@$add = $_POST['entry'];
if (isset ($add)) {
  $database_resources->save ($add, '<?php $output = "dummy"; ?>');
}


@$delete = $_GET['delete'];
if (isset ($delete)) {
  // User cannot delete Resources in an open demo.
  include ("config/open.php");
  if (!$open_installation) {
    $database_resources->delete ($delete);
    $database_offlineresources->delete ($delete);
  }
}


$database_resources = Database_Resources::getInstance ();


$header = new Assets_Header (Locale_Translate::_("Resources"));
$header->jQueryUIOn ("dialog");
$header->run ();
$view = new Assets_View (__FILE__);


$resources = $database_resources->getNames ();


$view->view->resources = $resources;
$view->render ("admin.php");
Assets_Page::footer ();


?>
