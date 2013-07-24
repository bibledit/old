<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);


$database_config_user = Database_Config_User::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_books = Database_Books::getInstance ();


$ipc_focus = Ipc_Focus::getInstance();


@$getbooks = $_GET['getbooks']; // Todo move this stuff to the logic.
if (isset ($getbooks)) {
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
  echo $html;
  die;
}


@$setbook = $_GET['setbook'];
if (isset ($setbook)) {
  $setbook = Filter_Numeric::integer_in_string ($setbook); // Todo move this stuff to the logic.
  $ipc_focus->set ($setbook, 1, 1);
}


@$getchapters = $_GET['getchapters'];
if (isset ($getchapters)) {
  $bible = $database_config_user->getBible (); // Todo move this stuff to the logic.
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
  echo $html;
  die;
}


@$setchapter = $_GET['setchapter'];
if (isset ($setchapter)) {
  $book = $ipc_focus->getBook (); // Todo move this stuff to the logic.
  $setchapter = Filter_Numeric::integer_in_string ($setchapter);
  $ipc_focus->set ($book, $setchapter, 1);
}


@$getverses = $_GET['getverses'];
if (isset ($getverses)) {
  $bible = $database_config_user->getBible (); // Todo move this stuff to the logic.
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
  echo $html;
  die;
}


@$setverse = $_GET['setverse'];
if (isset ($setverse)) {
  $book = $ipc_focus->getBook (); // Todo move this stuff to the logic.
  $chapter = $ipc_focus->getChapter ();
  $setverse = Filter_Numeric::integer_in_string ($setverse);
  $ipc_focus->set ($book, $chapter, $setverse);
}


echo Navigation_Logic::htmlNavigator ();

?>
