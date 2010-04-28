<?php
require_once 'PHPUnit/Framework.php';
 
class bookTest extends PHPUnit_Framework_TestCase
{

  public function testPassageDisplay()
  {
    $this->assertEquals("Genesis 10:2", Filter_Books::passageDisplay (1, 10, 2));
    $this->assertEquals("Exodus 11:2", Filter_Books::passageDisplay (2, 11, 2));
  }

  public function testPassagesDisplay()
  {
    $this->assertEquals("Genesis 10:2", Filter_Books::passagesDisplay (array (array (1, 10, 2))));
    $this->assertEquals("", Filter_Books::passagesDisplay (array ()));
    $this->assertEquals("Genesis 10:2 | Exodus 777:777", Filter_Books::passagesDisplay (array (array (1, 10, 2), array (2, 777, 777))));
    $this->assertEquals("Unknown 10:2 | Exodus 777:777", Filter_Books::passagesDisplay (array (array (-1, 10, 2), array (2, 777, 777))));
  }

  
}
?>


