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


class Filter_Hyphenate
{


  /**
  * This filter inserts soft hyphens in $text.
  * It goes through $text character by character.
  * At the transition from any character in $firstset
  * to any character in $secondset, it inserts a soft hyphen.
  * $firstset: array of characters.
  * $secondset: array of characters.
  * $text: A string of text to operate on.
  * Returns: The hyphenated $text.
  */
  public static function atTransition ($firstset, $secondset, $text)
  {
    // Verify the input.
    if (!is_array ($firstset)) return $text;
    if (count ($firstset) == 0) return $text;
    if (!is_array ($secondset)) return $text;
    if (count ($secondset) == 0) return $text;
    if (!is_string ($text)) return "";

    // Split the text up into lines and go through each one.
    $lines = explode ("\n", $text);
    foreach ($lines as &$line) {

      // Split the line up into an array of UTF8 Unicode characters.
      $characters = array();
      if ($line != "") {
        preg_match_all('/./u', $line, $characters);
        $characters = $characters[0];
      }

      // Processor flags.
      $previousCharacterWasRelevant = false;
      $thisCharacterIsRelevant = false;
      $isUsfm = false;

      // Process each character.
      foreach ($characters as $key => &$character) {

        // Skip USFM marker.
        if ($character == "\\") $isUsfm = true;

        if (!$isUsfm) {

          // Check whether to insert the soft hyphen here.
          $thisCharacterIsRelevant = in_array ($character, $secondset);
          if (($thisCharacterIsRelevant) && ($previousCharacterWasRelevant)) {
            if (!Filter_Hyphenate::nearWhiteSpace ($characters, $key)) {
              $character = Filter_Character::softHyphen () . $character;
            }
          }

          // Flag for next iteration.
          $previousCharacterWasRelevant = in_array ($character, $firstset);
        }

        if ($isUsfm) {
          // Look for the end of the USFM marker.
          if ($character == " ") $isUsfm = false;
          if ($character == "*") $isUsfm = false;
        }
      }

      // Re-assemble the line from the separate (updated) characters.
      $line = implode ("", $characters);

    }

    // Assemble the hyphenated text from the separate lines.
    $text = implode ("\n", $lines);
    return $text;
  }


  /**
  * This filter looks whether the $offset is near whitespace
  * in the array of $characters.
  * Returns: true or false.
  */
  private static function nearWhiteSpace ($characters, $offset)
  {
    $start = $offset - 2; // The constant for the nearness to the start of the word.
    $end = $offset + 2; // The constant for the nearness to the end of the word.
    if ($start < 0) $start = 0;
    if ($end > count ($characters)) $end = count ($characters);
    for ($i = $start; $i < $end; $i++) {
      if (ctype_space ($characters[$i])) return true;
    }
    return false;
  }


// Another method for hyphenation could be to rely on the routines as used by TeX.
// TeX has hyphenation patters. Perhaps these can be read or somehow used.
// The 'soul' package can show possible word breaks.


}

?>
