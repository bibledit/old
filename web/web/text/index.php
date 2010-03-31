<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2009 Teus Benschop.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */

require_once ("../bootstrap/bootstrap.php");
page_access_level (TRANSLATOR_LEVEL);

$smarty = new Smarty_Bibledit (__FILE__);

$database_books = Database_Books::getInstance();
$database_bibles = Database_Bibles::getInstance();
$database_snapshots = Database_Snapshots::getInstance();
$database_config_user = Database_Config_User::getInstance();
$ipc_focus = Ipc_Focus::getInstance();


$bible = $_GET['bible'];
$newbible = $_GET['newbible'];
if (isset ($newbible)) {
  if ($newbible == "") {
    $dialog_list = new Dialog_List (array ("bible", "book", "chapter", "verse"), gettext ("Would you like to change to another Bible?"), "", "");
    $all_bibles = $database_bibles->getBibles();
    foreach ($all_bibles as $value) {
      $dialog_list->add_row ($value, "&newbible=$value");
    }
    $dialog_list->run ();
    die;
  } else {
    $bible = $newbible;
    $database_config_user->setBible ($bible);
  }
}


$book = $_GET['book'];
if (!isset ($book)) {
  $book = $ipc_focus->getBook ();
  if (!is_numeric ($book)) {
    $book = 1;
  }
}
$chapter = $_GET['chapter'];
if (!isset ($chapter)) {
  $chapter = $ipc_focus->getChapter();
  if (!is_numeric ($chapter)) {
    $chapter = 1;
  }
}
$verse = $_GET['verse'];
if (!isset ($verse)) {
  $verse = $ipc_focus->getVerse();
  if ($verse == "") {
    $verse = 1;
  }
}

$smarty->assign ("bible", $bible);
$smarty->assign ("book", $book);
$smarty->assign ("chapter", $chapter);
$smarty->assign ("verse", $verse);

$smarty->assign ("book_name", gettext ($database_books->getEnglishFromId ($book)));

if ($book > 1) $previous_book = $book - 1;
$next_book = $book + 1;
if (!in_array ($next_book, $database_books->getIDs())) unset ($next_book);
if ($chapter > 0) $previous_chapter = $chapter - 1;
$next_chapter = $chapter + 1;
if ($next_chapter > 150) unset ($next_chapter);
if ($verse > 0) $previous_verse = $verse - 1;
$next_verse = $verse + 1;
if ($next_verse > 184) unset ($next_verse);

$smarty->assign ("previous_book", $previous_book);
$smarty->assign ("previous_chapter", $previous_chapter);
$smarty->assign ("previous_verse", $previous_verse);

$smarty->assign ("next_book", $next_book);
$smarty->assign ("next_chapter", $next_chapter);
$smarty->assign ("next_verse", $next_verse);



$chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
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
        if ((($book_number == $book) || ($book_number == 0)) && ($chapter_number == $chapter)) {
          $database_bibles->storeChapter ($bible, $book, $chapter, $chapter_data_to_save);
          $database_snapshots->snapChapter ($bible, $book, $chapter, $chapter_data_to_save, 0);
          $success_message .= " " . gettext ("Chapter was saved."); 
          $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
        } else {
          $error_message .= " $chapter_data_to_save"; 
        }
      }
      if ($error_message != "") {
        $error_message = gettext ("The following data could not be saved and was discarded:") . $error_message;
      }
    } else {
      $error_message = gettext ("The text was not valid Unicode UTF-8. The chapter could not saved.");
      $chapter_data = $data;
    }
  } else {
    $error_message = gettext ("There was no text. Nothing was saved. The original text of the chapter was reloaded.");
  }
}
$smarty->assign ("chapter_data", $chapter_data);
if ($chapter_data == "") {
  $error_message .= " " . gettext ("This chapter does not exist or is empty");
}



$ipc_focus->set ($book, $chapter, $verse);

$smarty->assign ("success_message", $success_message);
$smarty->assign ("error_message", $error_message);
$smarty->display ("index.tpl");

?>
