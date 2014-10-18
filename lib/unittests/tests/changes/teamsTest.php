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

 
class modificationsTeamsTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
    $database_search = Database_Search::getInstance ();
    $database_search->create ();
    $database_bibles = Database_Bibles::getInstance ();
    $database_bibles->createBible ("phpunit");
    $database_bibles->createBible ("phpunit2");
  }

  
  public function tearDown ()
  {
    $database_search = Database_Search::getInstance ();
    $database_search->create ();
    $database_bibles = Database_Bibles::getInstance ();
    $database_modifications = Database_Modifications::getInstance ();
    $database_bibles->deleteBible ("phpunit");
    $database_bibles->deleteBible ("phpunit2");
    $database_modifications->truncateTeams ();
  }
  
  
  public function testTeamExistence ()
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_modifications = Database_Modifications::getInstance ();
 
    // Initially no diff should exist.
    $exists = $database_modifications->teamDiffExists ("phpunit", 1, 2);
    $this->assertFalse ($exists);

    // After storing a chapter there should be a diff.
    Bible_Logic::storeChapter ("phpunit", 1, 2, "chapter text one");
    $exists = $database_modifications->teamDiffExists ("phpunit", 1, 2);
    $this->assertTrue ($exists);

    // After storing a chapter for the second time, the diff should still exist.
    Bible_Logic::storeChapter ("none", 1, 2, "chapter text two");
    $exists = $database_modifications->teamDiffExists ("phpunit", 1, 2);
    $this->assertTrue ($exists);
    
    // The diff data should not exist for another chapter.
    $this->assertFalse ($database_modifications->teamDiffExists ("phpunit", 1, 1));
  }
  

  public function testTeamBook ()
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_modifications = Database_Modifications::getInstance ();

    $this->assertFalse ($database_modifications->teamDiffExists ("phpunit", 2, 1));
    $this->assertFalse ($database_modifications->teamDiffExists ("phpunit", 2, 2));
    $this->assertFalse ($database_modifications->teamDiffExists ("phpunit", 2, 3));

    Bible_Logic::storeChapter ("phpunit", 2, 1, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 2, 2, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 2, 3, "chapter text");

    $this->assertTrue ($database_modifications->teamDiffExists ("phpunit", 2, 1));
    $this->assertTrue ($database_modifications->teamDiffExists ("phpunit", 2, 2));
    $this->assertTrue ($database_modifications->teamDiffExists ("phpunit", 2, 3));

    $database_modifications->truncateTeams ();

    // Test function storeTeamDiffBook.
    $database_modifications->storeTeamDiffBook ("phpunit", 2);
    $this->assertTrue ($database_modifications->teamDiffExists ("phpunit", 2, 1));
    $this->assertTrue ($database_modifications->teamDiffExists ("phpunit", 2, 2));
    $this->assertTrue ($database_modifications->teamDiffExists ("phpunit", 2, 3));
  }
  
  
  public function testTeamBible ()
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_modifications = Database_Modifications::getInstance ();

    $this->assertFalse ($database_modifications->teamDiffExists ("phpunit", 3, 1));
    $this->assertFalse ($database_modifications->teamDiffExists ("phpunit", 4, 1));
    $this->assertFalse ($database_modifications->teamDiffExists ("phpunit", 5, 1));

    Bible_Logic::storeChapter ("phpunit", 3, 1, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 4, 1, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 5, 1, "chapter text");

    $this->assertTrue ($database_modifications->teamDiffExists ("phpunit", 3, 1));
    $this->assertTrue ($database_modifications->teamDiffExists ("phpunit", 4, 1));
    $this->assertTrue ($database_modifications->teamDiffExists ("phpunit", 5, 1));

    $database_modifications->truncateTeams ();

    // Test function storeTeamDiffBible. It stores diff data for the whole bible.
    $database_modifications->storeTeamDiffBible ("phpunit");

    $this->assertTrue ($database_modifications->teamDiffExists ("phpunit", 3, 1));
    $this->assertTrue ($database_modifications->teamDiffExists ("phpunit", 4, 1));
    $this->assertTrue ($database_modifications->teamDiffExists ("phpunit", 5, 1));
  }


  public function testTeamGetDiff ()
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_modifications = Database_Modifications::getInstance ();

    $diff = $database_modifications->getTeamDiff ("phpunit", 1, 1);
    $this->assertEquals ("", $diff);

    Bible_Logic::storeChapter ("phpunit", 3, 1, "chapter text");
    $database_modifications->truncateTeams ();
    Bible_Logic::storeChapter ("phpunit", 3, 1, "longer chapter text");
    $diff = $database_modifications->getTeamDiff ("phpunit", 3, 1);
    $this->assertEquals ("chapter text", $diff);

    $diff = $database_modifications->getTeamDiff ("phpunit", 1, 2);
    $this->assertEquals ("", $diff);

    Bible_Logic::storeChapter ("phpunit", 5, 5, "chapter text");
    $database_modifications->truncateTeams ();
    Bible_Logic::storeChapter ("phpunit", 5, 5, "longer chapter text");
    $diff = $database_modifications->getTeamDiff ("phpunit", 5, 5);
    $this->assertEquals ("chapter text", $diff);
  }
  
  
  public function testTeamGetDiffChapters ()
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_modifications = Database_Modifications::getInstance ();

    Bible_Logic::storeChapter ("phpunit", 3, 1, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 3, 3, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 3, 5, "chapter text");

    $chapters = $database_modifications->getTeamDiffChapters ("phpunit", 1);
    $this->assertEquals (array (), $chapters);

    $chapters = $database_modifications->getTeamDiffChapters ("phpunit", 3);
    $this->assertEquals (array (1, 3, 5), $chapters);

    $chapters = $database_modifications->getTeamDiffChapters ("phpunit2", 3);
    $this->assertEquals (array (), $chapters);
  }


  public function testTeamDiffBible ()
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_modifications = Database_Modifications::getInstance ();

    Bible_Logic::storeChapter ("phpunit", 3, 1, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 3, 3, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 3, 5, "chapter text");
    
    $bibles = $database_modifications->getTeamDiffBibles ();
    $this->assertEquals (array ("phpunit"), $bibles);
    
    $database_modifications->deleteTeamDiffBible ("phpunit2");

    $bibles = $database_modifications->getTeamDiffBibles ();
    $this->assertEquals (array ("phpunit"), $bibles);

    $database_modifications->deleteTeamDiffBible ("phpunit");

    $bibles = $database_modifications->getTeamDiffBibles ();
    $this->assertEquals (array (), $bibles);
  }


  public function testTeamDiffChapter ()
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_modifications = Database_Modifications::getInstance ();

    Bible_Logic::storeChapter ("phpunit", 3, 1, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 3, 3, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 3, 5, "chapter text");
    
    $chapters = $database_modifications->getTeamDiffChapters ("phpunit", 3);
    $this->assertEquals (array (1, 3, 5), $chapters);

    $database_modifications->deleteTeamDiffChapter ("phpunit", 3, 1);

    $chapters = $database_modifications->getTeamDiffChapters ("phpunit", 3);
    $this->assertEquals (array (3, 5), $chapters);
  }


  public function testTeamDiffBook ()
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_modifications = Database_Modifications::getInstance ();

    Bible_Logic::storeChapter ("phpunit", 3, 1, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 3, 3, "chapter text");
    Bible_Logic::storeChapter ("phpunit", 4, 5, "chapter text");

    $books = $database_modifications->getTeamDiffBooks ("phpunit");
    $this->assertEquals (array (3, 4), $books);

    $books = $database_modifications->getTeamDiffBooks ("phpunit2");
    $this->assertEquals (array (), $books);
  }


  public function testGetTeamDiffCount ()
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_modifications = Database_Modifications::getInstance ();

    Bible_Logic::storeChapter ("phpunit", 3, 1, "chapter text");
    Bible_Logic::storeChapter ("phpunit2", 3, 3, "chapter text");
    Bible_Logic::storeChapter ("phpunit2", 3, 5, "chapter text");

    $count = $database_modifications->getTeamDiffCount ("phpunit");
    $this->assertEquals (1, $count);

    $count = $database_modifications->getTeamDiffCount ("phpunit2");
    $this->assertEquals (2, $count);

    $count = $database_modifications->getTeamDiffCount ("phpunit3");
    $this->assertEquals (0, $count);
  }
  
     
}


?>
