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
    $odfdom_text->newParagraph ();
    $odfdom_text->addText ("Paragraph One");
    $odfdom_text->newParagraph ();
    $odfdom_text->addText ("Paragraph Two");
    $odfdom_text->addHeading1 ("Heading One");
    $odfdom_text->finalize ("/tmp/odt.odt");
    @unlink ("/tmp/odt.odt");
    $directory = Filter_Java::compile ($odfdom_text->javaCode, array (Odfdom_Class::path (), Filter_Java::xercesClassPath()));
    $return_var = Filter_Java::run ($directory, array (Odfdom_Class::path (), Filter_Java::xercesClassPath()), "odt");
    $this->assertEquals (0, $return_var);
    exec ("odt2txt /tmp/odt.odt", $output, &$return_var);
    $this->assertEquals (array ("", "Paragraph One", "", "Paragraph Two", "", "Heading One", "-----------", ""), $output);
  }


}
?>


