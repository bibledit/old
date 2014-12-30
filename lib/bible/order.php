<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

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
page_access_level (Filter_Roles::translator ());


$header = new Assets_Header (gettext("Order"));
$header->jQueryUIOn ("sortable");
$header->run ();


$view = new Assets_View (__FILE__);


$database_config_bible = Database_Config_Bible::getInstance ();
$database_books = Database_Books::getInstance ();
$database_bibles = Database_Bibles::getInstance ();


$bible = access_bible_clamp ($_GET['bible']);
$view->view->bible = filter_string_sanitize_html ($bible);


@$reset = $_GET ["reset"];
if (isset ($reset)) {
  $database_config_bible->setBookOrder ($bible, "");
}


@$order = $_POST ['order'];
if (isset ($order)) {
  $order = explode (",", $order);
  $ids = array ();
  for ($order as $english) {
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
$books = filter_passage_get_ordered_books ($bible);
for ($books as $book) {
  $names [] = $database_books->getEnglishFromId ($book);
}
$view->view->names = $names;


$view->render ("order.php");


Assets_Page::footer ();


?>
