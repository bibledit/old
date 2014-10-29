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
    $value = $database_config_bible->getValue ("bible", "bible", "default");
    $this->assertEquals ("default", $value);
    $database_config_bible->setValue ("bible1", "bible2", "Val");
    $value = $database_config_bible->getValue ("bible1", "bible2", "default");
    $this->assertEquals ("Val", $value);
  }


  public function testVersificationMapping ()
  {
    $database_config_bible = Database_Config_Bible::getInstance ();
    $versification = $database_config_bible->getVersificationSystem ("phpunit");
    $this->assertEquals ("English", $versification);
    $mapping = $database_config_bible->getVerseMapping ("phpunit");
    $this->assertEquals ("English", $mapping);
    $versification = $database_config_bible->getVersificationSystem ("x");
    $this->assertEquals ("English", $versification);
    $mapping = $database_config_bible->getVerseMapping ("x");
    $this->assertEquals ("English", $mapping);
    $database_config_bible->setVersificationSystem ("phpunit", "VersificatioN");
    $versification = $database_config_bible->getVersificationSystem ("phpunit");
    $this->assertEquals ("VersificatioN", $versification);
    $database_config_bible->setVerseMapping ("phpunit", "VersificatioN");
    $mapping = $database_config_bible->getVerseMapping ("phpunit");
    $this->assertEquals ("VersificatioN", $mapping);
  }


}


?>
