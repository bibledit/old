<?php
require_once 'PHPUnit/Framework.php';
 
class odfdomTextTest extends PHPUnit_Framework_TestCase
{

private $temporary_folder;

  public function setUp ()
  {
    $this->tearDown ();
    $this->temporary_folder = tempnam (sys_get_temp_dir(), '');
    unlink ($this->temporary_folder);
    mkdir ($this->temporary_folder);
  }
  
  public function tearDown ()
  {
  }

  public function testOne()
  {
    $odfdom_text = new Odfdom_Text;
    $odfdom_text->createPageBreakStyle ();
    $odfdom_text->newParagraph ();
    $this->assertEquals ("Standard", $odfdom_text->currentParagraphStyle);
    $odfdom_text->addText ("Paragraph One");
    $this->assertEquals ("Paragraph One", $odfdom_text->currentParagraphContent);
    $odfdom_text->newParagraph ();
    $this->assertEquals ("", $odfdom_text->currentParagraphContent);
    $odfdom_text->addText ("Paragraph Two");
    $this->assertEquals ("Paragraph Two", $odfdom_text->currentParagraphContent);
    $odfdom_text->newHeading1 ("Heading One");
    $this->assertEquals ("", $odfdom_text->currentParagraphContent);
    $odfdom_text->newPageBreak ();
    $odfdom_text->newParagraph ();
    $odfdom_text->addText ("Paragraph Three");
    $odfdom_text->finalize ("/tmp/OdfTest1.odt");
    @unlink ("/tmp/OdfTest1.odt");
    $directory = Filter_Java::compile ($odfdom_text->javaCode, array (Odfdom_Class::path (), Filter_Java::xercesClassPath()));
    $return_var = Filter_Java::run ($directory, array (Odfdom_Class::path (), Filter_Java::xercesClassPath()), "odt");
    $this->assertEquals (0, $return_var);
    exec ("odt2txt /tmp/OdfTest1.odt", $output, &$return_var);
    $this->assertEquals (array ("", "Paragraph One", "", "Paragraph Two", "", "Heading One", "-----------", "", "Paragraph Three", ""), $output);
  }


  public function testTwo()
  {
    $odfdom_text = new Odfdom_Text;
    $odfdom_text->addText ("Should create new paragraph automatically");
    $odfdom_text->finalize ("/tmp/OdfTest2.odt");
    @unlink ("/tmp/OdfTest2.odt");
    $directory = Filter_Java::compile ($odfdom_text->javaCode, array (Odfdom_Class::path (), Filter_Java::xercesClassPath()));
    $return_var = Filter_Java::run ($directory, array (Odfdom_Class::path (), Filter_Java::xercesClassPath()), "odt");
    $this->assertEquals (0, $return_var);
    exec ("odt2txt /tmp/OdfTest2.odt", $output, &$return_var);
    $this->assertEquals (array ("", "Should create new paragraph automatically", ""), $output);
  }


}
?>


