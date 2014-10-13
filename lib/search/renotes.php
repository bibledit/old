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
$database_notes = Database_Notes::getInstance ();


// Set a flag indicating that the notes databases are not available to clients and other parties.
$database_notes->set_availability (false);


// Check on health of the databases, and optionally recreate them.
$recreate = $database_notes->checkup ();
if ($recreate) {
  $database_logs->log ("The Consultation Notes main index was damaged is being recreated" , Filter_Roles::MANAGER_LEVEL);
}
$recreate = $database_notes->checkup_checksums ();
if ($recreate) {
  $database_logs->log ("The Consultation Notes checksums database was damaged is being recreated" , Filter_Roles::MANAGER_LEVEL);
}


// Synchronize the notes databases with the notes on the filesystem,
// because it may have happened that certain values at certain times 
// could not be written to the database because of errors. 
// Then the notes index would not match the notes data.
// Syncing ensures the index matches the notes data in the filesystem.
$database_logs->log ("Updating Consultation Notes databases", Filter_Roles::MANAGER_LEVEL);
$database_notes->sync ();


// Set the availability flag so that clients and other parties access the notes databases again.
$database_notes->set_availability (true);


$database_logs->log ("Updating Consultation Notes databases ready", Filter_Roles::MANAGER_LEVEL);


?>
