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

 
class databaseJobsTest extends PHPUnit_Framework_TestCase
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
    $database_jobs = Database_Jobs::getInstance ();
    $database_jobs->optimize ();
  }


  public function testIdentifiers ()
  {
    $database_jobs = Database_Jobs::getInstance ();
    $id = $database_jobs->getNewId ();
    $exists = $database_jobs->idExists ($id);
    $this->assertTrue ($exists);
  }


  public function testLevel ()
  {
    $database_jobs = Database_Jobs::getInstance ();
    $id = $database_jobs->getNewId ();
    $level = $database_jobs->getLevel ($id);
    $this->assertNull ($level);
    $database_jobs->setLevel ($id, 123);
    $level = $database_jobs->getLevel ($id);
    $this->assertEquals (123, $level);
  }


  public function testProgress ()
  {
    $database_jobs = Database_Jobs::getInstance ();
    $id = $database_jobs->getNewId ();
    $progress = $database_jobs->getProgress ($id);
    $this->assertNull ($progress);
    $database_jobs->setProgress ($id, "progress");
    $progress = $database_jobs->getProgress ($id);
    $this->assertEquals ("progress", $progress);
  }


  public function testResult ()
  {
    $database_jobs = Database_Jobs::getInstance ();
    $id = $database_jobs->getNewId ();
    $result = $database_jobs->getResult ($id);
    $this->assertNull ($result);
    $database_jobs->setResult ($id, "result");
    $result = $database_jobs->getResult ($id);
    $this->assertEquals ("result", $result);
  }


}


?>
