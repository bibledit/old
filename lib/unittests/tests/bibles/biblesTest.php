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

 
class databaseBiblesTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
    $database_search = Database_Search::getInstance ();
    $database_search->create ();
    $database_bibleactions = Database_BibleActions::getInstance ();
    $database_bibleactions->create ();
  }


  public function tearDown ()
  {
    $database_search = Database_Search::getInstance ();
    $database_search->create ();
    $database_bibles = Database_Bibles::getInstance ();
    $database_bibles->deleteBible ("phpunit");
    $database_bibles->deleteBible ("phpunit2");
    $database_bibleactions = Database_BibleActions::getInstance ();
    $database_bibleactions->clear ();
  }


  public function testOptimizeOne ()
  {
    // Test whether optimizing works without errors.
    $database_bibles = Database_Bibles::getInstance ();
    $id = $database_bibles->createBible ("phpunit");
    $database_bibles->storeChapter ("phpunit", 2, 3, "a");
    $database_bibles->storeChapter ("phpunit", 2, 3, "b");
    $database_bibles->storeChapter ("phpunit", 2, 3, "c");
    $database_bibles->storeChapter ("phpunit", 2, 3, "d");
    $database_bibles->storeChapter ("phpunit", 2, 3, "e");
    $database_bibles->storeChapter ("phpunit", 2, 3, "f");
    $database_bibles->storeChapter ("phpunit", 2, 3, "g");
    $database_bibles->optimize ();
    $usfm = $database_bibles->getChapter ("phpunit", 2, 3);
    $this->assertEquals ("g", $usfm);
  }


  public function testOptimizeTwo ()
  {
    // Test whether optimizing removes files with 0 size.
    $database_bibles = Database_Bibles::getInstance ();
    $id = $database_bibles->createBible ("phpunit");
    $database_bibles->storeChapter ("phpunit", 2, 3, "a");
    $database_bibles->storeChapter ("phpunit", 2, 3, "b");
    $database_bibles->storeChapter ("phpunit", 2, 3, "c");
    $database_bibles->storeChapter ("phpunit", 2, 3, "d");
    $database_bibles->storeChapter ("phpunit", 2, 3, "e");
    $database_bibles->storeChapter ("phpunit", 2, 3, "f");
    $database_bibles->storeChapter ("phpunit", 2, 3, "");
    $usfm = $database_bibles->getChapter ("phpunit", 2, 3);
    $this->assertEquals ("", $usfm);
    $database_bibles->optimize ();
    $usfm = $database_bibles->getChapter ("phpunit", 2, 3);
    $this->assertEquals ("f", $usfm);
  }


  public function testCreateGetDeleteBibles ()
  {
    $database_bibles = Database_Bibles::getInstance ();

    $id = $database_bibles->createBible ("phpunit");
    $this->assertEquals (1, $id);
    
    $bibles = $database_bibles->getBibles ();
    $this->assertEquals (array ("phpunit"), $bibles);

    $id = $database_bibles->getID ("phpunit2");
    $this->assertEquals (0, $id);
    
    $database_bibles->deleteBible ("phpunit");

    $id = $database_bibles->getID ("phpunit");
    $this->assertEquals (0, $id);
  }
  
  
  public function testNamesIDs ()
  {
    $database_bibles = Database_Bibles::getInstance ();

    $id = $database_bibles->getID ("phpunit");
    $this->assertEquals (0, $id);

    $bible = $database_bibles->getName (0);
    $this->assertEquals ("Unknown", $bible);

    $id = $database_bibles->createBible ("phpunit");
    $this->assertEquals (1, $id);

    $id = $database_bibles->getID ("phpunit");
    $this->assertEquals (1, $id);

    $bible = $database_bibles->getName (1);
    $this->assertEquals ("phpunit", $bible);
    
    $bible = $database_bibles->getName (2);
    $this->assertEquals ("Unknown", $bible);
  }


  public function testStoreGetChapter ()
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_bibles->createBible ("phpunit");
$usfm = <<<EOD
\c 1
\p
\v 1 Verse 1.
EOD;
    $database_bibles->storeChapter ("phpunit", 2, 1, $usfm);
    $result = $database_bibles->getChapter ("phpunit", 2, 1);
    $this->assertEquals ($usfm, $result);
    $result = $database_bibles->getChapter ("phpunit2", 2, 1);
    $this->assertEquals ("", $result);
    $result = $database_bibles->getChapter ("phpunit", 1, 1);
    $this->assertEquals ("", $result);
  }


  public function testBooks ()
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_bibles->createBible ("phpunit");
    $books = $database_bibles->getBooks ("phpunit");
    $this->assertEquals (array (), $books);
    $database_bibles->storeChapter ("phpunit", 1, 2, "\c 1");
    $books = $database_bibles->getBooks ("phpunit");
    $this->assertEquals (array (1), $books);
    $database_bibles->storeChapter ("phpunit", 2, 3, "\c 0");
    $books = $database_bibles->getBooks ("phpunit");
    $this->assertEquals (array (1, 2), $books);
    $database_bibles->deleteBook ("phpunit", 3);
    $books = $database_bibles->getBooks ("phpunit");
    $this->assertEquals (array (1, 2), $books);
    $database_bibles->deleteBook ("phpunit", 1);
    $books = $database_bibles->getBooks ("phpunit");
    $this->assertEquals (array (2), $books);
    $database_bibles->deleteBook ("phpunit2", 2);
    $books = $database_bibles->getBooks ("phpunit");
    $this->assertEquals (array (2), $books);
  }


  public function testChapters ()
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_bibles->createBible ("phpunit");
    $chapters = $database_bibles->getChapters ("phpunit", 1);
    $this->assertEquals (array (), $chapters);
    $database_bibles->storeChapter ("phpunit", 1, 2, "\c 1");
    $chapters = $database_bibles->getChapters ("phpunit", 1);
    $this->assertEquals (array (2), $chapters);
    $chapters = $database_bibles->getChapters ("phpunit", 2);
    $this->assertEquals (array (), $chapters);
    $database_bibles->storeChapter ("phpunit", 1, 3, "\c 1");
    $chapters = $database_bibles->getChapters ("phpunit", 1);
    $this->assertEquals (array (2, 3), $chapters);
    $database_bibles->deleteChapter ("phpunit", 3, 3);
    $chapters = $database_bibles->getChapters ("phpunit", 1);
    $this->assertEquals (array (2, 3), $chapters);
    $database_bibles->deleteChapter ("phpunit", 1, 2);
    $chapters = $database_bibles->getChapters ("phpunit", 1);
    $this->assertEquals (array (3), $chapters);
    $database_bibles->deleteChapter ("phpunit", 1, 3);
    $chapters = $database_bibles->getChapters ("phpunit", 1);
    $this->assertEquals (array (), $chapters);
  }


  public function testChapterIDs ()
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_bibles->createBible ("phpunit");
    $database_bibles->storeChapter ("phpunit", 1, 2, "\c 1");
    $firstId = $database_bibles->getChapterId ("phpunit", 1, 2);
    $this->assertGreaterThan (0, $firstId);
    $database_bibles->storeChapter ("phpunit", 1, 2, "\c 1");
    $id = $database_bibles->getChapterId ("phpunit", 1, 2);
    $this->assertEquals ($firstId + 1, $id);
    $database_bibles->storeChapter ("phpunit", 1, 2, "\c 1");
    $database_bibles->storeChapter ("phpunit", 1, 2, "\c 1");
    $id = $database_bibles->getChapterId ("phpunit", 1, 2);
    $this->assertEquals ($firstId + 3, $id);
    $database_bibles->storeChapter ("phpunit", 2, 3, "\c 1");
    $id = $database_bibles->getChapterId ("phpunit", 1, 2);
    $this->assertEquals ($firstId + 3, $id);
  }
  
  
  public function testBibleActionsOne ()
  {
    $database_bibleactions = Database_BibleActions::getInstance ();

    $database_bibleactions->optimize ();
    
    $bibles = $database_bibleactions->getBibles ();
    $this->assertEquals (array (), $bibles);

    $database_bibleactions->record ("phpunit1", 1, 2, "data112");
    $database_bibleactions->record ("phpunit1", 1, 3, "data113");
    $database_bibleactions->record ("phpunit1", 2, 4, "data124");
    $database_bibleactions->record ("phpunit2", 5, 6, "data256");
    $database_bibleactions->record ("phpunit2", 5, 6, "data256: Not to be stored");

    $bibles = $database_bibleactions->getBibles ();
    $this->assertEquals (array ("phpunit1", "phpunit2"), $bibles);

    $books = $database_bibleactions->getBooks ("phpunit1");
    $this->assertEquals (array (1, 2), $books);

    $chapters = $database_bibleactions->getChapters ("phpunit1", 1);
    $this->assertEquals (array (2, 3), $chapters);
    
    $chapters = $database_bibleactions->getChapters ("phpunit1", 2);
    $this->assertEquals (array (4), $chapters);
    
    $database_bibleactions->delete ("phpunit1", 2, 3);
    
    $chapters = $database_bibleactions->getChapters ("phpunit1", 2);
    $this->assertEquals (array (4), $chapters);
    
    $database_bibleactions->delete ("phpunit1", 2, 4);
    
    $chapters = $database_bibleactions->getChapters ("phpunit1", 2);
    $this->assertEquals (array (), $chapters);
    
    $usfm = $database_bibleactions->getUsfm ("phpunit2", 5, 5);
    $this->assertFalse ($usfm);
    
    $usfm = $database_bibleactions->getUsfm ("phpunit2", 5, 6);
    $this->assertEquals ("data256", $usfm);
  }


}


?>
