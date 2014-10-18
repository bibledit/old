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

 
class databaseConfigUserTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("");
  }


  public function testValue ()
  {
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("phpunit1");
    $database_config_user = Database_Config_User::getInstance ();
    $value = $database_config_user->getValue ("test", "default");
    $this->assertEquals ("default", $value);
    $database_config_user->setValue ("test", "Val");
    $value = $database_config_user->getValue ("test", "default");
    $this->assertEquals ("Val", $value);
  }


  public function testValueForUser ()
  {
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("phpunit2");
    $database_config_user = Database_Config_User::getInstance ();
    $value = $database_config_user->getValueForUser ("phpunit2", "test", "default");
    $this->assertEquals ("default", $value);
    $database_config_user->setValue ("key", "Val");
    $value = $database_config_user->getValueForUser ("phpunit2", "key", "default");
    $this->assertEquals ("Val", $value);
  }
  

  public function testList ()
  {
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("phpunit3");
    $database_config_user = Database_Config_User::getInstance ();
    $values = $database_config_user->getList ("test2");
    $this->assertEquals (array (), $values);
    $database_config_user->setList ("test", array (1, 2, 3, "ah"));
    $values = $database_config_user->getList ("test");
    $this->assertEquals (array (1, 2, 3, "ah"), $values);
  }
  
  
  public function testTrim ()
  {
    $database_config_user = Database_Config_User::getInstance ();
    $database_config_user->trim ();
  }


}


?>
