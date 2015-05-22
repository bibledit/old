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

 
class databaseConfigGeneralTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
  }


  public function testValue ()
  {
    $database_config_general = Database_Config_General::getInstance ();
    $value = $database_config_general->getValue ("test", "default");
    $this->assertEquals ("default", $value);
    $database_config_general->setValue ("test", "Val");
    $value = $database_config_general->getValue ("test", "default");
    $this->assertEquals ("Val", $value);
  }


  public function testList ()
  {
    $database_config_general = Database_Config_General::getInstance ();
    $values = $database_config_general->getList ("test2");
    $this->assertEquals (array (), $values);
    $database_config_general->setList ("test", array (1, 2, 3));
    $values = $database_config_general->getList ("test");
    $this->assertEquals (array (1, 2, 3), $values);
  }

      
}
?>
