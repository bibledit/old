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


$view = new Assets_View (__FILE__);


$database_resources = Database_Resources::getInstance ();
$database_offlineresources = Database_OfflineResources::getInstance ();
$database_versifications = Database_Versifications::getInstance ();


@$name = request->query['name'];
$view.set_variable ("name = $name;


if (isset (request->query ['download'])) {
  $versification = "English";
  $books = $database_versifications->getBooks ($versification);
  for ($books as $book) {
    // Schedule the task with low priority so it does not get in the way of regular tasks.
    tasks_logic_queue (Tasks_Logic::PHP, array (__DIR__ . "/downloadcli", $name, $book));
  }
  redirect_browser ("../journal/index");
  die;
}


if (isset (request->query ['clear'])) {
  $database_offlineresources->delete ($name);
}


$count = $database_offlineresources->count ($name);
$view.set_variable ("count = $count;


$header = new Assets_Header (gettext("Download resource"));
$header->run ();


$view->render ("download");


Assets_Page::footer ();


?>
