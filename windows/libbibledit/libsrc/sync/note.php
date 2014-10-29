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


$username = Filter_Hex::hex2bin ($_POST ['u']);
$password = $_POST ['p'];
$level = $_POST ['l'];
$note = $_POST ['n'];
$timestamp = $_POST ['t'];
$action = $_POST ['a'];
$content = $_POST ['c'];


$database_users = Database_Users::getInstance (); 
$database_logs = Database_Logs::getInstance ();
$database_notes = Database_Notes::getInstance ();
$notes_logic = Notes_Logic::getInstance ();
$session_logic = Session_Logic::getInstance ();


$user_ok = $database_users->usernameExists ($username);
if (!$user_ok) $database_logs->log ("Non existing user $username", Filter_Roles::MANAGER_LEVEL);
$pass_ok = ($password == $database_users->getmd5 ($username));
if (!$pass_ok) $database_logs->log ("Incorrect password $password for user $username", Filter_Roles::MANAGER_LEVEL);
$level_ok = ($level == $database_users->getUserLevel ($username));
if (!$level_ok) $database_logs->log ("Incorrect role $level for user $username", Filter_Roles::MANAGER_LEVEL);
if (!$user_ok || !$pass_ok || !$level_ok) {
  // Unauthorized.
  http_response_code (401); 
  die;
}


$session_logic->setUsername ($username);


if ($action == Notes_Logic::noteActionCreate) {
  
  // Create the note on the server.  
  $data = Filter_Client::createNoteDecode ($content);
  $bible = $data ["bible"];
  $book = $data ["book"];
  $chapter = $data ["chapter"];
  $verse = $data ["verse"];
  $summary = $data ["summary"];
  $raw = $data ["raw"];
  $contents = $data ["contents"];
  $server_id = $notes_logic->createNote ($bible, $book, $chapter, $verse, $summary, $contents, $raw);

  // Update the note identifier on the server to be same as the client.
  $database_notes->setIdentifier ($server_id, $note);
  
  // Update search field.
  $database_notes->updateSearchFields ($note);

  $database_logs->log ("Client created note on server" . ": $summary", Filter_Roles::MANAGER_LEVEL);
  echo "The server created the note";

} else if ($action == Notes_Logic::noteActionComment) {
  
  // Add the comment to the note on the server.
  $notes_logic->addComment ($note, $content);

  // Update search field.
  $database_notes->updateSearchFields ($note);

  $database_logs->log ("Client added comment to note on server" . ": " . $database_notes->getSummary ($note), Filter_Roles::MANAGER_LEVEL);
  echo "The server added a comment to the note";

} else if ($action == Notes_Logic::noteActionSummary) {
  
  // Set the summary of the note on the server.
  $notes_logic->setSummary ($note, $content);

  // Update search field.
  $database_notes->updateSearchFields ($note);

  $database_logs->log ("Client updated the summary of note on server" . ": " . $content, Filter_Roles::MANAGER_LEVEL);
  echo "The server updated the summary of the note";

} else if ($action == Notes_Logic::noteActionSubscribe) {
  
  // Subscribe to the note on the server.
  $database_notes->subscribeUser ($note, $username);

  $database_logs->log ("Client subscribed to the note on server" . ": " . $database_notes->getSummary ($note), Filter_Roles::MANAGER_LEVEL);
  echo "The server subscribed a user to the note";

} else if ($action == Notes_Logic::noteActionUnsubscribe) {
  
  // Unsubscribe from the note on the server.
  $database_notes->unsubscribeUser ($note, $username);

  $database_logs->log ("Client unsubscribed from the note on server" . ": " . $database_notes->getSummary ($note), Filter_Roles::MANAGER_LEVEL);
  echo "The server unsubscribed a user from the note";

} else if ($action == Notes_Logic::noteActionAssign) {
  
  // Assign user to the note on the server.
  $notes_logic->assignUser ($note, $content);

  $database_logs->log ("Client assigned the note to a user on server" . ": " . $database_notes->getSummary ($note), Filter_Roles::MANAGER_LEVEL);
  echo "The server assigned a note to a user";

} else if ($action == Notes_Logic::noteActionUnassign) {
  
  // Unassign the user from the note on the server.
  $notes_logic->unassignUser ($note, $content);

  $database_logs->log ("Client unassigned a user from the note on server" . ": " . $database_notes->getSummary ($note), Filter_Roles::MANAGER_LEVEL);
  echo "The server unassigned a user from the note";

} else if ($action == Notes_Logic::noteActionStatus) {
  
  // Set the status for a note on the server.
  $notes_logic->setStatus ($note, $content);

  $database_logs->log ("Client set the note status on server" . ": " . $database_notes->getSummary ($note), Filter_Roles::MANAGER_LEVEL);
  echo "The server updated the status of the note";

} else if ($action == Notes_Logic::noteActionPassage) {
  
  // Set the passages for a note on the server.
  $notes_logic->setPassages ($note, unserialize ($content));

  $database_logs->log ("Client set the passages for a note on server" . ": " . $database_notes->getSummary ($note), Filter_Roles::MANAGER_LEVEL);
  echo "The server updated the passages of the note";

} else if ($action == Notes_Logic::noteActionSeverity) {
  
  // Set the severity for a note on the server.
  $notes_logic->setRawSeverity ($note, $content);

  $database_logs->log ("Client set the severity for a note on server" . ": " . $database_notes->getSummary ($note), Filter_Roles::MANAGER_LEVEL);
  echo "The server updated the severity of the note";

} else if ($action == Notes_Logic::noteActionBible) {
  
  // Set the Bible for a note on the server.
  $notes_logic->setBible ($note, $content);

  $database_logs->log ("Client set the Bible for a note on server" . ": " . $database_notes->getSummary ($note), Filter_Roles::MANAGER_LEVEL);
  echo "The server updated the Bible of the note";

} else if ($action == Notes_Logic::noteActionMarkDeletion) {
  
  // Mark note on server for deletion.
  $notes_logic->markForDeletion ($note);

  $database_logs->log ("Client marked a note on server for deletion" . ": " . $database_notes->getSummary ($note), Filter_Roles::MANAGER_LEVEL);
  echo "The server marked the note for deletion";

} else if ($action == Notes_Logic::noteActionUnmarkDeletion) {
  
  // Unmark note on server for deletion.
  $notes_logic->unmarkForDeletion ($note);

  $database_logs->log ("Client unmarked a note on server for deletion" . ": " . $database_notes->getSummary ($note), Filter_Roles::MANAGER_LEVEL);
  echo "The server unmarked the note for deletion";

} else if ($action == Notes_Logic::noteActionDelete) {
  
  // Delete note on server.
  $notes_logic->delete ($note);

  $database_logs->log ("Client deleted a note on server" . ": " . $database_notes->getSummary ($note), Filter_Roles::MANAGER_LEVEL);
  echo "The server deleted the note";

} else if ($action == Notes_Logic::noteActionGet) {
  
  // Update search and checksum.
  $database_notes->updateSearchFields ($note);
  $database_notes->updateChecksum ($note);

  // The client requests the entire note from the server.
  $modified = $database_notes->getModified ($note);
  $assignees = $database_notes->getAssignees ($note);
  $subscribers = $database_notes->getSubscribers ($note);
  $bible = $database_notes->getBible ($note);
  $passages = $database_notes->getPassages ($note);
  $status = $database_notes->getRawStatus ($note);
  $severity = $database_notes->getRawSeverity ($note);
  $summary = $database_notes->getSummary ($note);
  $contents = $database_notes->getContents ($note);

  $database_logs->log ("Client requested a note from server" . ": " . $summary, Filter_Roles::MANAGER_LEVEL);
  
  $data = array (
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
  $data = serialize ($data);
  
  echo $data;

} else {

  echo "The server did not recognize the note action and has discarded it";

}


?>
