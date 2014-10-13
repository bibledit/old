<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

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


// Security: Page only runs from the cli SAPI.
Filter_Cli::assert ();


$database_logs = Database_Logs::getInstance ();
$database_logs->log (Locale_Translate::_("Import Bible data has started"), true);


ignore_user_abort (true);
set_time_limit (0);


$location = Filter_Cli::argument (@$argv, 1);
$bible = Filter_Cli::argument (@$argv, 2);


$database_logs->log ("Importing from location $location into Bible $bible" , true);


$folder = Filter_Archive::uncompress ($location, true);
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
            Bible_Logic::storeChapter ($bible, $book_number, $chapter_number, $chapter_data);
            $book_name = $database_books->getUsfmFromId ($book_number);
            $database_logs->log ("Imported $book_name $chapter_number", Filter_Roles::MANAGER_LEVEL);
          } else {
            $database_logs->log ("Could not import this data: " . substr ($chapter_data, 0, 1000), Filter_Roles::MANAGER_LEVEL);
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
