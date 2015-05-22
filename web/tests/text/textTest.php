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

 
class textTextTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
  }


  public function testOne ()
  {
    $text_text = new Text_Text ();
    $text_text->text ("text one");
    $this->assertEquals ("text one", $text_text->get ());
  }


  public function testTwo ()
  {
    $text_text = new Text_Text ();
    $text_text->paragraph ("paragraph1");
    $text_text->paragraph ("paragraph2");
    $this->assertEquals ("paragraph1\nparagraph2", $text_text->get ());
  }


  public function testThree ()
  {
    $text_text = new Text_Text ();
    $text_text->paragraph ("paragraph");
    $this->assertEquals ("paragraph", $text_text->line ());
  }


}


?>
