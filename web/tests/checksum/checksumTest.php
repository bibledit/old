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


class checksumTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
    $database_search = Database_Search::getInstance ();
    $database_search->create ();
    $database_bibles = Database_Bibles::getInstance ();
    $database_bibles->storeChapter ("phpunit1", 1, 2, "data1");
    $database_bibles->storeChapter ("phpunit1", 1, 3, "data2");
    $database_bibles->storeChapter ("phpunit1", 1, 4, "data3");
    $database_bibles->storeChapter ("phpunit2", 2, 5, "data4");
  }

  
  public function tearDown ()
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_bibles->deleteBible ("phpunit1");
    $database_bibles->deleteBible ("phpunit2");
  }


  public function testSend1 ()
  {
    $data = '\v Verse 1';
    $json = Checksum_Logic::send ($data);
    $this->assertEquals ('{"data":"\\\\v Verse 1","checksum":3}', $json);
  }


  public function testSend2 ()
  {
    $data = 'Line one\nLine 2\n';
    $json = Checksum_Logic::send ($data);
    $this->assertEquals ('{"data":"Line one\\\\nLine 2\\\\n","checksum":3}', $json);
  }


  public function testGet1 ()
  {
    $data = '\v Verse 1';
    $checksum = Checksum_Logic::get ($data);
    $this->assertEquals (3, $checksum);
  }


  public function testGet2 ()
  {
    $data = 'Line one\nLine 2\n';
    $checksum = Checksum_Logic::get ($data);
    $this->assertEquals (3, $checksum);
  }
  
  
  public function testGetChapter1 ()
  {
    $checksum = Checksum_Logic::getChapter ("phpunit1", 1, 2);
    $this->assertEquals (md5 ("data1"), $checksum);
  }
  

  public function testGetChapter2 ()
  {
    $checksum = Checksum_Logic::getChapter ("phpunit2", 2, 6);
    $this->assertEquals (md5 (""), $checksum);
  }


  public function testGetBook1 ()
  {
    $checksum = Checksum_Logic::getBook ("phpunit1", 1);
    $this->assertEquals ("2ab6425924e6cd38b2474c543c5ea602", $checksum);
  }
  

  public function testGetBook2 ()
  {
    $checksum = Checksum_Logic::getBook ("phpunit3", 1);
    $this->assertEquals ("d41d8cd98f00b204e9800998ecf8427e", $checksum);
  }
  

  public function testGetBible1 ()
  {
    $checksum = Checksum_Logic::getBible ("phpunit1");
    $this->assertEquals ("f9dc679a8712eb6f65b584e9688e9680", $checksum);
  }

  
  public function testGetBible2 ()
  {
    $checksum = Checksum_Logic::getBible ("phpunit2");
    $this->assertEquals ("ee84a85bac14adb35e887c3d89bc80ab", $checksum);
  }
  

  public function testGetBibles1 ()
  {
    $checksum = Checksum_Logic::getBibles (array ("phpunit1", "phpunit2"));
    $this->assertEquals ("440b2008993816f0bc8c557b64fbdaf2", $checksum);
  }

  
  public function testGetBibles2 ()
  {
    $checksum = Checksum_Logic::getBibles (array ("phpunit3", "phpunit4"));
    $this->assertEquals ("020eb29b524d7ba672d9d48bc72db455", $checksum);
  }


}


?>
