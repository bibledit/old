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


require_once ("../bootstrap/bootstrap.php");
page_access_level (Filter_Roles::translator ());


$database_config_user = Database_Config_User::getInstance ();
$database_logs = Database_Logs::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_modifications = Database_Modifications::getInstance ();
$session_logic = Session_Logic::getInstance ();


$bible = $_POST['bible'];
$book = $_POST['book'];
$chapter = $_POST['chapter'];
$verse = $_POST['verse'];
$usfm = $_POST['usfm'];
$checksum = $_POST['checksum'];


// Check on information about where to save the verse.
$save = (isset ($bible) && isset ($book) && isset ($chapter) && isset ($verse) && isset ($usfm));
if (!$save) {
  echo gettext("Don't know where to save");
  die;
}


// Checksum.
if (Checksum_Logic::get ($usfm) != $checksum) {
  http_response_code (409);
  echo gettext("Checksum error");
  die;
}


// Check there's anything to save at all.
$usfm = trim ($usfm);
if ($usfm == "") {
  echo gettext("Nothing to save");
  die;
}


// Check on valid UTF-8.
if (!Validate_Utf8::valid ($usfm)) {
  echo gettext("Cannot save: Needs Unicode");
  die;
}


// Get the old chapter USFM into an array of verse => USFM fragment.
$usfmString = $database_bibles->getChapter ($bible, $book, $chapter);
$verses = usfm_get_verse_numbers ($usfmString);
$verses = array_unique ($verses);
sort ($verses, SORT_NUMERIC);
$usfmArray = array ();
for ($verses as $vs) {
  $usfmArray [$vs] = usfm_get_verse_text ($usfmString, $vs);
}


// Store the verse USFM in the array.
$usfmArray [$verse] = $usfm;


// Create the updated chapter USFM as a string.
$usfm = implode ("\n", $usfmArray);


$stylesheet = $database_config_user->getStylesheet();
$book_chapter_text = usfm_import ($usfm, $stylesheet);
for ($book_chapter_text as $data) {
  $book_number = $data[0];
  $chapter_number = $data[1];
  $chapter_data_to_save = $data[2];
  if ((($book_number == $book) || ($book_number == 0)) && ($chapter_number == $chapter)) {
    // Collect some data about the changes for this user.
    $username = $session_logic->currentUser ();
    $oldID = $database_bibles->getChapterId ($bible, $book, $chapter);
    $oldText = $database_bibles->getChapter ($bible, $book, $chapter);
    // Safely store the chapter.
    $saved = Filter_Bibles::safeStoreChapter ($bible, $book, $chapter, $chapter_data_to_save);
    if ($saved) {
      // Store details for the user's changes.
      $newID = $database_bibles->getChapterId ($bible, $book, $chapter);
      $newText = $chapter_data_to_save;
      $database_modifications->recordUserSave ($username, $bible, $book, $chapter, $oldID, $oldText, $newID, $newText);
      echo gettext("Saved");
    } else {
      echo gettext("Not saved because of too many changes");
    }
  } else {
    echo gettext("Save failure");
    $database_logs->log ("The following data could not be saved and was discarded: " . $chapter_data_to_save);
  }
}


?>
