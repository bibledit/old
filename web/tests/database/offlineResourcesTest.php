<?php

 
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
