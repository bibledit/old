<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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

class Resource_Logic
{

  public static function getExternal ($name, $book, $chapter, $verse)
  {
    $database_resources = Database_Resources::getInstance ();
    $details = $database_resources->getDetails ($name);
    $command = $details ['command'];
    $code = $details ['code'];
    $filename = tempnam (sys_get_temp_dir (), "");
    file_put_contents ($filename, $code);
    $command = str_replace ("script", "$filename $book $chapter $verse", $command) . " 2>&1";
    $output = shell_exec ($command);
    unlink ($filename);
    return $output;
  }

}

?>
