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

// Delete chapter.
@$deletechapter = $_GET['deletechapter'];
if ($deletechapter != "") {
  if ($_GET['confirm'] != "yes") {
    $dialog_yes = new Dialog_Yes (array ("bible", "book"), gettext ("Would you like to delete this chapter?"), "deletechapter");
    die;
  } else {
    $database_bibles->deleteChapter ($bible, $book, $deletechapter);
  }
}

// Add chapter.
if (isset ($_GET['createchapter'])) {
  $dialog_entry = new Dialog_Entry (array ("bible" => $bible, "book" => $book), gettext ("Please enter the number for the new chapter"), "", "createchapter", NULL);
  die;
}
if (isset($_POST['createchapter'])) {
  $createchapter = $_POST['entry'];
  $chapters = $database_bibles->getChapters ($bible, $book);
  // Only create the chapters if it does not yet exist.
  if (array_search ($createchapter, $chapters) === false) {
    new Book_Create ($bible, $book, $createchapter);
  } else {
    $error_message = gettext ("This chapter already exists");
  }
}

// Available chapters.
$chapters = $database_bibles->getChapters ($bible, $book);
$smarty->assign ("chapters", $chapters);

@$smarty->assign ("error_message", $error_message);

$smarty->display ("book.tpl");

?>