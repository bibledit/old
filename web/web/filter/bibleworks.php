<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2012 Teus Benschop.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


class Filter_Bibleworks
{


  /**
  * Imports BibleWorks $data as USFM code.
  * This is $data fit for the BibleWorks Version Database Compiler.
  * It returns an array of USFM code.
  */
  public function import ($data) // Todo
  {
    // Databases.
    $database_books = Database_Books::getInstance ();
    
    
    // Storage for the generated USFM.
    $usfm = array ();
    
    
    // The data comes as one string. Make it an array.
    $data = explode ("\n", $data);

    
    // Book / chapter trackers.
    $currentBibleWorksBookAbbreviation = "";
    $currentChapter = 0;


    // Go through each line of data to be imported.
    foreach ($data as $line) {
      $line = trim ($line);
      
      
      // Get the name of the book and remove the text fragment.
      // Upon encountering a new book, it generates USFM code for it.
      $bookAbbreviation = substr ($line, 0, 3);
      $line = substr ($line, 3, 10000);
      $line = trim ($line);
      if ($bookAbbreviation != $currentBibleWorksBookAbbreviation) {
        $currentBibleWorksBookAbbreviation = $bookAbbreviation;
        $currentChapter = 0;
        $bookID = Filter_Books::interpretBook ($bookAbbreviation);
        $book = $database_books->getUsfmFromId ($bookID);
        $usfm [] = '\id ' . $book;
      }
      
      
      // Get the chapter number and remove the text fragment.
      // Upon encountering a new chapter, it generates USFM code for it.
      $chapter = (int) $line;
      $line = substr ($line, strlen ($chapter) + 1, 10000);
      $line = trim ($line);
      if ($chapter != $currentChapter) {
        $currentChapter = $chapter;
        $usfm [] = '\c ' . $currentChapter;
        $usfm [] = '\p';
      }
      
      
      // Get the verse number and remove the text fragment and whitespace.
      $verse = (int) $line;
      $line = substr ($line, strlen ($verse), 10000);
      $line = trim ($line);


      // Convert markup for italics and footnotes.
      $line = Filter_Bibleworks::italics ($line);
      $line = Filter_Bibleworks::notes ($line);


      // Output the verse.            
      $usfm [] = '\v ' . $verse . ' ' . $line;
    }


    return $usfm;
  }



  /**
  * Converts the italics in BibleWorks format to italics in USFM format.
  * It returns the converted $line.
  */
  public function italics ($line)
  {
    $startPosition = strpos ($line, '[');
    while ($startPosition !== false) {
      $endPosition = strpos ($line, ']', $startPosition);
      if ($endPosition !== false) {
        $line = substr_replace ($line, '\add*', $endPosition, 1);
        $line = substr_replace ($line, '\add ', $startPosition, 1);
      }
      $startPosition = strpos ($line, '[', $startPosition + 1);
    }
    return $line;
  }


  /**
  * Converts the notes in BibleWorks format to footnotes in USFM format.
  * It returns the converted $line.
  */
  public function notes ($line) // Todo
  {
    $startPosition = strpos ($line, '{');
    while ($startPosition !== false) {
      $endPosition = strpos ($line, '}', $startPosition);
      if ($endPosition !== false) {
        $line = substr_replace ($line, '\f*', $endPosition, 1);
        $line = substr_replace ($line, '\f + ', $startPosition, 1);
      }
      $startPosition = strpos ($line, '{', $startPosition + 1);
    }
    return $line;
  }



}


?>
