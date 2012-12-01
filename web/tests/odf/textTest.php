<?php


class odfdomTextTest extends PHPUnit_Framework_TestCase
{


private $temporary_folder;


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
  }


  public function testOne()
  {
    $odf_text = new Odf_Text;
    $odf_text->createPageBreakStyle ();
    $odf_text->newParagraph ();
    $this->assertEquals ("Standard", $odf_text->currentParagraphStyle);
    $odf_text->addText ("Paragraph One");
    $this->assertEquals ("Paragraph One", $odf_text->currentParagraphContent);
    $odf_text->newParagraph ();
    $this->assertEquals ("", $odf_text->currentParagraphContent);
    $odf_text->addText ("Paragraph Two");
    $this->assertEquals ("Paragraph Two", $odf_text->currentParagraphContent);
    $odf_text->newHeading1 ("Heading One");
    $this->assertEquals ("", $odf_text->currentParagraphContent);
    $odf_text->newPageBreak ();
    $odf_text->newParagraph ();
    $odf_text->addText ("Paragraph Three");
    @unlink ("/tmp/OdfTextTest1.odt");
    $odf_text->save ("/tmp/OdfTextTest1.odt");
    exec ("odt2txt /tmp/OdfTextTest1.odt", $output, $return_var);
    $this->assertEquals (array ("", "Paragraph One", "", "Paragraph Two", "", "Heading One", "===========", "", "Paragraph Three", ""), $output);
    @unlink ("/tmp/OdfTextTest1.odt");
  }


  public function testTwo()
  {
    $odf_text = new Odf_Text;
    $odf_text->addText ("Should create new paragraph automatically");
    @unlink ("/tmp/OdfTextTest2.odt");
    $odf_text->save ("/tmp/OdfTextTest2.odt");
    exec ("odt2txt /tmp/OdfTextTest2.odt", $output, $return_var);
    $this->assertEquals (array ("", "Should create new paragraph automatically", ""), $output);
    @unlink ("/tmp/OdfTextTest2.odt");
  }


  public function testThree()
  {
    $odf_text = new Odf_Text;
    $odf_text->newParagraph ();
    $odf_text->addText ("Text1");
    $odf_text->addNote ("†", "");
    $odf_text->addNoteText ("Note1.");
    $odf_text->addText (".");
    @unlink ("/tmp/OdfTextTest3.odt");
    $odf_text->save ("/tmp/OdfTextTest3.odt");
    exec ("odt2txt /tmp/OdfTextTest3.odt", $output, $return_var);
    $this->assertEquals (array ("", "Text1†", "", "Note1.", "", ".", ""), $output);
    @unlink ("/tmp/OdfTextTest3.odt");
  }


}
?>

