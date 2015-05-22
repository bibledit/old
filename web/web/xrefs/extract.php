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


$database_config_user = Database_Config_User::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_volatile = Database_Volatile::getInstance ();
$ipc_focus = Ipc_Focus::getInstance ();


$bible = $database_config_user->getSourceXrefBible ();
$book = $ipc_focus->getBook ();
$chapter = $ipc_focus->getChapter ();


$allnotes = array ();


$usfm = $database_bibles->getChapter ($bible, $book, $chapter);
$verses = Filter_Usfm::getVerseNumbers ($usfm);
foreach ($verses as $verse) {
  $verse_usfm = Filter_Usfm::getVerseText ($usfm, $verse);
  $xrefs = Filter_Usfm::extractNotes ($verse_usfm, array ("x"));
  foreach ($xrefs as $xref) {
    $xref ['verse'] = $verse;
    $xref ['offset'] = $xref [0];
    $xref ['text'] = $xref [1];
    unset ($xref [0]);
    unset ($xref [1]);
    $allnotes [] = $xref;
  }
}


$identifier = Filter_User::myIdentifier ();
$value = serialize ($allnotes);
$database_volatile->setValue ($identifier, "sourcexrefs", $value);


$empty = true;
foreach ($allnotes as $items) {
  if (!empty ($items)) $empty = false;
}


if (!$empty) {
  Filter_Url::redirect ("interpret.php");
  die;
}


$header = new Assets_Header (Locale_Translate::_("Cross references"));
$header->run ();
$view = new Assets_View (__FILE__);
$view->render ("extract.php");
Assets_Page::footer ();


?>
