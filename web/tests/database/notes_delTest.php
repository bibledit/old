<?php

 
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
