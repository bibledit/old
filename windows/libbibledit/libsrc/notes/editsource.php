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


$header = new Assets_Header (Locale_Translate::_("Edit Note Source"));
$header->setBodyOnload ('document.noteid.identifier.focus();');
$header->run ();


$view = new Assets_View (__FILE__);


@$noteIdentifier = $_GET['identifier'];
if (isset($_POST['identifier'])) {
  $noteIdentifier = $_POST['identifier'];
  $noteIdentifier = Filter_Numeric::integer_in_string ($noteIdentifier);
}


$database_notes = Database_Notes::getInstance ();


if (isset($_POST['data'])) {
  $noteData = $_POST['data'];
  if ($database_notes->identifierExists ($noteIdentifier)) {
    $noteData = $database_notes->setContents ($noteIdentifier, $noteData);
    $view->view->success = Locale_Translate::_("The note was saved");
  } else {
    $view->view->error = Locale_Translate::_("Unknown Note Identifier");
  }
}


if ($noteIdentifier != "") {
  if ($database_notes->identifierExists ($noteIdentifier)) {
    $noteData = $database_notes->getContents ($noteIdentifier);
    $view->view->data = $noteData;
  } else {
    $view->view->error = Locale_Translate::_("Unknown Note Identifier");
  }
}


$view->view->identifier = $noteIdentifier;
$view->render ("editsource.php");


Assets_Page::footer ();


?>
