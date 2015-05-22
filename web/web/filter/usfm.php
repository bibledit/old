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


class Filter_Usfm
{


  // Returns the string $usfm as one long string.
  // $usfm may contain new lines, but the resulting long string won't.
  public static function oneString ($usfm)
  {
    $long_string = "";
    $usfm = explode ("\n", $usfm);
    foreach ($usfm as &$line) {
      $line = trim ($line);
      // Skip empty line.
      if ($line != "") {
        // The line will be appended to the output line.
        // If it does not start with a backslash (\), a space is inserted first.
        $pos = strpos ($line, "\\");
        if ($pos === false) {
          if ($long_string != "") $long_string .= " ";
        }
        $long_string .= $line;
      }
    }
    return $long_string;
  }


  // Returns the string $code as an array alternating between marker and text.
  // Example, input is:   \id GEN
  //                      \c 10
  //             ...
  // Output would be:     array ("\id ", "GEN", "\c ", "10", ...)
  // If $code does not start with a marker, this becomes visible in the output too.
  public static function getMarkersAndText ($code)
  {
    $markers_and_text = array ();
    $code = str_replace ("\n\\", "\\", $code); // New line followed by backslash: leave new line out.
    $code = str_replace ("\n", " ", $code); // New line only: change to space, according to the USFM specification.
    // No removal of double spaces, because it would remove an opening marker (which already has its own space), followed by a space.
    $code = trim ($code);
    while ($code != "") {
      if ($code != "") {
        $pos = strpos ($code, "\\");
        if ($pos === 0) {
          // Marker found.
          // The marker ends
          // - after the first space, or
          // - after the first asterisk (*), or
          // - at the first backslash (\), or
          // - at the end of the string,
          // whichever comes first.
          $positions = array ();
          $pos = strpos ($code, " ");
          if ($pos !== false) $positions [] = $pos + 1;
          $pos = strpos ($code, "*");
          if ($pos !== false) $positions [] = $pos + 1;
          $pos = strpos ($code, "\\", 1);
          if ($pos !== false) $positions [] = $pos;
          $positions [] = strlen ($code);
          sort ($positions, SORT_NUMERIC);
          $pos = $positions[0];
          $marker = substr ($code, 0, $pos);
          $markers_and_text [] = $marker;
          $code = substr ($code, $pos);
        } else {
          // Text found. It ends at the next backslash or at the end of the string.
          $pos = strpos ($code, "\\");
          if ($pos === false) $pos = strlen ($code);
          $text = substr ($code, 0, $pos);
          $markers_and_text [] = $text;
          $code = substr ($code, $pos);
        }
      }
    }
    return $markers_and_text;
  }


  // Gets the marker from $usfm if it is there, else returns an empty string.
  // Examples:
  // "\id"    -> "id"
  // "\id "   -> "id"
  // "\add*"  -> "add"
  // "\+add*" -> "add"
  public static function getMarker ($usfm)
  {
    if ($usfm == "")
      return $usfm;
    $pos = strpos ($usfm, "\\");
    if ($pos === 0) {
      // Marker found.
      // Erase backslash.
      $usfm = substr ($usfm, 1);
      // Optionally erase the + embedded marker.
      $pos = strpos ($usfm, "+");
      if ($pos === 0) {
        $usfm = substr ($usfm, 1);
      }
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


  // This imports USFM $input.
  // It takes raw $input,
  // and returns an array of an array [book_number, chapter_number, chapter_data].
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


  // Returns an array with the verse numbers found in $usfm.
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
      if (substr ($marker_or_text, 0, 2) == '\v') {
        $extract_verse = true;
      }
    }
    return $verse_numbers;
  }


  // Returns the verse number in the string of $usfm code at line number $line_number.
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


  // Returns the verse number in the string of $usfm code at offset $offset.
  // Offset is calculated with mb_strlen to support UTF-8.
  public static function offset2verseNumber ($usfm, $offset)
  {
    $totalOffset = 0;
    $lines = explode ("\n", $usfm);
    foreach ($lines as $lineNumber => $line) {
      $totalOffset += mb_strlen ($line);
      if ($totalOffset >= $offset) {
        return Filter_Usfm::lineNumber2VerseNumber ($usfm, $lineNumber);
      }
      // Add 1 for new line.
      $totalOffset += 1;
    }
    return 0;
  }


  // Returns the offset within the $usfm code where $verse number starts.
  public static function verseNumber2offset ($usfm, $verse)
  {
    // Verse number 0 starts at offset 0.
    if ($verse == 0) return 0;
    $totalOffset = 0;
    $lines = explode ("\n", $usfm);
    foreach ($lines as $line) {
      $verses = Filter_Usfm::getVerseNumbers ($line);
      @$v = $verses [1];
      if ($v == $verse) {
        return $totalOffset;
      }
      $totalOffset += mb_strlen ($line);
      // Add 1 for new line.
      $totalOffset += 1;
    }
    return mb_strlen ($usfm);
  }


  // Returns the verse text given a $verse_number and $usfm code.
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
    @$endPosition = strpos ($usfm, "\\v", $startPosition + 1);
    if ($endPosition === false) $endPosition = strlen ($usfm);

    // Return the verse text.
    $verseText = substr ($usfm, $startPosition, $endPosition - $startPosition);
    $verseText = trim ($verseText);
    return $verseText;
  }


  // Returns true if the $code contains an USFM marker.
  public static function isUsfmMarker ($code)
  {
    if (strlen ($code) < 2) return false;
    if (substr ($code, 0, 1) == "\\") return true;
    return false;
  }


  // Returns true if the marker in $usfm is an opening marker.
  // Else it returns false.
  public static function isOpeningMarker ($usfm)
  {
    return (strpos ($usfm, "*") === false);
  }


  // Returns true if the marker in $usfm is an embedded marker.
  // Else it returns false.
  public static function isEmbeddedMarker ($usfm)
  {
    return (strpos ($usfm, "+") !== false);
  }


  // Returns the USFM book identifier.
  // $usfm: array of strings alternating between USFM code and subsequent text.
  // $pointer: should point to the \id in $usfm. Gets increased by one.
  public static function getBookIdentifier ($usfm, $pointer)
  {
    $identifier = "XXX"; // Fallback value.
    if (++$pointer < count ($usfm)) {
      $identifier = substr ($usfm[$pointer], 0, 3);
    }
    return $identifier;
  }


  // Returns the text that follows a USFM marker.
  // $usfm: array of strings alternating between USFM code and subsequent text.
  // $pointer: should point to the marker in $usfm. Pointer is left as it is.
  public static function peekTextFollowingMarker ($usfm, $pointer)
  {
    return Filter_Usfm::getTextFollowingMarker ($usfm, $pointer);
  }


  // Returns the text that follows a USFM marker.
  // $usfm: array of strings alternating between USFM code and subsequent text.
  // $pointer: should point to the marker in $usfm. Is increased by one.
  public static function getTextFollowingMarker ($usfm, &$pointer)
  {
    $text = ""; // Fallback value.
    if (++$pointer < count ($usfm)) {
      $text = $usfm[$pointer];
    }
    return $text;
  }


  // Returns the verse number in the $usfm code.
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


  // Takes a marker in the form of text only, like "id" or "add",
  // and converts it into opening USFM, like "\id " or "\add ".
  // Supports the embedded markup "+".
  public static function getOpeningUsfm ($text, $embedded = false)
  {
    $embedded = $embedded ? "+" : "";
    return "\\$embedded$text ";
  }


  // Takes a marker in the form of text only, like "add",
  // and converts it into closing USFM, like "\add*".
  // Supports the embedded markup "+".
  public static function getClosingUsfm ($text, $embedded = false)
  {
    $embedded = $embedded ? "+" : "";
    return "\\$embedded$text*";
  }


  // This function extracts notes, e.g. cross references.
  // $usfm: Where to extract from.
  // $markers: Array of possible markers surrounding the notes.
  // It returns an array of array ($offset, $note).
  public static function extractNotes ($usfm, $markers)
  {
    $openers = array ();
    $closers = array ();
    foreach ($markers as $marker) {
      $openers [] = self::getOpeningUsfm ($marker);
      $closers [] = self::getClosingUsfm ($marker);
    }

    $usfm = self::getMarkersAndText ($usfm);

    $notes = array ();
    
    $within_note = false;
    $running_offset = 0;
    $running_note = "";

    foreach ($usfm as $item) {
      
      if (in_array ($item, $openers)) {
        $within_note = true;
      }

      // Caclulate the offset in the main text. 
      // That means not to consider the length of the notes.
      if (!$within_note) $running_offset += mb_strlen ($item);
      
      if ($within_note) {
        $running_note .= $item;
      }
      
      if (in_array ($item, $closers)) {
        $notes [] = array ($running_offset, $running_note);
        $running_note = "";
        $within_note = false;
      }

    }
    
    return $notes;
  }
  
  
  // This function removes the notes from the USFM.
  // $usfm: Where to extract from.
  // $markers: Array of markers surrounding the notes.
  // It returns the USFM without the notes.
  public static function removeNotes ($usfm, $markers)
  {
    $notes = self::extractNotes ($usfm, $markers);
    foreach ($notes as $note) {
      $search = $note [1];
      $usfm = str_replace ($search, "", $usfm);
    }
    return $usfm;
  }


  // This function inserts notes into USFM.
  // It replaces $search with $replace in the $notes.
  // $usfm: string where to insert the notes - it is assumed not to contain any notes yet.
  // $notes: array of offsets => notes.
  // $ratio: ratio to multiply the offsets with.
  // It returns the updated USFM.
  public static function insertNotes ($usfm, $notes, $ratio)
  {
    if (!isset ($usfm)) return $usfm;
    if ($usfm == "") return $usfm;
    if (!isset ($notes)) return $usfm;
    if (empty ($notes)) return $usfm;
    
    // Temporally extract existing notes.
    $existing = self::extractNotes ($usfm, array ("x", "f", "fe"));

    // Work with USFM without notes so that a note will not be inserted in another note.
    $usfm = self::removeNotes ($usfm, array ("x", "f", "fe"));

    // Calculate insertion positions before actually inserting the notes to avoid nested notes placement.
    foreach ($notes as $key => $note) {
      $position = $note [0];
      $position = intval ($position * $ratio);
      $position = self::getNewNotePosition ($usfm, $position, 0);
      $notes [$key] [0] = $position;
    }

    // Add existing notes data.
    $notes = array_merge ($notes, $existing);

    // Sort the notes such that the last one gets inserted first.
    // This way inserting happens from the end of the USFM towards the start.
    // Inserted text does not affect any insertion positions this way.
    $highest_position = 0;
    foreach ($notes as $note) {
      $position = $note [0];
      if ($position > $highest_position) {
        $highest_position = $position;
      }
    }
    $notes2 = array ();
    for ($i = $highest_position; $i >= 0; $i--) {
      foreach ($notes as $note) {
        if ($note [0] == $i) {
          $notes2 [] = $note;
        }
      }
    }

    // Insert the notes into the USFM at the correct position.
    foreach ($notes2 as $note) {
      $position = $note [0];
      $text = $note [1];
      $usfm = substr_replace ($usfm, $text, $position, 0);
    }

    return $usfm;
  }
  
  
  // This function moves a note one word back or forth in the USFM data.
  // $usfm: The USFM data.
  // $direction: The direction into which to move the note.
  // $number: The note number. Starts at 1.
  // Returns the updated USFM.
  public static function moveNote ($usfm, $direction, $number)
  {
    $notes = self::extractNotes ($usfm, array ("x", "f", "fe"));
    $key = $number - 1;
    if (!isset ($notes [$key])) return $usfm;
    $usfm = self::removeNotes ($usfm, array ("x", "f", "fe"));
    $position = $notes [$key] [0];
    $position += ($direction * 3);
    $position = self::getNewNotePosition ($usfm, $position, $direction);
    $notes [$key] [0] = $position;
    $usfm = self::insertNotes ($usfm, $notes, 1);
    return $usfm;
  }
  
  
  // This function gets a new position to insert a note.
  // $usfm: The USFM code to work with.
  // $position: Current position.
  // $direction: Direction where to go to find the new position:
  //   -1: Go back to the previous slot.
  //    0: Attempt current position, else take next slot.
  //    1: Go forward to the next slot.
  public static function getNewNotePosition ($usfm, $position, $direction)
  {
    $words = explode (" ", $usfm);

    $length = 0;

    $lengths = array ();

    foreach ($words as $word) {

      // Add length of item.
      $length += mb_strlen ($word);
      
      // Check whether at opening marker.
      $opening_marker = self::isUsfmMarker ($word);
      if ($opening_marker) {
        $opening_marker = self::isOpeningMarker ($word);
      }

      // Don't create a slot for placing a note right after an opening marker.
      if (!$opening_marker) $lengths [] = $length;

      // Add length of space.
      $length++;
      
    }
    
    $found = false;
    
    if ($direction > 0) {
      // Take next position.
      foreach ($lengths as $key => $length) {
        if ($found) continue;
        if ($length > $position) {
          $position = $length;
          $found = true;
        }
      }
    } else if ($direction < 0) {
      // Take previous position.
      $lengths_r = array_reverse ($lengths);
      foreach ($lengths_r as $key => $length) {
        if ($found) continue;
        if ($length < $position) {
          $position = $length;
          $found = true;
        }
      }
    } else {
      // Take optimal position.
      foreach ($lengths as $key => $length) {
        if ($found) continue;
        if ($length >= $position) {
          $position = $length;
          $found = true;
        }
      }
    }
    
    if (!$found) {
      $position = mb_strlen ($usfm);
    }
    
    // Move a note to before punctuation.
    $punctuation = array (".", ",", ";", ":", "?", "!");
    $character = mb_substr ($usfm, $position - 1, 1);
    if (in_array ($character, $punctuation)) $position--;

    return $position;
  }


}


?>
