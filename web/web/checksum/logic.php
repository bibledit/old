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


require_once ("bootstrap/bootstrap.php");


class Checksum_Logic
{


  // This function reads $data,
  // calculates a checksum by splitting the $data on the spaces,
  // converts it to JSON,
  // and returns the result.
  public static function send ($data)
  {
    $checksum = self::get ($data);
    $json = array ('data' => $data, 'checksum' => $checksum);
    $json = json_encode ($json);
    return $json;
  }


  // This function gets the checksum for $data, and returns it.
  // It uses a fast algorithm, good enough for notes.
  public static function get ($data)
  {
    $bits = explode (" ", $data);
    $checksum = count ($bits);
    return $checksum;
  }
  
  
  // Returns a proper checksum for the USFM in the chapter.
  public static function getChapter ($bible, $book, $chapter)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $usfm = $database_bibles->getChapter ($bible, $book, $chapter);
    $checksum = md5 ($usfm);
    return $checksum;
  }


  // Returns a proper checksum for the USFM in the book.
  public static function getBook ($bible, $book)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $chapters = $database_bibles->getChapters ($bible, $book);
    $checksum = array ();
    foreach ($chapters as $chapter) {
      $checksum [] = self::getChapter ($bible, $book, $chapter);
    }
    $checksum = implode ("", $checksum);
    $checksum = md5 ($checksum);
    return $checksum;
  }
  
  
  // Returns a proper checksum for the USFM in the $bible.
  public static function getBible ($bible)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $books = $database_bibles->getBooks ($bible);
    $checksum = array ();
    foreach ($books as $book) {
      $checksum [] = self::getBook ($bible, $book);
    }
    $checksum = implode ("", $checksum);
    $checksum = md5 ($checksum);
    return $checksum;
  }
  

  // Returns a proper checksum for the USFM in the array of $bibles.
  public static function getBibles ($bibles)
  {
    $checksum = array ();
    foreach ($bibles as $bible) {
      $checksum [] = self::getBible ($bible);
    }
    $checksum = implode ("", $checksum);
    $checksum = md5 ($checksum);
    return $checksum;
  }


}


?>
