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

Assets_Page::header (Locale_Translate::_("Versification system"));
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
foreach ($data as $row) {
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
$xml = htmlentities ($xml);
$view->view->xml = $xml;

$view->render ("system.php");
Assets_Page::footer ();

?>
