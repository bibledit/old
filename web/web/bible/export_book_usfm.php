<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);
$bible = $_GET['bible'];
$book = $_GET['book'];
$smarty->assign ("bible", Filter_Html::sanitize ($bible));
$smarty->assign ("book", Filter_Html::sanitize ($book));
$database_bibles = Database_Bibles::getInstance();
$chapters = $database_bibles->getChapters ($bible, $book);
foreach ($chapters as $chapter) {
  $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
  $data .= "\n";
  $data .= trim ($chapter_data);
}
$smarty->assign ("data", $data);
$smarty->display ("export_book_usfm.tpl");
?>
