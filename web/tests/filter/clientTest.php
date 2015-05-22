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

 
class filterClientTest extends PHPUnit_Framework_TestCase
{

  public function setUp ()
  {
    $this->tearDown ();
    Filter_Client::set (true);
  }


  public function tearDown ()
  {
    Filter_Client::set (false);
  }


  public function testEnabled ()
  {
    $enabled = Filter_Client::enabled ();
    $this->assertTrue ($enabled);
    Filter_Client::set (false);
    $enabled = Filter_Client::enabled ();
    $this->assertFalse ($enabled);
  }


  public function testCreateNote ()
  {
    $data = Filter_Client::createNoteEncode ("bible", 1, 2, 3, "summary", "line1\nline2", false);
$standard = <<<EOD
bible
1
2
3
summary

line1
line2
EOD;
    $this->assertEquals ($standard, $data);

    $data = Filter_Client::createNoteDecode ($standard);
    $this->assertEquals ("bible", $data ["bible"]);
    $this->assertEquals (1, $data ["book"]);
    $this->assertEquals (2, $data ["chapter"]);
    $this->assertEquals (3, $data ["verse"]);
    $this->assertEquals ("summary", $data ["summary"]);
    $this->assertEquals ("", $data ["raw"]);
$contents = <<<EOD
line1
line2
EOD;
    $this->assertEquals ($contents, $data ["contents"]);
  }
  
  
}


?>
