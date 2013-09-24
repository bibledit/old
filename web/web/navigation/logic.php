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

require_once ("bootstrap/bootstrap.php");

class Navigation_Logic
{

  public static function getContainer ()
  {
    return '<span id="bibleditnavigation"></span>';
  }
  
  public static function getNavigator ($bible)
  {
    $ipc_focus = Ipc_Focus::getInstance();
    $database_books = Database_Books::getInstance ();
    $database_bibles = Database_Bibles::getInstance ();
    $database_navigation = Database_Navigation::getInstance ();
    $session_logic = Session_Logic::getInstance ();
    
    $user = $session_logic->currentUser ();

    $fragment = '';

    // Links to go back and forward are grayed out or active depending on available passages to go to.
    $fragment .= " ";
    if ($database_navigation->previousExists ($user)) {
      $fragment .= '<a id="navigateback" href="navigateback" title="' . gettext ("Back") . '">↶</a>';
    } else {
      $fragment .= '<span class="grayedout">↶</span>';
    }
    $fragment .= " ";
    if ($database_navigation->nextExists ($user)) {
      $fragment .= '<a id="navigateforward" href="navigateforward" title="' . gettext ("Forward") . '">↷</a>';
    } else {
      $fragment .= '<span class="grayedout">↷</span>';
    }
    $fragment .= " ";
    
    if ($bible != "") {
      $fragment .= '<a id="selectbible" href="selectbible" title="' . gettext ("Select Bible") . '">' . $bible . '</a>';
      $fragment .= " ";
    }
    

    $book = $ipc_focus->getBook ();

    // Check that the book exists in the Bible.
    if ($bible != "") {
      $books = $database_bibles->getBooks ($bible);
      if (!in_array ($book, $books)) {
        if (count ($books) > 0) $book = $books [0];
        else $book = 0;
      }
    }

    $bookName = $database_books->getEnglishFromId ($book);
    $fragment .= '<a id="selectbook" href="selectbook" title="' . gettext ("Select book") . '">' . $bookName . '</a>';

    $fragment .= " ";

    $chapter = $ipc_focus->getChapter ();
    
    // Ensure that the chapter exists in the book.
    if ($bible != "") {
      $chapters = $database_bibles->getChapters ($bible, $book);
      if (!in_array ($chapter, $chapters)) {
        if (count ($chapters) > 0) $chapter = $chapters [0];
        else $chapter = 1;
      }
    }

    $fragment .= '<a id="selectchapter" href="selectchapter" title="' . gettext ("Select chapter") . '">' . $chapter . '</a>';

    $fragment .= ":";

    $verse = $ipc_focus->getVerse ();
    $fragment .= '<a id="selectverse" href="selectverse" title="' . gettext ("Select verse") . '">' . $verse . '</a>';

    return $fragment;
  }
  
  public static function getBiblesFragment ($bible)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $bibles = $database_bibles->getBibles ();
    $activeBible = $bible;
    $html = gettext ("Select Bible") . " " . '<span id="selectbibles">';
    foreach ($bibles as $offset => $bible) {
      if ($offset) $html .= " | ";
      if ($bible == $activeBible) $html .= "<mark>";
      $html .= '<a href="bible">' . $bible . '</a>';
      if ($bible == $activeBible) $html .= "</mark>";
    }
    $html .= "</span>";
    return $html;    
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
      $books = $database_bibles->getBooks ($bible);
    }
    $html = gettext ("Select book") . " " . '<span id="selectbooks">';
    foreach ($books as $offset => $book) {
      $bookName = $database_books->getEnglishFromId ($book);
      if ($offset) $html .= " | ";
      if ($book == $activeBook) $html .= "<mark>";
      $html .= '<a id="book' . $book . '" href="book">' . $bookName . '</a>';
      if ($book == $activeBook) $html .= "</mark>";
    }
    $html .= "</span>";
    return $html;    
  }
  
  public static function getChaptersFragment ($bible)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $ipc_focus = Ipc_Focus::getInstance();
    $book = $ipc_focus->getBook ();
    $activeChapter = $ipc_focus->getChapter ();
    if ($bible == "") {
      $database_versifications = Database_Versifications::getInstance ();
      $chapters = $database_versifications->getChapters ("English", $book, true);
    } else {
      $chapters = $database_bibles->getChapters ($bible, $book);
    }
    $html = gettext ("Chapter") . ' <span id="selectchapters">';
    foreach ($chapters as $offset => $chapter) {
      if ($offset) $html .= " | ";
      if ($chapter == $activeChapter) $html .= "<mark>";
      $html .= '<a id="chapter' . $chapter . '" href="chapter">' . $chapter . '</a>';
      if ($chapter == $activeChapter) $html .= "</mark>";
    }
    $html .= "</span>";
    return $html;
  }

  public static function getVersesFragment ($bible)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $ipc_focus = Ipc_Focus::getInstance();
    $book = $ipc_focus->getBook ();
    $chapter = $ipc_focus->getChapter ();
    $activeVerse = $ipc_focus->getVerse ();
    if ($bible == "") {
      $database_versifications = Database_Versifications::getInstance ();
      $verses = $database_versifications->getVerses ("English", $book, $chapter);
    } else {
      $verses = Filter_Usfm::getVerseNumbers ($database_bibles->getChapter ($bible, $book, $chapter));
    }
    $html = gettext ("Verse") . ' <span id="selectverses">';
    foreach ($verses as $offset => $verse) {
      if ($offset) $html .= " | ";
      if ($verse == $activeVerse) $html .= "<mark>";
      $html .= '<a id="verse' . $verse . '" href="verse">' . $verse . '</a>';
      if ($verse == $activeVerse) $html .= "</mark>";
    }
    $html .= "</span>";
    return $html;
  }
  
  public static function code ($bible)
  {
    $ipc_focus = Ipc_Focus::getInstance();
    $code = '<script type="text/javascript" src="../navigation/code.js"></script>';
    $code .= "\n";
    $code .= '<script type="text/javascript">';
    $code .= "\n";
    $code .= 'navigationBible = "' . $bible . '";';
    $code .= "\n";
    $code .= '</script>';
    $code .= "\n";
    return $code;
  }

  public static function setBook ($book)
  {
    $book = Filter_Numeric::integer_in_string ($book);
    $ipc_focus = Ipc_Focus::getInstance();
    $ipc_focus->set ($book, 1, 1);
    Navigation_Logic::recordHistory ($book, 1, 1);
  }
    
  public static function setChapter ($chapter)
  {
    $ipc_focus = Ipc_Focus::getInstance();
    $book = $ipc_focus->getBook ();
    $chapter = Filter_Numeric::integer_in_string ($chapter);
    $ipc_focus->set ($book, $chapter, 1);
    Navigation_Logic::recordHistory ($book, $chapter, 1);
  }
    
  public static function setVerse ($verse)
  {
    $ipc_focus = Ipc_Focus::getInstance();
    $book = $ipc_focus->getBook ();
    $chapter = $ipc_focus->getChapter ();
    $verse = Filter_Numeric::integer_in_string ($verse);
    $ipc_focus->set ($book, $chapter, $verse);
    Navigation_Logic::recordHistory ($book, $chapter, $verse);
  }
  
  public static function getEntry ()
  {
    $html = "";
    $html .= '<input name="selectpassage" id="selectpassage" type="text" value=""/>';
    $html .= '<input name="submitpassage" id="submitpassage"  type="submit" value="' . gettext ("OK") . '" />';
    return $html;
  }
  
   
  public static function setPassage ($bible, $passage)
  {
    $ipc_focus = Ipc_Focus::getInstance();
    $currentBook = $ipc_focus->getBook ();
    $currentChapter = $ipc_focus->getChapter ();
    $currentVerse = $ipc_focus->getVerse ();
    $passage = trim ($passage);
    if (($passage == "") || ($passage == "+")) {
      $passage = Navigation_Logic::nextVerse ($bible, $currentBook, $currentChapter, $currentVerse);
    } else if ($passage == "-") {
      $passage = Navigation_Logic::previousVerse ($bible, $currentBook, $currentChapter, $currentVerse);
    } else {
      $passage = Filter_Books::interpretPassage (array ($currentBook, $currentChapter, $currentVerse), $passage);
    }
    if ($passage[0] != 0) {
      $ipc_focus->set ($passage [0], $passage [1], $passage [2]);
      Navigation_Logic::recordHistory ($passage [0], $passage [1], $passage [2]);

    }
  }


  private static function nextVerse ($bible, $book, $chapter, $verse)
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


  private static function previousVerse ($bible, $book, $chapter, $verse)
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
