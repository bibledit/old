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


class Filter_Markup
{

  // This function marks the array of $words in the string $string.
  // It uses the <mark> markup for display as html.
  public static function words ($words, $string)
  {
    // Array of needles to look for.
    // The needles contain the search $words as they occur in the $string
    // in upper case or lower case, or any mixed case.
    $needles = array ();
    foreach ($words as $word) {
      if ($word == "") continue;
      $needles = array_merge ($needles, Filter_Search::needles ($word, $string));
    }
    $needles = array_unique ($needles);

    // All the $needles are converted to $markup,
    // which will replace the $needles.
    $markup = array ();
    foreach ($needles as $needle) {
      $markup [] = '<mark>' . $needle . '</mark>';
    }

    // Do the replacement.
    $string = str_replace ($needles, $markup, $string);

    // Result.
    return $string;
  }


}

?>
