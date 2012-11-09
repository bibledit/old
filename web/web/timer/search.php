<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2012 Teus Benschop.
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


require_once ("../bootstrap/bootstrap.php");


$database_logs = Database_Logs::getInstance ();
$database_logs->log ("search: Starting to index website", true);


// Security: The script runs from the cli SAPI only.
if (php_sapi_name () != "cli") {
  $database_logs->log ("search: Fatal: This only runs through the cli Server API", true);
  die;
}


// Update the paths in the Sphinx configuration file.
include ("paths/paths.php");
$filename = "../search/sphinx.conf";
$sphinxConfiguration = file ($filename, FILE_IGNORE_NEW_LINES);
foreach ($sphinxConfiguration as &$line) {
  if (strpos ($line, "path =") !== false) {
    $line = "  path = $localStatePath/$location/sphinxsearch";
  }
  if (strpos ($line, "log =") !== false) {
    $line = "  log = " . sys_get_temp_dir () . "/" . $location . "-sphinx.log";
  }
  if (strpos ($line, "pid_file =") !== false) {
    $line = "  pid_file = " . sys_get_temp_dir () . "/" . $location . "-sphinx.pid";
  }
}
$sphinxConfiguration = implode ("\n", $sphinxConfiguration);
file_put_contents ($filename, $sphinxConfiguration);
unset ($filename);
unset ($sphinxConfiguration);


$database_logs->log ("search: Completed", true);


?>
