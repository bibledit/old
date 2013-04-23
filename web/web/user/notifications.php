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

if (isset ($_GET['subscribednotenotificationtoggle'])) {
  $database_config_user->setSubscribedConsultationNoteNotification(!$database_config_user->getSubscribedConsultationNoteNotification());
}
$subscription_notification = $database_config_user->getSubscribedConsultationNoteNotification();
$smarty->assign ("subscription_notification", $subscription_notification);

if (isset ($_GET['notesassignmenttoggle'])) {
  $database_config_user->setAssignedToConsultationNotesChanges(!$database_config_user->getAssignedToConsultationNotesChanges());
}
$notes_assignment = $database_config_user->getAssignedToConsultationNotesChanges();
$smarty->assign ("notes_assignment", $notes_assignment);

if (isset ($_GET['assignednotenotificationtoggle'])) {
  $database_config_user->setAssignedConsultationNoteNotification(!$database_config_user->getAssignedConsultationNoteNotification());
}
$assignment_notification = $database_config_user->getAssignedConsultationNoteNotification();
$smarty->assign ("assignment_notification", $assignment_notification);

if (isset ($_GET['deletednotenotificationtoggle'])) {
  $database_config_user->setDeletedConsultationNoteNotification(!$database_config_user->getDeletedConsultationNoteNotification());
}
$deleted_note_notification = $database_config_user->getDeletedConsultationNoteNotification();
$smarty->assign ("deleted_note_notification", $deleted_note_notification);

if (isset ($_GET['biblechangesnotificationtoggle'])) {
  $database_config_user->setBibleChangesNotification(!$database_config_user->getBibleChangesNotification());
}
$bible_changes_notification = $database_config_user->getBibleChangesNotification();
$smarty->assign ("bible_changes_notification", $bible_changes_notification);

if (isset ($_GET['biblechecksnotificationtoggle'])) {
  $database_config_user->setBibleChecksNotification (!$database_config_user->getBibleChecksNotification ());
}
$bible_checks_notification = $database_config_user->getBibleChecksNotification ();
$smarty->assign ("bible_checks_notification", $bible_checks_notification);

$smarty->display("notifications.tpl");
Assets_Page::footer ();
?>
