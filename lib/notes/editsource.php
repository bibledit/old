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


$header = new Assets_Header (gettext("Edit Note Source"));
$header->setBodyOnload ('document.noteid.identifier.focus();'); // Use html5 autofocus
$header->run ();


$view = new Assets_View (__FILE__);


@$noteIdentifier = request->query['identifier'];
if (isset(request->post['identifier'])) {
  $noteIdentifier = request->post['identifier'];
  $noteIdentifier = Filter_Numeric::integer_in_string ($noteIdentifier);
}


$database_notes = Database_Notes::getInstance ();


if (isset(request->post['data'])) {
  $noteData = request->post['data'];
  if ($database_notes->identifierExists ($noteIdentifier)) {
    $noteData = $database_notes->setContents ($noteIdentifier, $noteData);
    $view->view->success = gettext("The note was saved");
  } else {
    $view->view->error = gettext("Unknown Note Identifier");
  }
}


if ($noteIdentifier != "") {
  if ($database_notes->identifierExists ($noteIdentifier)) {
    $noteData = $database_notes->getContents ($noteIdentifier);
    $view->view->data = $noteData;
  } else {
    $view->view->error = gettext("Unknown Note Identifier");
  }
}


$view->view->identifier = $noteIdentifier;
$view->render ("editsource");


Assets_Page::footer ();


?>
