<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

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


require_once ("bootstrap/bootstrap");


class Tasks_Logic
{


  const PHP = "php";
  
  
  // If $command is queued as a task, the function will return the name of the file that contains the task.
  // Else it returns null.
  // The $command may also be part of a queued task.
  static public function queued ($command)
  {
    $command = filter_string_trim ($command);
    $files = scandir (dirname (__DIR__) . "/processes");
    $files = Filter_Folders::cleanup ($files);
    for ($files as $file) {
      @$contents = filter_url_file_get_contents (dirname (__DIR__) . "/processes/$file");
      $contents = filter_string_trim ($contents);
      if (strpos ($contents, $command) !== false) {
        return $file;
      }
    }
    return null;
  }
  
  
}


?>
