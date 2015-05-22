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

 
class shellTest extends PHPUnit_Framework_TestCase
{


  private $processes = array();


  public function setUp ()
  {
  }

  
  public function tearDown ()
  {
    $database_shell = Database_Shell::getInstance();
    foreach ($this->processes as $process) {
      $database_shell->stopProcess ($process['name'], $process['pid']);
    }
  }
  
  
  public function testMaintenance ()
  {
    $database_shell = Database_Shell::getInstance();
    $database_shell->trim ();
    $database_shell->optimize ();
  }


  public function testShell()
  {
    $database_shell = Database_Shell::getInstance();
    $process['name'] = "PHPUnit";
    $process['pid'] = "10";
    $processes [] = $process;
    $output = "/path/to/file";
    $database_shell->removeProcess ($process['name'], $process['pid']);
    $this->assertFalse ($database_shell->processRuns ($process['name'], $process['pid']));
    $this->assertFalse ($database_shell->processDone ($process['name'], $process['pid']));
    $database_shell->enterProcess ($process['name'], $process['pid'], $output);
    $this->assertEquals ($output, $database_shell->getOutput ($process['name'], $process['pid']));
    $this->assertTrue ($database_shell->processRuns ($process['name'], $process['pid']));
    $this->assertFalse ($database_shell->processDone ($process['name'], $process['pid']));
    $database_shell->stopProcess ($process['name'], $process['pid']);
    $this->assertFalse ($database_shell->processRuns ($process['name'], $process['pid']));
    $this->assertTrue ($database_shell->processDone ($process['name'], $process['pid']));
    $database_shell->removeProcess ($process['name'], $process['pid']);
    $this->assertFalse ($database_shell->processRuns ($process['name'], $process['pid']));
    $this->assertFalse ($database_shell->processDone ($process['name'], $process['pid']));
  }

      
}
?>
