<?php
 
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
