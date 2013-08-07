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
    // This forwards links in emails to the new page for displaying one note.
    // It can go out as from bibledit-web version 1.2.
    if (isset ($_GET['consultationnote'])) {
      $id = $_GET['consultationnote'];
      header ("Location: ../notes/note.php?id=$id");
      die;
    }
    @$database_sessions->setConsultationNote ($consultationnote);
    $database_sessions->setDisplayConsultationNoteActions ($displayconsultationnoteactions);
    $database_sessions->setEditConsultationNoteView ($editconsultationnoteview);
    $database_sessions->setBulkUpdateConsultationNotes ($bulkupdateconsultationnotes);
    
    
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
    
    
    // Bulk note actions. Todo
    @$bulk_update = $_GET['consultationnotesbulkupdate'];
    if (isset ($bulk_update)) {
      // Bulk note delete.Todo
      if ($bulk_update == "delete") {
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

    if ($consultationnote != "") {
      // Note display.
      $view->view->summary = $database_notes->getSummary($consultationnote);
      $view->view->identifier = $consultationnote;
      if ($displayconsultationnoteactions) {
      } else {
        // Display one note.
        $contents = $database_notes->getContents($consultationnote);
        $view->view->note_content = $contents;
        $view->view->id = $consultationnote;
        $view->render ("note.php");
      }
    } else if ($editconsultationnoteview) {
      // Display note view editor.
      $identifiers = $database_notes->selectNotes($bible, $book, $chapter, $verse, $passage_selector, $edit_selector, $non_edit_selector, $status_selector, $bible_selector, $assignment_selector, $subscription_selector, $severity_selector, $text_selector, $search_text, NULL);
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
    } else {
      // Display notes list.
      // Total notes count.
      $identifiers = $database_notes->selectNotes($bible, $book, $chapter, $verse, $passage_selector, $edit_selector, $non_edit_selector, $status_selector, $bible_selector, $assignment_selector, $subscription_selector, $severity_selector, $text_selector, $search_text, NULL);
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
        $identifiers = $database_notes->selectNotes($bible, $book, $chapter, $verse, $passage_selector, $edit_selector, $non_edit_selector, $status_selector, $bible_selector, $assignment_selector, $subscription_selector, $severity_selector, $text_selector, $search_text, $startinglimit);
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
