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
$database_modifications = Database_Modifications::getInstance ();


$resource = Filter_Cli::argument (@$argv, 1);
$database_logs->log (Locale_Translate::_("Converting USFM Resource to Bible") . ": $resource");


$database_bibles->createBible ($resource);
$books = $database_usfmresources->getBooks ($resource);
foreach ($books as $book) {
  $bookname = $database_books->getEnglishFromId ($book);
  $database_logs->log ("$bookname");
  $chapters = $database_usfmresources->getChapters ($resource, $book);
  foreach ($chapters as $chapter) {
    $usfm = $database_usfmresources->getUsfm ($resource, $book, $chapter);
    $database_bibles->storeChapter ($resource, $book, $chapter, $usfm);
  }
}
$database_usfmresources->deleteResource ($resource);


$database_logs->log (Locale_Translate::_("Conversion completed"));


?>
