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

require_once ("../bootstrap/bootstrap.php");
$database_logs = Database_Logs::getInstance ();
$database_logs->log (gettext ("Generating lists of changes in the Bibles"), true);

// Security: The script runs from the cli SAPI only.
if (php_sapi_name () != "cli") {
  $database_logs->log ("Fatal: This only runs through the cli Server API", true);
  die;
}

$database_bibles = Database_Bibles::getInstance ();
$database_books = Database_Books::getInstance ();

// Go through all Bibles that have diffs.
$bibles = $database_bibles->getDiffBibles ();
foreach ($bibles as $bible) {
  // Go through all books in this $bible that have diffs.
  $biblename = $database_bibles->getName ($bible);
  $books = $database_bibles->getDiffBooks ($bible);
  foreach ($books as $book) {
    // Go through all chapters in this $bible and $book that have diffs.
    $bookname = $database_books->getEnglishFromId ($book);
    $chapters = $database_bibles->getDiffChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      $log = gettext ("Changes in");
      $database_logs->log ("$log $biblename $bookname $chapter", true);
    }
  }
}

/*
Todo working here.

*/

$database_logs->log (gettext ("The lists of changes in the Bibles have been generated"), true);

?>
