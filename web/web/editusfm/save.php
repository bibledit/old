<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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
page_access_level (TRANSLATOR_LEVEL);
$database_config_user = Database_Config_User::getInstance ();
$database_logs = Database_Logs::getInstance ();
$bible = $_POST['bible'];
$book = $_POST['book'];
$chapter = $_POST['chapter'];
$usfm = $_POST['usfm'];
if (isset ($bible) && isset ($book) && isset ($chapter) && isset ($usfm)) {
  $usfm = trim ($usfm);
  if ($usfm != "") {
    if (Validate_Utf8::valid ($usfm)) {
      $stylesheet = $database_config_user->getStylesheet();
      $book_chapter_text = Filter_Usfm::import ($usfm, $stylesheet);
      foreach ($book_chapter_text as $data) {
        $book_number = $data[0];
        $chapter_number = $data[1];
        $chapter_data_to_save = $data[2];
        if ((($book_number == $book) || ($book_number == 0)) && ($chapter_number == $chapter)) {
          $saved = Filter_Bibles::safeStoreChapter ($bible, $book, $chapter, $chapter_data_to_save);
          if ($saved) {
            echo gettext ("Saved");
          } else {
            echo gettext ("Not saved because of too many changes");
          }
        } else {
          echo gettext ("Save failure");
          $database_logs->log ("The following data could not be saved and was discarded: " . $chapter_data_to_save);
        }
      }
    } else {
      echo gettext ("Save failure");
      $database_logs->log ("The text was not valid Unicode UTF-8. The chapter could not saved and has been reverted to the last good version.");
    }
  } else {
    echo gettext ("Nothing to save");
    $database_logs->log ("There was no text. Nothing was saved. The original text of the chapter was reloaded.");
  }
} else {
  echo gettext ("Nothing to save");
}
?>
