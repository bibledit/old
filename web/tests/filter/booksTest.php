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

 
class booksTest extends PHPUnit_Framework_TestCase
{

  public function testPassageDisplay()
  {
    $this->assertEquals("Genesis 10:2", Filter_Books::passageDisplay (1, 10, 2));
    $this->assertEquals("Exodus 11:2", Filter_Books::passageDisplay (2, 11, 2));
  }

  public function testPassagesDisplayInline()
  {
    $this->assertEquals("Genesis 10:2", Filter_Books::passagesDisplayInline (array (array (1, 10, 2))));
    $this->assertEquals("", Filter_Books::passagesDisplayInline (array ()));
    $this->assertEquals("Genesis 10:2 | Exodus 777:777", Filter_Books::passagesDisplayInline (array (array (1, 10, 2), array (2, 777, 777))));
    $this->assertEquals("Unknown 10:2 | Exodus 777:777", Filter_Books::passagesDisplayInline (array (array (-1, 10, 2), array (2, 777, 777))));
  }

  public function testPassagesDisplayMultiline()
  {
    $this->assertEquals("Genesis 10:2\n", Filter_Books::passagesDisplayMultiline (array (array (1, 10, 2))));
    $this->assertEquals("", Filter_Books::passagesDisplayMultiline (array ()));
    $this->assertEquals("Genesis 10:2\nExodus 777:777\n", Filter_Books::passagesDisplayMultiline (array (array (1, 10, 2), array (2, 777, 777))));
    $this->assertEquals("Unknown 10:2\nExodus 777:777\n", Filter_Books::passagesDisplayMultiline (array (array (-1, 10, 2), array (2, 777, 777))));
  }

  public function testInterpretBook()
  {
    // Test interpretation of full English names.
    $this->assertEquals(1, Filter_Books::interpretBook ("Genesis"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1 Corinthians"));
    $this->assertEquals(22, Filter_Books::interpretBook ("Song of Solomon"));
    // Test interpretation of USFM abbreviations.
    $this->assertEquals(1, Filter_Books::interpretBook ("GEN"));
    $this->assertEquals(67, Filter_Books::interpretBook ("FRT"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1CO"));
    // Test interpretation of OSIS books.
    $this->assertEquals(1, Filter_Books::interpretBook ("Gen"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1Cor"));
    $this->assertEquals(22, Filter_Books::interpretBook ("Song"));
    // Test interpretation of BibleWorks abbreviations.
    $this->assertEquals(1, Filter_Books::interpretBook ("Gen"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1Co"));
    $this->assertEquals(22, Filter_Books::interpretBook ("Sol"));
    // Test interpretation of Online Bible abbreviations.
    $this->assertEquals(1, Filter_Books::interpretBook ("Ge"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1Co"));
    $this->assertEquals(22, Filter_Books::interpretBook ("So"));
    // Test partial book names.
    $this->assertEquals(1, Filter_Books::interpretBook ("G"));
    $this->assertEquals(1, Filter_Books::interpretBook ("g"));
    $this->assertEquals(1, Filter_Books::interpretBook ("ge"));
    $this->assertEquals(1, Filter_Books::interpretBook ("gene"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1 Cori"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1 cori"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1 corint"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1cor"));
    $this->assertEquals(22, Filter_Books::interpretBook ("song"));
    $this->assertEquals(22, Filter_Books::interpretBook ("song of"));
    $this->assertEquals(22, Filter_Books::interpretBook ("song of sol"));
    $this->assertEquals(11, Filter_Books::interpretBook ("1ki"));
  }


  public function testExplodePassage()
  {
    $this->assertEquals(array (1, 2, 2), Filter_Books::explodePassage ("Genesis 2:2"));
    $this->assertEquals(array (46, 2, 2), Filter_Books::explodePassage ("1 Corinth. 2:2"));
    $this->assertEquals(array (22, 2, 2), Filter_Books::explodePassage ("Song of Sol. 2:2"));
    $this->assertEquals(array (66, 2, 2), Filter_Books::explodePassage ("Revelation 2:2"));
  }
  
  
  public function testInterpretPassage ()
  {
    $currentPassage = array (2, 4, 6);

    $standard = array (1, 2, 3);
    $output = Filter_Books::interpretPassage ($currentPassage, " Genesis 2 3");
    $this->assertEquals ($standard, $output);

    $standard = array (46, 2, 3);
    $output = Filter_Books::interpretPassage ($currentPassage, "1  Corinthians 2:3");
    $this->assertEquals ($standard, $output);

    $standard = array (22, 2, 3);
    $output = Filter_Books::interpretPassage ($currentPassage, "Song of Solomon 2.3");
    $this->assertEquals ($standard, $output);

    $standard = $currentPassage;
    $output = Filter_Books::interpretPassage ($currentPassage, "");
    $this->assertEquals ($standard, $output);

    $standard = array (0, 1, 1);
    $output = Filter_Books::interpretPassage ($currentPassage, "Genesis Exodus");
    $this->assertEquals ($standard, $output);

    $standard = array (2, 1, 1);
    $output = Filter_Books::interpretPassage ($currentPassage, "Exodus");
    $this->assertEquals ($standard, $output);

    $standard = array (2, 4, 11);
    $output = Filter_Books::interpretPassage ($currentPassage, "11");
    $this->assertEquals ($standard, $output);

    $standard = array (2, 11, 14);
    $output = Filter_Books::interpretPassage ($currentPassage, "11 14");
    $this->assertEquals ($standard, $output);

    $standard = array (22, 2, 1);
    $output = Filter_Books::interpretPassage ($currentPassage, "Song of Solomon 2");
    $this->assertEquals ($standard, $output);
  }
  
    
  public function testSequenceAndRangeNone ()
  {
    $standard = array ("Exod. 30:4");
    $output = Filter_Books::handleSequencesRanges ("Exod. 30:4");
    $this->assertEquals ($standard, $output);
  }
  

  public function testSequence ()
  {
    $standard = array ("Exod. 37:5", 14, 28);
    $output = Filter_Books::handleSequencesRanges ("Exod. 37:5, 14, 28");
    $this->assertEquals ($standard, $output);
  }


  public function testRange ()
  {
    $standard = array ("Exod. 37:5", 14, 15, 16);
    $output = Filter_Books::handleSequencesRanges ("Exod. 37:5, 14 - 16");
    $this->assertEquals ($standard, $output);
  }
  
  
  public function testSequenceAndRangeOne ()
  {
    $standard = array ("Exod. 37:4", 5, 14, 15, 27, 28, 30, 40);
    $output = Filter_Books::handleSequencesRanges ("Exod. 37:4 - 5, 14 - 15, 27 - 28, 30, 40");
    $this->assertEquals ($standard, $output);
  }
  

  public function testSequenceAndRangeTwo () //
  {
    $standard = array ("Exod. 25:13", 14, 27, 28);
    $output = Filter_Books::handleSequencesRanges ("Exod. 25:13-14, 27-28");
    $this->assertEquals ($standard, $output);
  }
  
  
  
  
  
  
}
?>
