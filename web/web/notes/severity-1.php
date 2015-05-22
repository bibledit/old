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


$database_notes = Database_Notes::getInstance();
$notes_logic = Notes_Logic::getInstance();
$database_users = Database_Users::getInstance();


$id = $_GET ['id'];


@$severity = $_GET['severity'];
if (isset ($severity)) {
  $notes_logic->setRawSeverity ($id, $severity);
  Filter_Url::redirect ("actions.php?id=$id");
  die;
}


$assets_header = new Assets_Header (Locale_Translate::_("Severity"));
$assets_header->run();


$view = new Assets_View (__FILE__);


$view->view->id = $id;


$severities = $database_notes->getPossibleSeverities ();
$view->view->severities = $severities;


$view->render ("severity-1.php");


Assets_Page::footer ();


?>
