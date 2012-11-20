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


}
?>
