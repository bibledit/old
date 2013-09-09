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
  }

  public function testIt ()
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

}


?>
