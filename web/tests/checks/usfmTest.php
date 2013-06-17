<?php


class usfmTest extends PHPUnit_Framework_TestCase
{


  private $check;
  
  
  public function setUp ()
  {
    $this->tearDown ();
    $this->check = new Checks_Usfm ();
    $this->check->initialize ();
  }

  
  public function tearDown ()
  {
    unset ($this->check);
  }


  public function testMalformedVerse ()
  {
$usfm = <<<EOD
\\c 1
\\p
\\v 1 He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array ();
    $this->assertEquals ($results, $standard);
$usfm = <<<EOD
\\c 1
\\p
\\v 2,He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (2 => 'Malformed verse number: \v 2,He said.'));
    $this->assertEquals ($results, $standard);
  }



}
?>
