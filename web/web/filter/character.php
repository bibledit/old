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


class Filter_Character
{

  /**
  * Returns a soft hyphen.
  */
  public static function softHyphen ()
  {
    // The "­" below is not an empty string, but the soft hyphen U+00AD.
    return "­";
  }


  /**
  * Returns a no-break space (NBSP).
  */
  public static function noBreakSpace ()
  {
    // The space below is a no-break space.
    return " ";
  }


  /**
  * Returns an "en space", this is a nut, half an em space.
  */
  public static function enSpace ()
  {
    // The space below is U+2002.
    return " ";
  }


  /**
  * Returns an "em space", this is a mutton, nominally a space equal to the height of the font.
  */
  public static function emSpace ()
  {
    // The space below is U+2003.
    return " ";
  }


}

?>
