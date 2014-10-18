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


class checksVersesTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    $database_check = Database_Check::getInstance ();
    $database_check->truncateOutput ("");
  }


  public function testMissingPunctuationAtEnd1 ()
  {
    $verses = array (2 => "He said.", 3 => "He didn't say", 4 => "He said.");
    Checks_Verses::missingPunctuationAtEnd (1, 1, 1, $verses, ", ;", ". ! ? :");
    $database_check = Database_Check::getInstance ();
    $result = $database_check->getHits ();
    $standard = array (array (
     'rowid' => "1",
     'bible' => "0",
     'book' => "1",
     'chapter' => "1",
     'verse' => "3",
     'data' => "No punctuation at end of verse: y"
    ));
    $this->assertEquals ($standard, $result);
  }


  public function testPattern1 ()
  {
    $verses = array (2 => "He said.", 3 => "He didn't say", 4 => "He said.");
    Checks_Verses::patterns (1, 1, 1, $verses, array ("did"));
    $database_check = Database_Check::getInstance ();
    $result = $database_check->getHits ();
    $standard = array (array (
     'rowid' => "1",
     'bible' => "0",
     'book' => "1",
     'chapter' => "1",
     'verse' => "3",
     'data' => "Pattern found in text: did"
    ));
    $this->assertEquals ($standard, $result);
  }


  public function testPattern2 ()
  {
    $verses = array (2 => "He said.", 3 => "He didn't say", 4 => "He said.");
    Checks_Verses::patterns (1, 1, 1, $verses, array ("Did"));
    $database_check = Database_Check::getInstance ();
    $result = $database_check->getHits ();
    $standard = array ();
    $this->assertEquals ($standard, $result);
  }


  public function testPattern3 ()
  {
    $verses = array (2 => "He said.", 3 => "He didn't say", 4 => "He said.");
    Checks_Verses::patterns (1, 1, 1, $verses, array ("said"));
    $database_check = Database_Check::getInstance ();
    $result = $database_check->getHits ();
    $standard = array (array (
     'rowid' => "1",
     'bible' => "0",
     'book' => "1",
     'chapter' => "1",
     'verse' => "2",
     'data' => "Pattern found in text: said"
    ),
    array (
     'rowid' => "2",
     'bible' => "0",
     'book' => "1",
     'chapter' => "1",
     'verse' => "4",
     'data' => "Pattern found in text: said"
    ));
    $this->assertEquals ($standard, $result);
  }


}
?>
