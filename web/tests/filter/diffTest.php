<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
  
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


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


  public function testFilterOne ()
  {
    Filter_Diff::produceVerseLevel ($this->bible_id, $this->temporary_folder);
    $this->assertFileEquals (dirname (__FILE__) . "/diffStandards/verses_old.usfm", $this->temporary_folder . "/verses_old.usfm");
    $this->assertFileEquals (dirname (__FILE__) . "/diffStandards/verses_new.usfm", $this->temporary_folder . "/verses_new.usfm");
    $this->assertFileEquals (dirname (__FILE__) . "/diffStandards/verses_old.txt", $this->temporary_folder . "/verses_old.txt");
    $this->assertFileEquals (dirname (__FILE__) . "/diffStandards/verses_new.txt", $this->temporary_folder . "/verses_new.txt");
    Filter_Diff::runWDiffFile ($this->temporary_folder . "/verses_old.usfm", $this->temporary_folder . "/verses_new.usfm", $this->temporary_folder . "/changed_verses.html");
    $this->assertFileEquals (dirname (__FILE__) . "/diffStandards/changed_verses.html", $this->temporary_folder . "/changed_verses.html");
  }

  
  public function testDiff ()
  {
    $output = Filter_Diff::diff ("Old text", "New text");
    $standard = '<span style="text-decoration: line-through;">Old </span><span style="font-weight: bold;">New </span>text';
    $this->assertEquals ($standard, $output);
  }
  

}
?>
