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


// Create folders for the OpenDocument export.
$directory = Export_Logic::bibleDirectory ($bible) . "/opendocument";
if (!file_exists ($directory)) mkdir ($directory, 0777, true);


// Filenames for the various types of OpenDocument files.
$basename = Export_Logic::baseBookFileName ($book);
$standardFilename = "$directory/$basename" . "_standard.odt";
$textOnlyFilename = "$directory/$basename" . "_text_only.odt";
$textAndCitationsFilename = "$directory/$basename" . "_text_and_note_citations.odt";
$notesFilename = "$directory/$basename" . "_notes.odt";


$database_logs = Database_Logs::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_books = Database_Books::getInstance ();


$stylesheet = $database_config_bible->getExportStylesheet ($bible);


$filter_text = new Filter_Text ($bible);
$filter_text->odf_text_standard = new Odf_Text ($bible);
$filter_text->odf_text_text_only = new Odf_Text ($bible);
$filter_text->odf_text_text_and_note_citations = new Odf_Text ($bible);
$filter_text->odf_text_notes = new Odf_Text ($bible);


if ($book == 0) {
  // Load entire Bible.
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
} else {
  // Load one book.
  $chapters = $database_bibles->getChapters ($bible, $book);
  foreach ($chapters as $chapter) {
    $usfm = $database_bibles->getChapter ($bible, $book, $chapter);
    $usfm = trim ($usfm);
    // Use small chunks of USFM at a time for much better performance.
    $filter_text->addUsfmCode ($usfm);
  }
}


// Convert the USFM to OpenDocument.
$filter_text->run ($stylesheet);


// Save files.
$filter_text->odf_text_standard->save ($standardFilename);
$filter_text->odf_text_text_only->save ($textOnlyFilename);
$filter_text->odf_text_text_and_note_citations->save ($textAndCitationsFilename);
$filter_text->odf_text_notes->save ($notesFilename);


// Securing the OpenDocument export implies that the exported files are zipped and secured with a password.
// It uses the external zip binary.
// PHP 5.6 supports password protected archives: ZipArchive::setPassword ($password).
$secure = $database_config_bible->getSecureOdtExport ($bible);
$password = escapeshellarg ($database_config_bible->getExportPassword ($bible));
$directory = escapeshellarg ($directory);
$basefile = escapeshellarg (basename ($standardFilename));
@unlink ("$standardFilename.zip");
if ($secure) {
  $command = "cd $directory; zip -P $password $basefile.zip $basefile; rm $basefile";
  Tasks_Logic::save ($command);
}
$basefile = escapeshellarg (basename ($textOnlyFilename));
@unlink ("$textOnlyFilename.zip");
if ($secure) {
  $command = "cd $directory; zip -P $password $basefile.zip $basefile; rm $basefile";
  Tasks_Logic::save ($command);
}
$basefile = escapeshellarg (basename ($textAndCitationsFilename));
@unlink ("$textAndCitationsFilename.zip");
if ($secure) {
  $command = "cd $directory; zip -P $password $basefile.zip $basefile; rm $basefile";
  Tasks_Logic::save ($command);
}
$basefile = escapeshellarg (basename ($notesFilename));
@unlink ("$notesFilename.zip");
if ($secure) {
  $command = "cd $directory; zip -P $password $basefile.zip $basefile; rm $basefile";
  Tasks_Logic::save ($command);
}


$database_logs->log ($bible . " " . Export_Logic::baseBookFileName ($book) . ": " . Locale_Translate::_("Exported to OpenDocument files"), Filter_Roles::TRANSLATOR_LEVEL);


?>
