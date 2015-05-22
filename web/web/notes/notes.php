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


$database_notes = Database_Notes::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_users = Database_Users::getInstance ();


$notes_logic = Notes_Logic::getInstance();
$ipc_focus = Ipc_Focus::getInstance ();
$session_logic = Session_Logic::getInstance ();


$bible = Access_Bible::clamp ($database_config_user->getBible());
$book = $ipc_focus->getBook ();
$chapter = $ipc_focus->getChapter ();
$verse = $ipc_focus->getVerse ();


$passage_selector = $database_config_user->getConsultationNotesPassageSelector();
$edit_selector = $database_config_user->getConsultationNotesEditSelector();
$non_edit_selector = $database_config_user->getConsultationNotesNonEditSelector();
$status_selector = $database_config_user->getConsultationNotesStatusSelector();
$bible_selector = $database_config_user->getConsultationNotesBibleSelector();
$assignment_selector = $database_config_user->getConsultationNotesAssignmentSelector();
$subscription_selector = $database_config_user->getConsultationNotesSubscriptionSelector();
$severity_selector = $database_config_user->getConsultationNotesSeveritySelector();
$text_selector = $database_config_user->getConsultationNotesTextSelector();
$search_text = $database_config_user->getConsultationNotesSearchText();
$passage_inclusion_selector = $database_config_user->getConsultationNotesPassageInclusionSelector();
$text_inclusion_selector = $database_config_user->getConsultationNotesTextInclusionSelector();


$view = new Assets_View (__FILE__);


// The Bibles the current user has access to.
$bibles = Access_Bible::bibles ($session_logic->currentUser ());


// The admin disables notes selection on Bibles, so the admin sees all notes, even notes referring to non-existing Bibles.
if ($session_logic->currentLevel () == Filter_Roles::ADMIN_LEVEL) $bibles = NULL;


$identifiers = $database_notes->selectNotes ($bibles, $book, $chapter, $verse, $passage_selector, $edit_selector, $non_edit_selector, $status_selector, $bible_selector, $assignment_selector, $subscription_selector, $severity_selector, $text_selector, $search_text, NULL);
$view->view->identifiers = $identifiers;


$count = count ($identifiers);
$view->view->count = $count;


$summaries = array ();
$verse_texts = array ();
$contents = array ();
foreach ($identifiers as $identifier) {

  $summary = $database_notes->getSummary ($identifier);

  $passages = $database_notes->getPassages ($identifier);
  $verses = Filter_Books::passagesDisplayInline ($passages);
  $summaries[] = $summary . " | " . $verses;

  $verse_text = "";
  if ($passage_inclusion_selector) {
    $passages = $database_notes->getPassages ($identifier);
    foreach ($passages as $passage) {
      $usfm = $database_bibles->getChapter ($bible, $passage[0], $passage[1]);
      $text = Filter_Usfm::getVerseText ($usfm, $passage[2]);
      $verse_text .= $text;
      $verse_text .= "\n";
    }
  }
  $verse_texts [] = nl2br ($verse_text);

  $content = "";
  if ($text_inclusion_selector) {
    $content = $database_notes->getContents ($identifier);
  }
  $contents[] = $content;

}
$view->view->summaries = $summaries;
$view->view->versetexts = $verse_texts;
$view->view->contents = $contents;


$view->render ("notes.php");


if ($count == 0) {
  Assets_Page::message (Locale_Translate::_("This view does not display any notes."));
}


?>
