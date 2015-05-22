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

 
class databaseSprintTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    $database = Database_Sprint::getInstance ();
    $ids = $database->getTasks ("phpunit", 2014, 1);
    foreach ($ids as $id) {
      $database->deleteTask ($id);
    }
    $ids = $database->getTasks ("phpunit", 2014, 2);
    foreach ($ids as $id) {
      $database->deleteTask ($id);
    }
    $database->clearHistory ("phpunit", 2014, 1);
    $database->clearHistory ("phpunit", 2014, 2);
  }
  
  
  public function testMaintenance ()
  {
    $database = Database_Sprint::getInstance ();
    $database->optimize ();
  }


  public function testStoreTaskGetTask ()
  {
    $database = Database_Sprint::getInstance ();
    $ids = $database->getTasks ("phpunit", 2014, 1);
    $this->assertEquals (array (), $ids);
    $database->storeTask ("phpunit", 2014, 1, "phpunit");
    $ids = $database->getTasks ("phpunit", 2014, 1);
    $this->assertEquals (1, count ($ids));
    $ids = $database->getTasks ("phpunit", 2014, 2);
    $this->assertEquals (0, count ($ids));
    $ids = $database->getTasks ("phpunit2", 2014, 1);
    $this->assertEquals (0, count ($ids));
  }


  public function testGetTitle ()
  {
    $database = Database_Sprint::getInstance ();
    $database->storeTask ("phpunit", 2014, 1, "xyz");
    $ids = $database->getTasks ("phpunit", 2014, 1);
    $title = $database->getTitle ($ids[0]);
    $this->assertEquals ("xyz", $title);
  }
  
  
  public function testComplete ()
  {
    $database = Database_Sprint::getInstance ();
    $database->storeTask ("phpunit", 2014, 1, "phpunit");
    $ids = $database->getTasks ("phpunit", 2014, 1);
    $id = $ids[0];
    $complete = $database->getComplete ($id);
    $this->assertEquals (0, $complete);
    $database->updateComplete ($id, 95);
    $complete = $database->getComplete ($id);
    $this->assertEquals (95, $complete);
  }


  public function testHistory ()
  {
    $database = Database_Sprint::getInstance ();
    // Expect no history at all for January 2014.
    $history = $database->getHistory ("phpunit", 2014, 1);
    $this->assertEquals (array (), $history);
    // Log values for January 2014, and check that the database returns those values.
    $database->logHistory ("phpunit", 2014, 1, 10, 15, 50);
    $history = $database->getHistory ("phpunit", 2014, 1);
    $standard = array (
                        array ('day' => 10, 'tasks' => 15, 'complete' => 50)
                      );
    $this->assertEquals ($standard, $history);
    // Log values for February 2014, and don't expect them when requesting the history for January ...
    $database->logHistory ("phpunit", 2014, 2, 10, 15, 51);
    $history = $database->getHistory ("phpunit", 2014, 1);
    $standard = array (
                        array ('day' => 10, 'tasks' => 15, 'complete' => 50)
                      );
    $this->assertEquals ($standard, $history);
    // ... but get those values when requesting history for February.
    $history = $database->getHistory ("phpunit", 2014, 2);
    $standard = array (
                        array ('day' => 10, 'tasks' => 15, 'complete' => 51)
                      );
    $this->assertEquals ($standard, $history);
    // Log another history entry for January 2014, and expect two correct entries for this month.
    $database->logHistory ("phpunit", 2014, 1, 11, 16, 55);
    $history = $database->getHistory ("phpunit", 2014, 1);
    $standard = array (
                        array ('day' => 10, 'tasks' => 15, 'complete' => 50),
                        array ('day' => 11, 'tasks' => 16, 'complete' => 55)
                      );
    $this->assertEquals ($standard, $history);
  }


}


?>
