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
page_access_level (Filter_Roles::consultant ());


$database_notes = Database_Notes::getInstance ();
$notes_logic = Notes_Logic::getInstance ();


$id = request->query ['id'];


if (isset(request->post['submit'])) {
  $summary = request->post['entry'];
  $notes_logic->setSummary ($id, $summary);
  redirect_browser ("note?id=$id");
  die;
}


$header = new Assets_Header (gettext("Note summary"));
$header->setBodyOnload ('document.form.entry.focus();'); // Use html5 autofocus
$header->run();


$view = new Assets_View (__FILE__);


$view->view->id = $id;


$summary = $database_notes->getSummary ($id);
$summary = filter_string_sanitize_html ($summary);
$view->view->summary = $summary;


$view->render ("summary");


Assets_Page::footer ();


?>
