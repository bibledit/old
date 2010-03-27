<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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
  public function oneString ($usfm)
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
  * Output would be:     array ("\id", "GEN", "\c", "10", ...)
  * If $usfm does not start with a marker, this becomes visible in the output too.
  */
  public function getMarkersAndText ($usfm) // Todo
  {
    $markers_and_text = array ();
    mb_internal_encoding("UTF-8");
    $usfm = str_replace ("  ", " ", $usfm);
    $usfm = trim ($usfm);
    while ($usfm != "") {
      if ($usfm != "") {
        $pos = mb_strpos ($usfm, "\\");
        if ($pos === 0) {
          // Marker found. 
          // The marker ends
          // - after the first space, or
          // - after the first asterisk (*), or
          // - at the first backslash (\), or
          // - at the end of the string,
          // whichever comes first.
          $positions = array ();
          $pos = mb_strpos ($usfm, " ");
          if ($pos !== false) $positions [] = $pos + 1;
          $pos = mb_strpos ($usfm, "*");
          if ($pos !== false) $positions [] = $pos + 1;
          $pos = mb_strpos ($usfm, "\\", 1);
          if ($pos !== false) $positions [] = $pos;
          $positions [] = mb_strlen ($usfm);
          sort ($positions, SORT_NUMERIC);
          $pos = $positions[0];
          $marker = mb_substr ($usfm, 0, $pos);
          $markers_and_text [] = $marker;
          $usfm = mb_substr ($usfm, $pos);
        } else {
          // Text found. It ends at the next backslash or at the end of the string.
          $pos = mb_strpos ($usfm, "\\");
          if ($pos === false) $pos = strlen ($usfm);
          $text = mb_substr ($usfm, 0, $pos);
          $markers_and_text [] = $text;
          $usfm = mb_substr ($usfm, $pos);
        }
      }
    }
    return $markers_and_text;
  }
    

  /**
  * Gets the marker from $text if it is there, else returns an empty string.
  * Examples:
  * "\id"   -> "id"
  * "\id "  -> "id"
  * "\add*" -> "add"
  */
  public function getMarker ($text) // Todo
  {
    mb_internal_encoding("UTF-8");
    $pos = mb_strpos ($text, "\\");
    if ($pos === 0) {
      // Marker found. 
      // Erase backslash.
      $text = mb_substr ($text, 1);
      // The marker ends
      // - at the first space, or
      // - at the first asterisk (*), or
      // - at the first backslash (\), or
      // - at the end of the string,
      // whichever comes first.
      $positions = array ();
      $pos = mb_strpos ($usfm, " ");
      if ($pos !== false) $positions [] = $pos;
      $pos = mb_strpos ($usfm, "*");
      if ($pos !== false) $positions [] = $pos;
      $pos = mb_strpos ($usfm, "\\", 1);
      if ($pos !== false) $positions [] = $pos;
      $positions [] = mb_strlen ($usfm);
      sort ($positions, SORT_NUMERIC);
      $pos = $positions[0];
      $marker = mb_substr ($usfm, 0, $pos);
      return $marker;
    } else {
      // Text found. No marker.
      return "";
    }
  }

}

?>
