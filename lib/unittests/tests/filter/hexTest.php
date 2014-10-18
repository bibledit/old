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

 
class filterHexTest extends PHPUnit_Framework_TestCase
{


  public function testHex2binOne ()
  {
    $data = "abc";
    $hex = bin2hex ($data);
    $this->assertEquals ($data, hex2bin ($hex));
    $this->assertEquals ($data, Filter_Hex::hex2bin ($hex));
  }

  
  public function testHex2binTwo ()
  {
    $data = "";
    $hex = bin2hex ($data);
    $this->assertEquals ($data, hex2bin ($hex));
    $this->assertEquals ($data, Filter_Hex::hex2bin ($hex));
  }

  
  public function testHex2binThree ()
  {
    $data = "וּמִפְּרִ֣י הָעֵץ֮ אֲשֶׁ֣ר בְּתוֹךְ־הַגָּן֒ אָמַ֣ר אֱלֹהִ֗ים לֹ֤א תֹֽאכְלוּ֙ מִמֶּ֔נּוּ וְלֹ֥א תִגְּע֖וּ בּ֑וֹ פֶּן־תְּמֻתֽוּן׃ ";
    $hex = bin2hex ($data);
    $this->assertEquals ($data, hex2bin ($hex));
    $this->assertEquals ($data, Filter_Hex::hex2bin ($hex));
  }

  
}


?>
