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

 
class databaseStylesTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
    $database_styles = Database_Styles::getInstance ();
  }

  
  public function tearDown ()
  {
    $database_styles = Database_Styles::getInstance ();
    $database_styles->deleteSheet ("phpunit");
    $database_styles->revokeWriteAccess ("", "phpunit");
  }
  
  
  public function testOptimize ()
  {
    $database_styles = Database_Styles::getInstance ();
    $database_styles->optimize ();
  }


  public function testSheets ()
  {
    $database_styles = Database_Styles::getInstance ();

    $sheets = $database_styles->getSheets ();
    $this->assertEquals (array ("Standard"), $sheets);
    
    $database_styles->createSheet ("phpunit");
    $sheets = $database_styles->getSheets ();
    $this->assertEquals (array ("Standard", "phpunit"), $sheets);
    
    $database_styles->deleteSheet ("phpunit");
    $sheets = $database_styles->getSheets ();
    $this->assertEquals (array ("Standard"), $sheets);
    
    $database_styles->deleteSheet ("Standard");
    $sheets = $database_styles->getSheets ();
    $this->assertEquals (array ("Standard"), $sheets);
  }
  
  
  public function testMarkers ()
  {
    $database_styles = Database_Styles::getInstance ();
    $database_styles->createSheet ("phpunit");
    
    $markers = $database_styles->getMarkers ("phpunit");
    $count = count ($markers);
    $this->assertGreaterThan (150, $count);
    
    $this->assertContains ("p", $markers);
    $this->assertContains ("add", $markers);
    
    $markers_names = $database_styles->getMarkersAndNames ("phpunit");
    $count = count ($markers_names);
    $this->assertGreaterThan (150, $count);
    
    $this->assertContains (array ('marker' => "p", 'name' => "Normal, First Line Indent"), $markers_names);
    $this->assertContains (array ('marker' => "add", 'name' => "* Translational Addition"), $markers_names);
    
    $database_styles->deleteMarker ("phpunit", "p");

    $markers = $database_styles->getMarkers ("phpunit");
    $this->assertNotContains ("p", $markers);
    $this->assertContains ("add", $markers);

    $markers_names = $database_styles->getMarkersAndNames ("phpunit");
    $this->assertNotContains (array ('marker' => "p", 'name' => "Normal, First Line Indent"), $markers_names);
    $this->assertContains (array ('marker' => "add", 'name' => "* Translational Addition"), $markers_names);
  }
  
  
  public function testExport ()
  {
    $database_styles = Database_Styles::getInstance ();
    $database_styles->createSheet ("phpunit");

    $markers = $database_styles->getMarkers ("phpunit");

    Database_SQLite::exec ($database_styles->db, "BEGIN;");
    foreach ($markers as $marker) {
      if ($marker != "add") {
        $database_styles->deleteMarker ("phpunit", $marker);
      }
    }
    Database_SQLite::exec ($database_styles->db, "COMMIT;");
    
    $xml = $database_styles->exportXml ("phpunit");

$standard = <<<'EOD'
<?xml version="1.0" encoding="UTF-8"?>
<stylesheet>
  <style>
    <marker>add</marker>
    <name>* Translational Addition</name>
    <info>For a translational addition to the text</info>
    <category>st</category>
    <type>4</type>
    <subtype>0</subtype>
    <fontsize>60.0</fontsize>
    <italic>1</italic>
    <bold>0</bold>
    <underline>0</underline>
    <smallcaps>0</smallcaps>
    <superscript>0</superscript>
    <justification>0</justification>
    <spacebefore>0.0</spacebefore>
    <spaceafter>0.0</spaceafter>
    <leftmargin>0.0</leftmargin>
    <rightmargin>0.0</rightmargin>
    <firstlineindent>0.0</firstlineindent>
    <spancolumns>0</spancolumns>
    <color>000000</color>
    <print>1</print>
    <userbool1>0</userbool1>
    <userbool2>0</userbool2>
    <userbool3>0</userbool3>
    <userint1>0</userint1>
    <userint2>0</userint2>
    <userint3>0</userint3>
    <userstring1/>
    <userstring2/>
    <userstring3/>
  </style>
</stylesheet>
EOD;

    $this->assertEquals (trim ($standard), trim ($xml));
  }

  
  public function testGetMarkerData ()
  {
    $database_styles = Database_Styles::getInstance ();
    $database_styles->createSheet ("phpunit");
    $data = $database_styles->getMarkerData ("phpunit", "add");

    $count = count ($data);
    $this->assertGreaterThan (30, $count);
    
    $this->assertEquals ($data['sheet'], "phpunit");
    $this->assertEquals ($data['marker'], "add");
    $this->assertEquals ($data['category'], "st");
  }


  public function testUpdates ()
  {
    $database_styles = Database_Styles::getInstance ();
    $database_styles->createSheet ("phpunit");
    
    $database_styles->updateName ("phpunit", "add", "Addition");
    $data = $database_styles->getMarkerData ("phpunit", "add");
    $this->assertEquals ($data['name'], "Addition");

    $database_styles->updateInfo ("phpunit", "p", "Paragraph");
    $data = $database_styles->getMarkerData ("phpunit", "p");
    $this->assertEquals ($data['info'], "Paragraph");
  }
  
  
  public function testAccess ()
  {
    $database_styles = Database_Styles::getInstance ();
    $database_styles->createSheet ("phpunit");
    
    // A user does not have write access to the stylesheet.
    $write = $database_styles->hasWriteAccess ("user", "phpunit");
    $this->assertFalse ($write);
    
    // Grant write access, and test it for this user, and for another user.
    $database_styles->grantWriteAccess ("user", "phpunit");
    $write = $database_styles->hasWriteAccess ("user", "phpunit");
    $this->assertTrue ($write);
    $write = $database_styles->hasWriteAccess ("user2", "phpunit");
    $this->assertFalse ($write);
    $write = $database_styles->hasWriteAccess ("user", "phpunit2");
    $this->assertFalse ($write);
    
    // Revoke write access for a user, test it in various ways.
    $database_styles->revokeWriteAccess ("user2", "phpunit");
    $write = $database_styles->hasWriteAccess ("user", "phpunit");
    $this->assertTrue ($write);
    $database_styles->revokeWriteAccess ("user", "phpunit");
    $write = $database_styles->hasWriteAccess ("user", "phpunit");
    $this->assertFalse ($write);
    
    // Revoking write access for all users.
    $database_styles->grantWriteAccess ("user1", "phpunit");
    $database_styles->grantWriteAccess ("user2", "phpunit");
    $database_styles->revokeWriteAccess ("", "phpunit");
    $write = $database_styles->hasWriteAccess ("user1", "phpunit");
    $this->assertFalse ($write);
  }


  public function testAddStyle ()
  {
    $database_styles = Database_Styles::getInstance ();
    $database_styles->createSheet ("phpunit");
    
    // Get markers.
    $markers = $database_styles->getMarkers ("phpunit");
    $this->assertNotContains ("zhq", $markers);
    
    // Add marker.
    $database_styles->addMarker ("phpunit", "zhq");
    $markers = $database_styles->getMarkers ("phpunit");
    $this->assertContains ("zhq", $markers);
  }
  
  
}


?>
