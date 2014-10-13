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


// Processing thousands of notes may take time.
// Set the maximum execution time to indefinite.
set_time_limit (300);
ini_set ('max_execution_time', 300);


$database_users = Database_Users::getInstance (); 
$database_logs = Database_Logs::getInstance ();
$database_notes = Database_Notes::getInstance ();
$notes_logic = Notes_Logic::getInstance ();


$action = $_POST ['a'];


if ($action == "notes") {

  $user = Filter_Hex::hex2bin ($_POST ['u']);
  if (!$database_users->usernameExists ($user)) {
    $database_logs->log ("A client passes non existing user $user", Filter_Roles::MANAGER_LEVEL);
    die;
  }

  $md5 = $_POST ['p'];
  if ($md5 != $database_users->getmd5 ($user)) {
    $database_logs->log ("A client provides incorrect password for user $user", Filter_Roles::MANAGER_LEVEL); // Test it.
    die;
  }

  $bibles = Access_Bible::bibles ($user);

  $lowId = $_POST ['l'];
  $highId = $_POST ['h'];

  $identifiers = $database_notes->getNotesInRangeForBibles ($lowId, $highId, $bibles);
  $checksum = $database_notes->getMultipleChecksum ($identifiers);
  $response ['t'] = count ($identifiers);
  $response ['c'] = $checksum;
  die_if_databases_unhealthy_or_busy ();
  echo serialize ($response);

} else if ($action == "identifiers") {

  $user = Filter_Hex::hex2bin ($_POST ['u']);
  if (!$database_users->usernameExists ($user)) {
    die;
  }
  $md5 = $_POST ['p'];
  if ($md5 != $database_users->getmd5 ($user)) {
    die;
  }

  $bibles = Access_Bible::bibles ($user);

  $lowId = $_POST ['l'];
  $highId = $_POST ['h'];

  $identifiers = $database_notes->getNotesInRangeForBibles ($lowId, $highId, $bibles);
  $checksums = array ();
  foreach ($identifiers as $identifier) {
    $checksum = $database_notes->getChecksum ($identifier);
    $checksums [] = $checksum;
  }
  $response ['i'] = $identifiers;
  $response ['c'] = $checksums;
  die_if_databases_unhealthy_or_busy ();
  echo serialize ($response);

} else if ($action == "fetch") {

  $identifier = $_POST ['i'];

  // Update search and checksum.
  $database_notes->updateSearchFields ($identifier);
  $database_notes->updateChecksum ($identifier);

  $modified = $database_notes->getModified ($identifier);
  $assignees = $database_notes->getAssignees ($identifier);
  $subscribers = $database_notes->getSubscribers ($identifier);
  $bible = $database_notes->getBible ($identifier);
  $passages = $database_notes->getPassages ($identifier);
  $status = $database_notes->getRawStatus ($identifier);
  $severity = $database_notes->getRawSeverity ($identifier);
  $summary = $database_notes->getSummary ($identifier);
  $contents = $database_notes->getContents ($identifier);

  $database_logs->log ("Client requested a note from server" . ": " . $summary, Filter_Roles::MANAGER_LEVEL);

  $response = array (
    'm'  => $modified,
    'a'  => $assignees,
    'sub' => $subscribers,
    'b'  => $bible,
    'p'  => $passages,
    'st' => $status,
    'se' => $severity,
    'sum' => $summary,
    'c'  => $contents
  );

  $response = serialize ($response);
  die_if_databases_unhealthy_or_busy ();
  echo $response;
  
} else {

  echo "The server did not recognize the sync action";
  
}


function die_if_databases_unhealthy_or_busy ()
{
  $database_notes = Database_Notes::getInstance ();
  $available = true;
  if (!$database_notes->healthy ()) $available = false;
  if (!$database_notes->checksums_healthy ()) $available = false;
  if (!$database_notes->available ()) $available = false;
  if (!$available) {
    $database_logs = Database_Logs::getInstance ();
    $database_logs->log ("Notes databases are unhealthy or unavailable", Filter_Roles::TRANSLATOR_LEVEL);
    die;
  }
}


?>
