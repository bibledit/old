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


class htmlTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
  }


  public function testHtml2Text1()
  {
$html = <<<EOD
<p>author</p>
<p>Text 1<div>Text 2</div><div>Text 3</div></p>
EOD;
$plain = <<<EOD
author
Text 1
Text 2
Text 3
EOD;
    $this->assertEquals ($plain, Filter_Html::html2text ($html));
  }


  public function testHtml2Text2()
  {
$html = <<<EOD
<p>writer (15 Nov):</p>
<p>Second note.<div>Second matter.</div><div>A second round is needed.</div></p>
<p>Here is <b>bold</b>, and here is <i>italics</i>.<div>Here is <sup>superscript</sup>&nbsp;and here is <sub>subscript</sub>.</div><div style="text-align: center;">Centered</div><div style="text-align: left;">Left justified</div><div style="text-align: left;"><ol><li>Numbered list.</li></ol>No numbered text.</div><div style="text-align: left;">Link to <a href="http://google.nl">http://google.nl</a>.</div><div style="text-align: left;">Here follows an image:&nbsp;<img src="http://localhost/image">.<br></div><h1>Header 1</h1><div>Normal text again below the header.</div></p>
EOD;
$plain = <<<EOD
writer (15 Nov):
Second note.
Second matter.
A second round is needed.
Here is bold, and here is italics.
Here is superscript and here is subscript.
Centered
Left justified
Numbered list.
No numbered text.
Link to http://google.nl.
Here follows an image: .
Header 1
Normal text again below the header.
EOD;
    $this->assertEquals ($plain, Filter_Html::html2text ($html));
  }


  public function testHtml2Text3()
  {
$html = <<<EOD
test notes four

Logbook:

EOD;
$plain = <<<EOD
test notes fourLogbook:
EOD;
    $this->assertEquals ($plain, Filter_Html::html2text ($html));
  }


  public function testHtml2Text4 ()
  {
$html = <<<EOD
Line one.<BR>

Line two.<BR>

Line three.<BR>
EOD;
$plain = <<<EOD
Line one.
Line two.
Line three.
EOD;
    $this->assertEquals ($plain, Filter_Html::html2text ($html));
  }


}
?>
