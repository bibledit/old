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

 
class morphhbLogicTest extends PHPUnit_Framework_TestCase
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
    $database_morphhb = Database_Morphhb::getInstance ();
    $data = $database_morphhb->getVerse (18, 3, 2);
    $standard = array (
      array ('hebrew' => "וַיַּ֥עַן"),
      array ('hebrew' => "אִיּ֗וֹב"),
      array ('hebrew' => "וַיֹּאמַֽר")
    );
    $this->assertEquals ($standard, $data);
  }


  public function testSearchHebrew ()
  {
    $database_morphhb = Database_Morphhb::getInstance ();
    $data = $database_morphhb->searchHebrew ("יָדְע֥וּ");
    $standard = array (
      array ('book' => "19", 'chapter' => "95", 'verse' => "10"),
      array ('book' => "30", 'chapter' => "3", 'verse' => "10")
    );
    $this->assertEquals ($standard, $data);
  }


}


?>
