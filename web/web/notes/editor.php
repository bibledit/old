<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2013 Teus Benschop.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


class Notes_Editor
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Notes_Editor();
    }
    return self::$instance;
  }

  public function scripts ()
  {
    $code = "";
    if (isset ($_GET['createconsultationnote'])) {
      $code = "document.form.summary.focus();"; 
    }
    if (isset ($_GET['addtoconsultationnote'])) {
      $code = "document.form.comment.focus();"; 
    }
    return $code;
  }
  
  public function use_wysiwyg_editor ()
  {
    if (isset ($_GET['createconsultationnote'])) return true;
    if (isset ($_GET['addtoconsultationnote'])) return true;
    return false;
  }
  
  public function actions ()
  {
    $database_notes = Database_Notes::getInstance();
    $database_sessions = Database_Sessions::getInstance();
    $database_config_user = Database_Config_User::getInstance();
    $notes_logic = Notes_Logic::getInstance();
    $consultationnote = $database_sessions->getConsultationNote ();
    $displayconsultationnoteactions = $database_sessions->getDisplayConsultationNoteActions ();
    $editconsultationnoteview = $database_sessions->getEditConsultationNoteView ();
    $bulkupdateconsultationnotes = $database_sessions->getBulkUpdateConsultationNotes ();
    $displayconsultationnotestartinglimit = $database_sessions->getConsultationNoteStartingLimit ();

    // Whether to display the notes list, one note, or the note's actions.
    if (isset ($_GET['displaynoteslist'])) {
      unset ($consultationnote);
      $displayconsultationnoteactions = false;
      $editconsultationnoteview = false;
      $bulkupdateconsultationnotes = false;
    }
    if (isset ($_GET['showallconsultationnotes']))  {
      unset ($consultationnote);
      $displayconsultationnoteactions = false;
      $editconsultationnoteview = false;
      $bulkupdateconsultationnotes = false;
    }
    if (isset ($_GET['consultationnote'])) {
      $consultationnote = $_GET['consultationnote'];
      $displayconsultationnoteactions = false;
      $editconsultationnoteview = false;
      $bulkupdateconsultationnotes = false;
    }
    if (isset ($_GET['displaynotesactions'])) {
      $displayconsultationnoteactions = true;
      $editconsultationnoteview = false;
      $bulkupdateconsultationnotes = false;
    }
    if (isset ($_GET['displaynotecontent'])) {
      $displayconsultationnoteactions = false;
      $editconsultationnoteview = false;
      $bulkupdateconsultationnotes = false;
    }
    if (isset ($_GET['consultationnoteseditview'])) {
      $editconsultationnoteview = true;
      $displayconsultationnoteactions = false;
      $bulkupdateconsultationnotes = false;
    }
    if (isset ($_GET['consultationnotesbulkupdate'])) {
      $editconsultationnoteview = false;
      $displayconsultationnoteactions = false;
      $bulkupdateconsultationnotes = true;
    }
    @$database_sessions->setConsultationNote ($consultationnote);
    $database_sessions->setDisplayConsultationNoteActions ($displayconsultationnoteactions);
    $database_sessions->setEditConsultationNoteView ($editconsultationnoteview);
    $database_sessions->setBulkUpdateConsultationNotes ($bulkupdateconsultationnotes);
    
    // When a note is opened, then the passage navigator should go to the passage that belongs to that note.
    if (isset ($_GET['consultationnote'])) {
      // Variable $consultationnote has been set above. It contains the note identifier.
      $passages = $database_notes->getPassages ($consultationnote);
      if (is_array ($passages)) {
        if (!empty ($passages)) {
          $ipc_focus = Ipc_Focus::getInstance();
          $ipc_focus->set ($passages[0][0], $passages[0][1], $passages[0][2]);
        }
      }
    }
    
    // Save new note.
    if (isset ($_GET['savenewconsultationnote'])) {
      if (isset($_POST['submit'])) {
        $summary = trim ($_POST['summary']);
        $contents = trim ($_POST['contents']);
        if (($contents == "") && ($summary == "")) {
          Assets_Page::error (gettext ("There was nothing to save"));
        } else {
          $consultationnote = $database_notes->storeNewNote ($_GET['createnotebible'], $_GET['createnotebook'], $_GET['createnotechapter'], $_GET['createnoteverse'], $summary, $contents, false);
          $database_sessions->setConsultationNote ($consultationnote);
          $notes_logic->handlerNewNote ($consultationnote);
        }
      }
    }

    // Add comment to existing note.
    if (isset ($_GET['saveconsultationnotecomment'])) {
      if (isset($_POST['submit'])) {
        $comment = trim ($_POST['comment']);
        if ($comment == "") {
          Assets_Page::error (gettext ("There was nothing to save"));
        } else {
          $database_notes->addComment ($consultationnote, $comment);
          $notes_logic->handlerAddComment ($consultationnote);
        }
      }
    }

    // Edit summary.
    @$consultationnoteeditsummary = $_GET['consultationnoteeditsummary'];
    if (isset ($consultationnoteeditsummary)) {
      $dialog_entry = new Dialog_Entry (array ("session" => $database_sessions->getCurrentSessionId ()), gettext ("Please enter the summary"), $database_notes->getSummary ($consultationnote), "consultationnoteeditsummary", "");
      die;
    }
    @$consultationnoteeditsummary = $_POST['consultationnoteeditsummary'];
    if (isset ($consultationnoteeditsummary)) {
      $summary = $_POST['entry'];
      $database_notes->setSummary ($consultationnote, $summary);
      // $notes_logic->handlerUpdateNote ($consultationnote);
    }

    // Delete a note.
    @$deleteconsultationnote = $_GET['deleteconsultationnote'];
    if (isset ($deleteconsultationnote)) {
      $notes_logic->handlerDeleteNote ($deleteconsultationnote); // Notifications handling.
      $database_notes->delete ($deleteconsultationnote);
      $consultationnote = "";
      $database_sessions->setConsultationNote ($consultationnote);
      $displayconsultationnoteactions = false;
      $database_sessions->setDisplayConsultationNoteActions ($displayconsultationnoteactions);
    }
    
    // Unsubscribe from the note.
    if (isset ($_GET['consultationnoteunsubscribe'])) {
      $database_notes->unsubscribe ($consultationnote);
    }

    // Subscribe to the note.
    if (isset ($_GET['consultationnotesubscribe'])) {
      $database_notes->subscribe ($consultationnote);
    }
    
    // Add assignee.
    @$addassignee = $_GET['consultationnoteaddassignee'];
    if (isset ($addassignee)) {
      $database_users = Database_Users::getInstance();
      if ($addassignee == "") {
        $dialog_list = new Dialog_List2 (gettext ("Would you like to assign this note to a user?"));
        $users = $database_users->getUsers ();
        foreach ($users as $user) {
          $dialog_list->add_row ($user, "&consultationnoteaddassignee=$user");
        }
        $dialog_list->run();
      } else {
        if ($database_users->usernameExists ($addassignee)) {
          // Assign logic comes first.
          $notes_logic->handlerAssignNote ($consultationnote, $addassignee);
          $database_notes->assignUser ($consultationnote, $addassignee);
        }
      }
    }
    
    // Un-assign note.
    @$removeassignee = $_GET['consultationnoteremoveassignee'];
    if (isset ($removeassignee)) {
      $database_notes->unassignUser ($consultationnote, $removeassignee);
      // $notes_logic->handlerUpdateNote ($consultationnote);
    }
    @$unassignme = $_GET['consultationnoteunassignme'];
    if (isset ($unassignme)) {
      // If a user unassigns himself, the automatic assignment setting should not assign him again.
      // For that reason the assignment logic is done before the user is unasssigned.
      // $notes_logic->handlerUpdateNote ($consultationnote);
      $database_notes->unassign ($consultationnote);
    }

    // Assign to Bible.    
    @$consultationnotebible = $_GET['consultationnotechangebible'];
    if (isset ($consultationnotebible)) {
      $database_bibles = Database_Bibles::getInstance();
      if ($consultationnotebible == "") {
        $dialog_list = new Dialog_List2 (gettext ("Would you like to assign this note to another Bible?"));
        $bibles = $database_bibles->getBibles ();
        foreach ($bibles as $bible) {
          $dialog_list->add_row ($bible, "&consultationnotechangebible=$bible");
        }
        $dialog_list->add_row (gettext ("Make it a general note which does not apply to any particular Bible"), "&consultationnotechangebible=0gen0bible0");
        $dialog_list->run();
      } else {
        if ($consultationnotebible == "0gen0bible0") $consultationnotebible = "";
        $database_notes->setBible ($consultationnote, $consultationnotebible);
        // $notes_logic->handlerUpdateNote ($consultationnote);
      }
    }

    // Edit status.
    @$consultationnotestatus = $_GET['consultationnotestatus'];
    if (isset ($consultationnotestatus)) {
      if ($consultationnotestatus == "") {
        $dialog_list = new Dialog_List2 (gettext ("Would you like to change the status of this note?"));
        $statuses = $database_notes->getPossibleStatuses ();
        foreach ($statuses as $status) {
          $dialog_list->add_row ($status[1], "&consultationnotestatus=$status[0]");
        }
        $dialog_list->run();
      } else {
        $database_notes->setStatus ($consultationnote, $consultationnotestatus);
        // $notes_logic->handlerUpdateNote ($consultationnote);
      }
    }

    // Edit severity.
    @$consultationnoteseverity = $_GET['consultationnoteseverity'];
    if (isset ($consultationnoteseverity)) {
      if ($consultationnoteseverity == "") {
        $dialog_list = new Dialog_List2 (gettext ("Would you like to change the severity of this note?"));
        $severities = $database_notes->getPossibleSeverities ();
        foreach ($severities as $severity) {
          $dialog_list->add_row ($severity[1], "&consultationnoteseverity=$severity[0]");
        }
        $dialog_list->run();
      } else {
        $database_notes->setRawSeverity ($consultationnote, $consultationnoteseverity);
        // $notes_logic->handlerUpdateNote ($consultationnote);
      }
    }

    // Edit privacy. 
    @$consultationnoteprivacy = $_GET['consultationnoteprivacy'];
    if (isset ($consultationnoteprivacy)) {
      if ($consultationnoteprivacy == "") {
        $dialog_list = new Dialog_List2 (gettext ("Would you like to change the the visibility of this note?"));
        $dialog_list->info_top (gettext ("The note will be visible to:"));
        $privacies = $database_notes->getPossiblePrivacies ();
        foreach ($privacies as $privacy) {
          $dialog_list->add_row (Filter_Notes::privacy2text ($privacy), "&consultationnoteprivacy=$privacy");
        }
        $dialog_list->run();
      } else {
        $database_notes->setPrivacy ($consultationnote, $consultationnoteprivacy);
        // $notes_logic->handlerUpdateNote ($consultationnote);
      }
    }

    // Edit passages.
    @$consultationnoteeditverses = $_GET['consultationnoteeditverses'];
    if (isset ($consultationnoteeditverses)) {
      if (!isset($_POST['submit'])) {
        $text = Filter_Books::passagesDisplayMultiline ($database_notes->getPassages ($consultationnote));
        $dialog_text = new Dialog_Text (gettext ("Would you like to edit the verses?"), $text, "consultationnoteeditverses");
        $dialog_text->run ();
      } else {
        $lines = explode ("\n", $_POST['contents']);
        $passages = array ();
        foreach ($lines as $line) {
          $line = trim ($line);
          if ($line != "") {
            $passage = Filter_Books::explodePassage ($line);
            if ($passage[0] != 0) {
              $passages [] = $passage;
            }
          }
        }
        $database_notes->setPassages ($consultationnote, $passages);
        // $notes_logic->handlerUpdateNote ($consultationnote);
      }
    }
    
    // Note navigator.
    if (isset ($_GET['shownextconsultationnotes'])) {
      $startinglimit = $database_sessions->getConsultationNoteStartingLimit ();
      $database_sessions->setConsultationNoteStartingLimit ($startinglimit + 50);
    }    
    if (isset ($_GET['showpreviousconsultationnotes'])) {
      $startinglimit = $database_sessions->getConsultationNoteStartingLimit ();
      $startinglimit -= 50;
      if ($startinglimit < 0) $startinglimit = 0;
      $database_sessions->setConsultationNoteStartingLimit ($startinglimit);
    }    

    // Notes selector.
    @$passages_selector = $_GET['consultationnotespassageselector'];
    if (isset ($passages_selector)) {
      if (($passages_selector < 0) || ($passages_selector > 3)) $passages_selector = 0;
      $database_config_user->setConsultationNotesPassageSelector($passages_selector);
    }
    @$edit_selector = $_GET['consultationnoteseditselector'];
    if (isset ($edit_selector)) {
      if (($edit_selector < 0) || ($edit_selector > 4)) $edit_selector = 0;
      $database_config_user->setConsultationNotesEditSelector($edit_selector);
    }
    @$non_edit_selector = $_GET['consultationnotesnoneditselector'];
    if (isset ($non_edit_selector)) {
      if (($non_edit_selector < 0) || ($non_edit_selector > 5)) $non_edit_selector = 0;
      $database_config_user->setConsultationNotesNonEditSelector($non_edit_selector);
    }
    @$status_selector = $_GET['consultationnotesstatusselector'];
    if (isset ($status_selector)) {
      $database_config_user->setConsultationNotesStatusSelector($status_selector);
    }
    @$bible_selector = $_GET['consultationnotesbibleselector'];
    if (isset ($bible_selector)) {
      $database_config_user->setConsultationNotesBibleSelector($bible_selector);
      // Also set the Bible.
      if ($bible_selector != "") {
        $database_config_user->setBible($bible_selector);
        $database_sessions->setBible ($bible_selector);
      }
    }
    @$assignment_selector = $_GET['consultationnotesassignmentselector'];
    if (isset ($assignment_selector)) {
      $database_config_user->setConsultationNotesAssignmentSelector($assignment_selector);
    }
    @$subscription_selector = $_GET['consultationnotessubscriptionselector'];
    if (isset ($subscription_selector)) {
      if ($subscription_selector == 1) $subscription_selector = 1; else $subscription_selector = 0;
      $database_config_user->setConsultationNotesSubscriptionSelector($subscription_selector);
    }
    @$severity_selector = $_GET['consultationnotesseverityselector'];
    if (isset ($severity_selector)) {
      $database_config_user->setConsultationNotesSeveritySelector($severity_selector);
    }
    @$text_selector = $_GET['consultationnotestextselector'];
    if (isset ($text_selector)) {
      $database_config_user->setConsultationNotesTextSelector($text_selector);
      @$search_text = $_POST['text'];
      if (isset ($search_text)) {
        $database_config_user->setConsultationNotesSearchText($search_text);
        Assets_Page::success (gettext ("Search text saved"));
      }
    }
    @$passage_inclusion_selector = $_GET['consultationnotespassageinclusionyselector'];
    if (isset ($passage_inclusion_selector)) {
      $database_config_user->setConsultationNotesPassageInclusionSelector($passage_inclusion_selector);
    }
    @$text_inclusion_selector = $_GET['consultationnotestextinclusionyselector'];
    if (isset ($text_inclusion_selector)) {
      $database_config_user->setConsultationNotesTextInclusionSelector($text_inclusion_selector);
    }
    
    // Bulk note actions.
    @$bulk_update = $_GET['consultationnotesbulkupdate'];
    if (isset ($bulk_update)) {
      $assets_navigator = Assets_Navigator::getInstance();
      $identifiers = $database_notes->selectNotes($assets_navigator->bible(), $assets_navigator->book(), $assets_navigator->chapter(), $assets_navigator->verse(), 
                                                  $database_config_user->getConsultationNotesPassageSelector(), 
                                                  $database_config_user->getConsultationNotesEditSelector(),
                                                  $database_config_user->getConsultationNotesNonEditSelector(),
                                                  $database_config_user->getConsultationNotesStatusSelector(), 
                                                  $database_config_user->getConsultationNotesBibleSelector(), 
                                                  $database_config_user->getConsultationNotesAssignmentSelector(), 
                                                  $database_config_user->getConsultationNotesSubscriptionSelector(), 
                                                  $database_config_user->getConsultationNotesSeveritySelector(), 
                                                  $database_config_user->getConsultationNotesTextSelector(), 
                                                  $database_config_user->getConsultationNotesSearchText(), NULL, 0);
      foreach ($identifiers as $identifier) {
        @$identifierlist .= " $identifier";
      }                                          
      $database_logs = Database_Logs::getInstance ();
      // Bulk subscriptions.
      if ($bulk_update == "subscribe") {
        foreach ($identifiers as $identifier) {
          $database_notes->subscribe ($identifier);
        }
        Assets_Page::success (gettext ("You subscribed to these notes"));
      }
      // Bulk unsubscribe.
      if ($bulk_update == "unsubscribe") {
        foreach ($identifiers as $identifier) {
          $database_notes->unsubscribe ($identifier);
        }
        Assets_Page::success (gettext ("You unsubscribed from these notes"));
      }
      // Bulk assignment.
      if ($bulk_update == "assign") {
        @$assignee = $_GET['assignee'];
        $database_users = Database_Users::getInstance();
        if ($assignee == "") {
          $dialog_list = new Dialog_List2 (gettext ("Would you like to assign these notes to a user?"));
          $users = $database_users->getUsers ();
          foreach ($users as $user) {
            $dialog_list->add_row ($user, "&consultationnotesbulkupdate=assign&assignee=$user");
          }
          $dialog_list->run();
        } else {
          if ($database_users->usernameExists ($assignee)) {
            foreach ($identifiers as $identifier) {
              if (!$database_notes->isAssigned ($identifier, $assignee)) {
                // Assign logic comes first.
                $notes_logic->handlerAssignNote ($identifier, $assignee);
                $database_notes->assignUser ($identifier, $assignee);
              }
            }
          }
          Assets_Page::success (gettext ("The notes were assigned to the user"));
          $database_logs->log ("Notes assigned to user $assignee: $identifierlist", true);
        }        
      }
      // Bulk unassignment.
      if ($bulk_update == "unassign") {
        @$assignee = $_GET['assignee'];
        $database_users = Database_Users::getInstance();
        if ($assignee == "") {
          $dialog_list = new Dialog_List2 (gettext ("Would you like to remove the assignment of these notes to a user?"));
          $users = $database_users->getUsers ();
          foreach ($users as $user) {
            $dialog_list->add_row ($user, "&consultationnotesbulkupdate=unassign&assignee=$user");
          }
          $dialog_list->run();
        } else {
          if ($database_users->usernameExists ($assignee)) {
            foreach ($identifiers as $identifier) {
              if ($database_notes->isAssigned ($identifier, $assignee)) {
                $database_notes->unassignUser ($identifier, $assignee);
                // $notes_logic->handlerUpdateNote ($identifier);
              }
            }
          }
          Assets_Page::success (gettext ("The notes are no longer assigned to the user"));
          $database_logs->log ("Notes unassigned from user $assignee: $identifierlist", true);
        }        
      }
      // Bulk note status update.
      if ($bulk_update == "status") {
        @$status = $_GET['status'];
        if ($status == "") {
          $dialog_list = new Dialog_List2 (gettext ("Would you like to change the status of these notes?"));
          $statuses = $database_notes->getPossibleStatuses ();
          foreach ($statuses as $status) {
            $dialog_list->add_row ($status[1], "&consultationnotesbulkupdate=status&status=$status[0]");
          }
          $dialog_list->run();
        } else {
          foreach ($identifiers as $identifier) {
            if ($database_notes->getRawStatus ($identifier) != $status) {
              $database_notes->setStatus ($identifier, $status);
              // $notes_logic->handlerUpdateNote ($identifier);
            }
          }
          Assets_Page::success (gettext ("The status of the notes was updated"));
          $database_logs->log ("Status update of notes: $identifierlist", true);
        }
      }
      // Bulk note severity update.
      if ($bulk_update == "severity") {
        @$severity = $_GET['severity'];
        if ($severity == "") {
          $dialog_list = new Dialog_List2 (gettext ("Would you like to change the severity of these notes?"));
          $severities = $database_notes->getPossibleSeverities ();
          foreach ($severities as $severity) {
            $dialog_list->add_row ($severity[1], "&consultationnotesbulkupdate=severity&severity=$severity[0]");
          }
          $dialog_list->run();
        } else {
          foreach ($identifiers as $identifier) {
            if ($database_notes->getRawSeverity ($identifier) != $severity) {
              $database_notes->setRawSeverity ($identifier, $severity);
              // $notes_logic->handlerUpdateNote ($identifier);
            }
          }
          Assets_Page::success (gettext ("The severity of the notes was updated"));
          $database_logs->log ("Severity update of notes: $identifierlist", true);
        }
      }
      // Bulk note visibility update.
      if ($bulk_update == "visibility") {
        @$visibility = $_GET['visibility'];
        if ($visibility == "") {
          $dialog_list = new Dialog_List2 (gettext ("Would you like to change the visibility of these notes?"));
          $dialog_list->info_top (gettext ("The notes should be visible to:"));
          $privacies = $database_notes->getPossiblePrivacies ();
          foreach ($privacies as $visibility) {
            $dialog_list->add_row (Filter_Notes::privacy2text ($visibility), "&consultationnotesbulkupdate=visibility&visibility=$visibility");
          }
          $dialog_list->run();
        } else {
          foreach ($identifiers as $identifier) {
            if ($database_notes->getPrivacy ($identifier) != $visibility) {
              $database_notes->setPrivacy ($identifier, $visibility);
              // $notes_logic->handlerUpdateNote ($identifier);
            }
          }
          Assets_Page::success (gettext ("The visibility of the notes was updated"));
          $database_logs->log ("Visibility update of notes: $identifierlist", true);
        }
      }
      // Bulk note Bible update.
      if ($bulk_update == "bible") {
        @$bible = $_GET['bible'];
        if ($bible == "") {
          $database_bibles = Database_Bibles::getInstance ();
          $dialog_list = new Dialog_List2 (gettext ("Would you like to assign these notes to another Bible?"));
          $bibles = $database_bibles->getBibles ();
          foreach ($bibles as $bible) {
            $dialog_list->add_row ($bible, "&consultationnotesbulkupdate=bible&bible=$bible");
          }
          $dialog_list->add_row (gettext ("Make it a general note which does not apply to any particular Bible"), "&consultationnotesbulkupdate=bible&bible=0gen0bible0");
          $dialog_list->run();
        } else {
          if ($bible == "0gen0bible0") $bible = "";
          foreach ($identifiers as $identifier) {
            if ($database_notes->getBible ($identifier) != $bible) {
              $database_notes->setBible ($identifier, $bible);
              // $notes_logic->handlerUpdateNote ($identifier);
            }
          }
          Assets_Page::success (gettext ("The Bible of the notes was updated"));
          $database_logs->log ("Bible update of notes: $identifierlist", true);
        }
      }
      // Bulk note delete.
      if ($bulk_update == "delete") {
        if ($_GET['confirm'] != "yes") {
          $dialog_yes = new Dialog_Yes2 (gettext ("Would you like to delete the notes?"), "&consultationnotesbulkupdate=delete");
        } else {
          foreach ($identifiers as $identifier) {
            $notes_logic->handlerDeleteNote ($identifier); // Notifications handling.
            $database_notes->delete ($identifier);
          }
          Assets_Page::success (gettext ("The notes were deleted"));
          $database_logs->log ("Notes deleted: $identifierlist", true);
        }
      }
    }
  }
  
  public function display ()
  {
    $database_sessions = Database_Sessions::getInstance();
    $database_notes = Database_Notes::getInstance();
    $database_config_user = Database_Config_User::getInstance();
    $consultationnote = $database_sessions->getConsultationNote ();
    $displayconsultationnoteactions = $database_sessions->getDisplayConsultationNoteActions ();
    $editconsultationnoteview = $database_sessions->getEditConsultationNoteView ();
    $bulkupdateconsultationnotes = $database_sessions->getBulkUpdateConsultationNotes ();
    $view = new Assets_View (__FILE__);
    $caller = $_SERVER["PHP_SELF"];
    $view->view->caller = $caller;
    $view->view->session = $database_sessions->getCurrentSessionId ();
    $view->view->consultationnote = $consultationnote;
    $assets_navigator = Assets_Navigator::getInstance();
    // Values for $bible, $book and $chapter are passed to the template and on to any Save button.
    // This makes it more robust, keeping these values even in case someone navigates to some other verse.
    $bible = $assets_navigator->bible();
    $view->view->bible = $bible;
    $book = $assets_navigator->book();
    $view->view->book = $book;
    $chapter = $assets_navigator->chapter();
    $view->view->chapter = $chapter;
    $verse = $assets_navigator->verse();
    $view->view->verse = $verse;
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

    if (isset ($_GET['createconsultationnote'])) {
      // New note creation display.
      $view->render ("create.php");
    } 
    else if ($consultationnote != "") {
      // Note display.
      $view->view->summary = $database_notes->getSummary($consultationnote);
      $view->view->identifier = $consultationnote;
      if ($displayconsultationnoteactions) {
        // Display note actions.
        $session_logic = Session_Logic::getInstance();
        $user = $session_logic->currentUser ();
        $subscribed = $database_notes->isSubscribed ($consultationnote, $user);
        $view->view->subscribed = $subscribed;
        $assignees = $database_notes->getAssignees ($consultationnote);
        $view->view->assignees = $assignees;
        $assignee = $database_notes->isAssigned ($consultationnote, $user);
        $view->view->assignee = $assignee;
        $status = $database_notes->getStatus ($consultationnote);
        $view->view->status = $status;
        $verses = Filter_Books::passagesDisplayInline ($database_notes->getPassages ($consultationnote));
        $view->view->verses = $verses;
        $severity = $database_notes->getSeverity ($consultationnote);
        $view->view->severity = $severity;
        $privacy = $database_notes->getPrivacy ($consultationnote);
        $privacy = Filter_Notes::privacy2text ($privacy);
        $view->view->privacy = $privacy;
        $consultationnotebible = $database_notes->getBible ($consultationnote);
        $view->view->consultationnotebible = $consultationnotebible;
        $view->render ("actions.php");
      } else {
        // Display one note.
        $contents = $database_notes->getContents($consultationnote);
        $view->view->note_content = $contents;
        @$view->view->note_add_comment = $_GET['addtoconsultationnote'];
        $view->render ("note.php");
      }
    } else if ($editconsultationnoteview) {
      // Display note view editor.
      $identifiers = $database_notes->selectNotes($bible, $book, $chapter, $verse, $passage_selector, $edit_selector, $non_edit_selector, $status_selector, $bible_selector, $assignment_selector, $subscription_selector, $severity_selector, $text_selector, $search_text, NULL, 0);
      $totalcount = count ($identifiers);
      $view->view->totalcount = $totalcount;
      $view->view->passageselector = $passage_selector;
      $view->view->editselector = $edit_selector;
      $view->view->noneditselector = $non_edit_selector;
      $possible_statuses = $database_notes->getPossibleStatuses();
      foreach ($possible_statuses as $possible_status) {
        $status_ids [] = $possible_status[0];
        $status_localizations [] = $possible_status[1];
      }
      $view->view->statusids = $status_ids;
      $view->view->statuslocs = $status_localizations;
      $view->view->statusselector = $status_selector;
      $view->view->bibleselector = $bible_selector;
      $bibles = $database_notes->getAllBibles ();
      $view->view->bibles = $bibles;
      $view->view->assignmentselector = $assignment_selector;
      $assignees = $database_notes->getAllAssignees();
      $view->view->assignees = $assignees;
      if ($assignment_selector != "") {
        if (!in_array ($assignment_selector, $assignees)) {
          $view->view->nonexistingassignee = true;
        }
      }
      $view->view->subscriptionselector = $subscription_selector;
      $view->view->severityselector = $severity_selector;
      $view->view->severities = $database_notes->getPossibleSeverities();
      $view->view->textselector = $text_selector;
      $view->view->searchtext = Filter_Html::sanitize ($search_text);
      $view->view->passageinclusionselector = $passage_inclusion_selector;
      $view->view->textinclusionselector = $text_inclusion_selector;
      $view->render ("editview.php");
    } else if ($bulkupdateconsultationnotes) {
      $identifiers = $database_notes->selectNotes($bible, $book, $chapter, $verse, $passage_selector, $edit_selector, $non_edit_selector, $status_selector, $bible_selector, $assignment_selector, $subscription_selector, $severity_selector, $text_selector, $search_text, NULL, 0);
      $notescount = count ($identifiers);
      $view->view->notescount = $notescount;
      $view->render ("bulkupdate.php");
    } else {
      // Display notes list.
      // Total notes count.
      $identifiers = $database_notes->selectNotes($bible, $book, $chapter, $verse, $passage_selector, $edit_selector, $non_edit_selector, $status_selector, $bible_selector, $assignment_selector, $subscription_selector, $severity_selector, $text_selector, $search_text, NULL, 0);
      $totalcount = count ($identifiers);
      $view->view->totalcount = $totalcount;
      // First and last note to display, and notes count.
      if (isset ($_GET['showallconsultationnotes'])) {
        $startinglimit = 0;
        $lastnote = $totalcount;
        $displaycount = $totalcount;
      } else {
        $startinglimit = $database_sessions->getConsultationNoteStartingLimit ();
        if (!is_numeric ($startinglimit)) $startinglimit = 0;
        if ($startinglimit >= $totalcount) $startinglimit = $totalcount - 50;
        if ($startinglimit < 0) $startinglimit = 0;
        $lastnote = $startinglimit + 50;
        if ($lastnote > $totalcount) $lastnote = $totalcount;
        $identifiers = $database_notes->selectNotes($bible, $book, $chapter, $verse, $passage_selector, $edit_selector, $non_edit_selector, $status_selector, $bible_selector, $assignment_selector, $subscription_selector, $severity_selector, $text_selector, $search_text, $startinglimit, 0);
        $displaycount = count ($identifiers);
      }
      $view->view->firstnote = $startinglimit + 1;
      $view->view->lastnote = $lastnote;
      if ($lastnote == 0) $view->view->firstnote = 0;
      // Note data.
      $view->view->identifiers = $identifiers;
      $summaries = array ();
      $verse_texts = array ();
      $contents = array ();
      foreach ($identifiers as $identifier) {
        // Show summary.
        $summary = $database_notes->getSummary ($identifier);
        // Show passages as well.
        $passages = $database_notes->getPassages ($identifier);
        $verses = Filter_Books::passagesDisplayInline ($passages);
        $summaries[] = $summary . " | " . $verses;
        $verse_text = "";
        if ($passage_inclusion_selector) {
          $database_bibles = Database_Bibles::getInstance();
          $passages = $database_notes->getPassages ($identifier);
          foreach ($passages as $passage) {
            $usfm = $database_bibles->getChapter ($bible, $passage[0], $passage[1]);
            $text = Filter_Usfm::getVerseText ($usfm, $passage[2]);
            $verse_text .= $text;
            $verse_text .= "\n";
          }
        }
        $verse_texts [] = nl2br ($verse_text);
        if ($text_inclusion_selector) {
          $content = $database_notes->getContents($identifier);
        } else {
          $content = "";
        }
        $contents[] = $content;
      }
      $view->view->summaries = $summaries;
      $view->view->versetexts = $verse_texts;
      $view->view->contents = $contents;
      // Display page.
      $view->render ("notes.php");
      if ($displaycount == 0) {
        Assets_Page::message (gettext ("This view does not display any notes."));
      }
    }
  }

}  


?>
