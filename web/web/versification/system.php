<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);

Assets_Page::header (gettext ("Versification system"));
$view = new Assets_View (__FILE__);
$database_versifications = Database_Versifications::getInstance();

$name = $_GET['name'];
$view->view->name = Filter_Html::sanitize ($name);

if (isset($_POST['submit'])) {
  $data = $_POST['data'];
  if ($data != "") {
    $data = stripslashes ($data);
    $database_versifications->importBibleditXml ($data, $name);
  }
}

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
$view->view->books = $books;
$view->view->chapters = $chapters;
$view->view->verses = $verses;

$xml = $database_versifications->exportBibleditXmlFile ($name);
$view->view->xml = $xml;

$view->render ("system.php");
Assets_Page::footer ();

?>
