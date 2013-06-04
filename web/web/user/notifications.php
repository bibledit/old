<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (MEMBER_LEVEL);
Assets_Page::header (gettext ("Notifications"));
$smarty = new Smarty_Bibledit (__FILE__);

$database_config_user = Database_Config_User::getInstance ();

if (isset ($_GET['editednotessubscriptiontoggle'])) {
  $database_config_user->setSubscribeToConsultationNotesEditedByMe(!$database_config_user->getSubscribeToConsultationNotesEditedByMe());
}
$smarty->assign ("edited_notes_subscription", $database_config_user->getSubscribeToConsultationNotesEditedByMe());

if (isset ($_GET['anynotessubscriptiontoggle'])) {
  $database_config_user->setNotifyMeOfAnyConsultationNotesEdits(!$database_config_user->getNotifyMeOfAnyConsultationNotesEdits());
}
$smarty->assign ("any_notes_subscription", $database_config_user->getNotifyMeOfAnyConsultationNotesEdits());

// Todo
if (isset ($_GET['emailconfirmationyourpoststoggle'])) {
  $database_config_user->setNotifyMeOfMyPosts (!$database_config_user->getNotifyMeOfMyPosts ());
}
$smarty->assign ("email_confirmation_your_posts", $database_config_user->getNotifyMeOfMyPosts ());

if (isset ($_GET['subscribednotenotificationtoggle'])) {
  $database_config_user->setSubscribedConsultationNoteNotification(!$database_config_user->getSubscribedConsultationNoteNotification());
}
$smarty->assign ("subscription_notification", $database_config_user->getSubscribedConsultationNoteNotification());

if (isset ($_GET['notesassignmenttoggle'])) {
  $database_config_user->setAssignedToConsultationNotesChanges(!$database_config_user->getAssignedToConsultationNotesChanges());
}
$smarty->assign ("notes_assignment", $database_config_user->getAssignedToConsultationNotesChanges());

if (isset ($_GET['assignednotenotificationtoggle'])) {
  $database_config_user->setAssignedConsultationNoteNotification(!$database_config_user->getAssignedConsultationNoteNotification());
}
$smarty->assign ("assignment_notification", $database_config_user->getAssignedConsultationNoteNotification());

if (isset ($_GET['assignednotesnotificationtoggle'])) {
  $database_config_user->setAssignedNotesStatisticsNotification (!$database_config_user->getAssignedNotesStatisticsNotification ());
}
$smarty->assign ("assigned_notes_notifications", $database_config_user->getAssignedNotesStatisticsNotification ());

if (isset ($_GET['deletednotenotificationtoggle'])) {
  $database_config_user->setDeletedConsultationNoteNotification(!$database_config_user->getDeletedConsultationNoteNotification());
}
$smarty->assign ("deleted_note_notification", $database_config_user->getDeletedConsultationNoteNotification());

if (isset ($_GET['biblechangesnotificationtoggle'])) {
  $database_config_user->setBibleChangesNotification(!$database_config_user->getBibleChangesNotification());
}
$smarty->assign ("bible_changes_notification", $database_config_user->getBibleChangesNotification());

if (isset ($_GET['changenotificationstoggle'])) {
  $database_config_user->setGenerateChangeNotifications(!$database_config_user->getGenerateChangeNotifications());
}
$smarty->assign ("change_notifications", $database_config_user->getGenerateChangeNotifications());

if (isset ($_GET['pendingchangenotificationstoggle'])) {
  $database_config_user->setPendingChangesNotification (!$database_config_user->getPendingChangesNotification ());
}
$smarty->assign ("pending_changes_notifications", $database_config_user->getPendingChangesNotification ());

if (isset ($_GET['biblechecksnotificationtoggle'])) {
  $database_config_user->setBibleChecksNotification (!$database_config_user->getBibleChecksNotification ());
}
$smarty->assign ("bible_checks_notification", $database_config_user->getBibleChecksNotification ());

$smarty->display("notifications.tpl");
Assets_Page::footer ();
?>
