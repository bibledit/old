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


$database_notes = Database_Notes::getInstance();
$notes_logic = Notes_Logic::getInstance();


@$id = request->query ['id'];
if ($id == "") {
  @$id = request->post ['id'];
}


if (isset(request->post['submit'])) {
  $comment = filter_string_trim (request->post['comment']);
  $notes_logic->addComment ($id, $comment);
  redirect_browser ("note?id=$id&temporal");
  die;
}


if (isset(request->post['cancel'])) {
  redirect_browser ("note?id=$id");
  die;
}


$assets_header = new Assets_Header (gettext("Comment"));
$assets_header->run();


$view = new Assets_View (__FILE__);


$view->view->id = $id;


$summary = $database_notes->getSummary ($id);
$view->view->summary = $summary;


$content = $database_notes->getContents ($id);
$view->view->content = $content;


$view->render ("comment");


Assets_Page::footer ();
?>
