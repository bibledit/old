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
* Class for creating input files for the Online Bible compiler.
*/
class Onlinebible_Text
{
  private $output;
  private $currentLine;


  public function __construct ()
  {
    $this->output = array ();
  }


  public function storeData ()
  {
    if (isset ($this->currentLine)) {
      $this->output [] = trim ($this->currentLine);
    }
    unset ($this->currentLine);
  }


  public function newVerse ($bookIdentifier, $chapterNumber, $verseNumber)
  {
    $this->storeData ();
    $database_books = Database_Books::getInstance ();
    // Store passage and any text only in case the book is valid,
    // and the chapter and verse are non-zero.
    $book = $database_books->getOnlinebibleFromId ($bookIdentifier);
    if ($book != "") {
      if ($chapterNumber > 0) {
        $verse = Filter_Numeric::integer_in_string ($verseNumber);
        if ($verse > 0) {
          $this->output [] = "$$$ $book $chapterNumber:$verse";
          $this->currentLine = "";
        }
      }
    }
  }


  /**
  * This function adds text to the current line.
  * $text: The text to add.
  */
  public function addText ($text)
  {
    if (isset ($this->currentLine)) {
      $this->currentLine .= $text;
    }
  }


  /**
  * This function adds a note to the current paragraph.
  */
  public function addNote ()
  {
    $this->addText ("{");
  }


  /**
  * This function closes the current footnote.
  */
  public function closeCurrentNote ()
  {
    $this->addText ("}");
  }


  /**
  * This saves the data to file
  * $name: the name of the file to save to.
  */
  public function save ($name)
  {
    $this->storeData ();
    $data = implode ("\n", $this->output);
    file_put_contents ($name, $data);
  }


}


?>
