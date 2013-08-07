<?php

// Todo tearing down and transferring functionallity.
  
  public function actions ()
  {
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
  }
  
  public function display ()
  {
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
  }

  


?>
