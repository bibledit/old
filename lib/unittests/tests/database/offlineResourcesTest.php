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

 
class databaseOfflineResourcesTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    $database_offlineresources = Database_OfflineResources::getInstance ();
    $database_offlineresources->delete ("phpunit");
    $database_offlineresources->delete ("phpunit2");
  }


  public function testStoreExistsGet ()
  {
    $database_offlineresources = Database_OfflineResources::getInstance ();
    $database_offlineresources->store ("phpunit", 1, 2, 3, "xyz");
    $exists = $database_offlineresources->exists ("phpunit", 1, 2, 3);
    $this->assertEquals (true, $exists);
    $exists = $database_offlineresources->exists ("phpunit", 1, 2, 4);
    $this->assertEquals (false, $exists);
    $html = $database_offlineresources->get ("phpunit", 1, 2, 3);
    $this->assertEquals ("xyz", $html);
    $html = $database_offlineresources->get ("phpunit", 1, 2, 4);
    $this->assertEquals ("", $html);
  }


  public function testCountDelete ()
  {
    $database_offlineresources = Database_OfflineResources::getInstance ();
    $count = $database_offlineresources->count ("phpunit");
    $this->assertEquals (0, $count);
    $database_offlineresources->store ("phpunit", 1, 2, 3, "xyz");
    $count = $database_offlineresources->count ("phpunit");
    $this->assertEquals (1, $count);
    $database_offlineresources->store ("phpunit", 4, 5, 6, "xyz");
    $count = $database_offlineresources->count ("phpunit");
    $this->assertEquals (2, $count);
    $database_offlineresources->delete ("phpunit");
    $count = $database_offlineresources->count ("phpunit");
    $this->assertEquals (0, $count);
  }
  
  
  public function testNames ()
  {
    $database_offlineresources = Database_OfflineResources::getInstance ();

    $names = $database_offlineresources->names ();
    $this->assertEmpty ($names);

    $database_offlineresources->store ("phpunit", 1, 2, 3, "xyz");
    $names = $database_offlineresources->names ();
    $this->assertEquals (array ("phpunit"), $names);
    
    $database_offlineresources->store ("phpunit2", 1, 2, 3, "xyz");
    $names = $database_offlineresources->names ();
    $this->assertEquals (array ("phpunit", "phpunit2"), $names);
  }
  
  
  public function testFiles ()
  {
    $database_offlineresources = Database_OfflineResources::getInstance ();
    
    $files = $database_offlineresources->files ("phpunit");
    $this->assertEmpty ($files);
    
    $database_offlineresources->store ("phpunit", 1, 2, 3, "xyz");
    $files = $database_offlineresources->files ("phpunit");
    $this->assertEquals (array ("1.sqlite"), $files);

    $database_offlineresources->store ("phpunit", 1, 4, 5, "xyz");
    $files = $database_offlineresources->files ("phpunit");
    $this->assertEquals (array ("1.sqlite"), $files);

    $database_offlineresources->store ("phpunit", 11, 22, 33, "xyz");
    $files = $database_offlineresources->files ("phpunit");
    $this->assertEquals (array ("1.sqlite", "11.sqlite"), $files);
  }


  public function testSize ()
  {
    $database_offlineresources = Database_OfflineResources::getInstance ();

    clearstatcache ();

    $size = $database_offlineresources->size ("phpunit", "1.sqlite");
    $this->assertEquals (0, $size);
    
    clearstatcache ();

    $database_offlineresources->store ("phpunit", 1, 2, 3, str_repeat ("s", 100));
    $size = $database_offlineresources->size ("phpunit", "1.sqlite");
    $this->assertEquals (2048, $size);

    clearstatcache ();

    $database_offlineresources->store ("phpunit", 1, 3, 4, str_repeat ("s", 1000));
    $size = $database_offlineresources->size ("phpunit", "1.sqlite");
    $this->assertEquals (3072, $size);
  }


  public function testSaveLoad ()
  {
    $database_offlineresources = Database_OfflineResources::getInstance ();
    
    clearstatcache ();

    $size = $database_offlineresources->size ("phpunit", "1.sqlite");
    $this->assertEquals (0, $size);
    
    $standard = "abc";
    $database_offlineresources->save ("phpunit", "1.sqlite", $standard);
    $contents = $database_offlineresources->load ("phpunit", "1.sqlite");
    $this->assertEquals ($standard, $contents);
  }
  
  
  public function testUnlink ()
  {
    $database_offlineresources = Database_OfflineResources::getInstance ();

    $database_offlineresources->store ("phpunit", 1, 2, 3, "xyz");
    $files = $database_offlineresources->files ("phpunit");
    $this->assertEquals (array ("1.sqlite"), $files);
    
    $database_offlineresources->unlink ("phpunit", "2");
    
    clearstatcache ();

    $files = $database_offlineresources->files ("phpunit");
    $this->assertEquals (array ("1.sqlite"), $files);

    $database_offlineresources->unlink ("phpunit", "1.sqlite");
    
    clearstatcache ();

    $files = $database_offlineresources->files ("phpunit");
    $this->assertEmpty ($files);
  }
  
  
}


?>
