<?php
class Navigation_Passage
{


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


  public static function setBook ($book)
  {
    $book = Filter_Numeric::integer_in_string ($book);
    $ipc_focus = Ipc_Focus::getInstance();
    $ipc_focus->set ($book, 1, 1);
    Navigation_Passage::recordHistory ($book, 1, 1);
  }

    
  public static function setChapter ($chapter)
  {
    $ipc_focus = Ipc_Focus::getInstance();
    $book = $ipc_focus->getBook ();
    $chapter = Filter_Numeric::integer_in_string ($chapter);
    $ipc_focus->set ($book, $chapter, 1);
    Navigation_Passage::recordHistory ($book, $chapter, 1);
  }

    
  public static function setVerse ($verse)
  {
    $ipc_focus = Ipc_Focus::getInstance();
    $book = $ipc_focus->getBook ();
    $chapter = $ipc_focus->getChapter ();
    $verse = Filter_Numeric::integer_in_string ($verse);
    $ipc_focus->set ($book, $chapter, $verse);
    Navigation_Passage::recordHistory ($book, $chapter, $verse);
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
