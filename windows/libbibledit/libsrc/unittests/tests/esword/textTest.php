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


class eSwordTextTest extends PHPUnit_Framework_TestCase
{

  public function setUp ()
  {
  }

  
  public function tearDown ()
  {
  }


  public function testTitle()
  {
    $esword_text = new Esword_Text ("The Word of the Lord Jesus Christ");
$sql = <<<'EOD'
PRAGMA foreign_keys=OFF;
PRAGMA synchronous=OFF;
CREATE TABLE Details (Description NVARCHAR(255), Abbreviation NVARCHAR(50), Comments TEXT, Version INT, Font NVARCHAR(50), RightToLeft BOOL, OT BOOL, NT BOOL, Apocrypha BOOL, Strong BOOL);
INSERT INTO Details VALUES ('The Word of the Lord Jesus Christ', 'The Word of the Lord Jesus Christ', 'The Word of the Lord Jesus Christ', 1, 'DEFAULT', 0, 1, 1, 0, 0);
CREATE TABLE Bible (Book INT, Chapter INT, Verse INT, Scripture TEXT);
EOD;
    $this->assertEquals ($esword_text->sql, explode ("\n", $sql));
  }


  public function testZeroReference()
  {
    $esword_text = new Esword_Text ("");
    $esword_text->addText ("The Word of God");
    $esword_text->finalize ();
$sql = <<<'EOD'
PRAGMA foreign_keys=OFF;
PRAGMA synchronous=OFF;
CREATE TABLE Details (Description NVARCHAR(255), Abbreviation NVARCHAR(50), Comments TEXT, Version INT, Font NVARCHAR(50), RightToLeft BOOL, OT BOOL, NT BOOL, Apocrypha BOOL, Strong BOOL);
INSERT INTO Details VALUES ('', '', '', 1, 'DEFAULT', 0, 1, 1, 0, 0);
CREATE TABLE Bible (Book INT, Chapter INT, Verse INT, Scripture TEXT);
INSERT INTO Bible VALUES (0, 0, 0, 'The Word of God');
CREATE INDEX BookChapterVerseIndex ON Bible (Book, Chapter, Verse);
EOD;
    $this->assertEquals ($esword_text->sql, explode ("\n", $sql));
  }


  public function testZeroJohnTwoVerseThree()
  {
    $esword_text = new Esword_Text ("");
    $esword_text->newBook (43);
    $esword_text->newChapter (2);
    $esword_text->newVerse (3);
    $esword_text->addText ("In the beginning was the Word, and the Word was with God, and the Word was God.");
    $esword_text->finalize ();
$sql = <<<'EOD'
PRAGMA foreign_keys=OFF;
PRAGMA synchronous=OFF;
CREATE TABLE Details (Description NVARCHAR(255), Abbreviation NVARCHAR(50), Comments TEXT, Version INT, Font NVARCHAR(50), RightToLeft BOOL, OT BOOL, NT BOOL, Apocrypha BOOL, Strong BOOL);
INSERT INTO Details VALUES ('', '', '', 1, 'DEFAULT', 0, 1, 1, 0, 0);
CREATE TABLE Bible (Book INT, Chapter INT, Verse INT, Scripture TEXT);
INSERT INTO Bible VALUES (43, 2, 3, 'In the beginning was the Word, and the Word was with God, and the Word was God.');
CREATE INDEX BookChapterVerseIndex ON Bible (Book, Chapter, Verse);
EOD;
    $this->assertEquals ($esword_text->sql, explode ("\n", $sql));
  }


  public function testFragmentedText()
  {
    $esword_text = new Esword_Text ("");
    $esword_text->newBook (43);
    $esword_text->newChapter (1);
    $esword_text->newVerse (1);
    $esword_text->addText ("In the beginning was the Word");
    $esword_text->addText (", and the Word was with God");
    $esword_text->addText (", and the Word was God.");
    $esword_text->finalize ();
$sql = <<<'EOD'
PRAGMA foreign_keys=OFF;
PRAGMA synchronous=OFF;
CREATE TABLE Details (Description NVARCHAR(255), Abbreviation NVARCHAR(50), Comments TEXT, Version INT, Font NVARCHAR(50), RightToLeft BOOL, OT BOOL, NT BOOL, Apocrypha BOOL, Strong BOOL);
INSERT INTO Details VALUES ('', '', '', 1, 'DEFAULT', 0, 1, 1, 0, 0);
CREATE TABLE Bible (Book INT, Chapter INT, Verse INT, Scripture TEXT);
INSERT INTO Bible VALUES (43, 1, 1, 'In the beginning was the Word, and the Word was with God, and the Word was God.');
CREATE INDEX BookChapterVerseIndex ON Bible (Book, Chapter, Verse);
EOD;
    $this->assertEquals ($esword_text->sql, explode ("\n", $sql));
  }


  public function testSwitchReference()
  {
    $esword_text = new Esword_Text ("");
    $esword_text->newBook (1);
    $esword_text->newChapter (2);
    $esword_text->newVerse (3);
    $esword_text->addText ("But as many as received him, to them gave he power to become the sons of God, even to them that believe on his name.");
    $esword_text->newBook (4);
    $esword_text->newChapter (5);
    $esword_text->newVerse (6);
    $esword_text->addText ("Which were born, not of blood, nor of the will of the flesh, nor of the will of man, but of God.");
    $esword_text->finalize ();
$sql = <<<'EOD'
PRAGMA foreign_keys=OFF;
PRAGMA synchronous=OFF;
CREATE TABLE Details (Description NVARCHAR(255), Abbreviation NVARCHAR(50), Comments TEXT, Version INT, Font NVARCHAR(50), RightToLeft BOOL, OT BOOL, NT BOOL, Apocrypha BOOL, Strong BOOL);
INSERT INTO Details VALUES ('', '', '', 1, 'DEFAULT', 0, 1, 1, 0, 0);
CREATE TABLE Bible (Book INT, Chapter INT, Verse INT, Scripture TEXT);
INSERT INTO Bible VALUES (1, 2, 3, 'But as many as received him, to them gave he power to become the sons of God, even to them that believe on his name.');
INSERT INTO Bible VALUES (4, 5, 6, 'Which were born, not of blood, nor of the will of the flesh, nor of the will of man, but of God.');
CREATE INDEX BookChapterVerseIndex ON Bible (Book, Chapter, Verse);
EOD;
    $this->assertEquals ($esword_text->sql, explode ("\n", $sql));
  }


  public function testCreateModule()
  {
    $esword_text = new Esword_Text ("");
    $esword_text->addText ("In the beginning was the Word, and the Word was with God, and the Word was God.");
    $esword_text->finalize ();
    $filename = "/tmp/module.bblx";
    $esword_text->createModule ($filename);
    $this->assertFileExists ($filename, "The eSword module was not created");
    unlink ($filename);
  }


}


?>
