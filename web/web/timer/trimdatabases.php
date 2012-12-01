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
$database_logs->log ("trim: Trimming databases has started");


// Security: Page only runs from the cli SAPI.
if (php_sapi_name () != "cli") {
  $database_logs->log ("trim: Fatal: This only runs through the cli Server API", true);
  die;
}


$database_confirm = Database_Confirm::getInstance ();
$database_confirm->trim ();


$database_logs = Database_Logs::getInstance ();
$database_logs->trim ();


$database_mail = Database_Mail::getInstance ();
$database_mail->trim ();


$database_sessions = Database_Sessions::getInstance ();
$database_sessions->trim ();


$database_snapshots = Database_Snapshots::getInstance();
$database_snapshots->trim();


$database_logs->log ("trim: Completed");


?>