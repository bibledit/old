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
page_access_level (Filter_Roles::manager ());


$database_usfmresources = Database_UsfmResources::getInstance ();


@$delete = request->query['delete'];
if (isset ($delete)) {
  if (access_bible_write ($delete)) {
    $database_usfmresources->deleteResource ($delete);
  } else {
    Assets_Page::error (gettext("You do not have write access to this resource"));
  }
}


@$convert = request->query['convert'];
if (isset ($convert)) {
  if (access_bible_write ($convert)) {
    tasks_logic_queue (Tasks_Logic::PHP, array (__DIR__ . "/convert2bible", $convert));
    redirect_browser ("../journal/index");
    die;
  } else {
    Assets_Page::error (gettext("Insufficient privileges"));
  }
}


$header = new Assets_Header (gettext("USFM Resources"));
$header->jQueryUIOn ("dialog");
$header->run ();
$view = new Assets_View (__FILE__);


$resources = $database_usfmresources->getResources ();
$view->view->resources = $resources;


$view->render ("manage");
Assets_Page::footer ();


?>
