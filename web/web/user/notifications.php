<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (MEMBER_LEVEL);
Assets_Page::header (gettext ("Notifications"));
$smarty = new Smarty_Bibledit (__FILE__);

$database_config_user = Database_Config_User::getInstance ();

if (isset ($_GET['editednotessubscriptiontoggle'])) {
  $database_config_user->setSubscribeToConsultationNotesEditedByMe(!$database_config_user->getSubscribeToConsultationNotesEditedByMe());
}
$edited_notes_subscription = $database_config_user->getSubscribeToConsultationNotesEditedByMe();
$smarty->assign ("edited_notes_subscription", $edited_notes_subscription);

if (isset ($_GET['anynotessubscriptiontoggle'])) {
  $database_config_user->setNotifyMeOfAnyConsultationNotesEdits(!$database_config_user->getNotifyMeOfAnyConsultationNotesEdits());
}
$any_notes_subscription = $database_config_user->getNotifyMeOfAnyConsultationNotesEdits();
$smarty->assign ("any_notes_subscription", $any_notes_subscription);

if (isset ($_GET['notesassignmenttoggle'])) {
  $database_config_user->setGetAssignedToConsultationNotesChanges(!$database_config_user->getGetAssignedToConsultationNotesChanges());
}
$notes_assignment = $database_config_user->getGetAssignedToConsultationNotesChanges();
$smarty->assign ("notes_assignment", $notes_assignment);

$smarty->display("notifications.tpl");
Assets_Page::footer ();
?>
