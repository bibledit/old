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


class Filter_Java
{


  /**
  * This filter compiles a Java program.
  * $program: An array holding the code to be compiled.
  * $classpath: An array holding class paths.
  * Returns: path to the folder containing the compiled classes.
  */
  public static function compile ($program, $classpath)
  {
    // Output folder where the data goes.
    $outputFolder = tempnam (sys_get_temp_dir(), '');
    unlink ($outputFolder);
    mkdir ($outputFolder);

    // Store the $program to disk.
    $program = implode ("\n", $program);
    file_put_contents ("$outputFolder/program.java", $program);

    // Assemble the class path.
    $cp = ".";
    if (is_array ($classpath)) {
      foreach ($classpath as $path) {
        $cp = "$cp:$path";
      }
    }

    $database_logs = Database_Logs::getInstance ();

    // Compile the program.
    $workingdirectory = escapeshellarg ($outputFolder);
    $cp = escapeshellarg ($cp);
    $javafile = escapeshellarg ("$outputFolder/program.java");
    $command = "cd $workingdirectory && javac -cp $cp $javafile 2>&1";
    $database_logs->log ($command);
    exec ($command, $output, $return_var);
    foreach ($output as $line) {
      $database_logs->log ($line);
    }

    // Folder of compiled program.
    return $outputFolder;
  }


  /**
  * This filter runs a Java class.
  * $directory: Directory that holds the program that has the class.
  * $classpath: An array holding class paths.
  * $runclass: The name of the class to run.
  * It returns the exid code of the program.
  */
  public static function run ($directory, $classpath, $runclass)
  {
    // Assemble the class path.
    $cp = ".";
    if (is_array ($classpath)) {
      foreach ($classpath as $path) {
        $cp = "$cp:$path";
      }
    }

    $database_logs = Database_Logs::getInstance ();

    // Run the class.
    $workingdirectory = escapeshellarg ($directory);
    $cp = escapeshellarg ($cp);
    $runclass = escapeshellarg ($runclass);
    $command = "cd $workingdirectory && java -cp $cp $runclass 2>&1";
    $database_logs->log ($command);
    exec ($command, $output, $return_var);
    foreach ($output as $line) {
      $database_logs->log ($line);
    }
    return $return_var;
  }


}

?>
