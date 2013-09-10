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
$database_versifications = Database_Versifications::getInstance ();
$database_books = Database_Books::getInstance ();
$database_offlineresources = Database_OfflineResources::getInstance ();


$resource = $argv [1];


$database_logs->log ("download: Starting to download resource $resource");


// Security: Page only runs from the cli SAPI.
if (php_sapi_name () != "cli") {
  $database_logs->log ("download: Fatal: This only runs through the cli Server API", true);
  die;
}


$versification = "English";
$books = $database_versifications->getBooks ($versification);
foreach ($books as $book) {
  $bookName = $database_books->getEnglishFromId ($book);
  $chapters = $database_versifications->getChapters ($versification, $book, true);
  foreach ($chapters as $chapter) {
    $message = "download: $bookName chapter $chapter";
    $verses = $database_versifications->getVerses ($versification, $book, $chapter);
    foreach ($verses as $verse) {
      $message .= "; verse $verse: ";
      if ($database_offlineresources->exists ($resource, $book, $chapter, $verse)) {
        $message .= "exists";
      } else {
        $html = Resource_Logic::getExternal ($resource, $book, $chapter, $verse);
        $database_offlineresources->store ($resource, $book, $chapter, $verse, $html);
        $size = strlen ($html);
        $message .= "size $size";
      }
    }
    $message .= "; done";
    $database_logs->log ($message);
  }
}


$database_logs->log (gettext ("download: Completed"), true);


?>
