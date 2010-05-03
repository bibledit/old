<?php
require_once 'PHPUnit/Framework.php';
 
class notesTest extends PHPUnit_Framework_TestCase
{

  public function testPrivacy2text()
  {
    $this->assertEquals("Manager | Administrator", Filter_Notes::privacy2text (5));
    $this->assertEquals("Guest | Member | Consultant | Translator | Manager | Administrator", Filter_Notes::privacy2text (0));
    $this->assertEquals("", Filter_Notes::privacy2text (1000));
  }


 
}
?>


