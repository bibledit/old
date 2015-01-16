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


page_access_level (Filter_Roles::translator ());


$database_config_user = Database_Config_User::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$ipc_focus = Ipc_Focus::getInstance ();


$bible = request->database_config_user()->getTargetXrefBible ();
$book = Ipc_Focus::getBook ();
$chapter = Ipc_Focus::getChapter ();


$usfm = request->database_bibles()->getChapter (bible, book, chapter);


if (isset (request->query ['overwrite'])) {
  $usfm = Filter_Usfm::removeNotes ($usfm, array ("x"));
  Bible_Logic::storeChapter (bible, book, chapter, $usfm);
}


// Count the cross references in this chapter.
$xrefs = Filter_Usfm::extractNotes ($usfm, array ("x"));


// Count the number of xref openers / closers in this chapter.
$opener = usfm_get_opening_usfm ("x");
str_replace ($opener, "", $usfm, $openers);
$closer = usfm_get_closing_usfm ("x");
str_replace ($closer, "", $usfm, $closers);


if (empty ($xrefs)) {
  redirect_browser ("insert");
  die;
}


$header = new Assets_Header (gettext("Cross references"));
$header->run ();


$view = new Assets_View (__FILE__);


$view->view->count = count ($xrefs);
$view->view->tags = $openers - $closers;


$view->render ("clear");


Assets_Page::footer ();


?>
