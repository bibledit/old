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


// Todo DRY: Move to filter. Create unit test for it.
$filename = tempnam (sys_get_temp_dir (), "");
file_put_contents ($filename, $code);
$command = str_replace ("script", "$filename $book $chapter $verse", $command) . " 2>&1";
$output = shell_exec ($command);
unlink ($filename);
$view->view->output = $output;


$view->render ("edit.php");


Assets_Page::footer ();


?>
