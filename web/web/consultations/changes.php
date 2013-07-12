<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);


$database_changes = Database_Changes::getInstance ();
$session_logic = Session_Logic::getInstance ();
$database_notes = Database_Notes::getInstance ();
$database_logs = Database_Logs::getInstance ();


$username = $session_logic->currentUser ();


// Handle AJAX call to remove a change notification.
@$remove = $_POST['remove'];
if (isset ($remove)) {
  $id = $remove;
  // Log action just to safekeep the notification for a while.
  $passage = $database_changes->getPassage ($id);
  $passageText = Filter_Books::passagesDisplayInline (array (array ($passage['book'], $passage['chapter'], $passage['verse'])));
  $modification = $database_changes->getModification ($id);
  $database_logs->log ("User $username removed change notification $passageText $modification"); 
  // Remove change notification.
  $database_changes->delete ($id);
  die;
}


// Handle AJAX call to navigate to the passage belonging to the change notification.
@$navigate = $_POST['navigate'];
if (isset ($navigate)) {
  $id = $navigate;
  $passage = $database_changes->getPassage ($id);
  if ($passage != NULL) {
    $ipc_focus = Ipc_Focus::getInstance();
    $ipc_focus->set ($passage['book'], $passage['chapter'], $passage['verse']);
  }
  // Set the correct default Bible for the user.
  $database_config_user = Database_Config_User::getInstance ();
  $database_bibles = Database_Bibles::getInstance ();
  $bible = $database_changes->getBible ($id);
  $bible = $database_bibles->getName ($bible);
  $database_config_user->setBible ($bible);
  die;
}


$header = new Assets_Header (gettext ("Changes"));
$header->jQueryOn ();
$header->run ();


$view = new Assets_View (__FILE__);


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
