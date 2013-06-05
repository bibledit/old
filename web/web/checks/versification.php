<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2013 Teus Benschop.
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


class Checks_Versification
{


  public function availableBooks ($bible, $books)
  {
    $database_versifications = Database_Versifications::getInstance ();
    $data = $database_versifications->getBooksChaptersVerses ("English");
    $standardBooks = array ();
    while ($row = $data->fetch_assoc()) {
      $standardBooks [] = $row["book"];
    }
    $standardBooks = array_unique ($standardBooks, SORT_NUMERIC);
    $absentBooks = array_diff ($standardBooks, $books);
    $extraBooks = array_diff ($books, $standardBooks);
    $database_check = Database_Check::getInstance ();
    foreach ($absentBooks as $book) {
      $database_check->recordOutput ($bible, $book, 1, 1, "This book is absent from the Bible");
    }
    foreach ($extraBooks as $book) {
      $database_check->recordOutput ($bible, $book, 1, 1, "This book is extra in the Bible");
    }
  }



  public function availableChapters ($bible, $book, $chapters)
  {
    $database_versifications = Database_Versifications::getInstance ();
    $data = $database_versifications->getBooksChaptersVerses ("English");
    $standardChapters = array (0);
    while ($row = $data->fetch_assoc()) {
      if ($book != $row["book"]) continue;
      $standardChapters [] = $row["chapter"];  
    }
    $standardChapters = array_unique ($standardChapters, SORT_NUMERIC);
    $absentChapters = array_diff ($standardChapters, $chapters);
    $extraChapters = array_diff ($chapters, $standardChapters);
    $database_check = Database_Check::getInstance ();
    foreach ($absentChapters as $chapter) {
      $database_check->recordOutput ($bible, $book, $chapter, 1, "This chapter is missing");
    }
    foreach ($extraChapters as $chapter) {
      $database_check->recordOutput ($bible, $book, $chapter, 1, "This chapter is extra");
    }
  }


  
}


?>
