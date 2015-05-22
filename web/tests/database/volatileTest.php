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


class databaseVolatileTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $database_volatile = Database_Volatile::getInstance ();
    $database_volatile->create ();
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
  }


  public function testKeys ()
  {
    $database_volatile = Database_Volatile::getInstance ();
    
    // There should be no keys initially.
    $keys = $database_volatile->getKeys (1);
    $this->assertEmpty ($keys);

    // Store two keys/values.
    // There should be two keys now.
    $database_volatile->setValue (1, "key1", "value");
    $database_volatile->setValue (1, "key2", "value");
    $keys = $database_volatile->getKeys (1);
    $this->assertEquals (2, count ($keys));

    // A different identifier should have no keys.
    $keys = $database_volatile->getKeys (2);
    $this->assertEmpty ($keys);
  }
  

  public function testValues ()
  {
    $database_volatile = Database_Volatile::getInstance ();
    
    // No value initially.
    $value = $database_volatile->getValue (1, "key");
    $this->assertEquals ("", $value);
    
    // Store value and retrieve it.
    $database_volatile->setValue (2, "key2", "value2");
    $value = $database_volatile->getValue (2, "key2");
    $this->assertEquals ("value2", $value);
    
    // Another key should retrieve nothing.
    $value = $database_volatile->getValue (2, "key1");
    $this->assertEquals ("", $value);
  }

   
}


?>
