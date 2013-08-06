<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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
page_access_level (MANAGER_LEVEL);


$header = new Assets_Header (gettext ("Bulk update"));
$header->run();


$database_notes = Database_Notes::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_logs = Database_Logs::getInstance ();
$database_users = Database_Users::getInstance ();
$notes_logic = Notes_Logic::getInstance ();
$ipc_focus = Ipc_Focus::getInstance ();


$bible = $database_config_user->getBible();
$book = $ipc_focus->getBook();
$chapter = $ipc_focus->getChapter();
$verse = $ipc_focus->getVerse();
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


$identifiers = $database_notes->selectNotes ($bible, $book, $chapter, $verse, 
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

$identifierlist = "";
foreach ($identifiers as $identifier) {
  $identifierlist .= " $identifier";
}                                          


if (isset($_GET['subscribe'])) {
  foreach ($identifiers as $identifier) {
    $database_notes->subscribe ($identifier);
  }
  Assets_Page::success (gettext ("You subscribed to these notes"));
}


if (isset($_GET['unsubscribe'])) {
  foreach ($identifiers as $identifier) {
    $database_notes->unsubscribe ($identifier);
  }
  Assets_Page::success (gettext ("You unsubscribed from these notes"));
}


@$assign = $_GET['assign'];
if (isset ($assign)) {
  if ($database_users->usernameExists ($assign)) {
    foreach ($identifiers as $identifier) {
      if (!$database_notes->isAssigned ($identifier, $assign)) {
        // Assign logic comes first.
        $notes_logic->handlerAssignNote ($identifier, $assign);
        $database_notes->assignUser ($identifier, $assign);
      }
    }
  }
  Assets_Page::success (gettext ("The notes were assigned to the user"));
  $database_logs->log ("Notes assigned to user $assign: $identifierlist");
}


@$unassign = $_GET['unassign'];
if (isset ($unassign)) {
  if ($database_users->usernameExists ($unassign)) {
    foreach ($identifiers as $identifier) {
      if ($database_notes->isAssigned ($identifier, $unassign)) {
        $database_notes->unassignUser ($identifier, $unassign);
      }
    }
  }
  Assets_Page::success (gettext ("The notes are no longer assigned to the user"));
  $database_logs->log ("Notes unassigned from user $unassign: $identifierlist", true);
}


@$status = $_GET['status'];
if (isset ($status)) {
  foreach ($identifiers as $identifier) {
    if ($database_notes->getRawStatus ($identifier) != $status) {
      $database_notes->setStatus ($identifier, $status);
    }
  }
  Assets_Page::success (gettext ("The status of the notes was updated"));
  $database_logs->log ("Status update of notes: $identifierlist", true);
}


@$severity = $_GET['severity'];
if (isset ($severity)) {
  foreach ($identifiers as $identifier) {
    if ($database_notes->getRawSeverity ($identifier) != $severity) {
      $database_notes->setRawSeverity ($identifier, $severity);
    }
  }
  Assets_Page::success (gettext ("The severity of the notes was updated"));
  $database_logs->log ("Severity update of notes: $identifierlist", true);
}


@$bible = $_GET['bible'];
if (isset ($bible)) {
  foreach ($identifiers as $identifier) {
    if ($database_notes->getBible ($identifier) != $bible) {
      $database_notes->setBible ($identifier, $bible);
    }
  }
  Assets_Page::success (gettext ("The Bible of the notes was updated"));
  $database_logs->log ("Bible update of notes: $identifierlist", true);
}


$view = new Assets_View (__FILE__);


$identifiers = $database_notes->selectNotes ($bible, $book, $chapter, $verse, 
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


$notescount = count ($identifiers);
$view->view->notescount = $notescount;


$view->render ("bulk.php");


Assets_Page::footer ();


?>
