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

 
class databaseCommitsTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
  }


  public function testOptimize ()
  {
    $database_commits = Database_Commits::getInstance ();
    $database_commits->optimize ();
  }
  
  
  public function testAll ()
  {
    $database_commits = Database_Commits::getInstance ();

    $bible = "phpunit";
    $sha = "sha";

    $data = $database_commits->get ($bible);
    $this->assertEmpty ($data);
    
    // Record some data.
    $database_commits->record ($bible, $sha);
    $database_commits->record ($bible, $sha);

    // Check the data.
    $data = $database_commits->get ($bible);
    $this->assertEquals (2, count ($data));
    $this->assertEquals ("sha", $data [1]);

    // No data for another Bible
    $data = $database_commits->get ("none");
    $this->assertEquals (0, count ($data));
  }


}


?>
