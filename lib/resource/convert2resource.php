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


// Security: The script runs from the cli SAPI only.
Filter_Cli::assert ();


$database_logs = Database_Logs::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_usfmresources = Database_UsfmResources::getInstance ();
$database_books = Database_Books::getInstance ();


$bible = Filter_Cli::argument (@$argv, 1);


$database_logs->log (Locale_Translate::_("Converting Bible to USFM Resource") . ": $bible", Filter_Roles::MANAGER_LEVEL);


$books = $database_bibles->getBooks ($bible);
foreach ($books as $book) {
  $bookname = $database_books->getEnglishFromId ($book);
  $database_logs->log ("$bookname", Filter_Roles::MANAGER_LEVEL);
  $chapters = $database_bibles->getChapters ($bible, $book);
  foreach ($chapters as $chapter) {
    $usfm = $database_bibles->getChapter ($bible, $book, $chapter);
    $database_usfmresources->storeChapter ($bible, $book, $chapter, $usfm);
    $database_bibles->deleteChapter ($bible, $book, $chapter);
  }
}
$database_bibles->deleteBible ($bible);
// Any information about user access to this Bible is left untouched,
// because this information is useful when the USFM Resource is converted
// back to a Bible.


$database_logs->log (Locale_Translate::_("Completed"), Filter_Roles::MANAGER_LEVEL);


?>
