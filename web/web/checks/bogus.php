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




class Checks_Bogus
{


  public function run ($bible)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_check = Database_Check::getInstance ();
    // Go through the books in the $bible
    $books = $database_bibles->getBooks ($bible);
    foreach ($books as $book) {
      // Go through the chapters.
      $chapters = $database_bibles->getChapters ($bible, $book);
      foreach ($chapters as $chapter) {
        $verse = rand (1, 10);
        $database_check->recordOutput ($bible, $book, $chapter, $verse, md5 (rand ()));
      }
    }
  }
 
  
}


?>
