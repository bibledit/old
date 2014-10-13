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


ignore_user_abort (true);
set_time_limit (0);


// The input parameters.
$bible = Filter_Cli::argument (@$argv, 1);


// Create folders for the information.
$directory = Export_Logic::bibleDirectory ($bible) . "/info";
if (!file_exists ($directory)) mkdir ($directory, 0777, true);


// Filenames for the various types of OpenDocument files.
$informationdFilename = "$directory/information.html";
$falloutFilename = "$directory/fallout.html";


$database_logs = Database_Logs::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_books = Database_Books::getInstance ();


$stylesheet = $database_config_bible->getExportStylesheet ($bible);


$filter_text = new Filter_Text ($bible);


$books = $database_bibles->getBooks ($bible);
foreach ($books as $book) {
  $chapters = $database_bibles->getChapters ($bible, $book);
  foreach ($chapters as $chapter) {
    $usfm = $database_bibles->getChapter ($bible, $book, $chapter);
    $usfm = trim ($usfm);
    // Use small chunks of USFM at a time for much better performance.
    $filter_text->addUsfmCode ($usfm);
  }
}


// Go through USFM to find the info and fallout.
$filter_text->run ($stylesheet);


// Save files.
$filter_text->produceInfoDocument ($informationdFilename);
$filter_text->produceFalloutDocument ($falloutFilename);


$database_logs->log (Locale_Translate::_("Documents with information and fallout were created") . " $bible", Filter_Roles::TRANSLATOR_LEVEL);


?>
