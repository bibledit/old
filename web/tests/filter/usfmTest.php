<?php
require_once 'PHPUnit/Framework.php';
 
class usfmTest extends PHPUnit_Framework_TestCase
{

  public function testGetMarkersAndText()
  {
    $this->assertEquals(array("\\id ", "GEN", "\\c ", "10"), Filter_Usfm::getMarkersAndText ("\\id GEN\\c 10"));
    $this->assertEquals(array("noise", "\\id ", "GEN", "\\c ", "10"), Filter_Usfm::getMarkersAndText ("noise\\id GEN\\c 10"));
    $this->assertEquals(array("\\p", "\\v ", "1 In ", "\\add ", "the", "\\add*"), Filter_Usfm::getMarkersAndText ("\\p\\v 1 In \\add the\\add*"));
    //$this->assertEquals("test1", "test2");
  }

  public function testOneString()
  {
    $this->assertEquals("", Filter_Usfm::oneString (""));
    $this->assertEquals("\\id GEN", Filter_Usfm::oneString ("\\id GEN\n"));
    $this->assertEquals("\\v 10 text", Filter_Usfm::oneString ("\\v 10\ntext"));
    $this->assertEquals("\\v 10\\v 11", Filter_Usfm::oneString ("\\v 10\n\\v 11"));
    $this->assertEquals("\\v 10 text\\p\\v 11", Filter_Usfm::oneString ("\\v 10 text\n\\p\\v 11"));
  }
  
  public function testGetMarker()
  {
    $this->assertEquals("", Filter_Usfm::getMarker (""));
    $this->assertEquals("id", Filter_Usfm::getMarker ("\\id GEN"));
    $this->assertEquals("", Filter_Usfm::getMarker (" \\id GEN"));
    $this->assertEquals("add", Filter_Usfm::getMarker ("\\add insertion"));
    $this->assertEquals("add", Filter_Usfm::getMarker ("\\add"));
    $this->assertEquals("add", Filter_Usfm::getMarker ("\\add*"));
    $this->assertEquals("add", Filter_Usfm::getMarker ("\\add*\add"));
  }

  public function testImport()
  {
    $this->assertEquals(array(), Filter_Usfm::import ("", "Standard"));
    $this->assertEquals(array(
        array (33, 0, "\\id MIC"), 
        array (33, 1, "\\c 1\n\\s Heading\n\\p\n\\v 1 Verse one.")
      ), 
      Filter_Usfm::import ("\\id MIC\n\\c 1\n\\s Heading\n\\p\n\\v 1 Verse one.", 
      "Standard"));
  }
}
?>


