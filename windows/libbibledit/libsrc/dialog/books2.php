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


class Dialog_Books2
{
  private $view;

  /**
  * Constructs a book dialog
  * $action - GET or POST action to take.
  * $inclusions - NULL (in which case it does nothing), or an array of book IDs to include.
  * $exclusions - NULL (in which case it does nothing), or an array of book IDs to exclude.
  */
  public function __construct ($header, $info_top, $info_bottom, $action, $inclusions, $exclusions)
  {
    $this->view = new Assets_View (__FILE__);
    $caller = $_SERVER["PHP_SELF"] . "?" . http_build_query (array ());
    $this->view->view->caller = $caller;
    $this->view->view->header = $header;
    $this->view->view->info_top = $info_top;
    $this->view->view->info_bottom = $info_bottom;
    $this->view->view->action = $action;

    $database_books = Database_Books::getInstance();
    $book_ids = $database_books->getIDs ();
    if (is_array ($inclusions)) {
      $book_ids = $inclusions;
    }
    if (is_array ($exclusions)) {
      $book_ids = array_diff ($book_ids, $exclusions);
      $book_ids = array_values ($book_ids);
    }
    foreach ($book_ids as $id) {
      $book_names[] = $database_books->getEnglishFromId ($id);
    }
    $this->view->view->book_ids = $book_ids;
    $this->view->view->book_names = $book_names;
    $this->view->render ("books2.php");
    Assets_Page::footer();
    die;
  }

}
?>
