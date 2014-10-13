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


class Filter_String
{

  /**
  * Returns the reversed $text in UTF-8 format.
  */
  public static function reverse ($text)
  {
    preg_match_all ('/./us', $text, $ar);
    $text = join ('', array_reverse ($ar[0]));
    return $text;
  }


  /**
  * Returns the $string as an array cut on newlines.
  */
  public static function string2array ($string)
  {
    $string = str_replace ("\r", "", $string);
    $string = explode ("\n", $string);
    return $string;
  }


}

?>
