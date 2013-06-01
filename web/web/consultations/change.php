<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);
Assets_Page::header (gettext ("Change"));
$smarty = new Smarty_Bibledit (__FILE__);


$database_changes = Database_Changes::getInstance ();
$database_notes = Database_Notes::getInstance ();


$session_logic = Session_Logic::getInstance ();
$username = $session_logic->currentUser ();


// The identifier for the change notification.
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


// Get notes for the passage and sort them on relevance.
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
$summaries = array ();
foreach ($notes as $note) {
  $summary = $database_notes->getSummary ($note);
  $summary = Filter_Html::sanitize ($summary);
  $summaries [] = $summary;
}
$smarty->assign ("notes", $notes);
$smarty->assign ("summaries", $summaries);


// Time stamp.
$timestamp = $database_changes->getTimeStamp ($id);
$timestamp = date ('j F Y', $timestamp);
$smarty->assign ("timestamp", $timestamp);


/*
  $totalNotesCount [] = count ($notes);
  $subscribedNotes = $database_notes->selectNotes (
    "", // Bible.
    $passage['book'],
    $passage['chapter'], 
    $passage['verse'],
    0, // Passage selector.
    0, // Edit selector.
    0, // Non-edit selector.
    "", // Status selector.
    "", // Bible selector.
    "", // Assignment selector.
    1, // Subscription selector.
    -1, // Severity selector.
    0, // Text selector.
    "", // Search text.
    NULL, // Limit.
    0); // User level.
  $assignedNotes = $database_notes->selectNotes (
    "", // Bible.
    $passage['book'],
    $passage['chapter'], 
    $passage['verse'],
    0, // Passage selector.
    0, // Edit selector.
    0, // Non-edit selector.
    "", // Status selector.
    "", // Bible selector.
    $username, // Assignment selector.
    0, // Subscription selector.
    -1, // Severity selector.
    0, // Text selector.
    "", // Search text.
    NULL, // Limit.
    0); // User level.
  $yourNotes = array_merge ($subscribedNotes, $assignedNotes);
  $yourNotes = array_unique ($yourNotes);
  $yourNotesCount [] = count ($yourNotes);
*/

$smarty->display("change.tpl");
Assets_Page::footer ();
?>
