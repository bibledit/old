<?php
require_once 'PHPUnit/Framework.php';
require_once "../web/filter/usfm.php";
 
class OneStringTest extends PHPUnit_Framework_TestCase
{

  public function testOneString()
  {
    $this->assertEquals("", Filter_Usfm::oneString (""));
    $this->assertEquals("\\id GEN", Filter_Usfm::oneString ("\\id GEN\n"));
    $this->assertEquals("\\v 10 text", Filter_Usfm::oneString ("\\v 10\ntext"));
    $this->assertEquals("\\v 10\\v 11", Filter_Usfm::oneString ("\\v 10\n\\v 11"));
    $this->assertEquals("\\v 10 text\\p\\v 11", Filter_Usfm::oneString ("\\v 10 text\n\\p\\v 11"));
  }
  
}
?>
