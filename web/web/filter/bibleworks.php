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


class Filter_Bibleworks
{


  /**
  * Imports BibleWorks $data as USFM code.
  * This is $data fit for the BibleWorks Version Database Compiler.
  * It a string of USFM code.
  * $keepTags: Whether to keep the grammatical tags in the BibleWorks text.
  * These are the tags between () or <>.
  */
  public static function import ($data, $keepTags)
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


      // Deal with the grammatical tags.
      if (!$keepTags) {
        $malformed = array ();
        $line = Filter_Bibleworks::parenthesis ($line, $malformed);
        $line = Filter_Bibleworks::chevrons ($line, $malformed);
      }


      // Output the verse.
      $usfm [] = '\v ' . $verse . ' ' . $line;
    }


    $usfm = implode ("\n", $usfm);
    return $usfm;
  }



  /**
  * Converts the italics in BibleWorks format to italics in USFM format.
  * BibleWorks gives the italics between square brackets, like so: [italics].
  * It returns the converted $line.
  */
  public static function italics ($line)
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
  * BibleWorks gives the notes between curly brackets, like so: [italics].
  * It returns the converted $line.
  */
  public static function notes ($line)
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



  /**
  * Removes numericals in parenthesis.
  * The numericals in BibleWorks text contain grammatical tags.
  * Example: (08804)
  * It will only remove well-formed tags.
  * The malformed tags are not removed, and added to array $malformed.
  * It returns the $line without the grammatical tags in parenthesis.
  */
  public static function parenthesis ($line, &$malformed)
  {
    $startPosition = strpos ($line, '(');
    while ($startPosition !== false) {
      $endPosition = strpos ($line, ')', $startPosition);
      if ($endPosition !== false) {
        $text = substr ($line, $startPosition + 1, $endPosition - $startPosition - 1);
        if (is_numeric ($text)) {
          $line = substr_replace ($line, '', $startPosition, $endPosition - $startPosition + 1);
        } else {
          $malformed [] = substr ($line, $startPosition, $endPosition - $startPosition + 1);
        }
      }
      $startPosition = strpos ($line, '(', $startPosition + 1);
    }
    return $line;
  }


  /**
  * Removes grammatical tags between chevrons.
  * Example: <06030a>
  * It will only remove well-formed tags.
  * The malformed tags are not removed, and added to array $malformed.
  * It returns the $line without the grammatical tags between chevrons.
  */
  public static function chevrons ($line, &$malformed)
  {
    $startPosition = strpos ($line, '<');
    while ($startPosition !== false) {
      $endPosition = strpos ($line, '>', $startPosition);
      if ($endPosition !== false) {
        $text = substr ($line, $startPosition + 1, $endPosition - $startPosition - 1);
        // Tags may contgain an 'a' or a 'b', e.g.: <06030a>. Remove them.
        $text = str_replace ("a", "", $text);
        $text = str_replace ("b", "", $text);
        // Tags may also contain commas (,) or spaces ( ), e.g. <06030, 06031>
        $text = str_replace (",", "", $text);
        $text = str_replace (" ", "", $text);
        // Remove numerical tags, e.g.: <06030>
        if (is_numeric ($text)) {
          $line = substr_replace ($line, '', $startPosition, $endPosition - $startPosition + 1);
        } else {
          $malformed [] = substr ($line, $startPosition, $endPosition - $startPosition + 1);
        }
      }
      $startPosition = strpos ($line, '<', $startPosition + 1);
    }
    return $line;
  }


}


?>
