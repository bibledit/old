<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
Assets_Page::header (gettext ("Export"));
$smarty = new Smarty_Bibledit (__FILE__);
$bible = $_GET['bible'];
$smarty->assign ("bible", Filter_Html::sanitize ($bible));
$database_bibles = Database_Bibles::getInstance();
$books = $database_bibles->getBooks ($bible);
$data = "";
foreach ($books as $book) {
  $chapters = $database_bibles->getChapters ($bible, $book);
  foreach ($chapters as $chapter) {
    $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
    $data .= "\n";
    $data .= trim ($chapter_data);
  }
}
$smarty->assign ("data", $data);
$smarty->display ("export_bible_usfm.tpl");
Assets_Page::footer ();
?>
