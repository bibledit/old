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


class biblesTest extends PHPUnit_Framework_TestCase
{


  private $usfm;


  protected function setUp () 
  {
    $this->tearDown ();
    $database_search = Database_Search::getInstance ();
    $database_search->create ();
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
    $database_search = Database_Search::getInstance ();
    $database_search->create ();
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


  public function testSafeStoreChapterNoChange ()
  {
    $database_bibles = Database_Bibles::getInstance ();
    $currentId = $database_bibles->getChapterId ("phpunit", 1, 1);
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
    $this->assertEquals ($this->usfm, $result);
    $currentId2 = $database_bibles->getChapterId ("phpunit", 1, 1);
    $this->assertEquals ($currentId, $currentId2);
  }


}


?>
