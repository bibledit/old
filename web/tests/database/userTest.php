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

 
class databaseUserTest extends PHPUnit_Framework_TestCase
{

  public function setUp ()
  {
    $this->tearDown ();
  }
  
  public function tearDown ()
  {
    $database_config_user = Database_Config_User::getInstance ();
    $database_config_user->setList ("", "test", array ());
    $database_config_user->setList ("bible", "test", array ());

  }

  public function testOne ()
  {
    $database_config_user = Database_Config_User::getInstance ();
    $values = $database_config_user->getList ("", "test");
    $this->assertEquals (array (), $values);
    $values = $database_config_user->getList ("bible", "test");
    $this->assertEquals (array (), $values);
    $database_config_user->setList ("", "test", array (1, "two", "3"));
    $values = $database_config_user->getList ("", "test");
    $this->assertEquals (array (1, "two", 3), $values);
    $values = $database_config_user->getList ("bible", "test");
    $this->assertEquals (array (), $values);
  }


      
}
?>
