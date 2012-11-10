<?php

 
class onlineBibleTextTest extends PHPUnit_Framework_TestCase
{

  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
  }


  public function testOne()
  {
    $onlinebible_text = new Onlinebible_Text;
    $onlinebible_text->addText ("No verse given, so discard this.");
    $onlinebible_text->newVerse (2, 2, "2-6a");
    $onlinebible_text->addText ("Text for Exodus 2:2, not verse 2-6a.");
    $onlinebible_text->storeData ();
    $onlinebible_text->addText ("Verse was stored, no new verse given, so discard this.");
    $onlinebible_text->save ("/tmp/OLBTextTest1.exp");
    $this->assertFileEquals (dirname (__FILE__) . "/test1.exp", "/tmp/OLBTextTest1.exp");
    unlink ("/tmp/OLBTextTest1.exp");
  }


  public function testTwo()
  {
    $onlinebible_text = new Onlinebible_Text;
    $onlinebible_text->addNote ();
    $onlinebible_text->addText ("Discard this note text because no verse has been given yet.");
    $onlinebible_text->closeCurrentNote ();
    $onlinebible_text->newVerse (1, 2, 3);
    $onlinebible_text->addNote ();
    $onlinebible_text->addText ("Output this note text.");
    $onlinebible_text->closeCurrentNote ();
    $onlinebible_text->save ("/tmp/OLBTextTest2.exp");
    $this->assertFileEquals (dirname (__FILE__) . "/test2.exp", "/tmp/OLBTextTest2.exp");
    unlink ("/tmp/OLBTextTest2.exp");
  }


}
?>


