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


class databaseModificationsUsersTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $database_modifications->clearUserUser ("phpunit1");
    $database_modifications->clearUserUser ("phpunit2");
    $database_modifications->clearUserUser ("phpunit3");
  }


  public function testUsernames ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $users = $database_modifications->getUserUsernames ();
    $this->assertNotContains ("phpunit1", $users);
    $this->assertNotContains ("phpunit2", $users);
    $database_modifications->recordUserSave ("phpunit1", "bible", 1, 2, 3, "old", 4, "new");
    $users = $database_modifications->getUserUsernames ();
    $this->assertContains ("phpunit1", $users);
    $database_modifications->recordUserSave ("phpunit2", "bible", 1, 2, 3, "old", 5, "new");
    $database_modifications->recordUserSave ("phpunit3", "bible", 1, 2, 3, "old", 6, "new");
    $database_modifications->recordUserSave ("phpunit1", "bible", 1, 2, 3, "old", 7, "new");
    $database_modifications->recordUserSave ("phpunit2", "bible", 1, 2, 3, "old", 8, "new");
    $database_modifications->recordUserSave ("phpunit3", "bible", 1, 2, 3, "old", 9, "new");
    $users = $database_modifications->getUserUsernames ();
    $this->assertContains ("phpunit1", $users);
    $this->assertContains ("phpunit2", $users);
    $this->assertContains ("phpunit3", $users);
  }


  public function testBibles ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $bibles = $database_modifications->getUserBibles ("phpunit1");
    $this->assertEquals (array (), $bibles);
    $database_modifications->recordUserSave ("phpunit1", "bible1", 1, 2, 3, "old", 4, "new");
    $database_modifications->recordUserSave ("phpunit1", "bible1", 1, 2, 3, "old", 5, "new");
    $database_modifications->recordUserSave ("phpunit1", "bible1", 1, 2, 3, "old", 6, "new");
    $database_modifications->recordUserSave ("phpunit1", "bible1", 1, 2, 3, "old", 7, "new");
    $bibles = $database_modifications->getUserBibles ("phpunit1");
    $this->assertEquals (array ("bible1"), $bibles);
  }


  public function testBooks ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $books = $database_modifications->getUserBooks ("phpunit1", "bible1");
    $this->assertEquals (array (), $books);
    $database_modifications->recordUserSave ("phpunit1", "bible1", 1, 2, 3, "old", 4, "new");
    $database_modifications->recordUserSave ("phpunit1", "bible1", 2, 2, 3, "old", 5, "new");
    $database_modifications->recordUserSave ("phpunit1", "bible1", 3, 2, 3, "old", 6, "new");
    $database_modifications->recordUserSave ("phpunit1", "bible2", 4, 2, 3, "old", 7, "new");
    $books = $database_modifications->getUserBooks ("phpunit1", "bible1");
    $this->assertEquals (array (1, 2, 3), $books);
  }


  public function testChapters ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $chapters = $database_modifications->getUserChapters ("phpunit1", "bible1", 1);
    $this->assertEquals (array (), $chapters);
    $database_modifications->recordUserSave ("phpunit1", "bible1", 1, 2, 3, "old", 4, "new");
    $database_modifications->recordUserSave ("phpunit1", "bible1", 1, 3, 3, "old", 5, "new");
    $database_modifications->recordUserSave ("phpunit1", "bible1", 1, 4, 3, "old", 6, "new");
    $chapters = $database_modifications->getUserChapters ("phpunit1", "bible1", 1);
    $this->assertEquals (array (2, 3, 4), $chapters);
  }


  public function testIdentifiers ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $database_modifications->recordUserSave ("phpunit1", "bible", 1, 2, 3, "old", 4, "new");
    $database_modifications->recordUserSave ("phpunit1", "bible", 1, 2, 4, "old", 5, "new");
    $database_modifications->recordUserSave ("phpunit1", "bible", 1, 2, 5, "old", 6, "new");
    $identifiers = $database_modifications->getUserIdentifiers ("phpunit1", "bible", 1, 2);
    $this->assertEquals (array (array ('oldid' => 3, 'newid' => 4), array ('oldid' => 4, 'newid' => 5), array ('oldid' => 5, 'newid' => 6)), $identifiers);
  }


  public function testChapter ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $database_modifications->recordUserSave ("phpunit1", "bible", 1, 2, 3, "old1", 4, "new1");
    $database_modifications->recordUserSave ("phpunit1", "bible", 1, 2, 4, "old2", 5, "new2");
    $database_modifications->recordUserSave ("phpunit1", "bible", 1, 2, 5, "old3", 6, "new3");
    $chapter = $database_modifications->getUserChapter ("phpunit1", "bible", 1, 2, 4);
    $this->assertEquals (array ('oldtext' => 'old1', 'newtext' => 'new1'), $chapter);
    $chapter = $database_modifications->getUserChapter ("phpunit1", "bible", 1, 2, 5);
    $this->assertEquals (array ('oldtext' => 'old2', 'newtext' => 'new2'), $chapter);
  }
  
  
  public function testTimestamp ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $database_modifications->recordUserSave ("phpunit1", "bible", 1, 2, 3, "old1", 4, "new1");
    $time = $database_modifications->getUserTimestamp ("phpunit1", "bible", 1, 2, 4);
    $this->assertGreaterThanOrEqual (time (), $time);
    $this->assertLessThanOrEqual (time () + 1, $time);
  }


}


?>
