<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);


$database_changes = Database_Changes::getInstance ();
$session_logic = Session_Logic::getInstance ();
$database_notes = Database_Notes::getInstance ();


@$remove = $_POST['remove'];
if (isset ($remove)) {
  $database_changes->delete ($remove);
  $database_logs = Database_Logs::getInstance ();
  $database_logs->log ($remove);
  die;
}


$username = $session_logic->currentUser ();





$header = new Assets_Header (gettext ("Changes"));
$header->jQueryOn ();
$header->run ();


$view = new Assets_View (__FILE__);


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
  $view->view->success = gettext ("The change was approved.");
}


$ids = $database_changes->getIdentifiers ($username);
$view->view->ids = $ids;
  

// The first identifier of the change notification in the list.
$firstID = 0;
if (count ($ids) > 0) $firstID = $ids[0];
$lastID = 0;


$passages = array ();
$modifications = array ();
foreach ($ids as $id) {
  $lastID = $id;
  $passage = $database_changes->getPassage ($id);
  $passageText = Filter_Books::passagesDisplayInline (array (array ($passage['book'], $passage['chapter'], $passage['verse'])));
  $passageText = Filter_Html::sanitize ($passageText);
  $passages [] = $passageText;
  $modification = $database_changes->getModification ($id);
  $modifications [] = $modification;
  /*
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
  */
  //if (count ($passages) >= 35) break; // Todo remove this later.
}
$view->view->passages = $passages;
$view->view->modifications = $modifications;


$script = <<<EOD
var firstID = $firstID;
var lastID = $lastID;
var selectedID = $firstID;
EOD;
$view->view->script = $script;


$view->render ("changes.php");
Assets_Page::footer ();
?>
