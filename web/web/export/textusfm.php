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
$book = Filter_Cli::argument (@$argv, 2);


// Create folders for the clear text and the basic USFM exports.
$usfmDirectory = Export_Logic::USFMdirectory ($bible, 1);
if (!file_exists ($usfmDirectory)) mkdir ($usfmDirectory, 0777, true);
$textDirectory = Export_Logic::bibleDirectory ($bible) . "/text";
if (!file_exists ($textDirectory)) mkdir ($textDirectory, 0777, true);


// Filenames for text and usfm.
$usfmFilename = $usfmDirectory . "/" . Export_Logic::baseBookFileName ($book) . ".usfm";
$textFilename = $textDirectory . "/" . Export_Logic::baseBookFileName ($book) . ".txt";


$database_logs = Database_Logs::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_books = Database_Books::getInstance ();


$stylesheet = $database_config_bible->getExportStylesheet ($bible);


$filter_text_book = new Filter_Text ($bible);
$filter_text_book->text_text = new Text_Text ();


// Basic USFM.
if (file_exists ($usfmFilename)) unlink ($usfmFilename);
$basicUsfm = "\\id " . $database_books->getUsfmFromId ($book) . "\n";
file_put_contents ($usfmFilename, $basicUsfm, FILE_APPEND);
unset ($basicUsfm);


$chapters = $database_bibles->getChapters ($bible, $book);
foreach ($chapters as $chapter) {


  // The text filter for this chapter.
  $filter_text_chapter = new Filter_Text ($bible);


  // Basic USFM for this chapter.
  $filter_text_chapter->initializeHeadingsAndTextPerVerse ();


  // Get the USFM code for the current chapter.
  $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
  $chapter_data = trim ($chapter_data);


  // Add the chapter's USFM code to the Text_* filter for the book, and for the chapter.
  // Use small chunks of USFM at a time. This provides much better performance.
  $filter_text_book->addUsfmCode ($chapter_data);
  $filter_text_chapter->addUsfmCode ($chapter_data);


  // Convert the chapter
  $filter_text_chapter->run ($stylesheet);


  // Deal with basic USFM.
  if ($chapter > 0) {
    $verses_text = $filter_text_chapter->getVersesText ();
    $basicUsfm = "\\c $chapter\n";
    $basicUsfm .= "\\p\n";
    foreach ($verses_text as $verse => $text) {
      $basicUsfm .= "\\v $verse $text\n";
    }
    file_put_contents ($usfmFilename, $basicUsfm, FILE_APPEND);
  }


}


// Convert the book.
$filter_text_book->run ($stylesheet);


// Save the text export.
$filter_text_book->text_text->save ($textFilename);


$database_logs->log ($bible . " " . Export_Logic::baseBookFileName ($book) . ": " . Locale_Translate::_("Exported to basic USFM and text"), Filter_Roles::TRANSLATOR_LEVEL);


?>
