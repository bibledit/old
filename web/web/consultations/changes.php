<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);
Assets_Page::header (gettext ("Changes"));
$smarty = new Smarty_Bibledit (__FILE__);


$database_changes = Database_Changes::getInstance ();
$session_logic = Session_Logic::getInstance ();
$database_notes = Database_Notes::getInstance ();


$username = $session_logic->currentUser ();


@$goto = $_GET['goto'];
if (isset ($goto)) {
  $passage = $database_changes->getPassage ($goto);
  if ($passage != NULL) {
    $ipc_focus = Ipc_Focus::getInstance();
    $ipc_focus->set ($passage['book'], $passage['chapter'], $passage['verse']);
    header ("Location: ../desktop/index.php?desktop=edittext");
    die;
  } else {
    $smarty->assign ("error", gettext ("The passage for this change was not found."));
  }
}


@$approve = $_GET['approve'];
if (isset ($approve)) {
  $database_changes->delete ($approve);
  $smarty->assign ("success", gettext ("The change was approved."));
}


if (isset ($_GET['approvepage'])) {
  $ids = $database_changes->getIdentifiers ($username);
  $approvedCount = 0;
  foreach ($ids as $id) {
    if ($approvedCount >= 20) break;
    $approvedCount++;
    $database_changes->delete ($id);
  }
  $smarty->assign ("success", gettext ("The changes were approved."));
  unset ($ids);
}


$ids = $database_changes->getIdentifiers ($username);
$smarty->assign ("ids", $ids);
  

$passages = array ();
$modifications = array ();
$totalNotesCount = array ();
$yourNotesCount = array ();
foreach ($ids as $id) {
  $passage = $database_changes->getPassage ($id);
  $passageText = Filter_Books::passagesDisplayInline (array (array ($passage['book'], $passage['chapter'], $passage['verse'])));
  $passageText = Filter_Html::sanitize ($passageText);
  $passages [] = $passageText;
  $modification = $database_changes->getModification ($id);
  $modifications [] = $modification;
  $notes = $database_notes->selectNotes (
    "",   // Bible.
    $passage['book'],
    $passage['chapter'], 
    $passage['verse'],
    0,    // Passage selector.
    0,    // Edit selector.
    0,    // Non-edit selector.
    "",   // Status selector.
    "",   // Bible selector.
    "",   // Assignment selector.
    0,    // Subscription selector.
    -1,   // Severity selector.
    0,    // Text selector.
    "",   // Search text.
    NULL, // Limit.
    0);   // User level.
  $totalNotesCount [] = count ($notes);
  $subscribedNotes = $database_notes->selectNotes (
    "",   // Bible.
    $passage['book'],
    $passage['chapter'], 
    $passage['verse'],
    0,    // Passage selector.
    0,    // Edit selector.
    0,    // Non-edit selector.
    "",   // Status selector.
    "",   // Bible selector.
    "",   // Assignment selector.
    1,    // Subscription selector.
    -1,   // Severity selector.
    0,    // Text selector.
    "",   // Search text.
    NULL, // Limit.
    0);   // User level.
  $assignedNotes = $database_notes->selectNotes (
    "",   // Bible.
    $passage['book'],
    $passage['chapter'], 
    $passage['verse'],
    0,    // Passage selector.
    0,    // Edit selector.
    0,    // Non-edit selector.
    "",   // Status selector.
    "",   // Bible selector.
    $username, // Assignment selector.
    0,    // Subscription selector.
    -1,   // Severity selector.
    0,    // Text selector.
    "",   // Search text.
    NULL, // Limit.
    0);   // User level.
  $yourNotes = array_merge ($subscribedNotes, $assignedNotes);
  $yourNotes = array_unique ($yourNotes);
  $yourNotesCount [] = count ($yourNotes);
  // Display no more than 20 changes for performance reasons.
  if (count ($passages) >= 20) break;
}
$smarty->assign ("passages", $passages);
$smarty->assign ("modifications", $modifications);
$smarty->assign ("totalNotesCount", $totalNotesCount);
$smarty->assign ("yourNotesCount", $yourNotesCount);


// Information about the number of changes / total changes.
$smarty->assign ("displayedChangesCount", count ($passages));
$smarty->assign ("totalChangesCount", count ($ids));



$smarty->display("changes.tpl");
Assets_Page::footer ();
?>
