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


/**
* Class for creating Html text documents.
*/
class Osis_Text
{

  private $usfmFolder;
  private $osisFolder;
  private $bibleName;
  
  
  public function __construct ($usfmFolder, $osisFolder, $bibleName)
  {
    $this->usfmFolder = $usfmFolder;
    $this->osisFolder = $osisFolder;
    $this->bibleName = $bibleName;
  }
  
  
  public function run ($interpreter)
  {
    $scriptFolder = dirname (__FILE__);

    if ($interpreter == "py") {
      // At the time of writing this, the Perl script had a bug that it entered an infinite loop.
      // The timeout kills the program in case it runs too long.
      $command = "timeout 7200 python $scriptFolder/usfm2osis.py bible -r -x -s canonical -o ";
      $command .= " " . escapeshellarg ($this->osisFolder . "/00_Bible.xml") . " ";
      $command .= " " . $this->usfmFolder . "/* 2>&1";
    } else {
      $command = "perl $scriptFolder/usfm2osis.pl bible ";
      $command .= " -o " . escapeshellarg ($this->osisFolder . "/00_Bible.xml") . " ";
      $command .= " " . $this->usfmFolder . "/* 2>&1";
    }
    $database_logs = Database_Logs::getInstance ();
    $database_logs->log ("exports: $command");
    exec ($command, $output, $exit_code);
    foreach ($output as $line) {
      $database_logs->log ($line);
    }

  }  


}


?>
