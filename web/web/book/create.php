<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2012 Teus Benschop.
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


class Book_Create
{
  /**
  * Creates book template with ID $book in Bible $bible.
  * If a $chapter is given instead of NULL, it creates that chapter only.
  */
  public function __construct($bible, $book, $chapter)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_snapshots = Database_Snapshots::getInstance();
    $database_versifications = Database_Versifications::getInstance();
    $database_books = Database_Books::getInstance();
    $database_logs = Database_Logs::getInstance ();
    $bible_id = $database_bibles->getID ($bible);
    if ($bible_id == 0) {
      $database_logs->log ("Bible $bible does not exist: Cannot create book");
      return;
    }
    if ($book == 0) {
      $database_logs->log ("Invalid book while creating a book template");
      return;
    }

    // Chapter 0.
    if (!isset ($chapter) || ($chapter == 0)) {
      $data  = "\\id "    . $database_books->getUsfmFromId($book)     . "\n";
      $data .= "\\h "     . $database_books->getEnglishFromId ($book) . "\n";
      $data .= "\\toc2 "  . $database_books->getEnglishFromId ($book) . "\n";
      $database_bibles->storeChapter ($bible, $book, 0, $data);
      $database_snapshots->snapChapter ($bible, $book, 0, $data, false);
    }


    // Subsequent chapters.
    $versification = $database_bibles->getVersification ($bible);
    $versification_data = $database_versifications->getBooksChaptersVerses ($versification);
    while ($row = $versification_data->fetch_assoc()) {
      if ($book == $row["book"]) {
        $ch = $row["chapter"];
        $verse = $row["verse"];
        if (!isset ($chapter) || ($chapter == $ch)) {
          $data  = "\\c $ch\n";
          $data .= "\\p\n";
          for ($i = 1; $i <= $verse; $i++) {
            $data .= "\\v $i\n";
          }
          $database_bibles->storeChapter ($bible, $book, $ch, $data);
          $database_snapshots->snapChapter ($bible, $book, $ch, $data, false);
        }
      }
    }
  }
}


?>