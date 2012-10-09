<?php


require_once 'PHPUnit/Framework.php';

 
class bibleworksTest extends PHPUnit_Framework_TestCase
{


  public function testImport1() // Todo
  {
$bwdata = <<<EOD
Hab 1:1  La sentència ... profeta <05030> Habacuc <02265>.
Hab 1:2 ¿Fins <05704> quan <0575>, Jahveh ... no [em] salvaràs <03467>(08686)? 
Hab 1:3 ¿Per què ... i la controvèrsia <04066>.
EOD;
$usfmdata = <<<EOD
\\id HAB
\\c 1
\\p
\\v 1 La sentència ... profeta <05030> Habacuc <02265>.
\\v 2 ¿Fins <05704> quan <0575>, Jahveh ... no \add em\add* salvaràs <03467>(08686)?
\\v 3 ¿Per què ... i la controvèrsia <04066>.
EOD;
    $data = Filter_Bibleworks::import ($bwdata);
    $data = implode ("\n", $data);
    $this->assertEquals ($usfmdata, $data);
  }


  public function testItalics1()
  {
    $data = Filter_Bibleworks::italics ("Normal text and [italics].");
    $this->assertEquals($data, 'Normal text and \add italics\add*.');
  }
  
  
  public function testItalics2()
  {
    $data = Filter_Bibleworks::italics ("Normal text, [italics], and [italics again].");
    $this->assertEquals($data, 'Normal text, \add italics\add*, and \add italics again\add*.');
  }
  
  
  public function testItalics3()
  {
    $data = Filter_Bibleworks::italics ("[Italics] and [malformed italics.");
    $this->assertEquals($data, '\add Italics\add* and [malformed italics.');
  }
  

  public function testItalics4()
  {
    $data = Filter_Bibleworks::italics ("[Italics] and malformed italics].");
    $this->assertEquals($data, '\add Italics\add* and malformed italics].');
  }
  
  
  public function testItalics5()
  {
    $data = Filter_Bibleworks::italics ("Mal]formed [italics].");
    $this->assertEquals($data, 'Mal]formed \add italics\add*.');
  }
  
  
  public function testNotes1()
  {
    $data = Filter_Bibleworks::notes ("Normal text and {notes}.");
    $this->assertEquals($data, 'Normal text and \f + notes\f*.');
  }
  
  
  public function testNotes2()
  {
    $data = Filter_Bibleworks::notes ("Normal text, {notes}, and {notes again}.");
    $this->assertEquals($data, 'Normal text, \f + notes\f*, and \f + notes again\f*.');
  }
  
  
  public function testNotes3()
  {
    $data = Filter_Bibleworks::notes ("{notes} and {malformed notes.");
    $this->assertEquals($data, '\f + notes\f* and {malformed notes.');
  }
  

  public function testNotes4()
  {
    $data = Filter_Bibleworks::notes ("{notes} and malformed notes}.");
    $this->assertEquals($data, '\f + notes\f* and malformed notes}.');
  }
  
  
  public function testNotes5()
  {
    $data = Filter_Bibleworks::notes ("Mal}formed {notes}.");
    $this->assertEquals($data, 'Mal}formed \f + notes\f*.');
  }
  
  
}
?>


