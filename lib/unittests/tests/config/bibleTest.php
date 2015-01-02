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

 
class databaseConfigBibleTest extends PHPUnit_Framework_TestCase
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
    $database_config_bible = Database_Config_Bible::getInstance ();
    $value = Database_Config_Bible::getValue ("bible", "bible", "default");
    $this->assertEquals ("default", $value);
    Database_Config_Bible::setValue ("bible1", "bible2", "Val");
    $value = Database_Config_Bible::getValue ("bible1", "bible2", "default");
    $this->assertEquals ("Val", $value);
  }


  public function testVersificationMapping ()
  {
    $database_config_bible = Database_Config_Bible::getInstance ();
    $versification = Database_Config_Bible::getVersificationSystem ("phpunit");
    $this->assertEquals ("English", $versification);
    $mapping = Database_Config_Bible::getVerseMapping ("phpunit");
    $this->assertEquals ("English", $mapping);
    $versification = Database_Config_Bible::getVersificationSystem ("x");
    $this->assertEquals ("English", $versification);
    $mapping = Database_Config_Bible::getVerseMapping ("x");
    $this->assertEquals ("English", $mapping);
    Database_Config_Bible::setVersificationSystem ("phpunit", "VersificatioN");
    $versification = Database_Config_Bible::getVersificationSystem ("phpunit");
    $this->assertEquals ("VersificatioN", $versification);
    Database_Config_Bible::setVerseMapping ("phpunit", "VersificatioN");
    $mapping = Database_Config_Bible::getVerseMapping ("phpunit");
    $this->assertEquals ("VersificatioN", $mapping);
  }


}


?>
