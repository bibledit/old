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
page_access_level (MANAGER_LEVEL);


$header = new Assets_Header (gettext ("Edit USFM"));
$header->jQueryOn ();
$header->run ();


$database_config_user = Database_Config_User::getInstance ();
$database_snapshots = Database_Snapshots::getInstance();
$database_bibles = Database_Bibles::getInstance();
$ipc_focus = Ipc_Focus::getInstance ();


// Save the Bible text in USFM format.
if (isset($_POST['submit'])) {
  $data = $_POST['data'];
  $data = trim ($data);
  if ($data != "") {
    if (Validate_Utf8::valid ($data)) {
      $stylesheet = $database_config_user->getStylesheet();
      $book_chapter_text = Filter_Usfm::import ($data, $stylesheet);
      foreach ($book_chapter_text as $data) {
        $book_number = $data[0];
        $chapter_number = $data[1];
        $chapter_data_to_save = $data[2];
        $bible = $_POST['bible'];
        $book = $_POST['book'];
        $chapter = $_POST['chapter'];
        if ((($book_number == $book) || ($book_number == 0)) && ($chapter_number == $chapter)) {
          $database_bibles->storeChapter ($bible, $book, $chapter, $chapter_data_to_save);
          $database_snapshots->snapChapter ($bible, $book, $chapter, $chapter_data_to_save, 0);
          Assets_Page::success (gettext ("Chapter was saved."));
        } else {
          Assets_Page::error (gettext ("The following data could not be saved and was discarded:") . $chapter_data_to_save);
        }
      }
    } else {
      Assets_Page::error (gettext ("The text was not valid Unicode UTF-8. The chapter could not saved and has been reverted to the last good version."));
    }
  } else {
    Assets_Page::error (gettext ("There was no text. Nothing was saved. The original text of the chapter was reloaded."));
  }
}


// Display the Bible text in USFM format.
$view = new Assets_View (__FILE__);
// Values for $bible, $book and $chapter are passed to the form.
// This makes it more robust. 
// Else what could happen is that a chapter got loaded, 
// in the meantime the navigator moved to somewhere else, 
// then the chapter would be saved to the wrong place.
$bible = $database_config_user->getBible ();
$view->view->bible = $bible;
$book = $ipc_focus->getBook();
$view->view->book = $book;
$chapter = $ipc_focus->getChapter();
$view->view->chapter = $chapter;
$chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
if ($chapter_data == "") {
  Assets_Page::error (gettext ("This chapter does not exist or is empty"));
}
$view->view->chapter_data = $chapter_data;


$view->view->navigationHtml = Navigation_Logic::getContainer ();
$view->view->navigationCode = Navigation_Logic::code ($bible);


$view->render ("index.php");
Assets_Page::footer ();
?>
