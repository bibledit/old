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


require_once ("bootstrap/bootstrap.php");


class Fonts_Logic
{


  static public function getFonts ()
  {
    $fonts = scandir (__DIR__);
    $fonts = Filter_Folders::cleanup ($fonts);
    $fonts = array_diff ($fonts, array ("index.php", "logic.php", "scripts", "SIL-Licenses.txt", "SIL-README.txt"));
    return $fonts;
  }


  static public function fontExists ($font)
  {
    return file_exists (__DIR__ . "/" . $font);
  }


  static public function getFontPath ($font)
  {
    // Case of no font.
    if ($font == "") return "";

    // Case when the font is on the Bibledit-Web server.
    if (self::fontExists ($font)) {
      return "../fonts/$font";
    }

    // Case when the font is locally installed.
    if (basename ($font) == $font) {
      return $font;
    }

    // Font is on external server.
    return $font;
  }


}


?>
