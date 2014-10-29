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
page_access_level (Filter_Roles::TRANSLATOR_LEVEL);


$header = new Assets_Header (Locale_Translate::_("Order"));
$header->jQueryUIOn ("sortable");
$header->run ();


$view = new Assets_View (__FILE__);


$database_config_bible = Database_Config_Bible::getInstance ();
$database_books = Database_Books::getInstance ();
$database_bibles = Database_Bibles::getInstance ();


$bible = Access_Bible::clamp ($_GET['bible']);
$view->view->bible = Filter_Html::sanitize ($bible);


@$reset = $_GET ["reset"];
if (isset ($reset)) {
  $database_config_bible->setBookOrder ($bible, "");
}


@$order = $_POST ['order'];
if (isset ($order)) {
  $order = explode (",", $order);
  $ids = array ();
  foreach ($order as $english) {
    $id = $database_books->getIdFromEnglish ($english);
    $ids [] = $id;
  }
  $order = implode (" ", $ids);
  $database_config_bible->setBookOrder ($bible, $order);
  die;
}


$script = <<<EOD
var orderBible = '$bible';
EOD;
$view->view->script = $script;


$names = array ();
$books = Filter_Books::getOrdered ($bible);
foreach ($books as $book) {
  $names [] = $database_books->getEnglishFromId ($book);
}
$view->view->names = $names;


$view->render ("order.php");


Assets_Page::footer ();


?>
