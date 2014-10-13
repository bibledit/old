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


$database_usfmresources = Database_UsfmResources::getInstance ();


@$delete = $_GET['delete'];
if (isset ($delete)) {
  if (Access_Bible::write ($delete)) {
    $database_usfmresources->deleteResource ($delete);
  } else {
    Assets_Page::error (Locale_Translate::_("You do not have write access to this resource"));
  }
}


@$convert = $_GET['convert'];
if (isset ($convert)) {
  if (Access_Bible::write ($convert)) {
    Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/convert2bible.php", $convert));
    Filter_Url::redirect ("../journal/index.php");
    die;
  } else {
    Assets_Page::error (Locale_Translate::_("Insufficient privileges"));
  }
}


$header = new Assets_Header (Locale_Translate::_("USFM Resources"));
$header->jQueryUIOn ("dialog");
$header->run ();
$view = new Assets_View (__FILE__);


$resources = $database_usfmresources->getResources ();
$view->view->resources = $resources;


$view->render ("manage.php");
Assets_Page::footer ();


?>
