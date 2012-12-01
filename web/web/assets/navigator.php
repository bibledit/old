<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2012 Teus Benschop.
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
    $this->handle_switch_chapter ();
    $this->handle_switch_verse ();
  }

  private function handle_switch_bible ()
  {
    @$switchbible = $_GET['switchbible'];
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
        $database_sessions->setBible ($switchbible);
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
    $bible = $database_sessions->getBible ();
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

  private function handle_switch_book ()
  {
    @$switchbook = $_GET['switchbook'];
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


  private function handle_switch_chapter ()
  {
    @$switchchapter = $_GET['switchchapter'];
    if (isset ($switchchapter)) {
      if ($switchchapter == "") {
        $dialog_list = new Dialog_List2 (gettext ("Go to another chapter"));
        $dialog_list->horizontal ();
        $database_bibles = Database_Bibles::getInstance();
        $all_chapters = $database_bibles->getChapters($this->bible(), $this->book());
        foreach ($all_chapters as $chapter) {
          $dialog_list->add_row ($chapter, "&switchchapter=$chapter");
        }
        $dialog_list->run ();
      } else {
        $ipc_focus = Ipc_Focus::getInstance();
        $ipc_focus->set ($this->book(), $switchchapter, 1);
      }
    }
  }


  public function chapter () 
  {
    // Take the chapter from the user's focus.
    $ipc_focus = Ipc_Focus::getInstance();
    $chapter = $ipc_focus->getChapter ();
    // Else take chapter number 1. This may not be in the active Bible.
    if (!is_numeric ($chapter)) {
      $chapter = 1;
    }
    // The result.
    return $chapter;
  }
 
  
  private function handle_switch_verse ()
  {
    @$switchverse = $_GET['switchverse'];
    if (isset ($switchverse)) {
      if ($switchverse == "") {
        $dialog_list = new Dialog_List2 (gettext ("Go to another verse"));
        $dialog_list->horizontal ();
        $database_bibles = Database_Bibles::getInstance();
        $all_verses = Filter_Usfm::getVerseNumbers ($database_bibles->getChapter ($this->bible(), $this->book(), $this->chapter()));
        foreach ($all_verses as $verse) {
          $dialog_list->add_row ($verse, "&switchverse=$verse");
        }
        $dialog_list->run ();
      } else {
        $ipc_focus = Ipc_Focus::getInstance();
        $ipc_focus->set ($this->book(), $this->chapter(), $switchverse);
      }
    }
  }
  
  
  public function verse () 
  {
    // Take the verse from the user's focus.
    $ipc_focus = Ipc_Focus::getInstance();
    $verse = $ipc_focus->getVerse ();
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
    $database_bibles = Database_Bibles::getInstance();
    $database_sessions = Database_Sessions::getInstance();

    $smarty = new Smarty_Bibledit (__FILE__);
    $smarty->assign ("session", $database_sessions->getCurrentSessionId ());

    $smarty->assign ("caller", $_SERVER["PHP_SELF"]);

    $bible = $this->bible();
    $smarty->assign ("bible", $bible);

    $book = $this->book();
    $smarty->assign ("book",  $book);
    $smarty->assign ("book_name", gettext ($database_books->getEnglishFromId ($book)));
    $books = $database_bibles->getBooks($bible);
    $key = array_search ($book, $books);
    $previous_book = "";
    $next_book = "";
    if ($key !== false) {
      @$previous_book = $books [$key - 1];
      @$next_book = $books [$key + 1];
    }
    $smarty->assign ("previous_book", $previous_book);
    $smarty->assign ("next_book", $next_book);

    $chapter = $this->chapter();
    $smarty->assign ("chapter", $chapter);
    $chapters = $database_bibles->getChapters ($bible, $book);
    $key = array_search ($chapter, $chapters);
    $previous_chapter = "";
    $next_chapter = "";
    if ($key !== false) {
      @$previous_chapter = $chapters [$key - 1];
      @$next_chapter = $chapters [$key + 1];
    }
    $smarty->assign ("previous_chapter", $previous_chapter);
    $smarty->assign ("next_chapter", $next_chapter);

    $verse = $this->verse();
    $smarty->assign ("verse", $verse);
    $verses = Filter_Usfm::getVerseNumbers ($database_bibles->getChapter ($bible, $book, $chapter));
    $key = array_search ($verse, $verses);
    $previous_verse = "";
    $next_verse = "";
    if ($key !== false) {
      @$previous_verse = $verses [$key - 1];
      @$next_verse = $verses [$key + 1];
    }
    $smarty->assign ("previous_verse", $previous_verse);
    $smarty->assign ("next_verse", $next_verse);

    $smarty->display ("navigator.tpl");
  }


}


?>