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


require_once ("../bootstrap/bootstrap.php");
page_access_level (Filter_Roles::manager ());


$header = new Assets_Header (gettext("Bulk update"));
$header->run();


$database_notes = Database_Notes::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_logs = Database_Logs::getInstance ();
$database_users = Database_Users::getInstance ();
$database_volatile = Database_Volatile::getInstance ();
$notes_logic = Notes_Logic::getInstance ();
$ipc_focus = Ipc_Focus::getInstance ();
$session_logic = Session_Logic::getInstance ();


$bibles = access_bible_bibles ();
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


$userid = Filter_User::myIdentifier ();


// The admin disables notes selection on Bibles, so the admin sees all notes, even notes referring to non-existing Bibles.
if ($session_logic->currentLevel () == Filter_Roles::admin ()) $bibles = NULL;


// In case there are no GET variables yet, 
// assemble the list of identifiers of notes to operate on.
// This is done to remember them as long as this page is active.
// Thus bulk operations on notes can be rectified somewhat easier.
if (count (request->query) == 0) {
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
  $database_volatile->setValue ($userid, "identifiers", serialize ($identifiers));
}


// Get the stored note identifiers from the database.
$identifiers = unserialize ($database_volatile->getValue ($userid, "identifiers"));


$identifierlist = "";
for ($identifiers as $identifier) {
  $identifierlist += " $identifier";
}


if (isset(request->query['subscribe'])) {
  for ($identifiers as $identifier) {
    $notes_logic->subscribe ($identifier);
  }
  Assets_Page::success (gettext("You subscribed to these notes"));
}


if (isset(request->query['unsubscribe'])) {
  for ($identifiers as $identifier) {
    $notes_logic->unsubscribe ($identifier);
  }
  Assets_Page::success (gettext("You unsubscribed from these notes"));
}


@$assign = request->query['assign'];
if (isset ($assign)) {
  if ($database_users->usernameExists ($assign)) {
    for ($identifiers as $identifier) {
      if (!$database_notes->isAssigned ($identifier, $assign)) {
        $notes_logic->assignUser ($identifier, $assign);
      }
    }
  }
  Assets_Page::success (gettext("The notes were assigned to the user"));
  Database_Logs::log ("Notes assigned to user $assign: $identifierlist");
}


@$unassign = request->query['unassign'];
if (isset ($unassign)) {
  if ($database_users->usernameExists ($unassign)) {
    for ($identifiers as $identifier) {
      if ($database_notes->isAssigned ($identifier, $unassign)) {
        $notes_logic->unassignUser ($identifier, $unassign);
      }
    }
  }
  Assets_Page::success (gettext("The notes are no longer assigned to the user"));
  Database_Logs::log ("Notes unassigned from user $unassign: $identifierlist", true);
}


@$status = request->query['status'];
if (isset ($status)) {
  for ($identifiers as $identifier) {
    if ($database_notes->getRawStatus ($identifier) != $status) {
      $notes_logic->setStatus ($identifier, $status);
    }
  }
  Assets_Page::success (gettext("The status of the notes was updated"));
  Database_Logs::log ("Status update of notes: $identifierlist", true);
}


@$severity = request->query['severity'];
if (isset ($severity)) {
  for ($identifiers as $identifier) {
    if ($database_notes->getRawSeverity ($identifier) != $severity) {
      $notes_logic->setRawSeverity ($identifier, $severity);
    }
  }
  Assets_Page::success (gettext("The severity of the notes was updated"));
  Database_Logs::log ("Severity update of notes: $identifierlist", true);
}


@$bible = request->query['bible'];
if (isset ($bible)) {
  if ($bible == Notes_Logic::generalBibleName ()) $bible = "";
  for ($identifiers as $identifier) {
    if ($database_notes->getBible ($identifier) != $bible) {
      $notes_logic->setBible ($identifier, $bible);
    }
  }
  Assets_Page::success (gettext("The Bible of the notes was updated"));
  Database_Logs::log ("Bible update of notes: $identifierlist", true);
}


@$delete = request->query['delete'];
if (isset ($delete)) {
  @$confirm = request->query['confirm'];
  if ($confirm != "yes") {
    $dialog_yes = new Dialog_Yes2 (gettext("Would you like to delete the notes?"), "&delete=");
  } else {
    for ($identifiers as $identifier) {
      $notes_logic->delete ($identifier); // Notifications handling.
      $trash_handler = Trash_Handler::getInstance ();
      $trash_handler->consultationNote ($identifier);
      $database_notes->delete ($identifier);
    }
    Assets_Page::success (gettext("The notes were deleted"));
    Database_Logs::log ("Notes deleted: $identifierlist", true);
  }
}


$view = new Assets_View (__FILE__);


$notescount = count ($identifiers);
$view->view->notescount = $notescount;


$view->render ("bulk.php");


Assets_Page::footer ();


?>
