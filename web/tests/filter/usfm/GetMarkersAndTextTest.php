<?php
require_once 'PHPUnit/Framework.php';
require_once "../web/filter/usfm.php";
 
class GetMarkersAndTextTest extends PHPUnit_Framework_TestCase
{

  public function testIt()
  {
    $this->assertEquals(array("\\id ", "GEN", "\\c ", "10"), Filter_Usfm::getMarkersAndText ("\\id GEN\\c 10"));
    $this->assertEquals(array("noise", "\\id ", "GEN", "\\c ", "10"), Filter_Usfm::getMarkersAndText ("noise\\id GEN\\c 10"));
    $this->assertEquals(array("\\p", "\\v ", "1 In ", "\\add ", "the", "\\add*"), Filter_Usfm::getMarkersAndText ("\\p\\v 1 In \\add the\\add*"));
  }
  
}
?>
