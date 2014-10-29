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
page_access_level (Filter_Roles::TRANSLATOR_LEVEL);


$bible = $_GET ['bible'];
$book = $_GET ['book'];
$chapter = $_GET ['chapter'];


// At first the browser used the rangy library to get the offset of the caret.
// But the rangy library provides the offset relative to the element that contains the caret,
// not relative to the main editor element.
// Therefore a pure Javascript implementation was Googles for and implemented.
// This provides the offset of the caret relative to the <div id="editor">.
$offset = $_GET ['offset'];


$database_bibles = Database_Bibles::getInstance();
$ipc_focus = Ipc_Focus::getInstance ();
$database_config_user = Database_Config_User::getInstance ();


$stylesheet = $database_config_user->getStylesheet ();
$usfm = $database_bibles->getChapter ($bible, $book, $chapter);


$editor_import = Editor_Import::getInstance ();
$editor_import->load ($usfm);
$editor_import->stylesheet ($stylesheet);
$editor_import->run ();


// The caret offset should be in the main text body.
// If it is in a note body, skip the verse updating.
if ($offset <= $editor_import->textLength) {

  // Look for the verse that matches the offset.
  foreach ($editor_import->verseStartOffsets as $key => $value) {
    if ($offset >= $value) $verse = $key;
  }

  // Check whether a verse number was found at all.
  if (isset ($verse)) {

    // Only update navigation in case the verse changed.
    // This avoids unnecessary focus operations in the clients.
    if ($verse != $ipc_focus->getVerse ()) {
      $ipc_focus->set ($book, $chapter, $verse);
    }

    // The editor should scroll the verse into view,
    // because the caret is in the Bible text.
    echo $verse;
    // If the caret were in the notes area,
    // then the editor should not scroll the verse into view.

  }

}


?>
