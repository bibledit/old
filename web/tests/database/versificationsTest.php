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

 
class databaseVersificationsTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    $database_versifications = Database_Versifications::getInstance ();
    $database_versifications->delete ("phpunit");
  }


  public function textOptimize ()
  {
    $database_versifications = Database_Versifications::getInstance ();
    $database_versifications->optimize ();
  }


  public function testGetID ()
  {
    $database_versifications = Database_Versifications::getInstance ();
    $id = $database_versifications->getID ("English");
    $this->assertEquals (7, $id);
  }
  

  public function testBooks ()
  {
    $database_versifications = Database_Versifications::getInstance ();
    $books = $database_versifications->getBooks ("English");
    $standard = array (
      0 => "1",
      1 => "2",
      2 => "3",
      3 => "4",
      4 => "5",
      5 => "6",
      6 => "7",
      7 => "8",
      8 => "9",
      9 => "10",
      10 => "11",
      11 => "12",
      12 => "13",
      13 => "14",
      14 => "15",
      15 => "16",
      16 => "17",
      17 => "18",
      18 => "19",
      19 => "20",
      20 => "21",
      21 => "22",
      22 => "23",
      23 => "24",
      24 => "25",
      25 => "26",
      26 => "27",
      27 => "28",
      28 => "29",
      29 => "30",
      30 => "31",
      31 => "32",
      32 => "33",
      33 => "34",
      34 => "35",
      35 => "36",
      36 => "37",
      37 => "38",
      38 => "39",
      39 => "40",
      40 => "41",
      41 => "42",
      42 => "43",
      43 => "44",
      44 => "45",
      45 => "46",
      46 => "47",
      47 => "48",
      48 => "49",
      49 => "50",
      50 => "51",
      51 => "52",
      52 => "53",
      53 => "54",
      54 => "55",
      55 => "56",
      56 => "57",
      57 => "58",
      58 => "59",
      59 => "60",
      60 => "61",
      61 => "62",
      62 => "63",
      63 => "64",
      64 => "65",
      65 => "66"
    );
    $this->assertEquals ($standard, $books);
  }


  public function testChapters ()
  {
    $database_versifications = Database_Versifications::getInstance ();
    $chapters = $database_versifications->getChapters ("English", 1);
    $standard = array (
      0 => "1",
      1 => "2",
      2 => "3",
      3 => "4",
      4 => "5",
      5 => "6",
      6 => "7",
      7 => "8",
      8 => "9",
      9 => "10",
      10 => "11",
      11 => "12",
      12 => "13",
      13 => "14",
      14 => "15",
      15 => "16",
      16 => "17",
      17 => "18",
      18 => "19",
      19 => "20",
      20 => "21",
      21 => "22",
      22 => "23",
      23 => "24",
      24 => "25",
      25 => "26",
      26 => "27",
      27 => "28",
      28 => "29",
      29 => "30",
      30 => "31",
      31 => "32",
      32 => "33",
      33 => "34",
      34 => "35",
      35 => "36",
      36 => "37",
      37 => "38",
      38 => "39",
      39 => "40",
      40 => "41",
      41 => "42",
      42 => "43",
      43 => "44",
      44 => "45",
      45 => "46",
      46 => "47",
      47 => "48",
      48 => "49",
      49 => "50"
    );
    $this->assertEquals ($standard, $chapters);
    $database_versifications = Database_Versifications::getInstance ();
    $chapters = $database_versifications->getChapters ("English", 1, true);
    $this->assertContains (0, $chapters);
  }


  public function testVerses ()
  {
    $database_versifications = Database_Versifications::getInstance ();
    $verses = $database_versifications->getVerses ("English", 1, 2);
    $standard = array (
      0 => 0,
      1 => 1,
      2 => 2,
      3 => 3,
      4 => 4,
      5 => 5,
      6 => 6,
      7 => 7,
      8 => 8,
      9 => 9,
      10 => 10,
      11 => 11,
      12 => 12,
      13 => 13,
      14 => 14,
      15 => 15,
      16 => 16,
      17 => 17,
      18 => 18,
      19 => 19,
      20 => 20,
      21 => 21,
      22 => 22,
      23 => 23,
      24 => 24,
      25 => 25
    );
    $this->assertEquals ($standard, $verses);
  }


  public function testVersesInChapterZero ()
  {
    $database_versifications = Database_Versifications::getInstance ();
    $verses = $database_versifications->getVerses ("English", 1, 0);
    $standard = array (0);
    $this->assertEquals ($standard, $verses);
  }


  public function testCreateDelete ()
  {
    $database_versifications = Database_Versifications::getInstance ();
    $id = $database_versifications->createSystem ("phpunit");
    $this->assertEquals (1000, $id);
    $id = $database_versifications->getID ("phpunit");
    $this->assertEquals (1000, $id);
    $systems = $database_versifications->getSystems ();
    $this->assertContains ("phpunit", $systems);
    $database_versifications->delete ("phpunit");
    $systems = $database_versifications->getSystems ();
    $this->assertNotContains ("phpunit", $systems);
  }
  
  
  public function testBooksChaptersVerses ()
  {
    $database_versifications = Database_Versifications::getInstance ();
    $data = $database_versifications->getBooksChaptersVerses ("English");
    $this->assertEquals (1189, count ($data));
    $this->assertEquals (31, $data [0] ['verse']);
  }
  
  
  public function testImportExport ()
  {
    $database_versifications = Database_Versifications::getInstance ();
$xml = <<<'EOD'
<?xml version="1.0" encoding="UTF-8"?>
<bibledit-versification-system>
  <triad>
    <book>Genesis</book>
    <chapter>1</chapter>
    <verse>31</verse>
  </triad>
  <triad>
    <book>Genesis</book>
    <chapter>2</chapter>
    <verse>25</verse>
  </triad>
</bibledit-versification-system>
EOD;
    $database_versifications->importBibleditXml ($xml, "phpunit");
    $id = $database_versifications->getID ("phpunit");
    $this->assertEquals (1000, $id);
    $data = $database_versifications->getBooksChaptersVerses ("phpunit");
    $this->assertEquals (2, count ($data));
    $this->assertEquals (25, $data [1] ['verse']);
    $export = $database_versifications->exportBibleditXmlFile ("phpunit");
    $this->assertEquals (trim ($xml), trim ($export));
  }
  
  
}
?>
