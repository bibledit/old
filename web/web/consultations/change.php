<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);


$database_changes = Database_Changes::getInstance ();
$database_notes = Database_Notes::getInstance ();


// Note unsubscribe handler.
@$unsubscribe = $_GET['unsubscribe'];
if (isset ($unsubscribe)) {
  $database_notes->unsubscribe ($unsubscribe);
}


// Note unassign handler.
@$unassign = $_GET['unassign'];
if (isset ($unassign)) {
  $database_notes->unassign ($unassign);
}


// Note delete handler.
@$delete = $_GET['delete'];
if (isset ($delete)) {
  $database_notes->delete ($delete);
}


// Note create handler.
@$create = $_GET['createnote'];
if (isset ($create)) {
  $bible = $database_changes->getBible ($create);
  $database_bibles = Database_Bibles::getInstance ();
  $bible = $database_bibles->getName ($bible);
  $passage = $database_changes->getPassage ($create);
  $summary = gettext ("Query about a change in the text");
  $contents = "<p>" . gettext ("Old text:") . "</p>";
  $contents .= $database_changes->getOldText ($create);
  $contents .= "<p>" .  gettext ("Change:") . "</p>";
  $contents .= "<p>" . $database_changes->getModification ($create) . "</p>";
  $contents .= "<p>" . gettext ("New text:") . "</p>";
  $contents .= $database_changes->getNewText ($create);
  $newNoteID = $database_notes->storeNewNote ($bible, $passage['book'], $passage['chapter'], $passage['verse'], $summary, $contents, true);
  header ("Location: notes.php?consultationnote=$newNoteID");
  die;
}


// Edit text handler. Todo goes out.
@$edittext = $_GET['edittext'];
if (isset ($edittext)) {
  // Set the correct Bible.
  $database_config_user = Database_Config_User::getInstance ();
  $database_bibles = Database_Bibles::getInstance ();
  $bible = $database_changes->getBible ($edittext);
  $bible = $database_bibles->getName ($bible);
  $database_config_user->setBible ($bible);
  // Open the text editor.
  header ("Location: ../desktop/index.php?desktop=edittext");
  die;
}


// From here and down the script will produce output.
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
  NULL, // Limit.
  0); // User level.

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
