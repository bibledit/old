<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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

$database_logs = Database_Logs::getInstance ();
$database_logs->log (gettext ("Import Bible data has started"), true);

// Security: Page only runs from the cli SAPI.
if (php_sapi_name () != "cli") {
  $database_logs->log ("Fatal: This only runs through the cli Server API", true);
  die;
}

ignore_user_abort (true);
set_time_limit (0);

$location = $argv[1];
$bible = $argv[2];
$database_logs->log ("Importing from location $location into Bible $bible" , true);

$folder = Filter_Archive::uncompress ($file, true);
if ($folder != NULL) $location = $folder;
unset ($folder);

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
$database_logs = Database_Logs::getInstance ();


foreach ($files as $file) {
  $database_logs->log ("Examining file for import: $file", true);
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
            $database_logs->log ("Imported $book_name $chapter_number", MANAGER_LEVEL); 
          } else {
            $database_logs->log ("Could not import this data: " . substr ($chapter_data, 0, 1000), MANAGER_LEVEL); 
          }
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
