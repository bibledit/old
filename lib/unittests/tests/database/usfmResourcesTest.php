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

 
class databaseUsfmResourcesTest extends PHPUnit_Framework_TestCase
{

  public function setUp ()
  {
    $this->tearDown ();
  }
  
  
  public function tearDown ()
  {
    $database_usfmresources = Database_UsfmResources::getInstance ();
    $database_usfmresources->deleteResource ("bibledit");
  }


  public function testEmpty ()
  {
    $database_usfmresources = Database_UsfmResources::getInstance ();
    $resources = $database_usfmresources->getResources ();
    $this->assertNotContains ("bibledit", $resources);
  }


  public function testNames ()
  {
    $database_usfmresources = Database_UsfmResources::getInstance ();
    $database_usfmresources->storeChapter ("bibledit", 2, 3, "");
    $resources = $database_usfmresources->getResources ();
    $this->assertContains ("bibledit", $resources);
  }
    

  public function testStoreGetChapter ()
  {
    $database_usfmresources = Database_UsfmResources::getInstance ();
    $database_usfmresources->storeChapter ("bibledit", 2, 3, "usfm");
    $usfm = $database_usfmresources->getUsfm ("bibledit", 2, 3);
    $this->assertEquals ("usfm", $usfm);
    $usfm = $database_usfmresources->getUsfm ("bibledit", 2, 4);
    $this->assertEquals ("", $usfm);
  }
    

  public function testBooks ()
  {
    $database_usfmresources = Database_UsfmResources::getInstance ();
    $database_usfmresources->storeChapter ("bibledit", 2, 3, "usfm");
    $database_usfmresources->storeChapter ("bibledit", 3, 4, "usfm");
    $database_usfmresources->storeChapter ("bibledit", 1, 1, "usfm");
    $database_usfmresources->storeChapter ("bibledit", 1, 2, "usfm");
    $books = $database_usfmresources->getBooks ("bibledit");
    $this->assertEquals (array (1, 2, 3), $books);
  }
    

  public function testChapters ()
  {
    $database_usfmresources = Database_UsfmResources::getInstance ();
    $database_usfmresources->storeChapter ("bibledit", 2, 3, "usfm");
    $database_usfmresources->storeChapter ("bibledit", 3, 4, "usfm");
    $database_usfmresources->storeChapter ("bibledit", 1, 1, "usfm");
    $database_usfmresources->storeChapter ("bibledit", 1, 2, "usfm");
    $chapters = $database_usfmresources->getChapters ("bibledit", 1);
    $this->assertEquals (array (1, 2), $chapters);
    $chapters = $database_usfmresources->getChapters ("bibledit", 2);
    $this->assertEquals (array (3), $chapters);
  }


  public function testSizes ()
  {
    $database_usfmresources = Database_UsfmResources::getInstance ();

    clearstatcache ();
    $size = $database_usfmresources->getSize ("bibledit", 2, 3);
    $this->assertEquals (0, $size);

    clearstatcache ();
    $database_usfmresources->storeChapter ("bibledit", 2, 3, "usfm");
    $size = $database_usfmresources->getSize ("bibledit", 2, 3);
    $this->assertEquals (4, $size);
    
    clearstatcache ();
    $database_usfmresources->storeChapter ("bibledit", 2, 3, "chapter");
    $size = $database_usfmresources->getSize ("bibledit", 2, 3);
    $this->assertEquals (7, $size);
  }
  
  
  public function testDeleteBook ()
  {
    $database_usfmresources = Database_UsfmResources::getInstance ();
    $database_usfmresources->storeChapter ("bibledit", 2, 3, "usfm");
    $books = $database_usfmresources->getBooks ("bibledit");
    $this->assertEquals (array (2), $books);
    $database_usfmresources->deleteBook ("bibledit", 2);
    $books = $database_usfmresources->getBooks ("bibledit");
    $this->assertEmpty ($books);
  }
  
      
}


?>
