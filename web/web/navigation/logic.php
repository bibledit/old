<?php

require_once ("bootstrap/bootstrap.php");

class Navigation_Logic
{

  private static function getHtmlStartingFragment ()
  {
    return '<span id="bibleditnavigation">';
  }
  
  public static function getHtmlNavigatorFragment ()
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
  
  private static function getHtmlEndingFragment ()
  {
    return '</span>';
  }
  
  public static function htmlNavigator () // Todo
  {
    return Navigation_Logic::getHtmlStartingFragment () . Navigation_Logic::getHtmlNavigatorFragment () . Navigation_Logic::getHtmlEndingFragment ();
  }

  public static function code () // Todo
  {
    $code = '<script type="text/javascript" src="../navigation/code.js"></script>';
    $code .= "\n";
    return $code;
  }

}

?>
