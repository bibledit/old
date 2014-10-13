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


@$refresh = $_GET['refresh'];

if ($refresh == "bibles") {
  $directory = dirname (__DIR__) . "/search";
  Tasks_Logic::queue (Tasks_Logic::PHP, array ("$directory/rebibles.php"));
  Filter_Url::redirect ("../journal/index.php");
  die;
}


if ($refresh == "notes") {
  $directory = dirname (__DIR__) . "/search";
  Tasks_Logic::queue (Tasks_Logic::PHP, array ("$directory/renotes.php"));
  Filter_Url::redirect ("../journal/index.php");
  die;
}


Assets_Page::header (Locale_Translate::_("Indexing"));


$view = new Assets_View (__FILE__);
$view->render ("indexing.php");


Assets_Page::footer ();


?>
