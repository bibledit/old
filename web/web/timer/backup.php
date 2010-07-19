<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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
$database_logs->log ("Backup has started");

// Security: Page only runs from the cli SAPI.
if (php_sapi_name () != "cli") {
  $database_logs->log ("Fatal: This only runs through the cli Server API", true);
  die;
}

// Database credentials.
require ("database/credentials.php");
  
$file = Filter_Backup::file ();
if ($file == "") {
  $database_logs->log ("Backups have not been enabled. It is recommended to backup the data, either through the Backup function, or through other means, and to store the data safely.", true);
  die;
}

$command = "mysqldump -h $database_host --opt -c -e -Q -u$database_user -p$database_pass $database_name | gzip > $file";
$database_logs->log ("Backup command: $command");
exec ($command, $output, $exitcode);
foreach ($output as $line)
$database_logs->log ($line);
$database_logs->log ("Backup exit code: $exitcode");

$database_logs->log ("Backup has been created");

?>
