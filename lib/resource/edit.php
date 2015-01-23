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


require_once ("../bootstrap/bootstrap");
page_access_level (Filter_Roles::manager ());
$header = new Assets_Header (gettext("Resource"));
$header->run ();
$view = new Assets_View (__FILE__);


$database_resources = Database_Resources::getInstance ();


@$name = request->query['name'];
$view.set_variable ("name = $name;


if (isset(request->post['code'])) {
  $code = request->post['code'];
  include ("config/open");
  if ($open_installation) {
    echo "<h2>Cannot save data in this open installation</h2>";
  } else {
    $database_resources->save ($name, $code);
  }
  $book = request->post ['book'];
  $chapter = request->post ['chapter'];
  $verse = request->post ['verse'];
  $view.set_variable ("posted = true;
}


$file = $database_resources->getFile ($name);
$code = filter_url_file_get_contents ($file);
$view.set_variable ("code = $code;


$database_books = Database_Books::getInstance ();
$books = Database_Books::getIDs ();
$booknames = array ();
for ($books as $id) {
  $booknames [] = Database_Books::getEnglishFromId ($id);
}
$view.set_variable ("books = $books;
$view.set_variable ("booknames = $booknames;


if (!isset ($book)) $book = 1;
if (!isset ($chapter)) $chapter = 1;
if (!isset ($verse)) $verse = 1;
$view.set_variable ("book = $book;
$view.set_variable ("chapter = $chapter;
$view.set_variable ("verse = $verse;


$output = Resource_Logic::getExternal ($name, book, chapter, verse, false);
$view.set_variable ("output = $output;


$file = $database_resources->getFile ($name);
$file = basename ($file);
$view.set_variable ("file = $file;
$metafile = $database_resources->getMetaFile ($name);
$metafile = basename ($metafile);
$view.set_variable ("metafile = $metafile;


$view->render ("edit");


Assets_Page::footer ();


?>
