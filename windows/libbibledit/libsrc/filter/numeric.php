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


class Filter_Numeric
{

  /**
  * Looks for and returns a positive integer in a string.
  * If no integer exists, it returns an empty string.
  */
  public static function integer_in_string ($string)
  {
    // Get the string where the integer starts, if there is any.
    $string = strpbrk ($string, "0123456789");
    if ($string === false)
      return "";
    // Look for the length of the segment containing an integer.
    $length = strspn ($string, "0123456789");
    if ($length == 0)
      return "";
    // Return the integer.
    $string = substr ($string, 0, $length);
    return $string;
  }


  /**
  * Looks for and returns a float in a string.
  * If no float exists, it returns an empty string.
  */
  public static function float_in_string ($string)
  {
    // Get the string where the float starts, if there is any.
    $string = strpbrk ($string, "0123456789.-");
    if ($string === false)
      return "";
    // Look for the length of the segment containing the float.
    $length = strspn ($string, "0123456789.-");
    if ($length == 0)
      return "";
    // Return the float.
    $string = substr ($string, 0, $length);
    return $string;
  }


  // This function takes an ordinal $num, and adds a suffix like
  // 1st, 2nd, 3rd, 4th, 5th, 6th, and so on.
  public static function add_ordinal_number_suffix ($num)
  {
    if (!in_array (($num % 100), array (11,12,13))) {
      switch ($num % 10) {
        // Handle 1st, 2nd, 3rd
        case 1: return $num . 'st';
        case 2: return $num . 'nd';
        case 3: return $num . 'rd';
      }
    }
    return $num . 'th';  }
}



?>
