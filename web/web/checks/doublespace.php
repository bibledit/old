<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2013 Teus Benschop.
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


class Checks_Doublespace
{


  public function usfm ($bible, $book, $chapter, $verse, $data)
  {
    $database_check = Database_Check::getInstance ();
    $pos = strpos ($data, "  ");
    if ($pos !== false) {
      $start = $pos - 10;
      if ($start < 0) $start = 0;
      $fragment = substr ($data, $start, 20);
      $database_check->recordOutput ($bible, $book, $chapter, $verse, "Double space:" . " ... " . $fragment . " ...");
    }
  }

  
}


?>
