<?php
require_once 'PHPUnit/Framework.php';
 
class databaseNotesTest extends PHPUnit_Framework_TestCase
{

  public function setUp ()
  {
    $this->tearDown ();
    $database_bibles = Database_Bibles::getInstance ();
    $database_bibles->createBible ("none");
  }
  
  public function tearDown ()
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_bibles->deleteBible ("none");
    $database_bibles->truncateDiffs ();
  }

  public function testDiffs()
  {
    $database_bibles = Database_Bibles::getInstance ();
    
    // No diff should exist at this stage.
    $this->assertFalse ($database_bibles->diffExists ("none", 1, 2));
    
    // After storing a chapter once, no diff should exist,
    // because there was no data in the chapter yet.
    $database_bibles->storeChapter ("none", 1, 2, "chapter text one");
    $this->assertFalse ($database_bibles->diffExists ("none", 1, 2));
    
    // After storing a chapter for the second time, the diff should exist.
    $database_bibles->storeChapter ("none", 1, 2, "chapter text two");
    $this->assertTrue ($database_bibles->diffExists ("none", 1, 2));
    
    // There diff data should not exist for another chapter.
    $this->assertFalse ($database_bibles->diffExists ("none", 1, 1));
    
    // Test function storeDiffBook. It stores diff data for the whole book.
    $this->assertFalse ($database_bibles->diffExists ("none", 2, 1));
    $this->assertFalse ($database_bibles->diffExists ("none", 2, 2));
    $this->assertFalse ($database_bibles->diffExists ("none", 2, 3));
    $database_bibles->storeChapter ("none", 2, 1, "chapter text");
    $database_bibles->storeChapter ("none", 2, 2, "chapter text");
    $database_bibles->storeChapter ("none", 2, 3, "chapter text");
    $this->assertFalse ($database_bibles->diffExists ("none", 2, 1));
    $this->assertFalse ($database_bibles->diffExists ("none", 2, 2));
    $this->assertFalse ($database_bibles->diffExists ("none", 2, 3));
    $database_bibles->storeDiffBook ("none", 2);
    $this->assertTrue ($database_bibles->diffExists ("none", 2, 1));
    $this->assertTrue ($database_bibles->diffExists ("none", 2, 2));
    $this->assertTrue ($database_bibles->diffExists ("none", 2, 3));

    // Test function storeDiffBible. It stores diff data for the whole bible.
    $this->assertFalse ($database_bibles->diffExists ("none", 3, 1));
    $this->assertFalse ($database_bibles->diffExists ("none", 4, 1));
    $this->assertFalse ($database_bibles->diffExists ("none", 5, 1));
    $database_bibles->storeChapter ("none", 3, 1, "chapter text");
    $database_bibles->storeChapter ("none", 4, 1, "chapter text");
    $database_bibles->storeChapter ("none", 5, 1, "chapter text");
    $this->assertFalse ($database_bibles->diffExists ("none", 3, 1));
    $this->assertFalse ($database_bibles->diffExists ("none", 4, 1));
    $this->assertFalse ($database_bibles->diffExists ("none", 5, 1));
    $database_bibles->storeDiffBible ("none");
    $this->assertTrue ($database_bibles->diffExists ("none", 3, 1));
    $this->assertTrue ($database_bibles->diffExists ("none", 4, 1));
    $this->assertTrue ($database_bibles->diffExists ("none", 5, 1));
    
    // Test function to retrieve the diff data for a chapter.
    $this->assertEquals ($database_bibles->getDiff ("none", 1, 1), "");
    $this->assertEquals ($database_bibles->getDiff ("none", 1, 2), "chapter text one");
    $this->assertEquals ($database_bibles->getDiff ("none", 3, 1), "chapter text");
    
    // Test function to retrieve the chapters that have diff data.
    $this->assertEquals ($database_bibles->getDiffChapters ("none", 1), array (2));
    $this->assertEquals ($database_bibles->getDiffChapters ("none", 2), array (1, 2, 3));
    $this->assertEquals ($database_bibles->getDiffBooks ("none"), array (1, 2, 3, 4, 5));
    $this->assertGreaterThanOrEqual ($database_bibles->getDiffBibles (), array ("none"));
    $this->assertLessThanOrEqual (array ("none"), $database_bibles->getDiffBibles ());
    
    // Test some other code just to be sure that it is valid.
    $database_bibles->deleteBook ("none", 4);
    $database_bibles->deleteChapter ("none", 5, 5);

  }



      
}
?>


