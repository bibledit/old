<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
$header = new Assets_Header (gettext ("Resource"));
$header->jQueryOn ();
$header->run ();
$view = new Assets_View (__FILE__);


$database_resources = Database_Resources::getInstance ();


@$name = $_GET['name'];
$view->view->name = $name;


if (isset($_POST['command'])) {
  $command = $_POST['command'];
  $code = $_POST['code'];
  $default = $_POST['default'];
  if ($default != "") {
    $code = file_get_contents ($default);
  }
  include ("administration/credentials.php");
  if ($open_installation) {
    echo "<h2>Cannot save data in this open installation</h2>";
  } else {
    $database_resources->save ($name, $command, $code);
  }
  $book = $_POST ['book'];
  $chapter = $_POST ['chapter'];
  $verse = $_POST ['verse'];
  $view->view->posted = true;
}


$defaults = array ("");
foreach (new DirectoryIterator (".") as $fileInfo) {
  if($fileInfo->isDot()) continue;
  if($fileInfo->isDir()) continue;
  $default = $fileInfo->getFilename();
  if (strpos ($default, "default") === false) continue;
  $defaults [] = $default;
}
$view->view->defaults = $defaults;


$details = $database_resources->getDetails ($name);
$command = $details ['command'];
$code = $details ['code'];
$view->view->command = $command;
$view->view->code = $code;


$database_books = Database_Books::getInstance ();
$books = $database_books->getIDs ();
$booknames = array ();
foreach ($books as $id) {
  $booknames [] = $database_books->getEnglishFromId ($id);
}
$view->view->books = $books;
$view->view->booknames = $booknames;


if (!isset ($book)) $book = 1;
if (!isset ($chapter)) $chapter = 1;
if (!isset ($verse)) $verse = 1;
$view->view->book = $book;
$view->view->chapter = $chapter;
$view->view->verse = $verse;


$output = Resource_Logic::getExternal ($name, $book, $chapter, $verse);
$view->view->output = $output;


$view->render ("edit.php");


Assets_Page::footer ();


?>
