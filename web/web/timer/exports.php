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
$database_logs->log (gettext ("Exporting the Bibles"), true);

// Security: The script runs from the cli SAPI only.
if (php_sapi_name () != "cli") {
  $database_logs->log ("Fatal: This only runs through the cli Server API", true);
  die;
}

$database_config_general = Database_Config_General::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_books = Database_Books::getInstance ();

$siteUrl = $database_config_general->getSiteURL ();
$exportedBibles = $database_config_general->getExportedBibles ();
$stylesheet = $database_config_general->getExportStylesheet ();

// Files get stored in http://site.org/bibledit/downloads/exports/
$exportsDirectory = dirname (dirname (__FILE__)) . "/downloads/exports";
@mkdir ($bibleDirectory, 0777, true);

$bibles = $database_bibles->getBibles ();
foreach ($bibles as $bible) {

  // Files get stored in http://site.org/bibledit/downloads/exports/<Bible>
  // Clear this directory of old exports. Just in case something has changed in data or settings.
  $bibleDirectory = "$exportsDirectory/$bible";
  Filter_Rmdir::rmdir ($bibleDirectory);

  // Skip the $bible if it should not be exported.
  if (!in_array ($bible, $exportedBibles)) continue;
  
  // The user can access the files through the browser at the directory.
  @mkdir ($bibleDirectory, 0777, true);
  
  // USFM files go into the USFM folder.
  $usfmDirectory = $bibleDirectory . "/USFM";
  @mkdir ($usfmDirectory, 0777, true);

  // OpenDocument files to into the OpenDocument folder.
  $odtDirectory = $bibleDirectory . "/OpenDocument";
  @mkdir ($odtDirectory, 0777, true);
  
  // There is also a file that holds the USFM code of the entire Bible.
  $bibleUsfmData = "";

  // OpenDocument file for the whole Bible.
  $filter_text_bible = new Filter_Text;

  // Go through the Bible books.
  $books = $database_bibles->getBooks ($bible);
  foreach ($books as $book) {
    
    // Empty the USFM data for the current book.
    $bookUsfmData = "";

    // OpenDocument file for one Bible book.
    $filter_text_book = new Filter_Text;

    // Go through the chapters in this book.
    $chapters = $database_bibles->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      
      // Get the USFM code for the current chapter.
      $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
      $chapter_data = trim ($chapter_data);

      // Add the chapter USFM code to the book's USFM code.
      $bookUsfmData .= $chapter_data;
      $bookUsfmData .= "\n";
      
      // Add the chapter's USFM code to the OpenDocument files filter for the whole Bible, and for this book.
      // Small chunks of USFM optimize speed.
      $filter_text_bible->addUsfmCode ($chapter_data);
      $filter_text_book->addUsfmCode ($chapter_data);
    }

    // Store the USFM code for the book to disk.
    $baseBookFileName = sprintf("%0" . 2 . "d", $book) . "_" . $database_books->getEnglishFromId ($book);
    file_put_contents ("$usfmDirectory/$baseBookFileName.usfm", $bookUsfmData);

    // Create standard OpenDocument containing the Bible book.
    $filter_text_book->run ($stylesheet);
    $filter_text_book->odf_text_standard->save ("$odtDirectory/$baseBookFileName" . "_standard.odt"); // Todo
    $filter_text_book->odf_text_text_only->save ("$odtDirectory/$baseBookFileName" . "_text_only.odt");
    $filter_text_book->odf_text_text_and_note_citations->save ("$odtDirectory/$baseBookFileName" . "_text_and_note_citations.odt");

    // Add the book's USFM code to the whole Bible's USFM code.
    $bibleUsfmData .= $bookUsfmData;
        
  }

  // Save the USFM code for the whole Bible.
  file_put_contents ("$usfmDirectory/00_Bible.usfm", $bibleUsfmData);

  // Create standard OpenDocument containing the whole Bible.
  $filter_text_bible->run ($stylesheet);
  $filter_text_bible->odf_text_standard->save ("$odtDirectory/00_Bible_standard.odt"); // Todo
  $filter_text_bible->odf_text_text_only->save ("$odtDirectory/00_Bible_text_only.odt");
  $filter_text_bible->odf_text_text_and_note_citations->save ("$odtDirectory/00_Bible_text_and_note_citations.odt");

  // Create the info OpenDocument for the whole Bible.
  $filter_text_bible->produceInfoDocument ("$odtDirectory/00_Info.odt");
  
  // Create the fallout document.
  $filter_text_bible->produceFalloutDocument ("$odtDirectory/00_Fallout.odt");

}
$database_logs->log (gettext ("The Bibles have been exported"), true);

?>
