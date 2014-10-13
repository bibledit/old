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


class Filter_File
{


  // The function accepts a $path.
  // The function may add a numerical suffix 
  // to ensure that the $path does not yet exist in the filesystem.
  public static function unique ($path) 
  {
    if (!file_exists ($path)) return $path;
    for ($i = 1; $i < 100; $i++) {
      $uniquepath = $path . "." . $i;
      if (!file_exists ($uniquepath)) return $uniquepath;
    }
    return $path . "." . mt_rand (100, 1000);
  }


}

?>
