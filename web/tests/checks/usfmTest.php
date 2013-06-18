<?php


class checksUsfmTest extends PHPUnit_Framework_TestCase
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
\\v 2,He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (2 => 'Malformed verse number: \v 2,He said.'));
    $this->assertEquals ($results, $standard);
  }


  public function testNewLineInUSFMGood ()
  {
$usfm = <<<EOD
\\c 1
\\p He said.
\\v 1 He said.
\\p He said.
\\v 2 He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array ();
    $this->assertEquals ($results, $standard);
  }


  public function testNewLineInUSFMOne ()
  {
$usfm = <<<EOD
\\c 1
\\p He said.
\\v 1 He said.
\\
\\p He said.
\\v 2 He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (0 => 'New line within USFM:  \ \p He s'));
    $this->assertEquals ($results, $standard);
  }


  public function testNewLineInUSFMTwo ()
  {
$usfm = <<<EOD
\\c 1
\\p He said.
\\v 1 He said.
\\ 
\\p He said.
\\v 3 He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (0 => 'New line within USFM:  \  \p He '));
    $this->assertEquals ($results, $standard);
  }


  public function testUnknownUsfmOne ()
  {
$usfm = <<<EOD
\\c 1
\\p He said.
\\v 1 He said \add something\add*.
\\p,p He said.
\\v 3 He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (1 => 'Marker not in stylesheet: \p,p '));
    $this->assertEquals ($results, $standard);
  }


  public function testUnknownUsfmTwoe ()
  {
$usfm = <<<EOD
\\c 1
\\pHe said.
\\v 1 He said \add something\add*.
\\p He said.
\\v 3 He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (0 => 'Marker not in stylesheet: \pHe '));
    $this->assertEquals ($results, $standard);
  }


  public function testLineWithoutUsfm ()
  {
$usfm = <<<EOD
\\c 1
\\p 
He said.
\\p He said.
\\v 3 He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (0 => 'Line without USFM: He said.'));
    $this->assertEquals ($results, $standard);
  }


  public function testIdOne ()
  {
$usfm = <<<EOD
\\id GENN
\\p He said.
\\v 1 He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (0 => 'Unknown ID: \id GENN'));
    $this->assertEquals ($results, $standard);
  }


  public function testIdTwo()
  {
$usfm = <<<EOD
\\id
\\p He said.
\\v 1 He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (0 => 'Unknown ID: \id\p '));
    $this->assertEquals ($results, $standard);
  }


  public function testIdThree ()
  {
$usfm = <<<EOD
\\id Gen
\\p He said.
\\v 1 He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (0 => 'ID is not in uppercase: \id Gen'));
    $this->assertEquals ($results, $standard);
  }


  public function testForwardSlashOne ()
  {
$usfm = <<<EOD
\\id GEN
\\p He said.
\\v 1 He said. He said something/add*.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (0 => 'Forward slash instead of backslash: /add'));
    $this->assertEquals ($results, $standard);
  }


  public function testForwardSlashTwo ()
  {
$usfm = <<<EOD
\\id GEN
\\p He said.
\\v 1 /v He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (0 => 'Forward slash instead of backslash: /v'));
    $this->assertEquals ($results, $standard);
  }


}
?>
