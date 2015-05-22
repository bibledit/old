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

 
class kjvLogicTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
  }


  public function testGetVerse ()
  {
    $database_kjv = Database_Kjv::getInstance ();
    $data = $database_kjv->getVerse (43, 11, 35);
    $standard = array (
      array ('strong' => "G3588", 'english' => "Jesus"),
      array ('strong' => "G2424", 'english' => "Jesus"),
      array ('strong' => "G1145", 'english' => "wept")
    );
    $this->assertEquals ($standard, $data);
  }
  
  
  public function testSearchStrong ()
  {
    $database_kjv = Database_Kjv::getInstance ();
    $data = $database_kjv->searchStrong ("G909");
    $standard = array (
      array ('book' => "41", 'chapter' => "7", 'verse' => "4"),
      array ('book' => "41", 'chapter' => "7", 'verse' => "8"),
      array ('book' => "58", 'chapter' => "6", 'verse' => "2"),
      array ('book' => "58", 'chapter' => "9", 'verse' => "10"),
    );
    $this->assertEquals ($standard, $data);
  }


}


?>
