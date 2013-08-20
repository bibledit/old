<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);


$database_changes = Database_Changes::getInstance ();
$database_notes = Database_Notes::getInstance ();
$notes_logic = Notes_Logic::getInstance();


// Note unsubscribe handler.
@$unsubscribe = $_POST['unsubscribe'];
if (isset ($unsubscribe)) {
  $database_notes->unsubscribe (Filter_Numeric::integer_in_string ($unsubscribe));
  die;
}


// Note unassign handler.
@$unassign = $_POST['unassign'];
if (isset ($unassign)) {
  $database_notes->unassign (Filter_Numeric::integer_in_string ($unassign));
  die;
}


// Note mark for deletion handler.
@$delete = $_POST['delete'];
if (isset ($delete)) {
  $identifier = Filter_Numeric::integer_in_string ($delete);
  $database_notes->markForDeletion ($identifier);
  $trash_handler = Trash_Handler::getInstance ();
  $trash_handler->consultationNote ($identifier);
  $notes_logic->handlerMarkNoteForDeletion ($identifier);
  die;
}


// From here on the script will produce output.
$view = new Assets_View (__FILE__);
$session_logic = Session_Logic::getInstance ();
$username = $session_logic->currentUser ();
$level = $session_logic->currentLevel ();
$view->view->level = $level;


// The identifier of the change notification.
@$id = $_GET ["get"];
$view->view->id = $id;


// Get old text, modification, new text.
$old_text = $database_changes->getOldText ($id);
$view->view->old_text = $old_text;
$modification = $database_changes->getModification ($id);
$view->view->modification = $modification;
$new_text = $database_changes->getNewText ($id);
$view->view->new_text = $new_text;


// Passage.
$passage = $database_changes->getPassage ($id);


// Get notes for the passage.
$notes = $database_notes->selectNotes (
  "", // Bible.
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
foreach ($notes as $offset => $note) {
  if ($database_notes->isMarkedForDeletion ($note)) {
    unset ($notes [$offset]);
  }
}

// Sort them, most recent notes first.
$timestamps = array ();
foreach ($notes as $note) {
  $timestap = $database_notes->getModified ($note);
  $timestamps [] = $timestap;
}
array_multisort ($timestamps, SORT_DESC, $notes);


// Details for the notes.
$summaries = array ();
$subscriptions = array ();
$assignments = array ();
foreach ($notes as $note) {
  $summary = $database_notes->getSummary ($note);
  $summary = Filter_Html::sanitize ($summary);
  $summaries [] = $summary;
  $subscriptions [] = $database_notes->isSubscribed ($note, $username);
  $assignments [] = $database_notes->isAssigned ($note, $username);
}
$view->view->notes = $notes;
$view->view->summaries = $summaries;
$view->view->subscriptions = $subscriptions;
$view->view->assignments = $assignments;


// Time stamp.
$timestamp = $database_changes->getTimeStamp ($id);
$timestamp = date ('j F Y', $timestamp);
$view->view->timestamp = $timestamp;


// Display page.
$view->render ("change.php");

?>
