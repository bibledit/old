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
