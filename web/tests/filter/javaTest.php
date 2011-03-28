<?php
require_once 'PHPUnit/Framework.php';
 
class filterJavaTest extends PHPUnit_Framework_TestCase
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

  public function testSystem()
  {
    $tmp = $this->temporary_folder;
$code = <<<EOD
import java.io.*;
class FileWrite
{
  public static void main(String args[])
  {
    try{
      FileWriter fstream = new FileWriter("$tmp/out.txt");
      BufferedWriter out = new BufferedWriter(fstream);
      out.write("Hello Java");
      out.close();
    } catch (Exception e) {
      System.err.println("Error: " + e.getMessage());
    }
  }
}
EOD;
    $directory = Filter_Java::compile (explode ("\n", $code), NULL);
    Filter_Java::run ($directory, NULL, "FileWrite");
    $this->assertFileEquals (dirname (__FILE__) . "/javaStandards/out.txt", "$tmp/out.txt");
  }




}
?>


