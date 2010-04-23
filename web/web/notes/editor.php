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

    // Save new note.
    if (isset ($_GET['savecreatedconsultationnote'])) {
      if (isset($_POST['submit'])) {
        $summary = trim ($_POST['summary']);
        $contents = trim ($_POST['contents']);
        if (($contents == "") && ($summary == "")) {
          Assets_Page::error (gettext ("There was nothing to save"));
        } else {
          $identifier = $database_notes->storeNewNote ($_GET['createnotebible'], $_GET['createnotebook'], $_GET['createnotechapter'], $_GET['createnoteverse'], $summary, $contents);
          Assets_Page::success (gettext ("The note was saved"));
          $notes_logic = Notes_Logic::getInstance();
          $notes_logic->handlerNewNote ($identifier);
        }
      }
    }

    // Add comment to existing note.
    if (isset ($_GET['saveconsultationnotecomment'])) {
      $display_consultation_note_identifier = $_GET['displayconsultationnoteidentifier'];
      if (isset ($display_consultation_note_identifier)) {
        if (isset($_POST['submit'])) {
          $comment = trim ($_POST['comment']);
          if ($comment == "") {
            Assets_Page::error (gettext ("There was nothing to save"));
          } else {
            $database_notes->addComment ($display_consultation_note_identifier, $comment);
            Assets_Page::success (gettext ("The comment was added to the note"));
            $notes_logic = Notes_Logic::getInstance();
            $notes_logic->handlerCommentNote ($display_consultation_note_identifier); // Todo
          }
        }
      }
    }
   
    // Delete a note.
    $deleteconsultationnote = $_GET['deleteconsultationnote'];
    if (isset ($deleteconsultationnote)) {
      $database_notes->delete ($deleteconsultationnote);
    }
  }
  
  public function display () // Todo working here.
  {
    $database_sessions = Database_Sessions::getInstance();
    $smarty = new Smarty_Bibledit (__FILE__);
    $caller = $_SERVER["PHP_SELF"];
    $smarty->assign ("caller", $caller);
    $smarty->assign ("session", $database_sessions->getCurrentSessionId ());
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
    $display_consultation_note_identifier = $_GET['displayconsultationnoteidentifier'];
    if (isset ($display_consultation_note_identifier)) {
      // Display one note.
      $smarty->assign ("consultation_note_identifier", $display_consultation_note_identifier);
      $smarty->assign ("note_summary", $database_notes->getSummary($display_consultation_note_identifier));
      $contents = $database_notes->getContents($display_consultation_note_identifier);
      $smarty->assign ("note_content", $contents);
      $smarty->assign ("note_add_comment", $_GET['addtoconsultationnote']);
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
