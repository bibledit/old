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



class Assets_Navigator
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Assets_Navigator();
    }
    return self::$instance;
  }
  
  public function actions ()
  {
    $this->handle_switch_bible ();
    $this->handle_switch_book ();
  }

  private function handle_switch_bible ()
  {
    $switchbible = $_GET['switchbible'];
    if (isset ($switchbible)) {
      if ($switchbible == "") {
        $dialog_list = new Dialog_List2 (gettext ("Would you like to change to another Bible?"));
        $database_bibles = Database_Bibles::getInstance();
        $all_bibles = $database_bibles->getBibles();
        foreach ($all_bibles as $value) {
          $dialog_list->add_row ($value, "&switchbible=$value");
        }
        $dialog_list->run ();
      } else {
        $database_config_user = Database_Config_User::getInstance();
        $database_config_user->setBible ($switchbible);
        $database_sessions = Database_Sessions::getInstance();
        $database_sessions->setValue ('bible', $switchbible);
      }
    }
  }
  
  /**
  * Returns the selected Bible.
  */
  public function bible ()
  {
    // Take the Bible from the session.
    $database_sessions = Database_Sessions::getInstance();
    $bible = $database_sessions->getValue ('bible');
    // Else take the Bible from the user's settings.
    if ($bible == "") {
      $database_config_user = Database_Config_User::getInstance();
      $bible = $database_config_user->getBible ();
    }
    // Else take the first available Bible.
    if ($bible == "") {
      $database_bibles = Database_Bibles::getInstance();
      $bibles = $database_bibles->getBibles();
      if (is_array ($bibles)) {
        $bible = $bibles[0];
      }
      $database_config_user = Database_Config_User::getInstance();
      $database_config_user->setBible ($bible);
    }
    // The result.
    return $bible;
  }

  private function handle_switch_book () // Todo
  {
    $switchbook = $_GET['switchbook'];
    if (isset ($switchbook)) {
      if ($switchbook == "") {
        $database_bibles = Database_Bibles::getInstance ();
        $dialog_books = new Dialog_Books2 (gettext ("Go to another book"), "", "", "switchbook", $database_bibles->getBooks ($this->bible()), NULL);
      } else {
        $ipc_focus = Ipc_Focus::getInstance();
        $ipc_focus->set ($switchbook, 1, 1);
      }
    }
  }
      
  /**
  * Returns the selected book from the navigator.
  */
  public function book ()
  {
    // Take the book from the user's focus.
    $ipc_focus = Ipc_Focus::getInstance();
    $book = $ipc_focus->getBook ();
    // Else take book number 1. This may not be in the active Bible.
    if (!is_numeric ($book)) {
      $book = 1;
    }
    // The result.
    return $book;
  }


  public function chapter () 
  {
    // Take the book from the query.
    $chapter = $_GET['chapter'];
    // Else take the chapter from the user's focus.
    if (!is_numeric ($chapter)) {
      $ipc_focus = Ipc_Focus::getInstance();
      $chapter = $ipc_focus->getChapter ();
    }
    // Else take chapter number 1. This may not be in the active Bible.
    if (!is_numeric ($chapter)) {
      $chapter = 1;
    }
    // The result.
    return $chapter;
  }
 
  
  public function verse () 
  {
    // Take the verse from the query.
    $verse = $_GET['verse'];
    // Else take the verse from the user's focus.
    if (!is_numeric ($verse)) {
      $ipc_focus = Ipc_Focus::getInstance();
      $verse = $ipc_focus->getVerse ();
    }
    // Else take verse number 1. This may not be in the active Bible.
    if (!is_numeric ($verse)) {
      $verse = 1;
    }
    // The result.
    return $verse;
  }
  
  
  public function display ()
  {
    $database_books = Database_Books::getInstance();
    $smarty = new Smarty_Bibledit (__FILE__);
    $database_sessions = Database_Sessions::getInstance();
    $smarty->assign ("session", $database_sessions->getCurrentSessionId ());

    $bible = $this->bible();
    $smarty->assign ("bible", $bible);

    $book = $this->book();
    $smarty->assign ("book",  $book);
    $smarty->assign ("book_name", gettext ($database_books->getEnglishFromId ($book)));
    if ($book > 1) $previous_book = $book - 1;
    $next_book = $book + 1;
    if (!in_array ($next_book, $database_books->getIDs())) unset ($next_book);
    $smarty->assign ("previous_book", $previous_book);
    $smarty->assign ("next_book", $next_book);

    $chapter = $this->chapter();
    $smarty->assign ("chapter", $chapter);
    if ($chapter > 0) $previous_chapter = $chapter - 1;
    $next_chapter = $chapter + 1;
    if ($next_chapter > 150) unset ($next_chapter);
    $smarty->assign ("previous_chapter", $previous_chapter);
    $smarty->assign ("next_chapter", $next_chapter);

    $verse = $this->verse();
    $smarty->assign ("verse", $verse);
    if ($verse > 0) $previous_verse = $verse - 1;
    $next_verse = $verse + 1;
    if ($next_verse > 184) unset ($next_verse);
    $smarty->assign ("previous_verse", $previous_verse);
    $smarty->assign ("next_verse", $next_verse);

    $smarty->display ("navigator.tpl");
  }
  
}


/*

// Chapter.
$chapter = $_GET['chapter'];
$newchapter = $_GET['newchapter'];
if (isset ($newchapter)) {
  if ($newchapter == "") {
    $dialog_list = new Dialog_List (array ("bible", "book", "chapter", "verse"), gettext ("Go to another chapter"), "", "", true);
    $all_chapters = $database_bibles->getChapters($bible, $book);
    foreach ($all_chapters as $chapter) {
      $dialog_list->add_row ($chapter, "&newchapter=$chapter");
    }
    $dialog_list->run ();
    die;
  } else {
    $chapter = $newchapter;
  }
}
if (!isset ($chapter)) {
  $chapter = $ipc_focus->getChapter();
  if (!is_numeric ($chapter)) {
    $chapter = 1;
  }
}

// Verse.
$verse = $_GET['verse'];
$newverse = $_GET['newverse'];
if (isset ($newverse)) {
  if ($newverse == "") {
    $dialog_list = new Dialog_List (array ("bible", "book", "chapter", "verse"), gettext ("Go to another verse"), "", "", true);
    $all_verses = Filter_Usfm::getVerseNumbers ($database_bibles->getChapter ($bible, $book, $chapter));
    foreach ($all_verses as $verse) {
      $dialog_list->add_row ($verse, "&newverse=$verse");
    }
    $dialog_list->run ();
    die;
  } else {
    $verse = $newverse;
  }
}
if (!isset ($verse)) {
  $verse = $ipc_focus->getVerse();
  if ($verse == "") {
    $verse = 1;
  }
}






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
*/

?>
