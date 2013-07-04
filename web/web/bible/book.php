<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);

Assets_Page::header (gettext ("Book"));
$view = new Assets_View (__FILE__);
$database_bibles = Database_Bibles::getInstance();
$database_books = Database_Books::getInstance();

// The name of the Bible.
$bible = $_GET['bible'];
$view->view->bible = Filter_Html::sanitize ($bible);

// The book.
$book = $_GET['book'];
$view->view->book = Filter_Html::sanitize ($book);
$book_name = $database_books->getEnglishFromId ($book);
$view->view->book_name = Filter_Html::sanitize ($book_name);

// Delete chapter.
@$deletechapter = $_GET['deletechapter'];
if ($deletechapter != "") {
  @$confirm = $_GET['confirm'];
  if ($confirm != "yes") {
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
    $feedback = array ();
    $result = Book_Create::create ($bible, $book, $createchapter, $feedback);
    $feedback = implode (" ", $feedback);
    if ($result) $success_message = $feedback;
    else $error_message = $feedback;
  } else {
    $error_message = gettext ("This chapter already exists");
  }
}

// Available chapters.
$chapters = $database_bibles->getChapters ($bible, $book);
$view->view->chapters = $chapters;

@$view->view->success_message = $success_message;
@$view->view->error_message = $error_message;

$view->render ("book.php");
Assets_Page::footer ();

?>
