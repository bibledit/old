<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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
    $displayconsultationnotestartinglimit = $database_sessions->getConsultationNoteStartingLimit ();

    // Whether to display the notes list, one note, or the note's actions.
    if (isset ($_GET['displaynoteslist'])) {
      unset ($consultationnote);
      $displayconsultationnoteactions = false;
      $editconsultationnoteview = false;
    }
    if (isset ($_GET['showallconsultationnotes']))  {
      unset ($consultationnote);
      $displayconsultationnoteactions = false;
      $editconsultationnoteview = false;
    }
    if (isset ($_GET['consultationnote'])) {
      $consultationnote = $_GET['consultationnote'];
      $displayconsultationnoteactions = false;
      $editconsultationnoteview = false;
    }
    if (isset ($_GET['displaynotesactions'])) {
      $displayconsultationnoteactions = true;
      $editconsultationnoteview = false;
    }
    if (isset ($_GET['displaynotecontent'])) {
      $displayconsultationnoteactions = false;
      $editconsultationnoteview = false;
    }
    if (isset ($_GET['consultationnoteseditview'])) {
      $editconsultationnoteview = true;
      $displayconsultationnoteactions = false;
    }
    @$database_sessions->setConsultationNote ($consultationnote);
    $database_sessions->setDisplayConsultationNoteActions ($displayconsultationnoteactions);
    $database_sessions->setEditConsultationNoteView ($editconsultationnoteview);
    
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
    
    // Subscribe to the note.
    if (isset ($_GET['consultationnoteunsubscribe'])) {
      $database_notes->unsubscribe ($consultationnote);
    }

    // Unsubscribe from the note.
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
          $database_notes->assignUser ($consultationnote, $addassignee);
          $notes_logic->handlerUpdateNote ($consultationnote);
        }
      }
    }
    
    // Un-assign note.
    @$removeassignee = $_GET['consultationnoteremoveassignee'];
    if (isset ($removeassignee)) {
      $database_notes->unassignUser ($consultationnote, $removeassignee);
      $notes_logic->handlerUpdateNote ($consultationnote);
    }
    @$unassignme = $_GET['consultationnoteunassignme'];
    if (isset ($unassignme)) {
      // If a user unassigns himself, the automatic assignment setting should not assign him again.
      // For that reason the assignment logic is done before the user is unasssigned.
      $notes_logic->handlerUpdateNote ($consultationnote);
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
        $notes_logic->handlerUpdateNote ($consultationnote);
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
        $notes_logic->handlerUpdateNote ($consultationnote);
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
        $notes_logic->handlerUpdateNote ($consultationnote);
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
        $notes_logic->handlerUpdateNote ($consultationnote);
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
        $notes_logic->handlerUpdateNote ($consultationnote);
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
    @$status_selector = $_GET['consultationnotesstatusselector'];
    if (isset ($status_selector)) {
      $database_config_user->setConsultationNotesStatusSelector($status_selector);
    }
    @$bible_selector = $_GET['consultationnotesbibleselector'];
    if (isset ($bible_selector)) {
      if ($bible_selector == 1) $bible_selector = 1; else $bible_selector = 0;
      $database_config_user->setConsultationNotesBibleSelector($bible_selector);
    }
    @$assignment_selector = $_GET['consultationnotesassignmentselector'];
    if (isset ($assignment_selector)) {
      if (($assignment_selector < 0) || ($assignment_selector > 2)) $assignment_selector = 0;
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

  }
  
  public function display ()
  {
    $database_sessions = Database_Sessions::getInstance();
    $database_notes = Database_Notes::getInstance();
    $database_config_user = Database_Config_User::getInstance();
    $consultationnote = $database_sessions->getConsultationNote ();
    $displayconsultationnoteactions = $database_sessions->getDisplayConsultationNoteActions ();
    $editconsultationnoteview = $database_sessions->getEditConsultationNoteView ();
    $smarty = new Smarty_Bibledit (__FILE__);
    $caller = $_SERVER["PHP_SELF"];
    $smarty->assign ("caller", $caller);
    $smarty->assign ("session", $database_sessions->getCurrentSessionId ());
    $smarty->assign ("consultationnote", $consultationnote);
    $assets_navigator = Assets_Navigator::getInstance();
    // Values for $bible, $book and $chapter are passed to the template and on to any Save button.
    // This makes it more robust, keeping these values even in case someone navigates to some other verse.
    $bible = $assets_navigator->bible();
    $smarty->assign ("bible", $bible);
    $book = $assets_navigator->book();
    $smarty->assign ("book", $book);
    $chapter = $assets_navigator->chapter();
    $smarty->assign ("chapter", $chapter);
    $verse = $assets_navigator->verse();
    $smarty->assign ("verse", $verse);
    $passage_selector = $database_config_user->getConsultationNotesPassageSelector();
    $edit_selector = $database_config_user->getConsultationNotesEditSelector();
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
      $smarty->display ("create.tpl");
    } 
    else if ($consultationnote != "") {
      // Note display.
      $smarty->assign ("summary", $database_notes->getSummary($consultationnote));
      $smarty->assign ("identifier", $consultationnote);
      if ($displayconsultationnoteactions) {
        // Display note actions.
        $session_logic = Session_Logic::getInstance();
        $user = $session_logic->currentUser ();
        $subscribed = $database_notes->isSubscribed ($consultationnote, $user);
        $smarty->assign ("subscribed", $subscribed);
        $assignees = $database_notes->getAssignees ($consultationnote);
        $smarty->assign ("assignees", $assignees);
        $assignee = $database_notes->isAssigned ($consultationnote, $user);
        $smarty->assign ("assignee", $assignee);
        $status = $database_notes->getStatus ($consultationnote);
        $smarty->assign ("status", $status);
        $verses = Filter_Books::passagesDisplayInline ($database_notes->getPassages ($consultationnote));
        $smarty->assign ("verses", $verses);
        $severity = $database_notes->getSeverity ($consultationnote);
        $smarty->assign ("severity", $severity);
        $privacy = $database_notes->getPrivacy ($consultationnote);
        $privacy = Filter_Notes::privacy2text ($privacy);
        $smarty->assign ("privacy", $privacy);
        $consultationnotebible = $database_notes->getBible ($consultationnote);
        $smarty->assign ("consultationnotebible", $consultationnotebible);
        $smarty->display ("actions.tpl");
      } else {
        // Display one note.
        $contents = $database_notes->getContents($consultationnote);
        $smarty->assign ("note_content", $contents);
        @$smarty->assign ("note_add_comment", $_GET['addtoconsultationnote']);
        $smarty->display ("note.tpl");
      }
    } else if ($editconsultationnoteview) {
      // Display note view editor.
      $identifiers = $database_notes->selectNotes($bible, $book, $chapter, $verse, $passage_selector, $edit_selector, $status_selector, $bible_selector, $assignment_selector, $subscription_selector, $severity_selector, $text_selector, $search_text, NULL, 0);
      $totalcount = count ($identifiers);
      $smarty->assign ("totalcount", $totalcount);
      $smarty->assign ("passageselector", $passage_selector);
      $smarty->assign ("editselector", $edit_selector);
      $possible_statuses = $database_notes->getPossibleStatuses();
      foreach ($possible_statuses as $possible_status) {
        $status_ids [] = $possible_status[0];
        $status_localizations [] = $possible_status[1];
      }
      $smarty->assign ("statusids", $status_ids);
      $smarty->assign ("statuslocs", $status_localizations);
      $smarty->assign ("statusselector", $status_selector);
      $smarty->assign ("bibleselector", $bible_selector);
      $smarty->assign ("assignmentselector", $assignment_selector);
      $smarty->assign ("subscriptionselector", $subscription_selector);
      $smarty->assign ("severityselector", $severity_selector);
      $smarty->assign ("severities", $database_notes->getPossibleSeverities());
      $smarty->assign ("textselector", $text_selector);
      $smarty->assign ("searchtext", Filter_Html::sanitize ($search_text));
      $smarty->assign ("passageinclusionselector", $passage_inclusion_selector);
      $smarty->assign ("textinclusionselector", $text_inclusion_selector);
      $smarty->display ("editview.tpl");
    } else {
      // Display notes list.
      // Total notes count.
      $identifiers = $database_notes->selectNotes($bible, $book, $chapter, $verse, $passage_selector, $edit_selector, $status_selector, $bible_selector, $assignment_selector, $subscription_selector, $severity_selector, $text_selector, $search_text, NULL, 0);
      $totalcount = count ($identifiers);
      $smarty->assign ("totalcount", $totalcount);
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
        $identifiers = $database_notes->selectNotes($bible, $book, $chapter, $verse, $passage_selector, $edit_selector, $status_selector, $bible_selector, $assignment_selector, $subscription_selector, $severity_selector, $text_selector, $search_text, $startinglimit, 0);
        $displaycount = count ($identifiers);
      }
      $smarty->assign ("firstnote", $startinglimit + 1);
      $smarty->assign ("lastnote", $lastnote);
      if ($lastnote == 0) $smarty->assign ("firstnote", 0);
      // Note data.
      $smarty->assign ("identifiers", $identifiers);
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
      $smarty->assign ("summaries", $summaries);
      $smarty->assign ("versetexts", $verse_texts);
      $smarty->assign ("contents", $contents);
      // Display page.
      $smarty->display ("notes.tpl");
      if ($displaycount == 0) {
        Assets_Page::message (gettext ("This view does not display any notes."));
      }
    }
  }

}  


?>
