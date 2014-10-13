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


class Checks_Verses
{


  public static function missingPunctuationAtEnd ($bible, $book, $chapter, $verses, $centermarks, $endmarks)
  {
    if (!is_array ($verses)) return;
    $centermarks = explode (" ", $centermarks);
    $endmarks = explode (" ", $endmarks);
    $database_check = Database_Check::getInstance ();
    foreach ($verses as $verse => $text) {
      if ($verse == 0) continue;
      $lastCharacter = substr ($text, -1, 1);
      if (in_array ($lastCharacter, $centermarks)) continue;
      if (in_array ($lastCharacter, $endmarks)) continue;
      $database_check->recordOutput ($bible, $book, $chapter, $verse, "No punctuation at end of verse:" . " " . $lastCharacter);
    }
  }


  public static function patterns ($bible, $book, $chapter, $verses, $patterns)
  {
    if (!is_array ($verses)) return;
    if (!is_array ($patterns)) return;
    $database_check = Database_Check::getInstance ();
    foreach ($verses as $verse => $text) {
      foreach ($patterns as $pattern) {
        if (strpos ($text, $pattern) !== false) {
          $database_check->recordOutput ($bible, $book, $chapter, $verse, "Pattern found in text:" . " " . $pattern);
        }
      }
    }
  }


}


?>
