<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);
Assets_Page::header (gettext ("Change"));
$smarty = new Smarty_Bibledit (__FILE__);


$database_changes = Database_Changes::getInstance ();
$database_notes = Database_Notes::getInstance ();


$session_logic = Session_Logic::getInstance ();
$username = $session_logic->currentUser ();
$level = $session_logic->currentLevel ();
$smarty->assign ("level", $level);


// The identifier of the change notification.
@$id = $_GET['id'];
if (isset ($id)) {
  $passage = $database_changes->getPassage ($id);
  if ($passage != NULL) {
    $ipc_focus = Ipc_Focus::getInstance();
    $ipc_focus->set ($passage['book'], $passage['chapter'], $passage['verse']);
  }
} else {
  $id = 0;
}
$smarty->assign ("id", $id);


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


// Get old text, modification, new text.
$old_text = $database_changes->getOldText ($id);
$smarty->assign ("old_text", $old_text);
$modification = $database_changes->getModification ($id);
$smarty->assign ("modification", $modification);
$new_text = $database_changes->getNewText ($id);
$smarty->assign ("new_text", $new_text);


// Passage.
$passage = $database_changes->getPassage ($id);
$passageText = Filter_Books::passagesDisplayInline (array (array ($passage['book'], $passage['chapter'], $passage['verse'])));
$passageText = Filter_Html::sanitize ($passageText);
$smarty->assign ("passage", $passageText);


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
$smarty->assign ("notes", $notes);
$smarty->assign ("summaries", $summaries);
$smarty->assign ("subscriptions", $subscriptions);
$smarty->assign ("assignments", $assignments);


// Time stamp.
$timestamp = $database_changes->getTimeStamp ($id);
$timestamp = date ('j F Y', $timestamp);
$smarty->assign ("timestamp", $timestamp);


// Display page.
$smarty->display("change.tpl");
Assets_Page::footer ();
?>
