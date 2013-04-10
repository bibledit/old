<?php

 
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
