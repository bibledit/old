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


/*

This object creates a SWORD module.
Information about that is at:
http://www.crosswire.org/sword/develop/swordmodule/

Basic steps:
- Create a .conf file and put it in folder ./mods.d/
- The input format is OSIS as this offers most options and seems best supported.
- Create the module with program osis2mod.
- Store the text in ./modules/texts/bibledit-web/$bible/

*/


class Sword_Text
{

  private $osisFolder;
  private $swordFolder;
  private $bibleName;
  
  
  public function __construct ($osisFolder, $swordFolder, $bibleName)
  {
    $this->osisFolder = $osisFolder;
    $this->swordFolder = $swordFolder;
    $this->bibleName = $bibleName;
  }
  
  
  public function run ($osisFilename)
  {
    // Configuration folder.
    $configurationFolder = $this->swordFolder . "/mods.d";
    mkdir ($configurationFolder);

    // Write configuration.
    $contents = Sword_Text::getConfiguration ();
    $contents = str_replace ("<bible>", $this->bibleName, $contents);
    $contents = str_replace ("<date>", date ("Y.m.d"), $contents);
    // For right-to-left scripts, add this: Direction=RtoL
    // Bibledit-Web does not currently have a setting for that.
    file_put_contents ($configurationFolder . "/" . $this->bibleName . ".conf", $contents);
    unset ($contents);

    // Text folder.
    $textFolder = $this->swordFolder . "/modules/texts/bibledit-web/" . $this->bibleName;
    mkdir ($textFolder, 0777, true);

    // Convert OSIS to Sword through the Sword utility.
    $database_logs = Database_Logs::getInstance ();
    $command = "osis2mod ";
    $command .= escapeshellarg ($textFolder);
    $command .= " ";
    $command .= escapeshellarg ($this->osisFolder . "/$osisFilename");
    $command .= " 2>&1";
    $database_logs->log ("exports: $command");
    exec ($command, $output, $exit_code);
    $output = array_unique ($output);
    foreach ($output as $line) {
      $database_logs->log ($line);
    }
    
    // Save the module as a zip file.
    $zipfile = Filter_Archive::zip ($this->swordFolder);
    copy ($zipfile, $this->swordFolder . "/" . $this->bibleName . ".zip");
    unlink ($zipfile);
  }  


  static function getConfiguration ()
  {
    $database_config_general = Database_Config_General::getInstance ();
    $data = $database_config_general->getSwordConfiguration();
    if (empty ($data)) {
      $data = file_get_contents (dirname (__FILE__) . "/module.conf");
    } else {
      $data = implode ("\n", $data);
    }
    return $data;
  }
  
  
  static function saveConfiguration ($data)
  {
    $database_config_general = Database_Config_General::getInstance ();
    $data = trim ($data);
    $default = file_get_contents (dirname (__FILE__) . "/module.conf");
    $default = trim ($default);
    if ($data == $default) {
    } else {
      $database_config_general->setSwordConfiguration (array ());
    }
    $data = explode ("\n", $data);
    $database_config_general->setSwordConfiguration ($data);
  }
  
  
}


?>
