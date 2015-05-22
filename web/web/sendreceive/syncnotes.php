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


// The script is called with the first and last note identifier to deal with.
$lowId = Filter_Cli::argument (@$argv, 1);
$highId = Filter_Cli::argument (@$argv, 2);


$database_logs = Database_Logs::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_config_general = Database_Config_General::getInstance ();
$database_notes = Database_Notes::getInstance ();
$database_users = Database_Users::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$notes_logic = Notes_Logic::getInstance ();


// Check for the health of the notes databases and take action if needed.
$healthy = true;
if (!$database_notes->healthy ()) $healthy = false;
if (!$database_notes->checksums_healthy ()) $healthy = false;
if (!$healthy) {
  $database_logs->log ("Skipping sending and receiving notes just now because of database problems", Filter_Roles::TRANSLATOR_LEVEL);
  die;
}


$address = $database_config_general->getServerAddress ();
$url = "$address/sync/notes.php";


$main_script = ($lowId == Notes_Logic::lowNoteIdentifier) && ($highId == Notes_Logic::highNoteIdentifier);


// The client is supposed to have one user.
// Get this username, plus the md5 hash of the password, 
// in preparation for sending it to the server in in the script below.
// The server will use this user to find out the Bibles this user has access to,
// so the server can select the correct notes for this user.
// The client selects all available notes on the system.
$users = $database_users->getUsers ();
$user = $users [0];
$md5 = $database_users->getmd5 ($user);


// The script requests the total note count from the server, and their total checksum.
// It compares this with the local notes.
// If it matches, that means that the local notes match the notes on the server.
// The script is then ready.
$post = array (
  "a" => "notes",
  "u" => bin2hex ($user),
  "p" => $md5,
  "l" => $lowId,
  "h" => $highId
);
$response = Sync_Logic::post ($post, $url);
@$response = unserialize ($response);
if ($response === false) {
  $database_logs->log ("Failure synchronizing Consultation Notes while requesting totals", Filter_Roles::TRANSLATOR_LEVEL);
  die;
}
$server_total = $response ["t"];
$server_checksum = $response ["c"];
$identifiers = $database_notes->getNotesInRangeForBibles ($lowId, $highId, NULL);
$client_total = count ($identifiers);
$client_checksum = $database_notes->getMultipleChecksum ($identifiers);
if ($server_total == $client_total) {
  if ($server_checksum == $client_checksum) {
    if ($main_script) $database_logs->log ("The Consultation Notes are up to date", Filter_Roles::TRANSLATOR_LEVEL);
    die;
  }
}


// At this stage the total note count and/or their checksum on the client differs 
// from the total note count and/or their checksum on the server.
// This applies to a certain range of notes for certain Bibles.


// The script knows the total number of notes.
// It the total note count is too high, it divides the range of notes into smaller ranges,
// and then starts one script for each range.
if ($server_total > 20) {
  $ranges = Sync_Logic::create_range ($lowId, $highId);
  foreach ($ranges as $range) {
    $low = $range [0];
    $high = $range [1];
    Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/syncnotes.php", "$low", "$high"));
  }
  die;
}


// At this stage the total note count is small enough that the script can 
// start to deal with the individual notes.
// It requests the note identifiers, their checksums,
// and compares them with the local notes.
// If needed, it downloads the notes from the server.
// If needed, the client removes local notes no longer available on the server.


// Get note identifiers and checksums as on the server.
$post = array (
  "a" => "identifiers",
  "u" => bin2hex ($user),
  "p" => $md5,
  "l" => $lowId,
  "h" => $highId
);
$response = Sync_Logic::post ($post, $url);
@$response = unserialize ($response);
if ($response === false) {
  $database_logs->log ("Failure synchronizing Consultation Notes while identifiers and checksums", Filter_Roles::TRANSLATOR_LEVEL);
  die;
}
$server_identifiers = $response ['i'];
$server_checksums = $response ['c'];

// Get note identifiers as locally on the client.
$client_identifiers = $database_notes->getNotesInRangeForBibles ($lowId, $highId, NULL);

// Client deletes notes no longer on the server.
$identifiers = array_diff ($client_identifiers, $server_identifiers);
foreach ($identifiers as $identifier) {
  $summary = $database_notes->getSummary ($identifier);
  $database_notes->delete ($identifier);
  $database_logs->log ("Deleting note on client while syncing with server" . ": $summary", Filter_Roles::TRANSLATOR_LEVEL);
}

// Check whether the local notes on the client match the ones on the server.
// If needed download the note from the server.
foreach ($server_identifiers as $key => $identifier) {
  $server_checksum = $server_checksums [$key];
  $client_checksum = $database_notes->getChecksum ($identifier);
  if ($client_checksum != $server_checksum) {
    // If the note does not yet exist locally, create dummy note and set the identifier correct.
    if (!$database_notes->identifierExists ($identifier)) {
      $id = $database_notes->storeNewNote ("", 0, 0, 0, "dummy", "dummy", true);
      $database_notes->setIdentifier ($id, $identifier);
    }
    // Fetch the note from the server.
    $post = array (
      "a" => "fetch",
      "i" => $identifier,
    );
    $response = Sync_Logic::post ($post, $url);
    @$response = unserialize ($response);
    if ($response === false) {
      $database_logs->log ("Failure synchronizing Consultation Notes while downloading a note", Filter_Roles::TRANSLATOR_LEVEL);
      die;
    }

    $modified = $response ['m'];
    $assignees = $response ['a'];
    $subscribers = $response ['sub'];
    $bible = $response ['b'];
    $passages = $response ['p'];
    $status = $response ['st'];
    $severity = $response ['se'];
    $summary = $response ['sum'];
    $contents = $response ['c'];

    $database_notes->setAssignees ($identifier, $assignees);
    $database_notes->setSubscribers ($identifier, $subscribers);
    $database_notes->setBible ($identifier, $bible);
    $database_notes->setPassages ($identifier, $passages);
    $database_notes->setStatus ($identifier, $status);
    $database_notes->setRawSeverity ($identifier, $severity);
    $database_notes->setSummary ($identifier, $summary);
    $database_notes->setContents ($identifier, $contents);
    // Set modified field last as it is affected by the previous store actions.
    $database_notes->setModified ($identifier, $modified);

    // Update search and checksum.
    $database_notes->updateSearchFields ($identifier);
    $database_notes->updateChecksum ($identifier);

    $database_logs->log ("Note received from server" . ": " . $summary, Filter_Roles::MANAGER_LEVEL);

    $checksum = $database_notes->getChecksum ($identifier);

  }
}


?>
