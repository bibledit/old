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
page_access_level (Filter_Roles::manager ());


Assets_Page::header (gettext("Chapter"));
$view = new Assets_View (__FILE__);


$database_bibles = Database_Bibles::getInstance();
$database_books = Database_Books::getInstance();


// The Bible.
$bible = Access_Bible::clamp ($_GET['bible']);
$view->view->bible = filter_string_sanitize_html ($bible);


// Whether the user has write access to this Bible.
$write_access = access_bible_write ($bible);
$view->view->write_access = $write_access;


// The book.
$book = $_GET['book'];
$view->view->book = filter_string_sanitize_html ($book);
$book_name = $database_books->getEnglishFromId ($book);
$view->view->book_name = filter_string_sanitize_html ($book_name);


// The chapter.
$chapter = $_GET['chapter'];
$view->view->chapter = filter_string_sanitize_html ($chapter);


$view->render ("chapter.php");


Assets_Page::footer ();


?>
