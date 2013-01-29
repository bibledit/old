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


// Update the Sphinx configuration file.
$config_general = Database_Config_General::getInstance ();
$sphinxPort = $config_general->getSearchDaemonPort ();
$sphinxPort = (int) $sphinxPort;
include ("paths/paths.php");
$sphinxPidFilename = sys_get_temp_dir () . "/" . $location . "-sphinx.pid";
$configurationFilename = "../search/sphinx.conf";
$sphinxConfiguration = file ($configurationFilename, FILE_IGNORE_NEW_LINES);
foreach ($sphinxConfiguration as &$line) {
  if (strpos ($line, "path =") !== false) {
    $line = "  path = $localStatePath/$location/sphinxsearch";
  }
  if (strpos ($line, "log =") !== false) {
    $line = "  log = " . sys_get_temp_dir () . "/" . $location . "-sphinx.log";
  }
  if (strpos ($line, "pid_file =") !== false) {
    $line = "  pid_file = $sphinxPidFilename";
  }
  $line = str_replace ('9312', $sphinxPort, $line);
}
$sphinxConfiguration = implode ("\n", $sphinxConfiguration);
file_put_contents ($configurationFilename, $sphinxConfiguration);
unset ($configurationFilename);
unset ($sphinxConfiguration);


// Run the Sphinx indexer.
$success = true;
$command = "cd ../search; indexer --rotate --all --config sphinx.conf 2>&1";
$database_logs->log ("search: $command");
unset ($result);
exec ($command, $result, $exit_code);
if ($exit_code != 0) $success = false;
foreach ($result as $line) {
  if ($line == "") continue;
  $database_logs->log ("search: $line");
}
$database_logs->log ("search: Exit code $exit_code");


// Start the Sphinx daemon if it does not yet run.
$startDaemon = true;
if (file_exists ($sphinxPidFilename)) {
  if (filesize ($sphinxPidFilename) > 1) {
    $startDaemon = false;
  }
}
if ($startDaemon) {
  $command = "cd ../search; searchd --config sphinx.conf 2>&1";
  $database_logs->log ("search: $command");
  unset ($result);
  exec ($command, $result, $exit_code);
  foreach ($result as $line) {
    if ($line == "") continue;
    $database_logs->log ("search: $line");
  }
  $database_logs->log ("search: Exit code $exit_code");
}


$database_logs->log ("search: Completed", true);


?>
