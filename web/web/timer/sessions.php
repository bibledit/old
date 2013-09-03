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


$database_logs->log (gettext ("sessions: Clearing out sessions"), true);


// Security: The script runs from the cli SAPI only.
if (php_sapi_name () != "cli") {
  $database_logs->log ("sessions: Fatal: This only runs through the cli Server API", true);
  die;
}


$session_path = $argv [1];
$database_logs->log (gettext ("sessions: Path: $session_path"), true);


$time = time ();


$counter = 0;
foreach (new DirectoryIterator ($session_path) as $fileInfo) {
  if ($fileInfo->isDot ()) continue;
  if ($fileInfo->isDir ()) continue;
  $filename = $fileInfo->getFilename();
  if (strpos ($filename, "sess_") === false) continue;
  // Don't delete session files younger than the cookie timeout.
  $age = $time - $fileInfo->getMTime ();
  if ($age < 43200) continue;
  unlink ("$session_path/$filename");
  $counter++;
}


$database_logs->log (gettext ("sessions: Number of sessions cleared: $counter"), true);


?>
