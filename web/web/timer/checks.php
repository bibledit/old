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


require_once ("../bootstrap/bootstrap.php");
$database_logs = Database_Logs::getInstance ();
$database_logs->log (gettext ("checks: Running checks on the Bibles"), true);


// Security: The script runs from the cli SAPI only.
if (php_sapi_name () != "cli") {
  $database_logs->log ("checks: Fatal: This only runs through the cli Server API", true);
  die;
}


$database_config_general = Database_Config_General::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_check = Database_Check::getInstance ();


$database_check->truncateOutput ();


$checkedBibles = $database_config_general->getCheckedBibles ();
// Go through the Bibles.
$bibles = $database_bibles->getBibles ();
foreach ($bibles as $bible) {
  // Skip Bibles that should not be checked.
  if (!in_array ($bible, $checkedBibles)) continue;
  // Run the bogus check.
  $checks_bogus = new Checks_Bogus ();
  $checks_bogus->run ($bible);
}


$database_logs->log (gettext ("checks: Completed"), true);


?>
