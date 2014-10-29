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


require_once ("bootstrap/bootstrap.php");


class Navigation_Passage
{


  public static function getContainer ()
  {
    return '<div id="versepickerwrapper"></div>';
  }


  public static function getNavigator ($bible)
  {
    $ipc_focus = Ipc_Focus::getInstance();
    $database_books = Database_Books::getInstance ();
    $database_bibles = Database_Bibles::getInstance ();
    $database_navigation = Database_Navigation::getInstance ();
    $session_logic = Session_Logic::getInstance ();

    $user = $session_logic->currentUser ();

    $fragments = array ();

    // Links to go back and forward are grayed out or active depending on available passages to go to.
    $fragments [] = '<div id="backforward">';
    if ($database_navigation->previousExists ($user)) {
      $fragments [] = '<a id="navigateback" href="navigateback" title="' . Locale_Translate::_("Back") . '">↶</a>';
    } else {
      $fragments [] = '<span class="grayedout">↶</span>';
    }
    $fragments [] = "";
    if ($database_navigation->nextExists ($user)) {
      $fragments [] = '<a id="navigateforward" href="navigateforward" title="' . Locale_Translate::_("Forward") . '">↷</a>';
    } else {
      $fragments [] = '<span class="grayedout">↷</span>';
    }
    $fragments [] = '</div>';

    $book = $ipc_focus->getBook ();

    // The book should exist in the Bible.
    if ($bible != "") {
      $books = $database_bibles->getBooks ($bible);
      if (!in_array ($book, $books)) {
        if (count ($books) > 0) $book = $books [0];
        else $book = 0;
      }
    }

    $bookName = $database_books->getEnglishFromId ($book);

    $chapter = $ipc_focus->getChapter ();

    // The chapter should exist in the book.
    if ($bible != "") {
      $chapters = $database_bibles->getChapters ($bible, $book);
      if (!in_array ($chapter, $chapters)) {
        if (count ($chapters) > 0) $chapter = $chapters [0];
        else $chapter = 1;
      }
    }

    $verse = $ipc_focus->getVerse ();

    $fragments [] = '<div id="versepicker">';

    $fragments [] = '<input name="selectpassage" id="selectpassage" type="text" value="' . "$bookName $chapter:$verse" . '" size="14" />';
    $fragments [] = '<input name="submitpassage" id="submitpassage" type="submit" value="' . Locale_Translate::_("Go") . '" />';

    $fragments [] = '<div id="handpicker">';

    $fragments [] = '<select id="booklist" size=15>';
    $fragments [] = self::getBooksFragment ($bible);
    $fragments [] = '</select>';

    $fragments [] = '<select id="chapterlist" size=15>';
    $fragments [] = self::getChaptersFragment ($bible, $book, $chapter);
    $fragments [] = '</select>';

    $fragments [] = '<select id="verselist" size=15 multiple>';
    $fragments [] = self::getVersesFragment ($bible, $book, $chapter, $verse);

    $fragments [] = '</select>';

    $fragments [] = '</div>';

    $fragments [] = '</div>';

    // The result.
    return implode ("\n", $fragments);
  }


  public static function getBooksFragment ($bible)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_books = Database_Books::getInstance ();
    $ipc_focus = Ipc_Focus::getInstance();
    $activeBook = $ipc_focus->getBook ();
    // Take standard books in case of no Bible.
    if ($bible == "") {
      $books = $database_books->getIDs ();
    } else {
      $books = Filter_Books::getOrdered ($bible);
    }
    $html = "";
    foreach ($books as $book) {
      $bookName = $database_books->getEnglishFromId ($book);
      $selected = "";
      if ($book == $activeBook) $selected = " selected";
      $html .= "<option$selected>" . $bookName . "</option>";
    }
    return $html;
  }


  public static function getChaptersFragment ($bible, $book, $chapter)
  {
    if ($bible == "") {
      $database_versifications = Database_Versifications::getInstance ();
      $chapters = $database_versifications->getChapters ("English", $book, true);
    } else {
      $database_bibles = Database_Bibles::getInstance ();
      $chapters = $database_bibles->getChapters ($bible, $book);
    }
    $html = "";
    foreach ($chapters as $ch) {
      $selected = "";
      if ($ch == $chapter) $selected = " selected";
      $html .= "<option$selected>" . $ch . "</option>";;
    }
    return $html;
  }


  public static function getVersesFragment ($bible, $book, $chapter, $verse)
  {
    $database_bibles = Database_Bibles::getInstance ();
    if ($bible == "") {
      $database_versifications = Database_Versifications::getInstance ();
      $verses = $database_versifications->getVerses ("English", $book, $chapter);
    } else {
      $verses = Filter_Usfm::getVerseNumbers ($database_bibles->getChapter ($bible, $book, $chapter));
    }
    $html = "";
    foreach ($verses as $vs) {
      $selected = "";
      if ($verse == $vs) $selected = " selected";
      $html .= "<option$selected>" . $vs . "</option>";;
    }
    return $html;
  }


  public static function code ($bible, $header = false)
  {
    $code = '';
    if ($header) {
      $code .= '<script type="text/javascript" src="../navigation/passage.js"></script>';
      $code .= "\n";
    }
    $code .= '<script type="text/javascript">';
    $code .= "\n";
    $code .= 'navigationBible = "' . $bible . '";';
    $code .= "\n";
    $code .= '</script>';
    $code .= "\n";
    return $code;
  }


  public static function setBookChapterVerse ($book, $chapter, $verse)
  {
    $book = Filter_Numeric::integer_in_string ($book);
    $chapter = Filter_Numeric::integer_in_string ($chapter);
    $verse = Filter_Numeric::integer_in_string ($verse);
    $ipc_focus = Ipc_Focus::getInstance();
    $ipc_focus->set ($book, $chapter, $verse);
    Navigation_Passage::recordHistory ($book, $chapter, $verse);
  }


  public static function setPassage ($bible, $passage)
  {
    $ipc_focus = Ipc_Focus::getInstance();
    $currentBook = $ipc_focus->getBook ();
    $currentChapter = $ipc_focus->getChapter ();
    $currentVerse = $ipc_focus->getVerse ();
    $passage = trim ($passage);
    if (($passage == "") || ($passage == "+")) {
      $passage = Navigation_Passage::getNextVerse ($bible, $currentBook, $currentChapter, $currentVerse);
    } else if ($passage == "-") {
      $passage = Navigation_Passage::getPreviousVerse ($bible, $currentBook, $currentChapter, $currentVerse);
    } else {
      $passage = Filter_Books::interpretPassage (array ($currentBook, $currentChapter, $currentVerse), $passage);
    }
    if ($passage[0] != 0) {
      $ipc_focus->set ($passage [0], $passage [1], $passage [2]);
      Navigation_Passage::recordHistory ($passage [0], $passage [1], $passage [2]);

    }
  }


  private static function getNextVerse ($bible, $book, $chapter, $verse)
  {
    $verse++;
    if ($bible != "") {
      $database_bibles = Database_Bibles::getInstance ();
      $verses = Filter_Usfm::getVerseNumbers ($database_bibles->getChapter ($bible, $book, $chapter));
      if (!in_array ($verse, $verses)) {
        $verse = array_pop ($verses);
      }
    }
    $passage = array ($book, $chapter, $verse);
    return $passage;
  }


  private static function getPreviousVerse ($bible, $book, $chapter, $verse)
  {
    $verse--;
    if ($bible != "") {
      $database_bibles = Database_Bibles::getInstance ();
      $verses = Filter_Usfm::getVerseNumbers ($database_bibles->getChapter ($bible, $book, $chapter));
      if (!in_array ($verse, $verses)) {
        $verse = array_shift ($verses);
      }
    }
    $passage = array ($book, $chapter, $verse);
    return $passage;
  }


  public static function gotoNextVerse ($bible)
  {
    $ipc_focus = Ipc_Focus::getInstance();
    $currentBook = $ipc_focus->getBook ();
    $currentChapter = $ipc_focus->getChapter ();
    $currentVerse = $ipc_focus->getVerse ();
    $passage = Navigation_Passage::getNextVerse ($bible, $currentBook, $currentChapter, $currentVerse);
    if ($passage[0] != 0) {
      $ipc_focus->set ($passage [0], $passage [1], $passage [2]);
      Navigation_Passage::recordHistory ($passage [0], $passage [1], $passage [2]);
    }
  }


  public static function gotoPreviousVerse ($bible)
  {
    $ipc_focus = Ipc_Focus::getInstance();
    $currentBook = $ipc_focus->getBook ();
    $currentChapter = $ipc_focus->getChapter ();
    $currentVerse = $ipc_focus->getVerse ();
    $passage = Navigation_Passage::getPreviousVerse ($bible, $currentBook, $currentChapter, $currentVerse);
    if ($passage[0] != 0) {
      $ipc_focus->set ($passage [0], $passage [1], $passage [2]);
      Navigation_Passage::recordHistory ($passage [0], $passage [1], $passage [2]);
    }
  }


  public static function recordHistory ($book, $chapter, $verse)
  {
    $session_logic = Session_Logic::getInstance ();
    $user = $session_logic->currentUser ();
    $database_navigation = Database_Navigation::getInstance ();
    $database_navigation->record (time (), $user, $book, $chapter, $verse);
  }


  public static function goBack ()
  {
    $database_navigation = Database_Navigation::getInstance ();
    $session_logic = Session_Logic::getInstance ();
    $user = $session_logic->currentUser ();
    $passage = $database_navigation->getPrevious ($user);
    if ($passage) {
      $ipc_focus = Ipc_Focus::getInstance();
      $ipc_focus->set ($passage [0], $passage [1], $passage [2]);
    }
  }


  public static function goForward ()
  {
    $database_navigation = Database_Navigation::getInstance ();
    $session_logic = Session_Logic::getInstance ();
    $user = $session_logic->currentUser ();
    $passage = $database_navigation->getNext ($user);
    if ($passage) {
      $ipc_focus = Ipc_Focus::getInstance();
      $ipc_focus->set ($passage [0], $passage [1], $passage [2]);
    }
  }


}


?>
