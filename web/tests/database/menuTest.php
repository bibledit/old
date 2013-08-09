<?php

 
class databaseMenuTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    $database_menu = Database_Menu::getInstance ();
    $database_menu->clearAllAccessCounts ("phpunit1");
    $database_menu->clearAllAccessCounts ("phpunit2");
  }


  public function testZeroCount ()
  {
    $database_menu = Database_Menu::getInstance ();
    $count = $database_menu->getAccessCount ("phpunit1", "test");
    $this->assertEquals (0, $count);
  }


  public function testIncreaseCount ()
  {
    $database_menu = Database_Menu::getInstance ();
    $database_menu->increaseAccessCount ("phpunit1", "test");
    $count = $database_menu->getAccessCount ("phpunit1", "test");
    $this->assertEquals (1, $count);
    $count = $database_menu->getAccessCount ("phpunit2", "test");
    $this->assertEquals (0, $count);
    $database_menu->increaseAccessCount ("phpunit1", "test");
    $database_menu->increaseAccessCount ("phpunit1", "test");
    $count = $database_menu->getAccessCount ("phpunit1", "test");
    $this->assertEquals (3, $count);
  }
  
      
}
?>
