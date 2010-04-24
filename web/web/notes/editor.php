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

  public function scripts () // Todo work here.
  {
    if (isset ($_GET['createconsultationnote'])) {
      $code = "document.form.summary.focus();"; 
    }
    if (isset ($_GET['addtoconsultationnote'])) {
      $code = "document.form.comment.focus();"; 
    }
    return $code;
  }
  
  public function actions () // Todo working here.
  {
    $database_notes = Database_Notes::getInstance();
    $database_sessions = Database_Sessions::getInstance();
    $consultationnote = $database_sessions->getConsultationNote ();

    // Whether note to display.
    if (isset ($_GET['consultationnote'])) {
      $consultationnote = $_GET['consultationnote'];
      $database_sessions->setConsultationNote ($consultationnote);
    }
    if (isset ($_GET['displaynoteslist'])) {
      unset ($consultationnote);
      $database_sessions->setConsultationNote ($consultationnote);
    }
    
    // Save new note.
    if (isset ($_GET['savenewconsultationnote'])) {
      if (isset($_POST['submit'])) {
        $summary = trim ($_POST['summary']);
        $contents = trim ($_POST['contents']);
        if (($contents == "") && ($summary == "")) {
          Assets_Page::error (gettext ("There was nothing to save"));
        } else {
          $consultationnote = $database_notes->storeNewNote ($_GET['createnotebible'], $_GET['createnotebook'], $_GET['createnotechapter'], $_GET['createnoteverse'], $summary, $contents);
          $database_sessions->setConsultationNote ($consultationnote);
          Assets_Page::success (gettext ("The note was saved"));
          $notes_logic = Notes_Logic::getInstance();
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
          Assets_Page::success (gettext ("The comment was added to the note"));
          $notes_logic = Notes_Logic::getInstance();
          $notes_logic->handlerCommentNote ($consultationnote); // Todo
        }
      }
    }
   
    // Delete a note.
    $deleteconsultationnote = $_GET['deleteconsultationnote'];
    if (isset ($deleteconsultationnote)) {
      $notes_logic = Notes_Logic::getInstance();
      $notes_logic->handlerDeleteNote ($deleteconsultationnote); // Todo
      $database_notes->delete ($deleteconsultationnote);
      unset ($consultationnote);
      $database_sessions->setConsultationNote ($consultationnote);
    }
    
    // Subscribe to the note.
    if (isset ($_GET['consultationnoteunsubscribe'])) {
      $database_notes->unsubscribe ($consultationnote);
    }

    // Unsubscribe from the note.
    if (isset ($_GET['consultationnotesubscribe'])) {
      $database_notes->subscribe ($consultationnote);
    }
  }
  
  public function display () // Todo working here.
  {
    $database_sessions = Database_Sessions::getInstance();
    $consultationnote = $database_sessions->getConsultationNote ();
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

    // New note creation display.
    if (isset ($_GET['createconsultationnote'])) {
      $smarty->display ("create.tpl");
      return;
    }

    // Note or notes display.
    $database_notes = Database_Notes::getInstance();
    if ($consultationnote != "") {
      // Display one note.
      $smarty->assign ("note_summary", $database_notes->getSummary($display_consultation_note_identifier));
      $contents = $database_notes->getContents($consultationnote);
      $smarty->assign ("note_content", $contents);
      $smarty->assign ("note_add_comment", $_GET['addtoconsultationnote']);
      $session_logic = Session_Logic::getInstance();
      $user = $session_logic->currentUser ();
      $subscribed = $database_notes->isSubscribed ($consultationnote, $user);
      $smarty->assign ("subscribed", $subscribed);
      // Todo
      $smarty->display ("note.tpl");
    } else {
      // Display notes summaries.
      $identifiers = $database_notes->selectNotes();
      $smarty->assign ("identifiers", $identifiers);
      foreach ($identifiers as $identifier) {
        $summary = $database_notes->getSummary ($identifier);
        $summaries[] = $summary;
      }
      $smarty->assign ("summaries", $summaries);
      $smarty->display ("notes.tpl");
      if (count ($identifiers) == 0) {
        Assets_Page::message (gettext ("This view does not display any notes."));
      }
    }
  }

}  


?>
