<?php

require_once ("bootstrap/bootstrap.php");

class Navigation_Logic
{

  private static function getStartingFragment ()
  {
    return '<span id="bibleditnavigation">';
  }
  
  public static function getNavigatorFragment ()
  {
    $fragment = '';
    
    $ipc_focus = Ipc_Focus::getInstance();
    $database_books = Database_Books::getInstance ();

    $book = $ipc_focus->getBook ();
    $bookName = $database_books->getEnglishFromId ($book);
    $fragment .= '<a id="selectbook" href="selectbook">' . $bookName . '</a>';

    $fragment .= " ";

    $chapter = $ipc_focus->getChapter ();
    $fragment .= '<a id="selectchapter" href="selectchapter">' . $chapter . '</a>';

    $fragment .= ":";

    $verse = $ipc_focus->getVerse ();
    $fragment .= '<a id="selectverse" href="selectverse">' . $verse . '</a>';

    return $fragment;
  }
  
  private static function getEndingFragment ()
  {
    return '</span>';
  }
  
  public static function htmlNavigator ()
  {
    return Navigation_Logic::getStartingFragment () . Navigation_Logic::getNavigatorFragment () . Navigation_Logic::getEndingFragment ();
  }

  public static function getBooksFragment ()
  {
    $database_config_user = Database_Config_User::getInstance ();
    $database_bibles = Database_Bibles::getInstance ();
    $database_books = Database_Books::getInstance ();
    $ipc_focus = Ipc_Focus::getInstance();
    $bible = $database_config_user->getBible ();
    $activeBook = $ipc_focus->getBook ();
    $books = $database_bibles->getBooks ($bible);
    $html = '<span id="selectbooks">';
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
  
  public static function getChaptersFragment ()
  {
    $database_config_user = Database_Config_User::getInstance ();
    $database_bibles = Database_Bibles::getInstance ();
    $ipc_focus = Ipc_Focus::getInstance();
    $bible = $database_config_user->getBible ();
    $book = $ipc_focus->getBook ();
    $activeChapter = $ipc_focus->getChapter ();
    $chapters = $database_bibles->getChapters ($bible, $book);
    $html = '<span id="selectchapters">';
    foreach ($chapters as $offset => $chapter) {
      if ($offset) $html .= " | ";
      if ($chapter == $activeChapter) $html .= "<mark>";
      $html .= '<a id="chapter' . $chapter . '" href="chapter">' . $chapter . '</a>';
      if ($chapter == $activeChapter) $html .= "</mark>";
    }
    $html .= "</span>";
    return $html;
  }

  public static function getVersesFragment ()
  {
    $database_config_user = Database_Config_User::getInstance ();
    $database_bibles = Database_Bibles::getInstance ();
    $ipc_focus = Ipc_Focus::getInstance();
    $bible = $database_config_user->getBible ();
    $book = $ipc_focus->getBook ();
    $chapter = $ipc_focus->getChapter ();
    $activeVerse = $ipc_focus->getVerse ();
    $verses = Filter_Usfm::getVerseNumbers ($database_bibles->getChapter ($bible, $book, $chapter));
    $html = '<span id="selectverses">';
    foreach ($verses as $offset => $verse) {
      if ($offset) $html .= " | ";
      if ($verse == $activeVerse) $html .= "<mark>";
      $html .= '<a id="verse' . $verse . '" href="verse">' . $verse . '</a>';
      if ($verse == $activeVerse) $html .= "</mark>";
    }
    $html .= "</span>";
    return $html;
  }
  
  public static function code ()
  {
    $code = '<script type="text/javascript" src="../navigation/code.js"></script>';
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
