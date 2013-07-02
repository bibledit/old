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


class Text_Editor
{
  public $edit_usfm_line_number;
  public $goto_line_number;
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Text_Editor();
    }
    return self::$instance;
  }

  public function actions ()
  {
    // Handle picking out which line to edit and to move the screen to.
    $this->edit_usfm_line_number = -1;
    $this->goto_line_number = -1;
    @$line_number = $_GET ['editusfmline'];
    if (isset ($line_number)) {
      if (is_numeric ($line_number)) {
        $this->edit_usfm_line_number = $line_number;
        $this->goto_line_number = $line_number;
      }
    }
    
    
    // Handle edited line submission.
    if (isset($_POST['submit'])) {
      // Get data related to what to save.
      $bible = $_GET['bible'];
      $book = $_GET['book'];
      $chapter = $_GET['chapter'];
      $line_number = $_GET ['textsaveline'];
      $this->goto_line_number = $line_number; // Line to go to after saving.
      $data = $_POST['data'];
      $data = trim ($data);
      if (Validate_Utf8::valid ($data)) {
        // One line was edited. 
        // It retrieves the chapter contents and inserts the line into it at the right position.
        $database_bibles = Database_Bibles::getInstance();
        $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
        $chapter_lines = explode ("\n", $chapter_data);
        $chapter_data = "";
        for ($i = 0; $i < count ($chapter_lines); $i++) {
          if ($line_number == $i) {
            $chapter_data .= $data;
          } else {
            $chapter_data .= $chapter_lines[$i] . "\n";
          }
        }
        // It runs the import routine on that chapter with the inserted line,
        // in order to tidy up anything that the user might have inserted.
        // This produces an array of an array of book/chapter/lines.
        $database_config_user = Database_Config_User::getInstance();
        $stylesheet = $database_config_user->getStylesheet();
        $chapter_data = Filter_Usfm::import ($chapter_data, $stylesheet);
        // Convert the array to a string with new lines.
        foreach ($chapter_data as $data) {
          @$chapter_to_save .= "$data[2]\n";
        }
        // Save data.
        $database_bibles->storeChapter ($bible, $book, $chapter, $chapter_to_save);
        $database_snapshots = Database_Snapshots::getInstance();
        $database_snapshots->snapChapter ($bible, $book, $chapter, $chapter_to_save, 0);
        Assets_Page::success (gettext ("The line was saved."));
      } else {
        Assets_Page::error (gettext ("The text was not valid Unicode UTF-8. The chapter could not saved and has been reverted to the last good version."));
      }
    } 

    else 
    
    // Handle updating the navigator with the verse that the user clicked.
    {
      if ($this->goto_line_number >= 0) {
        $assets_navigator = Assets_Navigator::getInstance();
        $bible = $assets_navigator->bible();
        $book = $assets_navigator->book();
        $chapter = $assets_navigator->chapter();
        $database_bibles = Database_Bibles::getInstance();
        $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
        $verse = Filter_Usfm::lineNumber2VerseNumber ($chapter_data, $this->goto_line_number);
        $ipc_focus = Ipc_Focus::getInstance();
        $ipc_focus->set ($book, $chapter, $verse);
      }
    }
  }
  
  public function display ()
  {
    $database_sessions = Database_Sessions::getInstance();
    $assets_navigator = Assets_Navigator::getInstance();
    $database_bibles = Database_Bibles::getInstance();
    $view = new Assets_View (__FILE__);
    $caller = $_SERVER["PHP_SELF"];
    $view->view->caller = $caller;
    $view->view->session = $database_sessions->getCurrentSessionId ();
    // Values for $bible, $book and $chapter are passed to the template and on to the Save button.
    // This makes it more robust. Else what could happen is that a chapter got loaded, 
    // then the navigator moved to somewhere else, then this chapter would be saved to the wrong place.
    // But when these values are passed to the Save button, it will get it right always.
    $bible = $assets_navigator->bible();
    $view->view->bible = $bible;
    $book = $assets_navigator->book();
    $view->view->book = $book;
    $chapter = $assets_navigator->chapter();
    $view->view->chapter = $chapter;
    $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
    if ($chapter_data == "") {
      Assets_Page::error (gettext ("This chapter does not exist or is empty"));
    }

    // Divide the chapter data into three lots: Data before the editor, data for the editor, and data after the editor.
    $line_data_before = array ();
    $line_numbers_before = array ();
    $line_data_after = array ();
    $line_numbers_after = array ();
    $lines = explode ("\n", $chapter_data);
    for ($i = 0; $i < count ($lines); $i++) {
      if ($i < $this->edit_usfm_line_number) {
        $line_data_before[] = Filter_Html::sanitize ($lines[$i]);
        $line_numbers_before[] = $i;
      } else if ($i == $this->edit_usfm_line_number) {
        $line_data_edit = $lines[$i];
      } else {
        $line_data_after[] = Filter_Html::sanitize ($lines[$i]);
        $line_numbers_after[] = $i;
      }
    }
    $view->view->line_data_before = $line_data_before;
    $view->view->line_numbers_before = $line_numbers_before;
    @$view->view->line_data_edit = $line_data_edit;
    @$view->view->line_number_edit = $this->edit_usfm_line_number;
    $view->view->line_data_after = $line_data_after;
    $view->view->line_numbers_after = $line_numbers_after;

    // Line number where to move the screen to.
    $view->view->goto_line = $this->goto_line_number;
    
    $view->render ("editor.php");
  }

}  


?>
