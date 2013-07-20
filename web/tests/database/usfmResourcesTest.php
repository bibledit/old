<?php

 
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


      
}
?>
