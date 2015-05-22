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

 
class sblgntLogicTest extends PHPUnit_Framework_TestCase
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
    $database_sblgnt = Database_Sblgnt::getInstance ();
    $data = $database_sblgnt->getVerse (43, 11, 35);
    $standard = array (
      array ('greek' => "ἐδάκρυσεν"),
      array ('greek' => "ὁ"),
      array ('greek' => "Ἰησοῦς")
    );
    $this->assertEquals ($standard, $data);
  }


  public function testSearchGreek ()
  {
    $database_sblgnt = Database_Sblgnt::getInstance ();
    $data = $database_sblgnt->searchGreek ("βαπτισμῶν");
    $standard = array (
      array ('book' => "58", 'chapter' => "6", 'verse' => "2")
    );
    $this->assertEquals ($standard, $data);
  }


}


?>
