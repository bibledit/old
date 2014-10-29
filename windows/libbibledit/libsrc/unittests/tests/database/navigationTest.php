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

 
class databaseNavigationTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    $database = Database_Navigation::getInstance ();
    $database->trim ();
  }


  public function testTrim ()
  {
    $database = Database_Navigation::getInstance ();
    $database->trim ();
  }
  

  public function testOne ()
  {
    $database = Database_Navigation::getInstance ();
    // Use current time.
    $time = time ();
    // Record one entry. As a result there should be no previous entry.
    $database->record ($time, "phpunit", 1, 2, 3);
    $previous = $database->previousExists ("phpunit");
    $this->assertFalse ($previous);
    // Record another entry, with the same time.
    // This should remove the already existing entry.
    // As a result there should be no previous entry.
    $database->record ($time, "phpunit", 4, 5, 6);
    $previous = $database->previousExists ("phpunit");
    $this->assertFalse ($previous);
    // Record another entry 4 seconds later.
    // This should remove the already existing entry.
    // As a result there should be no previous entry.
    $time += 4;
    $database->record ($time, "phpunit", 4, 5, 6);
    $previous = $database->previousExists ("phpunit");
    $this->assertFalse ($previous);
    // Record another entry 5 seconds later.
    // This should remove the already existing entry.
    // As a result there should be no previous entry.
    $time += 5;
    $database->record ($time, "phpunit", 4, 5, 6);
    $previous = $database->previousExists ("phpunit");
    $this->assertFalse ($previous);
    // Record another entry 6 seconds later.
    // This should not remove the already existing entry.
    // As a result there should be a previous entry.
    $time += 6;
    $database->record ($time, "phpunit", 4, 5, 6);
    $previous = $database->previousExists ("phpunit");
    $this->assertTrue ($previous);
  }


  public function testTwo ()
  {
    $database = Database_Navigation::getInstance ();
    // Use current time.
    $time = time ();
    // Record one entry, and another 6 seconds later.
    $database->record ($time, "phpunit", 1, 2, 3);
    $time += 6;
    $database->record ($time, "phpunit", 4, 5, 6);
    // Get previous entry, which should be the first one entered.
    $passage = $database->getPrevious ("phpunit");
    $this->assertEquals (array (1, 2, 3), $passage);
  }
  
  
  public function testThree ()
  {
    $database = Database_Navigation::getInstance ();
    // Use current time.
    $time = time ();
    // Record one entry, and another 6 seconds later.
    $database->record ($time, "phpunit", 1, 2, 3);
    $time += 6;
    $database->record ($time, "phpunit", 4, 5, 6);
    // Get previous entry for another user: It should not be there.
    $passage = $database->getPrevious ("phpunit2");
    $this->assertEquals (NULL, $passage);
  }


  public function testFour ()
  {
    $database = Database_Navigation::getInstance ();
    // Use current time.
    $time = time ();
    // Record three entries, each one 6 seconds later.
    $database->record ($time, "phpunit", 1, 2, 3);
    $time += 6;
    $database->record ($time, "phpunit", 4, 5, 6);
    $time += 6;
    $database->record ($time, "phpunit", 7, 8, 9);
    // Get previous entry, which should be the second one entered.
    $passage = $database->getPrevious ("phpunit");
    $this->assertEquals (array (4, 5, 6), $passage);
  }
  
  
  public function testFive ()
  {
    $database = Database_Navigation::getInstance ();
    // Use current time.
    $time = time ();
    // Record five entries, each one 6 seconds later.
    $database->record ($time, "phpunit", 1, 2, 3);
    $time += 6;
    $database->record ($time, "phpunit", 4, 5, 6);
    $time += 6;
    $database->record ($time, "phpunit", 7, 8, 9);
    $time += 6;
    $database->record ($time, "phpunit", 10, 11, 12);
    $time += 6;
    $database->record ($time, "phpunit", 13, 14, 15);
    // Get previous entry, which should be the last but one passage recorded.
    $passage = $database->getPrevious ("phpunit");
    $this->assertEquals (array (10, 11, 12), $passage);
  }
  
  
  public function testSix ()
  {
    $database = Database_Navigation::getInstance ();
    // There should be no next passage.
    $passage = $database->getNext ("phpunit");
    $this->assertEquals (NULL, $passage);
  }
  
  
  public function testSeven ()
  {
    $database = Database_Navigation::getInstance ();
    // Use current time.
    $time = time ();
    // Record several entries, all spaced apart by 6 seconds.
    $database->record ($time, "phpunit", 1, 2, 3);
    $time += 6;
    $database->record ($time, "phpunit", 1, 2, 3);
    $time += 6;
    $database->record ($time, "phpunit", 1, 2, 3);
    $time += 6;
    $database->record ($time, "phpunit", 1, 2, 3);
    $time += 6;
    $database->record ($time, "phpunit", 1, 2, 3);
    $passage = $database->getNext ("phpunit");
    $this->assertEquals (NULL, $passage);
  }
  
  
  public function testEight ()
  {
    // Record two entries at an interval.
    $database = Database_Navigation::getInstance ();
    $time = time ();
    $database->record ($time, "phpunit", 1, 2, 3);
    $time += 6;
    $database->record ($time, "phpunit", 4, 5, 6);
    // Next entry is not there.
    $passage = $database->getNext ("phpunit");
    $this->assertEquals (NULL, $passage);
    // Previous entry should be there.
    $passage = $database->getPrevious ("phpunit");
    $this->assertEquals (array (1, 2, 3), $passage);
    // Next entry should be there since we moved to the previous one.
    $passage = $database->getNext ("phpunit");
    $this->assertEquals (array (4, 5, 6), $passage);
    // Previous entry should be there.
    $passage = $database->getPrevious ("phpunit");
    $this->assertEquals (array (1, 2, 3), $passage);
    // Previous entry before previous entry should not be there.
    $passage = $database->getPrevious ("phpunit");
    $this->assertEquals (NULL, $passage);
    // Next entry should be there since we moved to the previous one.
    $passage = $database->getNext ("phpunit");
    $this->assertEquals (array (4, 5, 6), $passage);
    // The entry next to the next entry should not be there.
    $passage = $database->getNext ("phpunit");
    $this->assertEquals (NULL, $passage);
  }
  
  
}


?>
