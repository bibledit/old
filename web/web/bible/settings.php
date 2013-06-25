<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);

$smarty = new Smarty_Bibledit (__FILE__);
$database_bibles = Database_Bibles::getInstance();
$database_books = Database_Books::getInstance();

// The Bible.
$bible = $_GET['bible'];
$smarty->assign ("bible", Filter_Html::sanitize ($bible));

// Versification.
@$versification = $_GET['versification'];
if (isset ($versification)) {
  if ($versification == "") {
    $dialog_versifications = new Dialog_List (array ("bible"), gettext ("Would you like to change the versification system?"), gettext ("A versification system determines how many chapters are in each book, and how many verses are in each chapter. Please make your choice below."), "");
    $database_versifications = Database_Versifications::getInstance ();
    $versification_names = $database_versifications->getSystems ();
    foreach ($versification_names as $versification_name) {
      $dialog_versifications->add_row ($versification_name, "&versification=$versification_name");
    }
    $dialog_versifications->run();
    die;
  } else {
    $database_bibles->setVersification ($bible, $versification);
  }
}
$versification = $database_bibles->getVersification ($bible);
$smarty->assign ("versification", $versification);

// Book creation.
@$createbook = $_GET['createbook'];
if (isset ($createbook)) {
  if ($createbook == "") {
    $dialog_books = new Dialog_Books (array ("bible"), gettext ("Create book"), "", "", "createbook", NULL, $database_bibles->getBooks ($bible));
    die;
  } else {
    $feedback = array ();
    Book_Create::create ($bible, $createbook, NULL, $feedback);
  }
}

// Book deletion.
@$deletebook = $_GET['deletebook'];
if ($deletebook != "") {
  @$confirm = $_GET['confirm'];
  if ($confirm != "") {
    $database_bibles->deleteBook ($bible, $deletebook);
  } else {
    $dialog_yes = new Dialog_Yes (array ("bible"), gettext ("Would you like to delete this book?"), "deletebook");
    die;
  }
}


// Available books.
$book_ids = $database_bibles->getBooks ($bible);
foreach ($book_ids as $book) {
  $book_name = $database_books->getEnglishFromId ($book);
  $book_name = gettext ($book_name);
  $book_names [] = $book_name;
}
@$smarty->assign ("book_ids", $book_ids);
@$smarty->assign ("book_names", $book_names);

$smarty->display ("settings.tpl");

Assets_Page::footer ();
?>
