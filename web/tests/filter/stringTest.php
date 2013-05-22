<?php

 
class stringTest extends PHPUnit_Framework_TestCase
{

  public function testReverse ()
  {
    $this->assertEquals ("drow", Filter_String::reverse ("word"));
    $this->assertEquals ("ῦοσ ὰτεμ", Filter_String::reverse ("μετὰ σοῦ"));
  }
  
  
}
?>
