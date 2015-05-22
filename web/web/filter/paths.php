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


class Filter_Paths
{


  /*
   * The function returns the filename for a html file for a Bible.
   * $path    - The path where to store the files.
   * $book    - The book identifier.
   * $chapter - The chapter number.
   */
  public static function htmlFileNameBible ($path = "", $book = "", $chapter = "")
  {
    $database_books = Database_Books::getInstance ();
    $filename = "";

    // If a path is given, prefix it.
    if ($path != "") {
      $filename = $path . '/';
    }

    // No book ID given: Return the name for the index file for the Bible.
    if ($book == "") {
      $filename .= "index.html";
      return $filename;
    }

    // Add the name for the book. No spaces.
    $filename .= str_pad ($book, 2, "0", STR_PAD_LEFT);
    $book = $database_books->getEnglishFromId ($book);
    $book = str_replace (" ", "", $book);
    $filename .= '-' . $book;

    // No chapter given: Provide name for the index files of all chapters in a book.
    if ($chapter !== "") {
      $filename .= '-' . str_pad ($chapter, 3, "0", STR_PAD_LEFT);
    }

    $filename .= ".html";

    return $filename;
  }




}

?>
