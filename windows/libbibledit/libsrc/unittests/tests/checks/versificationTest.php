<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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


class versificationsTest extends PHPUnit_Framework_TestCase
{


  public $versificationID;

 
  public function setUp ()
  {
    $this->tearDown ();
    $database_versifications = Database_Versifications::getInstance ();
    $this->versificationID = $database_versifications->getID ("English");
  }

  
  public function tearDown ()
  {
    $database_check = Database_Check::getInstance ();
    $database_check->truncateOutput ("");
  }


  public function testAvailableBooks ()
  {
    $database_versifications = Database_Versifications::getInstance ();
    $data = $database_versifications->getBooksChaptersVerses ("English");
    $books = array ();
    foreach ($data as $row) {
      $book = $row ["book"];
      $books [] = $book;
    }
    $books = array_unique ($books, SORT_NUMERIC);
    $fault = array_diff ($books, array (10));
    Checks_Versification::books ("Bible", $fault);
    $database_check = Database_Check::getInstance ();
    $result = $database_check->getHits ();
    $standard = array (array (
     'rowid' => "1",
     'bible' => "0",
     'book' => "10",
     'chapter' => "1",
     'verse' => "1",
     'data' => "This book is absent from the Bible"
    ));
    $this->assertEquals ($standard, $result);
  }


  public function testAvailableChapters ()
  {
    // Ruth.
    $book = 8; 
    // One missing and one extra chapter.
    $chapters = array (0, 2, 3, 4, 5);
    // Run check.
    Checks_Versification::chapters ("Bible", $book, $chapters);
    // Verify results.
    $database_check = Database_Check::getInstance ();
    $result = $database_check->getHits ();
    $standard = array (
      array (
       'rowid' => "1",
       'bible' => "0",
       'book' => "8",
       'chapter' => "1",
       'verse' => "1",
       'data' => "This chapter is missing"
      ), 
      array (
       'rowid' => "2",
       'bible' => "0",
       'book' => "8",
       'chapter' => "5",
       'verse' => "1",
       'data' => "This chapter is extra"
      )
    );
    $this->assertEquals ($standard, $result);
  }


  public function testAvailableVersesOne ()
  {
    // Ruth 1.
    $book = 8; 
    $chapter = 1;
    // One missing and one extra verse.
    $verses = array (0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24);
    // Run check.
    Checks_Versification::verses ("Bible", $book, $chapter, $verses);
    // Verify results.
    $database_check = Database_Check::getInstance ();
    $results = $database_check->getHits ();
    $standard = array (
      array (
       'rowid' => "1",
       'bible' => "0",
       'book' => "8",
       'chapter' => "1",
       'verse' => "1",
       'data' => "This verse is missing according to the versification system"
      ), 
      array (
       'rowid' => "2",
       'bible' => "0",
       'book' => "8",
       'chapter' => "1",
       'verse' => "24",
       'data' => "This verse is extra according to the versification system"
      ),
      array (
       'rowid' => "3",
       'bible' => "0",
       'book' => "8",
       'chapter' => "1",
       'verse' => "2",
       'data' => "The verse is out of sequence"
      ),
      array (
       'rowid' => "4",
       'bible' => "0",
       'book' => "8",
       'chapter' => "1",
       'verse' => "24",
       'data' => "The verse is out of sequence"
      )
    );
    $this->assertEquals ($standard, $results);
  }


  public function testAvailableVersesTwo ()
  {
    // Ruth 1.
    $book = 8; 
    $chapter = 1;
    // Verses out of order.
    $verses = array (0, 2, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22);
    // Run check.
    Checks_Versification::verses ("Bible", $book, $chapter, $verses);
    // Verify results.
    $database_check = Database_Check::getInstance ();
    $results = $database_check->getHits ();
    $standard = array (
      array (
       'rowid' => "1",
       'bible' => "0",
       'book' => "8",
       'chapter' => "1",
       'verse' => "2",
       'data' => "The verse is out of sequence"
      ),
      array (
       'rowid' => "2",
       'bible' => "0",
       'book' => "8",
       'chapter' => "1",
       'verse' => "1",
       'data' => "The verse is out of sequence"
      ),
      array (
       'rowid' => "3",
       'bible' => "0",
       'book' => "8",
       'chapter' => "1",
       'verse' => "3",
       'data' => "The verse is out of sequence"
      )
    );
    $this->assertEquals ($standard, $results);
  }


}
?>
