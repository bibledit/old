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

 
class databaseHistoryTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
  }


  public function testOptimize ()
  {
    $database_history = Database_History::getInstance ();
    $database_history->optimize ();
  }


  public function testTrim ()
  {
    $database_history = Database_History::getInstance ();
    $database_history->trim ();
  }
  
  
  public function testAll ()
  {
    $database_history = Database_History::getInstance ();

    $author = "test";
    $bible = "phpunit";
    $book = 1;
    $chapter = 2;
    $verse = 3;
    $start = 0;

    // Start with an empty history.
    $count = $database_history->count ($author, array ($bible), $book, $chapter, $verse);
    $this->assertEquals (0, $count);

    $count = $database_history->count (NULL, array (), NULL, NULL, NULL);
    $this->assertEquals (0, $count);
    
    $data = $database_history->get ($author, array ($bible), $book, $chapter, $verse, $start);
    $this->assertEmpty ($data);
    
    $authors = $database_history->authors (array ($bible));
    $this->assertEmpty ($authors);
    
    // Record some data.
    $database_history->record (time(), $author, $bible, $book, $chapter, $verse, "old1", "mod1", "new1");
    $database_history->record (time(), $author, $bible, $book, $chapter, $verse, "old2", "mod2", "new2");

    // Check the data.
    $count = $database_history->count ($author, array ($bible), $book, $chapter, $verse);
    $this->assertEquals (2, $count);
    
    $count = $database_history->count (NULL, array (), NULL, NULL, NULL);
    $this->assertEquals (2, $count);

    $data = $database_history->get ($author, array ($bible), $book, $chapter, $verse, $start);
    $this->assertEquals (2, count ($data));
    $this->assertEquals ("test", $data [0] ['author']);
    $this->assertEquals ("new2", $data [1] ['newtext']);
  }


}


?>
