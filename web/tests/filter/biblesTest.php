<?php


class biblesTest extends PHPUnit_Framework_TestCase
{


  private $usfm;


  protected function setUp () 
  {
    $this->tearDown ();
    $database_bibles = Database_Bibles::getInstance ();
    $database_bibles->createBible ("phpunit");
$this->usfm = <<<'EOD'
\c 1
\p
\v 1 Verse 1.
\v 2 Verse 2.
\v 3 Verse 3.
\v 4 Verse 4.
\v 5 Verse 5.
EOD;
    $database_bibles->storeChapter ("phpunit", 1, 1, $this->usfm);
  }


  protected function tearDown () 
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_bibles->deleteBible ("phpunit");
  }


  public function testSafeStoreChapterSaveOne ()
  {
    $database_bibles = Database_Bibles::getInstance ();
$usfm = <<<'EOD'
\c 1
\p
\v 1 Verse 1.
\v 2 Verse 2.
\v 3 Verse 3.
\v 4 Verse 4.
\v 5 Verse 5.
EOD;
    $result = Filter_Bibles::safeStoreChapter ("phpunit", 1, 1, $usfm);
    $this->assertTrue ($result);
    $result = $database_bibles->getChapter ("phpunit", 1, 1);    
    $this->assertEquals($this->usfm, $result);
  }


  public function testSafeStoreChapterSaveTwo ()
  {
    $database_bibles = Database_Bibles::getInstance ();
$usfm = <<<'EOD'
\c 1
\p
\v 1 Verse 1.
\v 2 Verse 2.
\v 3 Verse 3.
\v 4 Verse 4.
EOD;
    $result = Filter_Bibles::safeStoreChapter ("phpunit", 1, 1, $usfm);
    $this->assertTrue ($result);
    $result = $database_bibles->getChapter ("phpunit", 1, 1);    
    $this->assertEquals($usfm, $result);
  }


  public function testSafeStoreChapterLength ()
  {
    $database_bibles = Database_Bibles::getInstance ();
$usfm = <<<'EOD'
\c 1
\p
\v 1 Verse 1.
\v 2 Verse 2.
\v 3 Verse 3.
EOD;
    $result = Filter_Bibles::safeStoreChapter ("phpunit", 1, 1, $usfm);
    $this->assertFalse ($result);
    $result = $database_bibles->getChapter ("phpunit", 1, 1);    
    $this->assertEquals($this->usfm, $result);
  }


  public function testSafeStoreChapterSimilarity ()
  {
    $database_bibles = Database_Bibles::getInstance ();
$usfm = <<<'EOD'
\c 1
\p
\v 1 Verse 1.
\v 3 Verse 3.
\v 2 Verse 2.
\v 4 Verse 4.
EOD;
    $result = Filter_Bibles::safeStoreChapter ("phpunit", 1, 1, $usfm);
    $this->assertFalse ($result);
    $result = $database_bibles->getChapter ("phpunit", 1, 1);    
    $this->assertEquals($this->usfm, $result);
  }


}


?>
