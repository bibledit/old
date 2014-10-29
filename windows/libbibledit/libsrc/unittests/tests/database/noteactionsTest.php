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

 
class databaseNoteActionsTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    $database = Database_NoteActions::getInstance ();
    $database->clear ();
  }
  
  
  public function testOptimize ()
  {
    $database = Database_Sprint::getInstance ();
    $database->optimize ();
  }


  public function testRecord ()
  {
    $database = Database_NoteActions::getInstance ();
    $database->record ("phpunit", 2, 3, "content");
    $notes = $database->getNotes ();
    $this->assertEquals (array (2), $notes);
  }


  public function testGetNotes ()
  {
    $database = Database_NoteActions::getInstance ();
    $database->record ("phpunit", 2, 3, "content");
    $database->record ("phpunit", 2, 4, "content");
    $database->record ("phpunit", 3, 3, "content");
    $notes = $database->getNotes ();
    $this->assertEquals (array (2, 3), $notes);
  }


  public function testGetNoteData ()
  {
    $database = Database_NoteActions::getInstance ();
    $database->record ("phpunit1", 2, 3, "content3");
    $database->record ("phpunit2", 2, 4, "content4");
    $database->record ("phpunit3", 3, 4, "content5");
    $data = $database->getNoteData (2);
    $data [0] ["timestamp"] = 12345678;
    $data [1] ["timestamp"] = 12345678;
    $standard = array (
      array ('rowid' => 1, 'username' => "phpunit1", 'timestamp' => 12345678, 'action' => 3, 'content' => "content3"),
      array ('rowid' => 2, 'username' => "phpunit2", 'timestamp' => 12345678, 'action' => 4, 'content' => "content4"),
    );
    $this->assertEquals ($standard, $data);
  }


  public function testUpdateNotes ()
  {
    $database = Database_NoteActions::getInstance ();
    $database->record ("phpunit", 2, 3, "content");
    $database->record ("phpunit", 2, 4, "content");
    $database->record ("phpunit", 3, 3, "content");
    $database->updateNotes (2, 12345);
    $notes = $database->getNotes ();
    $this->assertEquals (array (12345, 3), $notes);
  }


  public function testDelete ()
  {
    $database = Database_NoteActions::getInstance ();
    $database->record ("phpunit1", 2, 3, "content1");
    $database->record ("phpunit2", 4, 5, "content2");
    $database->delete (1);
    $notes = $database->getNotes ();
    $this->assertEquals (array (4), $notes);
  }


}


?>
