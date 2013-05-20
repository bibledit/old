<?php

 
class databaseGitTest extends PHPUnit_Framework_TestCase
{
  private $directories = array();
  private $outputs = array();

  public function setUp ()
  {
  }
  
  public function testGit()
  {
    $database_git = Database_Git::getInstance();
    $database_git->optimize ();
    $directory = "/var/www/bibledit";
    $output = "x/y/z";
    $this->directories [] = $directory;
    $this->outputs [] = $output;
    $database_git->insert ($directory, $output);
    $data = $database_git->get ();
    $this->assertEquals ($data, array ("directory" => $directory, "output" => $output));
    $database_git->delete ($directory, $output);
    $data = $database_git->get ();
    $this->assertEquals ($data, NULL);
  }

  public function tearDown ()
  {
    $database_git = Database_Git::getInstance();
    for ($i = 0; $i < count ($this->directories); $i++) {
      $database_git->delete ($this->directories[$i], $this->outputs[$i]);
    }
  }

}

?>
