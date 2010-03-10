<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);

$smarty = new Smarty_Bibledit (__FILE__);
$database_bibles = Database_Bibles::getInstance();
$database_books = Database_Books::getInstance();

// The name of the Bible.
$bible = $_GET['bible'];
$smarty->assign ("bible", Filter_Html::sanitize ($bible));

// The book.
$book = $_GET['book'];
$smarty->assign ("book", Filter_Html::sanitize ($book));
$book_name = $database_books->getEnglishFromId ($book);
$smarty->assign ("book_name", Filter_Html::sanitize ($book_name));


// Versification.
$versification = $_GET['versification'];
if (isset ($versification)) {
  if ($versification == "") {
    $dialog_versifications = new Dialog_List (array ("name"), gettext ("Would you like to change the versification system?"), gettext ("A versification system determines how many chapters are in each book, and how many verses are in each chapter. Please make your choice below."), "");
    $database_versifications = Database_Versifications::getInstance ();
    $versification_names = $database_versifications->getSystems ();
    foreach ($versification_names as $versification_name) {
      $dialog_versifications->add_row ($versification_name, "&versification=$versification_name");
    }
    $dialog_versifications->run();
    die;
  } else {
    $database_bibles->setVersification ($name, $versification);
  }
}
$versification = $database_bibles->getVersification ($name);
$smarty->assign ("versification", $versification);

// Book creation.
$createbook = $_GET['createbook'];
if (isset ($createbook)) {
  if ($createbook == "") {
    $dialog_books = new Dialog_Books (NULL, array ("name"), gettext ("Create book"), "", "", "createbook", NULL, $database_bibles->getBooks ($name));
    die;
  } else {
    new Book_Create ($name, $createbook);
  }
}

// Available books.
$books = $database_bibles->getBooks ($name);
foreach ($books as $book) {
  $name = $database_books->getEnglishFromId ($book);
  $name = gettext ($name);
  $available_books .= " $name";
}
$smarty->assign ("available_books", $available_books);


$smarty->display ("book.tpl");

?>
