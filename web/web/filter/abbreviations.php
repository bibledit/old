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


class Filter_Abbreviations
{


  // This function reads the $data for the abbreviations per Bible.
  // It then transforms that data into an array of abbreviation => book identifier.
  // It returns that array.
  public static function read ($data)
  {
    $database_books = Database_Books::getInstance ();

    $output = array ();

    $data = Filter_String::string2array ($data);
    foreach ($data as &$entry) {
      $entry = trim ($entry);
      $entry = explode ("=", $entry);
      if (count ($entry) != 2) continue;
      $book = trim ($entry [0]);
      $book = $database_books->getIdFromEnglish ($book);
      if ($book == 0) continue;
      $abbrev = trim ($entry [1]);
      if ($abbrev == "") continue;
      // Array (book => abbrev) does not support multiple abbreviations for one book, 
      // but array (abbrev => book) does.
      $output [$abbrev] = intval ($book);
    }
    
    return $output;
  }
  
  
  // This function takes the abbreviations stored for a Bible as $data.
  // It then sorts them out.
  // It adds the Bible books not in the $data.
  // It sorts everything in the standard canonical order.
  // It presents everything for display to the user for editing and update.
  static public function display ($data)
  {
    $data = self::read ($data);
    
    $database_books = Database_Books::getInstance ();

    $output = array ();

    // Check for books, order them, supply missing ones.
    $books = $database_books->getIDs ();
    foreach ($books as $book) {
      $found = false;
      foreach ($data as $abbreviation => $bookId) {
        if ($book == $bookId) {
          $output [] = $database_books->getEnglishFromId ($book) . " = " . $abbreviation;
          $found = true;
        }
      }
      if (!$found) {
        $output [] = $database_books->getEnglishFromId ($book) . " = ";
      }
    }

    // Get data as a string for display.
    return implode ("\n", $output);
  }


}

?>
