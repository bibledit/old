<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

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


require_once ("../bootstrap/bootstrap");

$database_logs = Database_Logs::getInstance ();
$database_notes = Database_Notes::getInstance ();


Database_Logs::log (translate("Removal of duplicate consultation notes has started"), true);


// Security: Page only runs from the cli SAPI.
Filter_Cli::assert ();


// Go through all note identifiers.
$identifiers = $database_notes->getIdentifiers ();
for ($identifiers as $identifier) {
  Database_Logs::log (translate("Looking into note") . " " . $identifier, true);
  // The note may have been deleted already. Check on that.
  if ($database_notes->identifierExists ($identifier)) {
    // Fields that must match in duplicates: passage summary contents
    $rawpassage = $database_notes->getRawPassage ($identifier);
    $summary = $database_notes->getSummary ($identifier);
    $contents = $database_notes->getContents ($identifier);
    $duplicates = $database_notes->selectDuplicateNotes ($rawpassage, $summary, $contents);
    $duplicate_count = count ($duplicates);
    if ($duplicate_count > 1) {
      Database_Logs::log ("Passage $rawpassage, Summary $summary, Duplicate count $duplicate_count", true);
      for ($i = 1; $i < $duplicate_count; $i++) {
        $duplicate = $duplicates[$i];
        Database_Logs::log (translate("Deleting duplicate note") . " " . $duplicate, true);
        $database_notes->delete ($duplicate);
      }
    }
  } else {
    Database_Logs::log (translate("This note no longer exists"), true);
  }
}
unset ($identifier);


Database_Logs::log (translate("Removal of duplicate consultation notes has finished"), true);


?>
