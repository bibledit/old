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

 
class databaseMenuTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    $database_menu = Database_Menu::getInstance ();
    $database_menu->clearAllAccessCounts ("phpunit1");
    $database_menu->clearAllAccessCounts ("phpunit2");
  }


  public function testZeroCount ()
  {
    $database_menu = Database_Menu::getInstance ();
    $count = $database_menu->getAccessCount ("phpunit1", "test");
    $this->assertEquals (0, $count);
  }


  public function testIncreaseCount ()
  {
    $database_menu = Database_Menu::getInstance ();
    $database_menu->increaseAccessCount ("phpunit1", "test");
    $count = $database_menu->getAccessCount ("phpunit1", "test");
    $this->assertEquals (1, $count);
    $count = $database_menu->getAccessCount ("phpunit2", "test");
    $this->assertEquals (0, $count);
    $database_menu->increaseAccessCount ("phpunit1", "test");
    $database_menu->increaseAccessCount ("phpunit1", "test");
    $count = $database_menu->getAccessCount ("phpunit1", "test");
    $this->assertEquals (3, $count);
  }
  
      
}
?>
