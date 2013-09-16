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

 
class databaseChangesUserTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    $database = Database_ChangesUser::getInstance ();
    $database->clearUser ("phpunit1");
    $database->clearUser ("phpunit2");
    $database->clearUser ("phpunit3");
  }


  public function testUsernames ()
  {
    $database = Database_ChangesUser::getInstance ();
    $users = $database->getUsernames ();
    $this->assertNotContains ("phpunit1", $users);
    $this->assertNotContains ("phpunit2", $users);
    $database->recordSave ("phpunit1", "bible", 1, 2, 3, "old", 4, "new");
    $users = $database->getUsernames ();
    $this->assertContains ("phpunit1", $users);
    $database->recordSave ("phpunit2", "bible", 1, 2, 3, "old", 4, "new");
    $database->recordSave ("phpunit3", "bible", 1, 2, 3, "old", 4, "new");
    $database->recordSave ("phpunit1", "bible", 1, 2, 3, "old", 4, "new");
    $database->recordSave ("phpunit2", "bible", 1, 2, 3, "old", 4, "new");
    $database->recordSave ("phpunit3", "bible", 1, 2, 3, "old", 4, "new");
    $users = $database->getUsernames ();
    $this->assertContains ("phpunit1", $users);
    $this->assertContains ("phpunit2", $users);
    $this->assertContains ("phpunit3", $users);
  }


  public function testBibles ()
  {
    $database = Database_ChangesUser::getInstance ();
    $bibles = $database->getBibles ("phpunit1");
    $this->assertEquals (array (), $bibles);
    $database->recordSave ("phpunit1", "bible1", 1, 2, 3, "old", 4, "new");
    $database->recordSave ("phpunit1", "bible1", 1, 2, 3, "old", 4, "new");
    $database->recordSave ("phpunit1", "bible1", 1, 2, 3, "old", 4, "new");
    $database->recordSave ("phpunit1", "bible1", 1, 2, 3, "old", 4, "new");
    $bibles = $database->getBibles ("phpunit1");
    $this->assertEquals (array ("bible1"), $bibles);
  }


  public function testBooks ()
  {
    $database = Database_ChangesUser::getInstance ();
    $books = $database->getBooks ("phpunit1", "bible1");
    $this->assertEquals (array (), $books);
    $database->recordSave ("phpunit1", "bible1", 1, 2, 3, "old", 4, "new");
    $database->recordSave ("phpunit1", "bible1", 2, 2, 3, "old", 4, "new");
    $database->recordSave ("phpunit1", "bible1", 3, 2, 3, "old", 4, "new");
    $database->recordSave ("phpunit1", "bible2", 4, 2, 3, "old", 4, "new");
    $books = $database->getBooks ("phpunit1", "bible1");
    $this->assertEquals (array (1, 2, 3), $books);
  }


  public function testChapters ()
  {
    $database = Database_ChangesUser::getInstance ();
    $chapters = $database->getChapters ("phpunit1", "bible1", 1);
    $this->assertEquals (array (), $chapters);
    $database->recordSave ("phpunit1", "bible1", 1, 2, 3, "old", 4, "new");
    $database->recordSave ("phpunit1", "bible1", 1, 3, 3, "old", 4, "new");
    $database->recordSave ("phpunit1", "bible1", 1, 4, 3, "old", 4, "new");
    $chapters = $database->getChapters ("phpunit1", "bible1", 1);
    $this->assertEquals (array (2, 3, 4), $chapters);
  }


  public function testIdentifiers ()
  {
    $database = Database_ChangesUser::getInstance ();
    $database->recordSave ("phpunit1", "bible", 1, 2, 3, "old", 4, "new");
    $database->recordSave ("phpunit1", "bible", 1, 2, 4, "old", 5, "new");
    $database->recordSave ("phpunit1", "bible", 1, 2, 5, "old", 6, "new");
    $identifiers = $database->getIdentifiers ("phpunit1", "bible", 1, 2);
    $this->assertEquals (array (array ('oldid' => 3, 'newid' => 4), array ('oldid' => 4, 'newid' => 5), array ('oldid' => 5, 'newid' => 6)), $identifiers);
  }


  public function testChapter ()
  {
    $database = Database_ChangesUser::getInstance ();
    $database->recordSave ("phpunit1", "bible", 1, 2, 3, "old", 4, "new");
    $database->recordSave ("phpunit1", "bible", 1, 2, 4, "old", 5, "new");
    $database->recordSave ("phpunit1", "bible", 1, 2, 5, "old", 6, "new");
    $chapter = $database->getChapter ("phpunit1", "bible", 1, 2, 3);
    $this->assertEquals (array ('oldtext' => 'old', 'newtext' => 'new'), $chapter);
  }


}


?>
