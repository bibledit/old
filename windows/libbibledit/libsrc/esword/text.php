<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

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


/**
* Class for creating Html text documents.
*/
class Esword_Text
{

  public $sql; // Array with the generated SQL.

  private $currentBook;
  private $currentChapter;
  private $currentVerse;
  private $currentText;


  public function __construct ($bible)
  {
    $this->sql = array ();
    $this->currentBook = 0;
    $this->currentChapter = 0;
    $this->currentVerse = 0;
    $this->currentText = "";
    $bible = Database_SQLiteInjection::no ($bible);
    $this->sql [] = 'PRAGMA foreign_keys=OFF;';
    $this->sql [] = 'PRAGMA synchronous=OFF;';
    $this->sql [] = 'CREATE TABLE Details (Description NVARCHAR(255), Abbreviation NVARCHAR(50), Comments TEXT, Version INT, Font NVARCHAR(50), RightToLeft BOOL, OT BOOL, NT BOOL, Apocrypha BOOL, Strong BOOL);';
    $this->sql [] = "INSERT INTO Details VALUES ('$bible', '$bible', '$bible', 1, 'DEFAULT', 0, 1, 1, 0, 0);";
    $this->sql [] = 'CREATE TABLE Bible (Book INT, Chapter INT, Verse INT, Scripture TEXT);';
  }


  public function flushCache ()
  {
    $text = trim ($this->currentText);
    if ($text != "") {
      $text = Database_SQLiteInjection::no ($text);
      $book = (int) $this->currentBook;
      $chapter = (int) $this->currentChapter;
      $verse = (int) $this->currentVerse;
      $statement = "INSERT INTO Bible VALUES ($book, $chapter, $verse, '$text');";
      $this->sql [] = $statement;
    }
    $this->currentText = "";
  }


  public function newBook ($book)
  {
    $this->flushCache ();
    $this->currentBook = $book;
    $this->newChapter (0);
  }


  public function newChapter ($chapter)
  {
    $this->flushCache ();
    $this->currentChapter = $chapter;
    $this->currentVerse = 0;
  }


  public function newVerse ($verse)
  {
    $this->flushCache ();
    $this->currentVerse = $verse;
  }


  public function addText ($text)
  {
    if ($text != "") {
      $this->currentText .= $text;
    }
  }


  /**
  * This finalizes the SQL script.
  */
  public function finalize ()
  {
    $this->flushCache ();
    // Add the final SQL statements.
    $this->sql [] = 'CREATE INDEX BookChapterVerseIndex ON Bible (Book, Chapter, Verse);';
  }


  /**
  * This creates the eSword module.
  * $filename: the name of the file to create.
  */
  public function createModule ($filename)
  {
    $this->flushCache ();
    $db = new PDO ("sqlite:$filename");
    foreach ($this->sql as $sql) {
      $db->exec ($sql);
    }
  }


}


?>
