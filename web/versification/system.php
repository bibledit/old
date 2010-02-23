<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);

$smarty = new Smarty_Bibledit (__FILE__);
$database_versifications = Database_Versifications::getInstance();

$name = $_GET['name'];
$smarty->assign ("name", $name);

$database_books = Database_Books::getInstance();
$data = $database_versifications->getBooksChaptersVerses ($name);
while ($row = $data->fetch_assoc()) {
  $book       = $row["book"];
  $chapter    = $row["chapter"];
  $verse      = $row["verse"];
  $book       = $database_books->getEnglishFromId ($book);     
  $books[]    = $book;
  $chapters[] = $chapter;
  $verses[]   = $verse;
}
$smarty->assign ("books",    $books);
$smarty->assign ("chapters", $chapters);
$smarty->assign ("verses",   $verses);

$xml = $database_versifications->exportBibleditXmlFile ($name);
$smarty->assign ("xml", $xml);

$smarty->display ("system.tpl");

?>
