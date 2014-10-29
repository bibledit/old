<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


require_once ("../bootstrap/bootstrap.php");
page_access_level (Filter_Roles::MANAGER_LEVEL);
$header = new Assets_Header (Locale_Translate::_("Resource"));
$header->run ();
$view = new Assets_View (__FILE__);


$database_resources = Database_Resources::getInstance ();


@$name = $_GET['name'];
$view->view->name = $name;


if (isset($_POST['code'])) {
  $code = $_POST['code'];
  include ("config/open.php");
  if ($open_installation) {
    echo "<h2>Cannot save data in this open installation</h2>";
  } else {
    $database_resources->save ($name, $code);
  }
  $book = $_POST ['book'];
  $chapter = $_POST ['chapter'];
  $verse = $_POST ['verse'];
  $view->view->posted = true;
}


$file = $database_resources->getFile ($name);
$code = file_get_contents ($file);
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


$output = Resource_Logic::getExternal ($name, $book, $chapter, $verse, false);
$view->view->output = $output;


$file = $database_resources->getFile ($name);
$file = basename ($file);
$view->view->file = $file;
$metafile = $database_resources->getMetaFile ($name);
$metafile = basename ($metafile);
$view->view->metafile = $metafile;


$view->render ("edit.php");


Assets_Page::footer ();


?>
