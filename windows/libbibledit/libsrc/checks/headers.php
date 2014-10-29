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


class Checks_Headers
{


  public static function noPunctuationAtEnd ($bible, $book, $chapter, $headings, $centermarks, $endmarks)
  {
    if (!is_array ($headings)) return;
    $database_check = Database_Check::getInstance ();
    foreach ($headings as $verse => $heading) {
      // Full stops often occur in the inspired headings of many Psalms in verse 0.
      // Skip these.
      if (($book == 19) && ($verse == 0)) continue;
      $lastCharacter = substr ($heading, -1, 1);
      $message = false;
      if (strpos ($centermarks, $lastCharacter) !== false) $message = true;
      if (strpos ($endmarks, $lastCharacter) !== false) $message = true;
      if ($message) {
        $database_check->recordOutput ($bible, $book, $chapter, $verse, "Punctuation at end of heading:" . " " . $heading);
      }
    }
  }


}


?>
