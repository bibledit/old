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


require_once ("../bootstrap/bootstrap.php");


// Security: The script runs from the cli SAPI only.
Filter_Cli::assert ();


$database_logs = Database_Logs::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();


$bibles = $database_bibles->getBibles ();


// Go through all sub directories of the exports directory.
// Remove subdirectories if their corresponding Bible no longer exists in the system.
$directory = Export_Logic::mainDirectory ();
foreach (new DirectoryIterator ($directory) as $fileInfo) {
  if ($fileInfo->isDot ()) continue;
  if ($fileInfo->isDir ()) {
    $bible = $fileInfo->getFilename ();
    if (in_array ($bible, $bibles)) continue;
    $path = $fileInfo->getPathname ();
    Filter_Rmdir::rmdir ($path);
    $database_logs->log ("Removing exported Bible $bible", Filter_Roles::ADMIN_LEVEL);
  }
}


// Schedule the relevant Bibles for export.
foreach ($bibles as $bible) {

  if ($database_config_bible->getExportWebDuringNight ($bible)) {
    Export_Logic::scheduleWeb ($bible);
    Export_Logic::scheduleWebIndex ($bible);
  }

  if ($database_config_bible->getExportHtmlDuringNight ($bible)) Export_Logic::scheduleHtml ($bible);

  if ($database_config_bible->getExportUsfmDuringNight ($bible)) Export_Logic::scheduleUsfm ($bible);

  if ($database_config_bible->getExportTextDuringNight ($bible)) Export_Logic::scheduleTextAndBasicUsfm ($bible);

  if ($database_config_bible->getExportOdtDuringNight ($bible)) Export_Logic::scheduleOpenDocument ($bible);

  if ($database_config_bible->getGenerateInfoDuringNight ($bible)) Export_Logic::scheduleInfo ($bible);

  if ($database_config_bible->getExportESwordDuringNight ($bible)) Export_Logic::scheduleESword ($bible);

  if ($database_config_bible->getExportOnlineBibleDuringNight ($bible)) Export_Logic::scheduleOnlineBible ($bible);
  
}


?>
