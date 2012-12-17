<?php

 
class pathsTest extends PHPUnit_Framework_TestCase // Todo
{

  public function testHtmlFileNameBible ()
  {
    $this->assertEquals ("path/index.html", Filter_Paths::htmlFileNameBible ("path"));
  }

  
  
}
?>
