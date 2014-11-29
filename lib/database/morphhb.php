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


// This is the database for the Hebrew Bible.
// Resilience: It is never written to.
// Chances of corruption are nearly zero.


class Database_Morphhb
{
  private static $instance;
  private $db;
  private function __construct() {
    $this->db = Database_SQLite::connect ("morphhb");
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Morphhb ();
    }
    return self::$instance;
  }


  // Get Hebrew words for $book $chapter $verse.
  public function getVerse ($book, $chapter, $verse)
  {
    $book = Database_SQLiteInjection::no ($book);
    $chapter = Database_SQLiteInjection::no ($chapter);
    $verse = Database_SQLiteInjection::no ($verse);
    $query = "SELECT hebrew FROM morphhb WHERE book = $book AND chapter = $chapter AND verse = $verse;";
    $hits = array ();
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      unset ($row[0]);
      unset ($row[1]);
      $hits [] = $row;
    }
    return $hits;
  }
  

  // Get array of book / chapter / verse of all passages that contain a $hebrew word.
  public function searchHebrew ($hebrew)
  {
    $hebrew = Database_SQLiteInjection::no ($hebrew);
    $query = "SELECT DISTINCT book, chapter, verse FROM morphhb WHERE hebrew = '$hebrew';";
    $hits = array ();
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      unset ($row[0]);
      unset ($row[1]);
      unset ($row[2]);
      $hits [] = $row;
    }
    return $hits;
  }


}


?>
