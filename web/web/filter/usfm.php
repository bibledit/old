<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2013 Teus Benschop.
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


class Filter_Usfm
{

  /**
  * Returns the string $usfm as one long string.
  * $usfm may contain new lines, but the resulting long string won't.
  */
  public static function oneString ($usfm)
  {
    $long_string = "";
    $usfm = explode ("\n", $usfm);
    foreach ($usfm as &$line) {
      $line = trim ($line);
      // Skip empty line.
      if ($line != "") {
        // The line will be appended to the output line, 
        // but if it does not start with a backslash (\), a space is inserted first.
        $pos = strpos ($line, "\\");
        if ($pos === false) {
          if ($long_string != "") $long_string .= " ";
        }
        $long_string .= $line;
      }
    }
    return $long_string;
  }


  /**
  * Returns the string $usfm as an array alternating between marker and text.
  * Example, input is:   \id GEN
  *                      \c 10
  *             ...
  * Output would be:     array ("\id ", "GEN", "\c ", "10", ...)
  * If $usfm does not start with a marker, this becomes visible in the output too.
  */
  public static function getMarkersAndText ($usfm)
  {
    $markers_and_text = array ();
    $usfm = str_replace ("\n\\", "\\", $usfm); // New line followed by backslash: leave new line out.
    $usfm = str_replace ("\n", " ", $usfm); // New line only: change to space, according to the USFM specification.
    $usfm = str_replace ("  ", " ", $usfm);
    $usfm = trim ($usfm);
    while ($usfm != "") {
      if ($usfm != "") {
        $pos = strpos ($usfm, "\\");
        if ($pos === 0) {
          // Marker found. 
          // The marker ends
          // - after the first space, or
          // - after the first asterisk (*), or
          // - at the first backslash (\), or
          // - at the end of the string,
          // whichever comes first.
          $positions = array ();
          $pos = strpos ($usfm, " ");
          if ($pos !== false) $positions [] = $pos + 1;
          $pos = strpos ($usfm, "*");
          if ($pos !== false) $positions [] = $pos + 1;
          $pos = strpos ($usfm, "\\", 1);
          if ($pos !== false) $positions [] = $pos;
          $positions [] = strlen ($usfm);
          sort ($positions, SORT_NUMERIC);
          $pos = $positions[0];
          $marker = substr ($usfm, 0, $pos);
          $markers_and_text [] = $marker;
          $usfm = substr ($usfm, $pos);
        } else {
          // Text found. It ends at the next backslash or at the end of the string.
          $pos = strpos ($usfm, "\\");
          if ($pos === false) $pos = strlen ($usfm);
          $text = substr ($usfm, 0, $pos);
          $markers_and_text [] = $text;
          $usfm = substr ($usfm, $pos);
        }
      }
    }
    return $markers_and_text;
  }
    

  /**
  * Gets the marker from $usfm if it is there, else returns an empty string.
  * Examples:
  * "\id"   -> "id"
  * "\id "  -> "id"
  * "\add*" -> "add"
  */
  public static function getMarker ($usfm)
  {
    if ($usfm == "")
      return $usfm;
    $pos = strpos ($usfm, "\\");
    if ($pos === 0) {
      // Marker found. 
      // Erase backslash.
      $usfm = substr ($usfm, 1);
      // The marker ends
      // - at the first space, or
      // - at the first asterisk (*), or
      // - at the first backslash (\), or
      // - at the end of the string,
      // whichever comes first.
      $positions = array ();
      $pos = strpos ($usfm, " ");
      if ($pos !== false) $positions [] = $pos;
      $pos = strpos ($usfm, "*");
      if ($pos !== false) $positions [] = $pos;
      $pos = strpos ($usfm, "\\");
      if ($pos !== false) $positions [] = $pos;
      $positions [] = strlen ($usfm);
      sort ($positions, SORT_NUMERIC);
      $pos = $positions[0];
      $marker = substr ($usfm, 0, $pos);
      return $marker;
    } else {
      // Text found. No marker.
      return "";
    }
  }

  /**
  * This imports USFM $input.
  * It takes raw $input,
  * and returns an array of an array [book_number, chapter_number, chapter_data].
  */  
  public static function import ($input, $stylesheet)
  {
    $result = array ();

    $book_number = 0;
    $chapter_number = 0;
    $chapter_data = "";

    $input = Filter_Usfm::oneString ($input);
    $markers_and_text = Filter_Usfm::getMarkersAndText ($input);
    $retrieve_book_number_on_next_iteration = false;
    $retrieve_chapter_number_on_next_iteration = false;

    foreach ($markers_and_text as $marker_or_text) {
      if ($retrieve_book_number_on_next_iteration) {
        $database_books = Database_Books::getInstance ();
        $book_number = $database_books->getIdFromUsfm (substr ($marker_or_text, 0, 3));
        $chapter_number = 0;
        $retrieve_book_number_on_next_iteration = false;
      }
      if ($retrieve_chapter_number_on_next_iteration) {
        $retrieve_chapter_number_on_next_iteration = false;
        $chapter_number = Filter_Numeric::integer_in_string ($marker_or_text);
        if ($chapter_number == "") {
          $chapter_number = 0;
        }
      }
      $marker = Filter_Usfm::getMarker ($marker_or_text);
      if ($marker != "") {
        // USFM marker found.
        if ($marker == "id") {
          $retrieve_book_number_on_next_iteration = true;
          $store_chapter_data = true;
        }
        if ($marker == "c") {
          $retrieve_chapter_number_on_next_iteration = true;
          $store_chapter_data = true;
        }
        if ($store_chapter_data) {
          $chapter_data = trim ($chapter_data);
          if ($chapter_data != "") $result [] = array ($book_number, $chapter_number, $chapter_data);
          $chapter_number = 0;
          $chapter_data = "";
          $store_chapter_data = false;
        }
        $database_styles = Database_Styles::getInstance ();
        $marker_data = $database_styles->getMarkerData ($stylesheet, $marker);
        $type = $marker_data['type'];
        $subtype = $marker_data['subtype'];
        $styles_logic = Styles_Logic::getInstance();
        if ($styles_logic->startsNewLineInUsfm ($type, $subtype)) {
          $chapter_data .= "\n";
        }
      }
      $chapter_data .= $marker_or_text;
    }
    $chapter_data = trim ($chapter_data);
    if ($chapter_data != "") $result [] = array ($book_number, $chapter_number, $chapter_data);
    return $result;    
  }

  /**
  * Returns an array with the verse numbers found in $usfm.
  */
  public static function getVerseNumbers ($usfm)
  {
    $verse_numbers = array (0);
    $markers_and_text = Filter_Usfm::getMarkersAndText ($usfm);
    $extract_verse = false;
    foreach ($markers_and_text as $marker_or_text) {
      if ($extract_verse) {
        $verse_numbers [] = Filter_Numeric::integer_in_string ($marker_or_text);
        $extract_verse = false;
      }
      if (substr ($marker_or_text, 1, 1) == "v") {
        $extract_verse = true;
      }
    }
    return $verse_numbers;
  }

  /**
  * Returns the verse number in the string of $usfm code at line number $line_number.
  */
  public static function lineNumber2VerseNumber ($usfm, $line_number)
  {
    $verse_number = 0; // Initial verse number.
    $lines = explode ("\n", $usfm);
    for ($i = 0; $i < count ($lines); $i++) {
      if ($i <= $line_number) {
        $verse_numbers = Filter_Usfm::getVerseNumbers ($lines[$i]);
        if (count ($verse_numbers) >= 2) {
          $verse_number = $verse_numbers[1];
        }
      }
    }
    return $verse_number;
  }

  /**
  * Returns the verse text given a $verse_number and $usfm code.
  */
  public static function getVerseText ($usfm, $verse_number)
  {
    // The start of the requested verse number.
    $cleanPos = strpos ($usfm, "\\v $verse_number ");
    $dirtyPos = strpos ($usfm, "\\v $verse_number");
    if ($verse_number == 0) {
      $startPosition = 0;
    } else if ($cleanPos !== false) {
      $startPosition = $cleanPos;
    } else if ($dirtyPos !== false) {
      $startPosition = $dirtyPos;
    } else {
      // The verse number was not found.
      return "";
    }

    // The end of the requested verse number.
    $endPosition = strpos ($usfm, "\\v", $startPosition + 1);
    if ($endPosition === false) $endPosition = strlen ($usfm);
    
    // Return the verse text.
    $verseText = substr ($usfm, $startPosition, $endPosition - $startPosition);
    $verseText = trim ($verseText);
    return $verseText;
  }


  /**
  * Returns true if the $code contains an USFM marker.
  */
  public static function isUsfmMarker ($code)
  {
    if (strlen ($code) < 2) return false;
    if (substr ($code, 0, 1) == "\\") return true;
    return false;
  }


  /**
  * Returns true if the marker in $usfm is an opening marker. 
  * Else it returns false.
  */
  public static function isOpeningMarker ($usfm)
  {
    return (strpos ($usfm, "*") === false);
  }
  

  /**
  * Returns the USFM book identifier. 
  * $usfm: array of strings alternating between USFM code and subsequent text.
  * $pointer: should point to the \id in $usfm. Gets increased by one.
  */
  public static function getBookIdentifier ($usfm, $pointer)
  {
    $identifier = "XXX"; // Fallback value.
    if (++$pointer < count ($usfm)) {
      $identifier = substr ($usfm[$pointer], 0, 3);
    }
    return $identifier;
  }


  /**
  * Returns the text that follows a USFM marker.
  * $usfm: array of strings alternating between USFM code and subsequent text.
  * $pointer: should point to the marker in $usfm. Pointer is left as it is.
  */
  public static function peekTextFollowingMarker ($usfm, $pointer)
  {
    return Filter_Usfm::getTextFollowingMarker ($usfm, $pointer);
  }


  /**
  * Returns the text that follows a USFM marker.
  * $usfm: array of strings alternating between USFM code and subsequent text.
  * $pointer: should point to the marker in $usfm. Is increased by one.
  */
  public static function getTextFollowingMarker ($usfm, &$pointer)
  {
    $text = ""; // Fallback value.
    if (++$pointer < count ($usfm)) {
      $text = $usfm[$pointer];
    }
    return $text;
  }


  /**
  * Returns the verse number in the $usfm code.
  */
  public static function peekVerseNumber ($usfm)
  {
    // Make it robust, even handling cases like \v 1-2“Moi - No space after verse number.
    $verseNumber = "";
    $usfmStringLength = strlen ($usfm);
    for ($i = 0; $i < $usfmStringLength; $i++) {
      $character = substr ($usfm, $i, 1);
      switch ($character) {
        case "0":
        case "1":
        case "2":
        case "3":
        case "4":
        case "5":
        case "6":
        case "7":
        case "8":
        case "9":
        case ",":
        case "-":
        case "a":
        case "b":
          break;
        default:
          break 2;
      }
    }
    $verseNumber = substr ($usfm, 0, $i);
    $verseNumber = trim ($verseNumber);
    return $verseNumber;
  }

}

?>
