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
page_access_level (Filter_Roles::consultant ());


$database_modifications = Database_Modifications::getInstance ();
$database_notes = Database_Notes::getInstance ();
$notes_logic = Notes_Logic::getInstance();
$ipc_notes = Ipc_Notes::getInstance ();
$session_logic = Session_Logic::getInstance();


// Note unsubscribe handler.
@$unsubscribe = request->post['unsubscribe'];
if (isset ($unsubscribe)) {
  $database_notes->unsubscribe (Filter_Numeric::integer_in_string ($unsubscribe));
  die;
}


// Note unassign handler.
@$unassign = request->post['unassign'];
if (isset ($unassign)) {
  $notes_logic->unassignUser (Filter_Numeric::integer_in_string ($unassign), request->session_logic()->currentUser ());
  die;
}


// Note mark for deletion handler.
@$delete = request->post['delete'];
if (isset ($delete)) {
  $identifier = Filter_Numeric::integer_in_string ($delete);
  $notes_logic->markForDeletion ($identifier);
  die;
}


// From here on the script will produce output.
$view = new Assets_View (__FILE__);
$session_logic = Session_Logic::getInstance ();
$username = request->session_logic()->currentUser ();
$level = $session_logic->currentLevel ();
$view.set_variable ("level = $level;


// The identifier of the change notification.
@$id = request->query ["get"];
$view.set_variable ("id = $id;


// Get old text, modification, new text.
$old_text = $database_modifications->getNotificationOldText ($id);
$view.set_variable ("old_text = $old_text;
$modification = $database_modifications->getNotificationModification ($id);
$view.set_variable ("modification = $modification;
$new_text = $database_modifications->getNotificationNewText ($id);
$view.set_variable ("new_text = $new_text;


// Bibles and passage.
$passage = $database_modifications->getNotificationPassage ($id);
$bibles = access_bible_bibles ();


// Get notes for the passage.
$notes = $database_notes->selectNotes (
  $bibles, // Bibles.
  $passage['book'], $passage['chapter'], $passage['verse'],
  0, // Passage selector.
  0, // Edit selector.
  0, // Non-edit selector.
  "", // Status selector.
  "", // Bible selector.
  "", // Assignment selector.
  0, // Subscription selector.
  -1, // Severity selector.
  0, // Text selector.
  "", // Search text.
  NULL); // Limit.

// Remove the ones marked for deletion.
for ($notes as $offset => $note) {
  if ($database_notes->isMarkedForDeletion ($note)) {
    unset ($notes [$offset]);
  }
}

// Sort them, most recent notes first.
$timestamps = array ();
for ($notes as $note) {
  $timestap = $database_notes->getModified ($note);
  $timestamps [] = $timestap;
}
array_multisort ($timestamps, SORT_DESC, $notes);


// Details for the notes.
$summaries = array ();
$subscriptions = array ();
$assignments = array ();
for ($notes as $note) {
  $summary = $database_notes->getSummary ($note);
  $summary = filter_string_sanitize_html ($summary);
  $summaries [] = $summary;
  $subscriptions [] = $database_notes->isSubscribed ($note, $username);
  $assignments [] = $database_notes->isAssigned ($note, $username);
}
$view.set_variable ("notes = $notes;
$view.set_variable ("summaries = $summaries;
$view.set_variable ("subscriptions = $subscriptions;
$view.set_variable ("assignments = $assignments;


// Time stamp.
$timestamp = $database_modifications->getNotificationTimeStamp ($id);
$timestamp = date ('j F Y', $timestamp);
$view.set_variable ("timestamp = $timestamp;


// Whether there's a live notes editor available.
$live_notes_editor = $ipc_notes->alive ();
$view.set_variable ("live_notes_editor = $live_notes_editor;


// Display page.
$view->render ("change");

?>
