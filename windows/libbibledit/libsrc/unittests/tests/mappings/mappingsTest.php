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

 
class databaseMappingsTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $database_mappings = Database_Mappings::getInstance ();
    $database_mappings->create1 ();
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    $database_mappings = Database_Mappings::getInstance ();
    $names = $database_mappings->names ();
    foreach ($names as $name) {
      $database_mappings->delete ($name);
    }
  }


  public function testSetup ()
  {
    $database_mappings = Database_Mappings::getInstance ();
    $database_mappings->defaults ();
    $database_mappings->create2 ();
    $database_mappings->optimize ();
    $names = $database_mappings->names ();
    $this->assertContains ("Hebrew Greek", $names);
    $this->assertContains ("English", $names);
    $this->assertContains ("Dutch Traditional", $names);
  }
  
  
  public function testImportExport ()
  {
    $database_mappings = Database_Mappings::getInstance ();
    $database_mappings->create1 ();
$import = <<< EOD
2 Chronicles 14:15 = 2 Chronicles 14:14
Nehemiah 4:1 = Nehemiah 3:33
Song of Solomon 7:2 = Song of Solomon 7:3
EOD;
    $database_mappings->import ("phpunit", $import);
    $names = $database_mappings->names ();
    $this->assertEquals (array ("Hebrew Greek", "phpunit"), $names);
    $export = $database_mappings->export ("phpunit");
    $this->assertEquals ($import, $export);
  }


  public function testCreate ()
  {
    $database_mappings = Database_Mappings::getInstance ();
    $database_mappings->create ("phpunit");
    $names = $database_mappings->names ();
    $this->assertContains ("phpunit", $names);
  }
    
  
  public function testTranslateSame ()
  {
    $database_mappings = Database_Mappings::getInstance ();
    $passage = $database_mappings->translate ("ABC", "ABC", 14, 14, 15);
    $this->assertEquals (array (array (14, 14, 15)), $passage);
    $passage = $database_mappings->translate ("--X", "--X", 15, 16, 17);
    $this->assertEquals (array (array (15, 16, 17)), $passage);
  }
  
  
  public function testTranslateOne ()
  {
    $database_mappings = Database_Mappings::getInstance ();
    $database_mappings->create1 ();
$import = <<< EOD
2 Chronicles 14:15 = 2 Chronicles 14:14
Nehemiah 4:1 = Nehemiah 3:33
Song of Solomon 7:2 = Song of Solomon 7:3
EOD;
    $database_mappings->import ("ABC", $import);
$import = <<< EOD
2 Chronicles 14:15 = 2 Chronicles 14:14
Nehemiah 4:1 = Nehemiah 3:33
Song of Solomon 7:2 = Song of Solomon 7:3
EOD;
    $database_mappings->import ("XYZ", $import);
    // Test mapping 2 Chronicles.
    $passage = $database_mappings->translate ("ABC", "XYZ", 14, 14, 15);
    $this->assertEquals (array (array (14, 14, 15)), $passage);
  }


  public function testTranslateTwo ()
  {
    $database_mappings = Database_Mappings::getInstance ();
    $database_mappings->create1 ();
$import = <<< EOD
2 Chronicles 14:15 = 2 Chronicles 14:14
Nehemiah 4:1 = Nehemiah 3:33
Song of Solomon 7:2 = Song of Solomon 7:3
EOD;
    $database_mappings->import ("ABC", $import);
$import = <<< EOD
2 Chronicles 14:13 = 2 Chronicles 14:14
Nehemiah 4:1 = Nehemiah 3:33
Song of Solomon 7:2 = Song of Solomon 7:3
EOD;
    $database_mappings->import ("XYZ", $import);
    // Test mapping 2 Chronicles.
    $passage = $database_mappings->translate ("ABC", "XYZ", 14, 14, 15);
    $this->assertEquals (array (array (14, 14, 13)), $passage);
  }


  public function testTranslateDoubleResult ()
  {
    $database_mappings = Database_Mappings::getInstance ();
    $database_mappings->create1 ();
$import = <<< EOD
2 Chronicles 14:15 = 2 Chronicles 14:14
Nehemiah 4:1 = Nehemiah 3:33
Song of Solomon 7:2 = Song of Solomon 7:3
EOD;
    $database_mappings->import ("ABC", $import);
$import = <<< EOD
2 Chronicles 14:12 = 2 Chronicles 14:14
2 Chronicles 14:13 = 2 Chronicles 14:14
Nehemiah 4:1 = Nehemiah 3:33
Song of Solomon 7:2 = Song of Solomon 7:3
EOD;
    $database_mappings->import ("XYZ", $import);
    // Test mapping 2 Chronicles.
    $passage = $database_mappings->translate ("ABC", "XYZ", 14, 14, 15);
    $this->assertEquals (array (array (14, 14, 12), array (14, 14, 13)), $passage);
  }


  public function testTranslateFromOriginal ()
  {
    $database_mappings = Database_Mappings::getInstance ();
    $database_mappings->create1 ();
$import = <<< EOD
2 Chronicles 14:12 = 2 Chronicles 14:14
EOD;
    $database_mappings->import ("VVV", $import);
    $passage = $database_mappings->translate ("Hebrew Greek", "VVV", 14, 14, 14);
    $this->assertEquals (array (array (14, 14, 12)), $passage);
  }


  public function testTranslateFromOriginalDouble ()
  {
    $database_mappings = Database_Mappings::getInstance ();
    $database_mappings->create1 ();
$import = <<< EOD
2 Chronicles 14:12 = 2 Chronicles 14:14
2 Chronicles 14:13 = 2 Chronicles 14:14
EOD;
    $database_mappings->import ("VVV", $import);
    $passage = $database_mappings->translate ("Hebrew Greek", "VVV", 14, 14, 14);
    $this->assertEquals (array (array (14, 14, 12), array (14, 14, 13)), $passage);
  }


  public function testTranslateFromOriginalNoMapping ()
  {
    $database_mappings = Database_Mappings::getInstance ();
    $database_mappings->create1 ();
$import = <<< EOD
2 Chronicles 14:12 = 2 Chronicles 14:14
EOD;
    $database_mappings->import ("VVV", $import);
    $passage = $database_mappings->translate ("Hebrew Greek", "VVV", 14, 15, 14);
    $this->assertEquals (array (array (14, 15, 14)), $passage);
  }


  public function testTranslateToOriginal ()
  {
    $database_mappings = Database_Mappings::getInstance ();
    $database_mappings->create1 ();
$import = <<< EOD
2 Chronicles 14:12 = 2 Chronicles 14:14
EOD;
    $database_mappings->import ("ABA", $import);
    $passage = $database_mappings->translate ("ABA", "Hebrew Greek", 14, 14, 12);
    $this->assertEquals (array (array (14, 14, 14)), $passage);
  }


  public function testTranslateToOriginalDouble ()
  {
    $database_mappings = Database_Mappings::getInstance ();
    $database_mappings->create1 ();
$import = <<< EOD
2 Chronicles 14:12 = 2 Chronicles 14:13
2 Chronicles 14:12 = 2 Chronicles 14:14
EOD;
    $database_mappings->import ("ABA", $import);
    $passage = $database_mappings->translate ("ABA", "Hebrew Greek", 14, 14, 12);
    $this->assertEquals (array (array (14, 14, 13), array (14, 14, 14)), $passage);
  }


}


?>
