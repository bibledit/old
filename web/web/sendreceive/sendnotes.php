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
$database_config_bible = Database_Config_Bible::getInstance ();
$database_config_general = Database_Config_General::getInstance ();
$database_notes = Database_Notes::getInstance ();
$database_noteactions = Database_NoteActions::getInstance ();
$database_users = Database_Users::getInstance ();
$notes_logic = Notes_Logic::getInstance ();


$database_logs->log (Locale_Translate::_("Sending and receiving Consultation Notes"), Filter_Roles::TRANSLATOR_LEVEL);


$response = Filter_Client::setup ();
if ($response === false || $response < Filter_Roles::GUEST_LEVEL || $response > Filter_Roles::ADMIN_LEVEL) {
  $database_logs->log (Locale_Translate::_("Failure sending and receiving Consultation Notes"), Filter_Roles::TRANSLATOR_LEVEL);
  die;
}


$address = $database_config_general->getServerAddress ();


// Go through all notes which have actions recorded for them.
$notes = $database_noteactions->getNotes ();
foreach ($notes as $note) {

  $summary = $database_notes->getSummary ($note);
  $database_logs->log (Locale_Translate::_("Sending note to server") . ": $summary", Filter_Roles::TRANSLATOR_LEVEL);


  // Go through all the actions for the current note.
  $note_request_added = false;

  // Get all the actions for the current note.
  $note_actions = $database_noteactions->getNoteData ($note);
  
  // Add a final action to get the updated note from the server.
  if (!empty ($note_actions)) {
    $get_action = $note_actions [0];
    $get_action ['rowid'] = 0;
    $get_action ['action'] = Notes_Logic::noteActionGet;
    $get_action ['content'] = "";
    $note_actions [] = $get_action;
  }

  // Deal with the note actions for this note.
  foreach ($note_actions as $note_action) {

    $rowid = $note_action ['rowid'];
    $username = $note_action ['username'];
    $timestamp = $note_action ['timestamp'];
    $action = $note_action ['action'];
    $content = $note_action ['content'];

    // When requesting the updated note, check that it exists at all on the client.
    if ($action == Notes_Logic::noteActionGet) {
      if (!$database_notes->identifierExists ($note)) continue;
    }

    // Generate a POST request.
    $post = array (
      "u" => bin2hex ($username), 
      "p" => $database_users->getmd5 ($username),
      "l" => $database_users->getUserLevel ($username),
      "n" => $note,
      "t" => $timestamp,
      "a" => $action,
      "c" => $content
    );
    
    $url = "$address/sync/note.php";
    $response = Sync_Logic::post ($post, $url);

    if ($response === false) {
      $database_logs->log ("Failure sending and receiving Consultation Notes", Filter_Roles::TRANSLATOR_LEVEL);
      die;
    }

    // Deal with the server's response.
    if ($action == Notes_Logic::noteActionGet) {
      
      @$response = unserialize ($response);
      if ($response === false) {
        $database_logs->log ("Failure receiving Consultation Note", Filter_Roles::TRANSLATOR_LEVEL);
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

      $database_notes->setAssignees ($note, $assignees);
      $database_notes->setSubscribers ($note, $subscribers);
      $database_notes->setBible ($note, $bible);
      $database_notes->setPassages ($note, $passages);
      $database_notes->setStatus ($note, $status);
      $database_notes->setRawSeverity ($note, $severity);
      $database_notes->setSummary ($note, $summary);
      $database_notes->setContents ($note, $contents);
      // Set modified field last as it is affected by the previous store actions.
      $database_notes->setModified ($note, $modified);

      $database_logs->log ("Updated note received from server" . ": " . $summary, Filter_Roles::MANAGER_LEVEL);

      // Update search and checksum.
      $database_notes->updateSearchFields ($note);
      $database_notes->updateChecksum ($note);

    } else {
      $database_logs->log ($response, Filter_Roles::TRANSLATOR_LEVEL);
    }

    // Delete this note action because it has been dealt with.
    $database_noteactions->delete ($rowid);

  }
}


// After all note actions have been sent to the server, and the notes updated on the client,
// the client will now sync its notes with the server's notes.
$lowId = Notes_Logic::lowNoteIdentifier;
$highId = Notes_Logic::highNoteIdentifier;
Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/syncnotes.php", "$lowId", "$highId"));


?>
