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
$database_config_bible = Database_Config_Bible::getInstance ();
$database_books = Database_Books::getInstance ();
$database_volatile = Database_Volatile::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$ipc_focus = Ipc_Focus::getInstance ();


$sourceBible = $database_config_user->getSourceXrefBible ();
$targetBible = $database_config_user->getTargetXrefBible ();
$book = $ipc_focus->getBook ();
$chapter = $ipc_focus->getChapter ();


// Retrieve all abbreviations for the source Bible, sort them, longest first.
// The replace routines replaces the longer strings first,
// to be sure that no partial book abbreviations are replaced.
$sourceAbbreviations = $database_config_bible->getBookAbbreviations ($sourceBible);
$sourceAbbreviations = Filter_Abbreviations::read ($sourceAbbreviations);
$sorter = array ();
foreach ($sourceAbbreviations as $abbrev => $dummy) {
  $sorter [] = mb_strlen ($abbrev);
}
array_multisort ($sorter, SORT_DESC, SORT_NUMERIC, $sourceAbbreviations);


$targetAbbreviations = $database_config_bible->getBookAbbreviations ($targetBible);
$targetAbbreviations = Filter_Abbreviations::read ($targetAbbreviations);


// Create array with book abbreviations to find, and one with their matching replacements.
$find = array ();
$replace = array ();
foreach ($sourceAbbreviations as $sourceAbbreviation => $bk) {
  $find [] = $sourceAbbreviation;
  $key = array_search ($bk, $targetAbbreviations);
  if ($key === false) {
    $replace [] = $sourceAbbreviation;
  } else {
    $replace [] = $key;
  }
}


// Storage identifier, based on the user's name.
$identifier = Filter_User::myIdentifier ();


// Retrieve all notes from the database.
$allxrefs = $database_volatile->getValue ($identifier, "sourcexrefs");
$allxrefs = unserialize ($allxrefs);


// Replace the abbreviations in the cross references.
foreach ($allxrefs as $key => $xref) {
  $allxrefs [$key] ['text'] = str_replace ($find, $replace, $allxrefs [$key] ['text']);
}


// Get the target USFM into an array of verse => USFM fragment.
$usfmArray = array ();
$usfmString = $database_bibles->getChapter ($targetBible, $book, $chapter);
$verses = Filter_Usfm::getVerseNumbers ($usfmString);
$verses = array_unique ($verses);
foreach ($verses as $verse) {
  $usfmArray [$verse] = Filter_Usfm::getVerseText ($usfmString, $verse);
}


// Go through each verse, through each note within that verse,
// look at source location, define target location, and insert the xref.
foreach ($verses as $verse) {
  
  
  // Gather array of cross references for this verse, if any.
  $xrefs = array ();
  reset ($allxrefs);
  foreach ($allxrefs as $xref) {
    if ($xref ['verse'] == $verse) {
      $xrefs [] = array ($xref ['offset'], $xref ['text']);
    }
  }
  if (empty ($xrefs)) continue;


  // Get the USFM for the current verse in the target Bible, if any.
  if (!isset ($usfmArray [$verse])) continue;
  $usfm = $usfmArray [$verse];
  
  
  // Get the length of the text of the verse in the source Bible without the xrefs.
  // Get the ratio for positioning the xrefs by comparing the lengths of source and target verse text.
  $sourceUsfm = $database_bibles->getChapter ($sourceBible, $book, $chapter);
  $sourceUsfm = Filter_Usfm::getVerseText ($sourceUsfm, $verse);
  $sourceUsfm = Filter_Usfm::removeNotes ($sourceUsfm, array ("x"));
  $sourceLength = mb_strlen ($sourceUsfm);
  $targetLength = mb_strlen ($usfm);
  $ratio = $targetLength / $sourceLength;
  
  
  // Insert the notes.
  $usfm = Filter_Usfm::insertNotes ($usfm, $xrefs, $ratio);
  $usfmArray [$verse] = $usfm;
 

}


$usfm = implode ("\n", $usfmArray);
Bible_Logic::storeChapter ($targetBible, $book, $chapter, $usfm);


$header = new Assets_Header (Locale_Translate::_("Cross references"));
$header->run ();


$view = new Assets_View (__FILE__);


$view->render ("insert.php");


Assets_Page::footer ();


?>
