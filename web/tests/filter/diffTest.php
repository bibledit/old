<?php


class diffTest extends PHPUnit_Framework_TestCase
{


private $database_bibles;
private $bible_id;
private $temporary_folder;


  public function setUp ()
  {
    $this->database_bibles = Database_Bibles::getInstance ();
    $this->tearDown ();
    $this->temporary_folder = uniqid (sys_get_temp_dir() . "/");
    mkdir ($this->temporary_folder);
    $this->database_bibles->createBible ("_diff_");
    $this->bible_id = $this->database_bibles->getID ("_diff_");
    $this->database_bibles->storeChapter ("_diff_", 1, 2, "old chapter text");
    $this->database_bibles->storeChapter ("_diff_", 1, 2, "new chapter text");
  }

  
  public function tearDown ()
  {
    $this->database_bibles->deleteBible ("_diff_");
    $this->database_bibles->truncateDiffs ();
    Filter_Rmdir::rmdir ($this->temporary_folder);
  }


  public function testFilter()
  {
    Filter_Diff::produceVerseLevel ($this->bible_id, $this->temporary_folder);
    $this->assertFileEquals (dirname (__FILE__) . "/diffStandards/verses_old.usfm", $this->temporary_folder . "/verses_old.usfm");
    $this->assertFileEquals (dirname (__FILE__) . "/diffStandards/verses_new.usfm", $this->temporary_folder . "/verses_new.usfm");
    Filter_Diff::runWDiff ($this->temporary_folder . "/verses_old.usfm", $this->temporary_folder . "/verses_new.usfm", $this->temporary_folder . "/changed_verses.html");
    $this->assertFileEquals (dirname (__FILE__) . "/diffStandards/changed_verses.html", $this->temporary_folder . "/changed_verses.html");
  }


}
?>
