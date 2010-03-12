<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);

$smarty = new Smarty_Bibledit (__FILE__);
$database_bibles = Database_Bibles::getInstance();
$database_books = Database_Books::getInstance();

// The Bible.
$bible = $_GET['bible'];
$smarty->assign ("bible", Filter_Html::sanitize ($bible));

// The book.
$book = $_GET['book'];
$smarty->assign ("book", Filter_Html::sanitize ($book));
$book_name = $database_books->getEnglishFromId ($book);
$smarty->assign ("book_name", Filter_Html::sanitize ($book_name));

// The chapter.
$chapter = $_GET['chapter'];
$smarty->assign ("chapter", Filter_Html::sanitize ($chapter));

$smarty->display ("chapter.tpl");

?>
