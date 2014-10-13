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


class Tasks_Logic
{


  const PHP = "php";
  
  
  // Queue $binary to run through the shell, with $parameters.
  static public function queue ($binary, $parameters = array ())
  {
    $command = array ();
    $command [] = $binary;
    foreach ($parameters as $parameter) {
      $parameter = escapeshellarg ($parameter);
      $command [] = $parameter;
    }
    $command = implode (" ", $command);
    self::save ($command);
  }


  // Save task $command
  static public function save ($command)
  {
    // Store task to disk with a unique name.
    $microtime = microtime ();
    $microtime = explode (" ", $microtime);
    $microtime = $microtime [1] . $microtime [0];
    $path = dirname (__DIR__) . "/processes/$microtime";
    // On Windows the microtime is not fine enough which leads to one task overwriting another. 
    // Deal with that problem here.
    $path = Filter_File::unique ($path);
    $content = "$command";
    file_put_contents ($path, $content);
  }
  
  
  // If $command is queued as a task, the function will return the name of the file that contains the task.
  // Else it returns null.
  // The $command may also be part of a queued task.
  static public function queued ($command)
  {
    $command = trim ($command);
    $files = scandir (dirname (__DIR__) . "/processes");
    $files = Filter_Folders::cleanup ($files);
    foreach ($files as $file) {
      @$contents = file_get_contents (dirname (__DIR__) . "/processes/$file");
      $contents = trim ($contents);
      if (strpos ($contents, $command) !== false) {
        return $file;
      }
    }
    return null;
  }
  
  
}


?>
