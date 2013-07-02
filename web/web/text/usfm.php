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


class Text_Usfm
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Text_Usfm();
    }
    return self::$instance;
  }

  public function actions ()
  {
    if (isset ($_GET ['usfmsavetext'])) {
      $database_config_user = Database_Config_User::getInstance();
      $database_snapshots = Database_Snapshots::getInstance();
      $database_bibles = Database_Bibles::getInstance();
      if (isset($_POST['submit'])) {
        $data = $_POST['data'];
        $data = trim ($data);
        if ($data != "") {
          if (Validate_Utf8::valid ($data)) {
            $stylesheet = $database_config_user->getStylesheet();
            $book_chapter_text = Filter_Usfm::import ($data, $stylesheet);
            foreach ($book_chapter_text as $data) {
              $book_number = $data[0];
              $chapter_number = $data[1];
              $chapter_data_to_save = $data[2];
              $bible = $_GET['bible'];
              $book = $_GET['book'];
              $chapter = $_GET['chapter'];
              if ((($book_number == $book) || ($book_number == 0)) && ($chapter_number == $chapter)) {
                $database_bibles->storeChapter ($bible, $book, $chapter, $chapter_data_to_save);
                $database_snapshots->snapChapter ($bible, $book, $chapter, $chapter_data_to_save, 0);
                Assets_Page::success (gettext ("Chapter was saved."));
              } else {
                Assets_Page::error (gettext ("The following data could not be saved and was discarded:") . $chapter_data_to_save);
              }
            }
          } else {
            Assets_Page::error (gettext ("The text was not valid Unicode UTF-8. The chapter could not saved and has been reverted to the last good version."));
          }
        } else {
          Assets_Page::error (gettext ("There was no text. Nothing was saved. The original text of the chapter was reloaded."));
        }
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
    $view->view->chapter_data = $chapter_data;
    $view->render ("usfm.php");
  }

}  


?>
