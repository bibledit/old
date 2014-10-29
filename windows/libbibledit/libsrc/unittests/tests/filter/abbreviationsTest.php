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

 
class filterAbbreviationsTest extends PHPUnit_Framework_TestCase
{


  public function testRead ()
  {
$input = <<<EOD
Psalms = Ps.

Exodus = Exod.

Exodu = Exod.

Psalms = Psa.

Joshua = 

EOD;
    $output = Filter_Abbreviations::read ($input);
    $standard = array ('Ps.' => 19, 'Exod.' => 2, 'Psa.' => 19);
    $this->assertEquals ($standard, $output);
  }
  
  
  public function testDisplay ()
  {
$input = <<<EOD
Psalms = Ps.

Exodus = Exod.

EOD;
    $data = Filter_Abbreviations::display ($input);
$output = <<<EOD
Genesis = 
Exodus = Exod.
Leviticus = 
Numbers = 
Deuteronomy = 
Joshua = 
Judges = 
Ruth = 
1 Samuel = 
2 Samuel = 
1 Kings = 
2 Kings = 
1 Chronicles = 
2 Chronicles = 
Ezra = 
Nehemiah = 
Esther = 
Job = 
Psalms = Ps.
Proverbs = 
Ecclesiastes = 
Song of Solomon = 
Isaiah = 
Jeremiah = 
Lamentations = 
Ezekiel = 
Daniel = 
Hosea = 
Joel = 
Amos = 
Obadiah = 
Jonah = 
Micah = 
Nahum = 
Habakkuk = 
Zephaniah = 
Haggai = 
Zechariah = 
Malachi = 
Matthew = 
Mark = 
Luke = 
John = 
Acts = 
Romans = 
1 Corinthians = 
2 Corinthians = 
Galatians = 
Ephesians = 
Philippians = 
Colossians = 
1 Thessalonians = 
2 Thessalonians = 
1 Timothy = 
2 Timothy = 
Titus = 
Philemon = 
Hebrews = 
James = 
1 Peter = 
2 Peter = 
1 John = 
2 John = 
3 John = 
Jude = 
Revelation = 
Front Matter = 
Back Matter = 
Other Material = 
EOD;
    $this->assertEquals ($output, $data);
  }
  
  
}


?>
