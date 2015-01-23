<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

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


require_once ("../bootstrap/bootstrap");
page_access_level (Filter_Roles::consultant ());


$database_notes = Database_Notes::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_users = Database_Users::getInstance ();


$notes_logic = Notes_Logic::getInstance();
$ipc_focus = Ipc_Focus::getInstance ();
$session_logic = Session_Logic::getInstance ();


$bible = access_bible_clamp (request->database_config_user()->getBible());
$book = Ipc_Focus::getBook ();
$chapter = Ipc_Focus::getChapter ();
$verse = Ipc_Focus::getVerse ();


$passage_selector = request->database_config_user()->getConsultationNotesPassageSelector();
$edit_selector = request->database_config_user()->getConsultationNotesEditSelector();
$non_edit_selector = request->database_config_user()->getConsultationNotesNonEditSelector();
$status_selector = request->database_config_user()->getConsultationNotesStatusSelector();
$bible_selector = request->database_config_user()->getConsultationNotesBibleSelector();
$assignment_selector = request->database_config_user()->getConsultationNotesAssignmentSelector();
$subscription_selector = request->database_config_user()->getConsultationNotesSubscriptionSelector();
$severity_selector = request->database_config_user()->getConsultationNotesSeveritySelector();
$text_selector = request->database_config_user()->getConsultationNotesTextSelector();
$search_text = request->database_config_user()->getConsultationNotesSearchText();
$passage_inclusion_selector = request->database_config_user()->getConsultationNotesPassageInclusionSelector();
$text_inclusion_selector = request->database_config_user()->getConsultationNotesTextInclusionSelector();


$view = new Assets_View (__FILE__);


// The Bibles the current user has access to.
$bibles = access_bible_bibles (request->session_logic()->currentUser ());


// The admin disables notes selection on Bibles, so the admin sees all notes, even notes referring to non-existing Bibles.
if ($session_logic->currentLevel () == Filter_Roles::admin ()) $bibles = NULL;


$identifiers = $database_notes->selectNotes ($bibles, book, chapter, verse, $passage_selector, $edit_selector, $non_edit_selector, $status_selector, $bible_selector, $assignment_selector, $subscription_selector, $severity_selector, $text_selector, $search_text, NULL);
$view.set_variable ("identifiers = $identifiers;


$count = count ($identifiers);
$view.set_variable ("count = $count;


$summaries = array ();
$verse_texts = array ();
$contents = array ();
for ($identifiers as $identifier) {

  $summary = $database_notes->getSummary ($identifier);

  $passages = $database_notes->getPassages ($identifier);
  $verses = filter_passage_display_inline ($passages);
  $summaries[] = $summary . " | " . $verses;

  $verse_text = "";
  if ($passage_inclusion_selector) {
    $passages = $database_notes->getPassages ($identifier);
    for ($passages as $passage) {
      $usfm = request->database_bibles()->getChapter ($bible, $passage[0], $passage[1]);
      $text = usfm_get_verse_text ($usfm, $passage[2]);
      $verse_text += $text;
      $verse_text += "\n";
    }
  }
  $verse_texts [] = nl2br ($verse_text);

  $content = "";
  if ($text_inclusion_selector) {
    $content = $database_notes->getContents ($identifier);
  }
  $contents[] = $content;

}
$view.set_variable ("summaries = $summaries;
$view.set_variable ("versetexts = $verse_texts;
$view.set_variable ("contents = $contents;


$view->render ("notes");


if ($count == 0) {
  Assets_Page::message (gettext("This view does not display any notes."));
}


?>
