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


$bible = request->post['bible'];
$book = request->post['book'];
$chapter = request->post['chapter'];
$usfm = request->post['usfm'];
$checksum = request->post['checksum'];


if (isset ($bible) && isset ($book) && isset ($chapter) && isset ($usfm)) {
  if (Checksum_Logic::get ($usfm) == $checksum) {
    $usfm = filter_string_trim ($usfm);
    if ($usfm != "") {
      if (unicode_string_is_valid ($usfm)) {
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
            Database_Logs::log ("The following data could not be saved and was discarded: " . $chapter_data_to_save);
          }
        }
      } else {
        echo gettext("Save failure");
        Database_Logs::log ("The text was not valid Unicode UTF-8. The chapter could not saved and has been reverted to the last good version.");
      }
    } else {
      echo gettext("Nothing to save");
      Database_Logs::log ("There was no text. Nothing was saved. The original text of the chapter was reloaded.");
    }
  } else {
    http_response_code (409);
    echo gettext("Checksum error");
  }
} else {
  echo gettext("Nothing to save");
}


?>
