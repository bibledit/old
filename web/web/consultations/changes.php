<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);
Assets_Page::header (gettext ("Changes"));
$smarty = new Smarty_Bibledit (__FILE__);


$database_changes = Database_Changes::getInstance ();
$session_logic = Session_Logic::getInstance ();
$database_notes = Database_Notes::getInstance ();


$username = $session_logic->currentUser ();


@$approve = $_GET['approve'];
if (isset ($approve)) {
  $database_changes->delete ($approve);
  $smarty->assign ("success", gettext ("The change was approved."));
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
    0, // Subscription selector.
    -1, // Severity selector.
    0, // Text selector.
    "", // Search text.
    NULL, // Limit.
    0); // User level.
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
}
$smarty->assign ("passages", $passages);
$smarty->assign ("modifications", $modifications);
$smarty->assign ("totalNotesCount", $totalNotesCount);
$smarty->assign ("yourNotesCount", $yourNotesCount);


$smarty->display("changes.tpl");
Assets_Page::footer ();
?>
