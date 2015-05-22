<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


require_once ("../bootstrap/bootstrap.php");
page_access_level (Filter_Roles::CONSULTANT_LEVEL);


$header = new Assets_Header (Locale_Translate::_("Select notes"));
$header->run();


$database_notes = Database_Notes::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_users = Database_Users::getInstance ();
$notes_logic = Notes_Logic::getInstance ();
$ipc_focus = Ipc_Focus::getInstance ();
$session_logic = Session_Logic::getInstance ();


@$passage_selector = $_GET['passageselector'];
if (isset ($passage_selector)) {
  if (($passage_selector < 0) || ($passage_selector > 3)) $passage_selector = 0;
  $database_config_user->setConsultationNotesPassageSelector ($passage_selector);
}


@$edit_selector = $_GET['editselector'];
if (isset ($edit_selector)) {
  if (($edit_selector < 0) || ($edit_selector > 4)) $edit_selector = 0;
  $database_config_user->setConsultationNotesEditSelector ($edit_selector);
}


@$non_edit_selector = $_GET['noneditselector'];
if (isset ($non_edit_selector)) {
  if (($non_edit_selector < 0) || ($non_edit_selector > 5)) $non_edit_selector = 0;
  $database_config_user->setConsultationNotesNonEditSelector($non_edit_selector);
}


@$status_selector = $_GET['statusselector'];
if (isset ($status_selector)) {
  $database_config_user->setConsultationNotesStatusSelector($status_selector);
}


@$bible_selector = $_GET['bibleselector'];
if (isset ($bible_selector)) {
  $database_config_user->setConsultationNotesBibleSelector ($bible_selector);
  // Also set the active Bible for the user.
  if ($bible_selector != "") {
    $database_config_user->setBible ($bible_selector);
  }
}


@$assignment_selector = $_GET['assignmentselector'];
if (isset ($assignment_selector)) {
  $database_config_user->setConsultationNotesAssignmentSelector($assignment_selector);
}


@$subscription_selector = $_GET['subscriptionselector'];
if (isset ($subscription_selector)) {
  if ($subscription_selector == 1) $subscription_selector = 1; else $subscription_selector = 0;
  $database_config_user->setConsultationNotesSubscriptionSelector($subscription_selector);
}


@$severity_selector = $_GET['severityselector'];
if (isset ($severity_selector)) {
  $database_config_user->setConsultationNotesSeveritySelector($severity_selector);
}


@$text_selector = $_GET['textselector'];
if (isset ($text_selector)) {
  $database_config_user->setConsultationNotesTextSelector ($text_selector);
  @$search_text = $_POST['text'];
  if (isset ($search_text)) {
    $database_config_user->setConsultationNotesSearchText ($search_text);
    Assets_Page::success (Locale_Translate::_("Search text saved"));
  }
}


@$passage_inclusion_selector = $_GET['passageinclusionyselector'];
if (isset ($passage_inclusion_selector)) {
  $database_config_user->setConsultationNotesPassageInclusionSelector ($passage_inclusion_selector);
}


@$text_inclusion_selector = $_GET['textinclusionyselector'];
if (isset ($text_inclusion_selector)) {
  $database_config_user->setConsultationNotesTextInclusionSelector ($text_inclusion_selector);
}


$view = new Assets_View (__FILE__);


$passage_selector = $database_config_user->getConsultationNotesPassageSelector();
$view->view->passageselector = $passage_selector;


$edit_selector = $database_config_user->getConsultationNotesEditSelector();
$view->view->editselector = $edit_selector;


$non_edit_selector = $database_config_user->getConsultationNotesNonEditSelector();
$view->view->noneditselector = $non_edit_selector;


$possible_statuses = $database_notes->getPossibleStatuses();
foreach ($possible_statuses as $possible_status) {
  $status_ids [] = $possible_status[0];
  $status_localizations [] = $possible_status[1];
}
$view->view->statusids = $status_ids;
$view->view->statuslocs = $status_localizations;


$status_selector = $database_config_user->getConsultationNotesStatusSelector();
$view->view->statusselector = $status_selector;


// The information about available Bibles could be gathered from the notes database.
// But multiple teams can be hosted, the information about available Bibles
// is gathered from the Bibles the user has access to.
$bibles = Access_Bible::bibles ();
$view->view->bibles = $bibles;


$bible_selector = $database_config_user->getConsultationNotesBibleSelector();
$view->view->bibleselector = $bible_selector;


$assignees = $database_notes->getAllAssignees ($bibles);
$view->view->assignees = $assignees;
if ($assignment_selector != "") {
  if (!in_array ($assignment_selector, $assignees)) {
    $view->view->nonexistingassignee = true;
  }
}


$assignment_selector = $database_config_user->getConsultationNotesAssignmentSelector();
$view->view->assignmentselector = $assignment_selector;


$subscription_selector = $database_config_user->getConsultationNotesSubscriptionSelector();
$view->view->subscriptionselector = $subscription_selector;


$view->view->severities = $database_notes->getPossibleSeverities();


$severity_selector = $database_config_user->getConsultationNotesSeveritySelector();
$view->view->severityselector = $severity_selector;


$text_selector = $database_config_user->getConsultationNotesTextSelector();
$view->view->textselector = $text_selector;


$search_text = $database_config_user->getConsultationNotesSearchText();
$view->view->searchtext = Filter_Html::sanitize ($search_text);


$passage_inclusion_selector = $database_config_user->getConsultationNotesPassageInclusionSelector();
$view->view->passageinclusionselector = $passage_inclusion_selector;


$text_inclusion_selector = $database_config_user->getConsultationNotesTextInclusionSelector();
$view->view->textinclusionselector = $text_inclusion_selector;


// The admin disables notes selection on Bibles, so the admin sees all notes, even notes referring to non-existing Bibles.
if ($session_logic->currentLevel () == Filter_Roles::ADMIN_LEVEL) $bibles = NULL;


$book = $ipc_focus->getBook ();
$chapter = $ipc_focus->getChapter ();
$verse = $ipc_focus->getVerse ();
$identifiers = $database_notes->selectNotes ($bibles, $book, $chapter, $verse,
                                             $passage_selector,
                                             $edit_selector,
                                             $non_edit_selector,
                                             $status_selector,
                                             $bible_selector,
                                             $assignment_selector,
                                             $subscription_selector,
                                             $severity_selector,
                                             $text_selector,
                                             $search_text,
                                             NULL);


$count = count ($identifiers);
$view->view->count = $count;


$view->render ("select.php");


Assets_Page::footer ();


?>
