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


@$resource = $_GET['resource'];
@$book = $_GET['book'];
@$chapter = $_GET['chapter'];
@$verse = $_GET['verse'];


if ($resource == "") die;
if ($book == "") die;
if ($chapter == "") die;
if ($verse == "") die;


// In JavaScript the resource identifier starts at 1 instead of at 0.
$resource--;


$database_config_user = Database_Config_User::getInstance ();
$resources = $database_config_user->getActiveResources ();
$resource = $resources [$resource];


$html = Resource_Logic::getHtml ($resource, $book, $chapter, $verse);
echo $html;


?>
