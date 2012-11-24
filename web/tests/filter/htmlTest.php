<?php


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


}
?>
