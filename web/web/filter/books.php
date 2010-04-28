<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2009 Teus Benschop.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */

class Filter_Books
{

  public function passageDisplay ($book, $chapter, $verse)
  {
    $database_books = Database_Books::getInstance ();
    $book = gettext ($database_books->getEnglishFromId ($book));
    return "$book $chapter:$verse";
  }


  /**
  * Returns the display string for the $passages.
  * $passages is an array of array (book, chapter, verse).
  */
  public function passagesDisplay ($passages)
  {
    $database_books = Database_Books::getInstance ();
    $display = "";
    foreach ($passages as $passage) {
      if ($display != "")
        $display .= " | ";
      $book = gettext ($database_books->getEnglishFromId ($passage[0]));
      $chapter = $passage[1];
      $verse = $passage[2];
      $display .= "$book $chapter:$verse";
    }
    return $display;
  }

}

?>
