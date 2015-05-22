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

 
class databaseCheckTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
    $database_search = Database_Search::getInstance ();
    $database_search->create ();
    $database_bibles = Database_Bibles::getInstance ();
    $database_bibles->createBible ("phpunit");
  }

  
  public function tearDown ()
  {
    $database_search = Database_Search::getInstance ();
    $database_search->create ();
    $database_bibles = Database_Bibles::getInstance ();
    $database_check = Database_Check::getInstance ();
    $database_bibles->deleteBible ("phpunit");
    $database_check->truncateOutput ("");
    $database_check->release (0);
    $database_check->release (1);
    $database_check->release (2);
  }
  
  
  public function testOptimize ()
  {
    $database_check = Database_Check::getInstance ();
    $database_check->optimize ();
  }
  
  
  public function testRecordGetTruncate ()
  {
    $database_check = Database_Check::getInstance ();

    $hits = $database_check->getHits ();
    $this->assertEmpty ($hits);

    $database_check->recordOutput ("phpunit", 1, 2, 3, "test");
    $hits = $database_check->getHits ();
    $this->assertEquals (1, count ($hits));
    
    $database_check->truncateOutput ("");
    $hits = $database_check->getHits ();
    $this->assertEmpty ($hits);
  }


  public function testGettingDetails ()
  {
    $database_check = Database_Check::getInstance ();
    $database_check->recordOutput ("phpunit", 5, 2, 3, "test");
    $hits = $database_check->getHits ();
    $this->assertEquals (1, $hits [0] ['bible']);
    $this->assertEquals (5, $hits [0] ['book']);
    $this->assertEquals (2, $hits [0] ['chapter']);
    $this->assertEquals ("test", $hits [0] ['data']);
  }


  public function testApprovals ()
  {
    $database_check = Database_Check::getInstance ();

    $database_check->recordOutput ("phpunit", 3, 4, 5, "test1");
    $database_check->recordOutput ("phpunit", 3, 4, 5, "test2");

    $hits = $database_check->getHits ();
    $this->assertEquals (2, count ($hits));

    $id = $hits [0] ['rowid'];
    $database_check->approve ($id);
    $hits = $database_check->getHits ();
    $this->assertEquals (1, count ($hits));
    
    $suppressions = $database_check->getSuppressions ();
    $this->assertEquals (1, count ($suppressions));
    
    $id = $suppressions [0] ['rowid'];
    
    $database_check->release (1);
    $hits = $database_check->getHits ();
    $this->assertEquals (2, count ($hits));
  }

  
  public function testDelete ()
  {
    $database_check = Database_Check::getInstance ();
    $database_check->recordOutput ("phpunit", 3, 4, 5, "test1");
    $database_check->recordOutput ("phpunit", 3, 4, 5, "test2");
    $hits = $database_check->getHits ();
    $this->assertEquals (2, count ($hits));
    $id = $hits [0] ['rowid'];
    $database_check->delete ($id);
    $hits = $database_check->getHits ();
    $this->assertEquals (1, count ($hits));
  }


  public function testPassage ()
  {
    $database_check = Database_Check::getInstance ();
    $database_check->recordOutput ("phpunit", 3, 4, 5, "test1");
    $database_check->recordOutput ("phpunit", 6, 7, 8, "test2");
    $passage = $database_check->getPassage (2);
    $standard = array ('book' => 6, 'chapter' => 7, 'verse' => 8);
    $this->assertEquals ($standard, $passage);
  }
  
      
}


?>
