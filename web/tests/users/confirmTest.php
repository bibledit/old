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

 
class databaseConfirmTest extends PHPUnit_Framework_TestCase
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
    $database_confirm = Database_Confirm::getInstance ();
    $database_confirm->optimize ();
  }


  public function testTrim ()
  {
    $database_confirm = Database_Confirm::getInstance ();
    $database_confirm->trim ();
  }
  
  
  public function testAll ()
  {
    $database_confirm = Database_Confirm::getInstance ();
    
    // New ID generation test.
    $id = $database_confirm->getNewID ();
    $this->assertGreaterThan (10000, $id);
    
    // Store data for the ID.
    $database_confirm->store ($id, "SELECT x, y, z FROM a;", "email", "subject", "body");
    
    // Search for this ID based on subject.
    $id2 = $database_confirm->searchID ("Subject line CID$id Re:");
    $this->assertEquals ($id, $id2);

    // Retrieve data for the ID.
    $query = $database_confirm->getQuery ($id);
    $this->assertEquals ("SELECT x, y, z FROM a;", $query);

    $to = $database_confirm->getMailTo ($id);
    $this->assertEquals ("email", $to);

    $subject = $database_confirm->getSubject ($id);
    $this->assertEquals ("subject", $subject);

    $body = $database_confirm->getBody ($id);
    $this->assertEquals ("body", $body);

    // Delete this ID.
    $database_confirm->delete ($id);
    $query = $database_confirm->getQuery ($id);
    $this->assertEquals ("", $query);
  }


}


?>
