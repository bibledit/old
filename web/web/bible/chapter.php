<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);

Assets_Page::header (gettext ("Chapter"));
$view = new Assets_View (__FILE__);
$database_bibles = Database_Bibles::getInstance();
$database_books = Database_Books::getInstance();

// The Bible.
$bible = $_GET['bible'];
$view->view->bible = Filter_Html::sanitize ($bible);

// The book.
$book = $_GET['book'];
$view->view->book = Filter_Html::sanitize ($book);
$book_name = $database_books->getEnglishFromId ($book);
$view->view->book_name = Filter_Html::sanitize ($book_name);

// The chapter.
$chapter = $_GET['chapter'];
$view->view->chapter = Filter_Html::sanitize ($chapter);

$view->render ("chapter.php");

Assets_Page::footer ();

?>
