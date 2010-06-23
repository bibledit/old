<?php
require_once 'PHPUnit/Framework.php';
 
class shellTest extends PHPUnit_Framework_TestCase
{
  private $processes = array();

  public function setUp ()
  {
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


  public function tearDown ()
  {
    $database_shell = Database_Shell::getInstance();
    foreach ($this->processes as $process) {
      $database_shell->stopProcess ($process['name'], $process['pid']);
    }
    
  }
  

      
}
?>


