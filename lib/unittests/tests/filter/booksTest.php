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

  public function setUp ()
  {
    $this->tearDown ();
  }


  public function tearDown ()
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_bibles->deleteBible ("phpunit");
    $database_bibles->deleteBible ("php unit");
  }


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


  public function testInterpretBookEnglish()
  {
    $this->assertEquals (1, Filter_Books::interpretBook ("Genesis"));
    $this->assertEquals (46, Filter_Books::interpretBook ("1 Corinthians"));
    $this->assertEquals (22, Filter_Books::interpretBook ("Song of Solomon"));
    $this->assertEquals (60, Filter_Books::interpretBook ("I Peter"));
    $this->assertEquals (63, Filter_Books::interpretBook ("II John"));
    $this->assertEquals (64, Filter_Books::interpretBook ("III John"));
  }


  public function testInterpretBookUSFM ()
  {
    $this->assertEquals(1, Filter_Books::interpretBook ("GEN"));
    $this->assertEquals(67, Filter_Books::interpretBook ("FRT"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1CO"));
  }
  
  
  public function testInterpretBookOSIS ()
  {
    $this->assertEquals(1, Filter_Books::interpretBook ("Gen"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1Cor"));
    $this->assertEquals(22, Filter_Books::interpretBook ("Song"));
  }


  public function testInterpretBookBibleWorks ()
  {
    $this->assertEquals (1,  Filter_Books::interpretBook ("Gen"));
    $this->assertEquals (2,  Filter_Books::interpretBook ("Exod"));
    $this->assertEquals (3,  Filter_Books::interpretBook ("Lev"));
    $this->assertEquals (4,  Filter_Books::interpretBook ("Num"));
    $this->assertEquals (5,  Filter_Books::interpretBook ("Deut"));
    $this->assertEquals (6,  Filter_Books::interpretBook ("Jos"));
    $this->assertEquals (7,  Filter_Books::interpretBook ("Jdg"));
    $this->assertEquals (8,  Filter_Books::interpretBook ("Ruth"));
    $this->assertEquals (9,  Filter_Books::interpretBook ("1 Sam"));
    $this->assertEquals (10, Filter_Books::interpretBook ("2 Sam"));
    $this->assertEquals (11, Filter_Books::interpretBook ("1 Ki"));
    $this->assertEquals (12, Filter_Books::interpretBook ("2 Ki"));
    $this->assertEquals (13, Filter_Books::interpretBook ("1 Chr"));
    $this->assertEquals (14, Filter_Books::interpretBook ("2 Chr"));
    $this->assertEquals (15, Filter_Books::interpretBook ("Ezr"));
    $this->assertEquals (16, Filter_Books::interpretBook ("Neh"));
    $this->assertEquals (17, Filter_Books::interpretBook ("Est"));
    $this->assertEquals (18, Filter_Books::interpretBook ("Job"));
    $this->assertEquals (19, Filter_Books::interpretBook ("Ps"));
    $this->assertEquals (20, Filter_Books::interpretBook ("Prov"));
    $this->assertEquals (21, Filter_Books::interpretBook ("Eccl"));
    $this->assertEquals (22, Filter_Books::interpretBook ("Cant"));
    $this->assertEquals (22, Filter_Books::interpretBook ("Sol"));
    $this->assertEquals (23, Filter_Books::interpretBook ("Isa"));
    $this->assertEquals (24, Filter_Books::interpretBook ("Jer"));
    $this->assertEquals (25, Filter_Books::interpretBook ("Lam"));
    $this->assertEquals (26, Filter_Books::interpretBook ("Ezek"));
    $this->assertEquals (27, Filter_Books::interpretBook ("Dan"));
    $this->assertEquals (28, Filter_Books::interpretBook ("Hos"));
    $this->assertEquals (29, Filter_Books::interpretBook ("Joel"));
    $this->assertEquals (30, Filter_Books::interpretBook ("Amos"));
    $this->assertEquals (31, Filter_Books::interpretBook ("Obad"));
    $this->assertEquals (32, Filter_Books::interpretBook ("Jon"));
    $this->assertEquals (33, Filter_Books::interpretBook ("Mic"));
    $this->assertEquals (34, Filter_Books::interpretBook ("Nah"));
    $this->assertEquals (35, Filter_Books::interpretBook ("Hab"));
    $this->assertEquals (36, Filter_Books::interpretBook ("Zeph"));
    $this->assertEquals (37, Filter_Books::interpretBook ("Hag"));
    $this->assertEquals (38, Filter_Books::interpretBook ("Zech"));
    $this->assertEquals (39, Filter_Books::interpretBook ("Mal"));
    $this->assertEquals (40, Filter_Books::interpretBook ("Matt"));
    $this->assertEquals (41, Filter_Books::interpretBook ("Mk"));
    $this->assertEquals (42, Filter_Books::interpretBook ("Lk"));
    $this->assertEquals (43, Filter_Books::interpretBook ("Jn"));
    $this->assertEquals (44, Filter_Books::interpretBook ("Acts"));
    $this->assertEquals (45, Filter_Books::interpretBook ("Rom"));
    $this->assertEquals (46, Filter_Books::interpretBook ("1 Co"));
    $this->assertEquals (46, Filter_Books::interpretBook ("1Co"));
    $this->assertEquals (47, Filter_Books::interpretBook ("2 Co"));
    $this->assertEquals (48, Filter_Books::interpretBook ("Gal"));
    $this->assertEquals (49, Filter_Books::interpretBook ("Eph"));
    $this->assertEquals (50, Filter_Books::interpretBook ("Phil"));
    $this->assertEquals (51, Filter_Books::interpretBook ("Col"));
    $this->assertEquals (52, Filter_Books::interpretBook ("1 Thess"));
    $this->assertEquals (53, Filter_Books::interpretBook ("2 Thess"));
    $this->assertEquals (54, Filter_Books::interpretBook ("1 Tim"));
    $this->assertEquals (55, Filter_Books::interpretBook ("2 Tim"));
    $this->assertEquals (56, Filter_Books::interpretBook ("Tit"));
    $this->assertEquals (57, Filter_Books::interpretBook ("Phlm"));
    $this->assertEquals (58, Filter_Books::interpretBook ("Heb"));
    $this->assertEquals (59, Filter_Books::interpretBook ("Jas"));
    $this->assertEquals (60, Filter_Books::interpretBook ("1 Pet"));
    $this->assertEquals (61, Filter_Books::interpretBook ("2 Pet"));
    $this->assertEquals (62, Filter_Books::interpretBook ("1 Jn"));
    $this->assertEquals (63, Filter_Books::interpretBook ("2 Jn"));
    $this->assertEquals (64, Filter_Books::interpretBook ("3 Jn"));
    $this->assertEquals (65, Filter_Books::interpretBook ("Jude"));
    $this->assertEquals (66, Filter_Books::interpretBook ("Rev"));
  }


  public function testInterpretBookOnlineBibleAbbreviations ()
  {
    $this->assertEquals(1, Filter_Books::interpretBook ("Ge"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1Co"));
    $this->assertEquals(22, Filter_Books::interpretBook ("So"));
  }  


  public function testInterpretBookPartialNames ()
  {
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
  

  public function testSequenceAndRangeTwo ()
  {
    $standard = array ("Exod. 25:13", 14, 27, 28);
    $output = Filter_Books::handleSequencesRanges ("Exod. 25:13-14, 27-28");
    $this->assertEquals ($standard, $output);
  }
  
  
  public function testPassageInteger ()
  {
    $standard = explode (".", "6.4.2");
    $numeric = Filter_Books::passage2integer ($standard);
    $passage = Filter_Books::integer2passage ($numeric);
    $this->assertEquals ($standard, $passage);
  }
  
  
  public function testGetOrdered ()
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_config_bible = Database_Config_Bible::getInstance ();
    $bible = "php unit";

    // No ordering.
    $database_bibles->createBible ($bible);
    $database_bibles->storeChapter ($bible, 1, 1, "data");
    $database_bibles->storeChapter ($bible, 2, 1, "data");
    $database_bibles->storeChapter ($bible, 3, 1, "data");
    $database_bibles->storeChapter ($bible, 4, 1, "data");
    $database_bibles->storeChapter ($bible, 5, 1, "data");
    $books = Filter_Books::getOrdered ($bible);
    $standard = array (1, 2, 3, 4, 5);
    $this->assertEquals ($standard, $books);

    // Existing books re-ordered.
    $database_config_bible->setBookOrder ($bible, "1 3 2 5 4");
    $books = Filter_Books::getOrdered ($bible);
    $standard = array (1, 3, 2, 5, 4);
    $this->assertEquals ($standard, $books);

    // Some books ordered, and Bible has extra books: These are to be added to the end.
    $database_config_bible->setBookOrder ($bible, "1 3 2");
    $books = Filter_Books::getOrdered ($bible);
    $standard = array (1, 3, 2, 4, 5);
    $this->assertEquals ($standard, $books);

    // More books ordered than in Bible: Remove the extra ones.
    $database_config_bible->setBookOrder ($bible, "1 3 2 5 4 6");
    $books = Filter_Books::getOrdered ($bible);
    $standard = array (1, 3, 2, 5, 4);
    $this->assertEquals ($standard, $books);
  }

  
}


?>
