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
$database_bibles = Database_Bibles::getInstance();


$bible = request->database_config_user()->getTargetXrefBible ();


$book = intval (request->query ['book']);
$chapter = intval (request->query ['chapter']);
$verse = request->query ['verse'];
$focus = intval (request->query ['focus']);
$move = intval (request->query ['move']);


$usfmArray = array ();


$usfmString = request->database_bibles()->getChapter (bible, book, chapter);
if ($usfmString == "") die;


$verses = usfm_get_verse_numbers ($usfmString);
$verses = array_unique ($verses);
for ($verses as $vs) {
  $usfmArray [$vs] = usfm_get_verse_text ($usfmString, $vs);
}


$usfm = $usfmArray [$verse];
if ($usfm == "") die;


$usfm = Filter_Usfm::moveNote ($usfm, $move, $focus);
if ($usfm == "") die;


$usfmArray [$verse] = $usfm;


$usfm = implode ("\n", $usfmArray);
Bible_Logic::storeChapter (bible, book, chapter, $usfm);


?>
