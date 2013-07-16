<?php

 
class databaseUserTest extends PHPUnit_Framework_TestCase
{

  public function setUp ()
  {
    $this->tearDown ();
  }
  
  public function tearDown ()
  {
    $database_config_user = Database_Config_User::getInstance ();
    $database_config_user->setList ("", "test", array ());
    $database_config_user->setList ("bible", "test", array ());

  }

  public function testOne ()
  {
    $database_config_user = Database_Config_User::getInstance ();
    $values = $database_config_user->getList ("", "test");
    $this->assertEquals (array (), $values);
    $values = $database_config_user->getList ("bible", "test");
    $this->assertEquals (array (), $values);
    $database_config_user->setList ("", "test", array (1, "two", "3"));
    $values = $database_config_user->getList ("", "test");
    $this->assertEquals (array (1, "two", 3), $values);
    $values = $database_config_user->getList ("bible", "test");
    $this->assertEquals (array (), $values);
  }


      
}
?>
