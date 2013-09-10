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

$view->view->navigationCode = Navigation_Logic::code ("");

$view->render ("index.php");
Assets_Page::footer ();
?>
