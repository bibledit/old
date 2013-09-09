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


class databaseLoggerTest extends PHPUnit_Framework_TestCase
{
  private $pid = 12345678;
  private $file = "phpunit";

  public function setUp ()
  {

  }
  
  public function tearDown ()
  {
    $database_logger = Database_Logger::getInstance();
    $database_logger->erase ($this->pid);
  }

  public function testOne()
  {
    $database_logger = Database_Logger::getInstance();
    $records = $database_logger->getAll ();
    $this->assertNotContains ($this->pid, $records);
  }
  
  public function testTwo ()
  {
    $database_logger = Database_Logger::getInstance();
    $database_logger->record ($this->pid, $this->file);
    $records = $database_logger->getAll ();
    $this->assertContains ($this->file, $records);
  }
  
  public function testThree ()
  {
    $database_logger = Database_Logger::getInstance();
    $pid = $database_logger->getPID ($this->file);
    $this->assertEquals (0, $pid);
  }

  public function testFour ()
  {
    $database_logger = Database_Logger::getInstance();
    $database_logger->record ($this->pid, $this->file);
    $pid = $database_logger->getPID ($this->file);
    $this->assertEquals ($this->pid, $pid);
  }
   
}

?>
