<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2012 Teus Benschop.
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

require_once ("../bootstrap/bootstrap.php");

$database_logs = Database_Logs::getInstance ();
$database_logs->log (gettext ("Import Bible data has started"), true);

// Security: Page only runs from the cli SAPI.
if (php_sapi_name () != "cli") {
  $database_logs->log ("Fatal: This only runs through the cli Server API", true);
  die;
}

$location = $argv[1];
$bible = $argv[2];
$database_logs->log ("Importing from location $location into Bible $bible" , true);

$files = array ();

if (is_dir ($location)) {
  $objects = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($location));
  foreach($objects as $name => $object) {
    if (is_file ($name)) {
      $files [] = $name;
    }
  }
} else {
  $files [] = $location;
}

$database_config_user = Database_Config_User::getInstance();
$stylesheet = $database_config_user->getStylesheet();

$database_bibles = Database_Bibles::getInstance();
$database_snapshots = Database_Snapshots::getInstance();
$database_books = Database_Books::getInstance ();

foreach ($files as $file) {
  $database_logs->log ("Examining file $file", true);
  $success_message = "";
  $error_message = "";
  $data = file_get_contents ($file);
  if ($data !== false) {
    if ($data != "") {
      if (Validate_Utf8::valid ($data)) {
        $book_chapter_text = Filter_Usfm::import ($data, $stylesheet);
        foreach ($book_chapter_text as $data) {
          $book_number = $data[0];
          $chapter_number = $data[1];
          $chapter_data = $data[2];
          if ($book_number > 0) {
            $database_bibles->storeChapter ($bible, $book_number, $chapter_number, $chapter_data);
            $database_snapshots->snapChapter ($bible, $book_number, $chapter_number, $chapter_data, 1);
            $book_name = $database_books->getUsfmFromId ($book_number);
            $success_message .= " $book_name $chapter_number"; 
          } else {
            $error_message .= " " . substr ($chapter_data, 0, 1000); 
          }
        }
        if ($error_message != "") {
          $error_message = "Could not import this data:" . $error_message;
          $database_logs->log ($error_message, true);
        }
        if ($success_message != "") {
          $success_message = "The following was imported:" . $success_message;
          $database_logs->log ($success_message, true);
        }
      } else {
        $database_logs->log ("The file does not contain valid Unicode UTF-8 text.", true);
      }
    } else {
      $database_logs->log ("Nothing was imported.", true);
    }
  }
 
}

$database_logs->log ("Import Bible data has finished", true);

?>
