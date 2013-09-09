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

 
class databaseNotesDelTest extends PHPUnit_Framework_TestCase
{
  private $identifier1 = 1234;
  private $identifier2 = 2345;
  private $identifier3 = 3456;


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    $database_notes = Database_Notes::getInstance ();
    $database_notes->unmarkForDeletion ($this->identifier1);
    $database_notes->unmarkForDeletion ($this->identifier2);
    $database_notes->unmarkForDeletion ($this->identifier3);
  }


  public function testOneNote ()
  {
    // It tests whether a note marked for deletion,
    // after touching it 7 or 8 times, is returned as due for deletion,
    // and whether it is not yet due for deletion before that.
    $database_notes = Database_Notes::getInstance ();
    $database_notes->markForDeletion ($this->identifier1);
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array (), $identifiers);
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array (), $identifiers);
    $database_notes->touchMarkedForDeletion ();
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array ($this->identifier1), $identifiers);
    $database_notes->touchMarkedForDeletion ();
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array ($this->identifier1), $identifiers);
  }


  public function testUnmarkNote ()
  {
    // It tests whethe a note marked for deletion,
    // touched 6 times, then unmarked, touched again,
    // will not be due for deletion.
    $database_notes = Database_Notes::getInstance ();
    $database_notes->markForDeletion ($this->identifier1);
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $database_notes->unmarkForDeletion ($this->identifier1);
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array (), $identifiers);
    $database_notes->touchMarkedForDeletion ();
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array (), $identifiers);
  }


  public function testMoreNotes ()
  {
    // It tests whether three notes, marked for deletion on different days,
    // are properly touched so they keep their own number of days.
    $database_notes = Database_Notes::getInstance ();
    $database_notes->markForDeletion ($this->identifier1);
    $database_notes->touchMarkedForDeletion ();
    $database_notes->markForDeletion ($this->identifier2);
    $database_notes->touchMarkedForDeletion ();
    $database_notes->markForDeletion ($this->identifier3);
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array (), $identifiers);
    $database_notes->touchMarkedForDeletion ();
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array ($this->identifier1), $identifiers);
    $database_notes->unmarkForDeletion ($this->identifier1);
    $database_notes->touchMarkedForDeletion ();
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array ($this->identifier2), $identifiers);
    $database_notes->unmarkForDeletion ($this->identifier2);
    $database_notes->touchMarkedForDeletion ();
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array ($this->identifier3), $identifiers);
  }  
 
      
  public function testIsMarked ()
  {
    $database_notes = Database_Notes::getInstance ();
    $database_notes->markForDeletion ($this->identifier1);
    $this->assertTrue ($database_notes->isMarkedForDeletion ($this->identifier1));
    $this->assertFalse ($database_notes->isMarkedForDeletion ($this->identifier2));
    $database_notes->unmarkForDeletion ($this->identifier2);
    $this->assertTrue ($database_notes->isMarkedForDeletion ($this->identifier1));
    $this->assertFalse ($database_notes->isMarkedForDeletion ($this->identifier2));
    $database_notes->unmarkForDeletion ($this->identifier1);
    $this->assertFalse ($database_notes->isMarkedForDeletion ($this->identifier1));
    $this->assertFalse ($database_notes->isMarkedForDeletion ($this->identifier2));
    $database_notes->markForDeletion ($this->identifier2);
    $this->assertTrue ($database_notes->isMarkedForDeletion ($this->identifier2));
  }
 
      
}
?>
