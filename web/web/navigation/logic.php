<?php

require_once ("bootstrap/bootstrap.php");

class Navigation_Logic
{

  public static function getContainer ()
  {
    return '<span id="bibleditnavigation"></span>';
  }
  
  public static function getNavigator ($bible) // Todo clamp books / chapters / verse within their ranges.
  {
    $fragment = '';
    
    if ($bible != "") {
      $fragment .= '<a id="selectbible" href="selectbible">' . $bible . '</a>';
      $fragment .= " ";
    }
    
    $ipc_focus = Ipc_Focus::getInstance();
    $database_books = Database_Books::getInstance ();
    $database_bibles = Database_Bibles::getInstance ();

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
    $fragment .= '<a id="selectbook" href="selectbook">' . $bookName . '</a>';

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

    $fragment .= '<a id="selectchapter" href="selectchapter">' . $chapter . '</a>';

    $fragment .= ":";

    $verse = $ipc_focus->getVerse ();
    $fragment .= '<a id="selectverse" href="selectverse">' . $verse . '</a>';

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
      $data = $database_versifications->getBooksChaptersVerses ("English");
      $chapters = array (0);
      while ($row = $data->fetch_assoc()) {
        if ($book == $row ["book"]) {
          $chapters [] = $row ["chapter"];
        }
      }
      $chapters = array_unique ($chapters, SORT_NUMERIC);
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
      $data = $database_versifications->getBooksChaptersVerses ("English");
      $highestVerse = 0;
      while ($row = $data->fetch_assoc()) {
        if (($book == $row ["book"]) && ($chapter == $row ["chapter"])) {
          $highestVerse = $row ["verse"];
        }
      }
      $verses = array ();
      for ($i = 0; $i <= $highestVerse; $i++) {
        $verses [] = $i;
      }
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
    $code = "";
    $code .= '<script type="text/javascript">';
    $code .= 'var bible = "' . $bible . '";';
    $code .= '</script>';
    $code .= "\n";
    $code .= '<script type="text/javascript" src="../navigation/code.js"></script>';
    $code .= "\n";
    return $code;
  }

  public static function setBook ($book)
  {
    $book = Filter_Numeric::integer_in_string ($book);
    $ipc_focus = Ipc_Focus::getInstance();
    $ipc_focus->set ($book, 1, 1);
  }
    
  public static function setChapter ($chapter)
  {
    $ipc_focus = Ipc_Focus::getInstance();
    $book = $ipc_focus->getBook ();
    $chapter = Filter_Numeric::integer_in_string ($chapter);
    $ipc_focus->set ($book, $chapter, 1);
  }
    
  public static function setVerse ($verse)
  {
    $ipc_focus = Ipc_Focus::getInstance();
    $book = $ipc_focus->getBook ();
    $chapter = $ipc_focus->getChapter ();
    $verse = Filter_Numeric::integer_in_string ($verse);
    $ipc_focus->set ($book, $chapter, $verse);
  }
  
}

?>
