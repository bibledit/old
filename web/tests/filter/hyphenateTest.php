<?php

 
class filterHyphenateTest extends PHPUnit_Framework_TestCase
{

  private $firstset;
  private $secondset;
  
  public function setUp ()
  {
    $this->tearDown ();
    $this->firstset = array();
    preg_match_all('/./u', "aeiou", $this->firstset);
    $this->firstset = $this->firstset[0];
    $this->secondset = array();
    preg_match_all('/./u', "bcdfghjklmnpqrstvwxyz", $this->secondset);
    $this->secondset = $this->secondset[0];
  }

  
  public function tearDown ()
  {

  }


  public function testOne()
  {
$input = <<<'EOD'
\s \nd UNkulunkulu\nd* u\add ba\add*xwayisa ngokulunga okungokwabantu 文字ab化け
\s Ukulunga okuku\nd Kristu\nd* אבabגד kuyinzuzo אבגד ab
EOD;
    $output = Filter_Hyphenate::atTransition ($this->firstset, $this->secondset, $input);
$standard = <<<'EOD'
\s \nd UNku­lu­nku­lu\nd* u\add ba\add*­xwa­yi­sa ngo­ku­lu­nga oku­ngo­kwa­ba­ntu 文字a­b化け
\s Uku­lu­nga oku­ku\nd Kri­stu\nd* אבa­bגד ku­yi­nzu­zo אבגד ab
EOD;
    $this->assertEquals ($standard, $output);
  }


}
?>