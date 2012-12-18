<?php

 
class pathsTest extends PHPUnit_Framework_TestCase
{


  public function testHtmlFileNameBible ()
  {
    $this->assertEquals ("index.html", Filter_Paths::htmlFileNameBible ());
    $this->assertEquals ("path/index.html", Filter_Paths::htmlFileNameBible ("path"));
    $this->assertEquals ("path/01-Genesis.html", Filter_Paths::htmlFileNameBible ("path", 1));
    $this->assertEquals ("01-Genesis.html", Filter_Paths::htmlFileNameBible ("", 1));
    $this->assertEquals ("path/11-1Kings.html", Filter_Paths::htmlFileNameBible ("path", 11));
    $this->assertEquals ("path/22-SongofSolomon-000.html", Filter_Paths::htmlFileNameBible ("path", 22, 0));
    $this->assertEquals ("path/33-Micah-333.html", Filter_Paths::htmlFileNameBible ("path", 33, 333));
    $this->assertEquals ("33-Micah-333.html", Filter_Paths::htmlFileNameBible ("", 33, 333));
  }
  
  
}
?>
