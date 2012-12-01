<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2012 Teus Benschop.
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
$database_logs->log ("exports: Exporting the Bibles", true);


// Security: The script runs from the cli SAPI only.
if (php_sapi_name () != "cli") {
  $database_logs->log ("exports: Fatal: This only runs through the cli Server API", true);
  die;
}


$database_config_general = Database_Config_General::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_books = Database_Books::getInstance ();


$exportedBibles = $database_config_general->getExportedBibles ();
$stylesheet = $database_config_general->getExportStylesheet ();


// Where to store the exported Bibles.
include ("paths/paths.php");
$exportsDirectory = "$localStatePath/$location/exports";


$bibles = $database_bibles->getBibles ();
foreach ($bibles as $bible) {


  // Where to store this Bible to be exported.
  // Remove this directory just case something has changed in the data or the settings.
  $bibleDirectory = "$exportsDirectory/$bible";
  Filter_Rmdir::rmdir ($bibleDirectory);


  // Skip the $bible if it should not be exported.
  if (!in_array ($bible, $exportedBibles)) continue;

  
  // The user can access the files through the browser at the directory.
  mkdir ($bibleDirectory);

  
  // USFM files go into the USFM folder.
  $usfmDirectory = $bibleDirectory . "/USFM";
  mkdir ($usfmDirectory);


  // OpenDocument files go into the OpenDocument folder.
  $odtDirectory = $bibleDirectory . "/OpenDocument";
  mkdir ($odtDirectory);

  
  // Folder where the plain web files go.
  $plainWebDirectory = $bibleDirectory . "/PlainWeb";
  mkdir ($plainWebDirectory);

  
  // Folder for the linked web data.
  $linkedWebDirectory = $bibleDirectory . "/Web";
  mkdir ($linkedWebDirectory);

  
  // Online Bible files go into the OnlineBible folder.
  $onlineBibleDirectory = $bibleDirectory . "/OnlineBible";
  mkdir ($onlineBibleDirectory);


  // Folder for the eSword module.
  $eSwordDirectory = $bibleDirectory . "/eSword";
  mkdir ($eSwordDirectory);
  
  
  // USFM code of the entire Bible.
  $bibleUsfmData = "";


  // OpenDocument / Web / Online Bible data for the whole Bible.
  $filter_text_bible = new Filter_Text ($bible);

  
  // LinkedWeb index file.
  $html_text_linked_index = new Html_Text ($bible);
  $html_text_linked_index->newParagraph ();
  $html_text_linked_index->addText ($bible);


  // Go through the Bible books.
  $books = $database_bibles->getBooks ($bible);
  foreach ($books as $book) {
    
    $database_logs->log ("exports: " . $bible . " " . $database_books->getEnglishFromId ($book), true);

    // Empty the USFM data for the current book.
    $bookUsfmData = "";

    // OpenDocument and Web data for one Bible book.
    $filter_text_book = new Filter_Text ($bible);

    // Go through the chapters in this book.
    $chapters = $database_bibles->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      
      // Get the USFM code for the current chapter.
      $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
      $chapter_data = trim ($chapter_data);

      // Add the chapter USFM code to the book's USFM code.
      $bookUsfmData .= $chapter_data;
      $bookUsfmData .= "\n";
      
      // Add the chapter's USFM code to the Text_* filter for the whole Bible, and for this book.
      // Use small chunks of USFM at a time. This gives much better speed.
      $filter_text_bible->addUsfmCode ($chapter_data);
      $filter_text_book->addUsfmCode ($chapter_data);
    }

    // Store the USFM code for the book to disk.
    $baseBookFileName = sprintf("%0" . 2 . "d", $book) . "_" . $database_books->getEnglishFromId ($book);
    file_put_contents ("$usfmDirectory/$baseBookFileName.usfm", $bookUsfmData);

    // Create OpenDocument and Web files for the Bible book.
    $filter_text_book->run ($stylesheet);
    $filter_text_book->odf_text_standard->save ("$odtDirectory/$baseBookFileName" . "_standard.odt");
    $filter_text_book->odf_text_text_only->save ("$odtDirectory/$baseBookFileName" . "_text_only.odt");
    $filter_text_book->odf_text_text_and_note_citations->save ("$odtDirectory/$baseBookFileName" . "_text_and_note_citations.odt");
    $filter_text_book->odf_text_notes->save ("$odtDirectory/$baseBookFileName" . "_notes.odt");
    $filter_text_book->html_text_standard->save ("$plainWebDirectory/$baseBookFileName" . ".html");
    $filter_text_book->html_text_linked->save ("$linkedWebDirectory/$baseBookFileName" . ".html");

    // Add the book's USFM code to the whole Bible's USFM code.
    $bibleUsfmData .= $bookUsfmData;
    
    // Add this book to the index for the linked web.
    $html_text_linked_index->newParagraph ();
    $html_text_linked_index->addLink ($html_text_linked_index->currentPDomElement,  $baseBookFileName . ".html", "", $database_books->getEnglishFromId ($book), "", $database_books->getEnglishFromId ($book));
  }

  // Save the USFM code for the whole Bible.
  file_put_contents ("$usfmDirectory/00_Bible.usfm", $bibleUsfmData);

  // Create standard OpenDocument containing the whole Bible.
  $filter_text_bible->run ($stylesheet);
  $filter_text_bible->odf_text_standard->save ("$odtDirectory/00_Bible_standard.odt");
  $filter_text_bible->odf_text_text_only->save ("$odtDirectory/00_Bible_text_only.odt");
  $filter_text_bible->odf_text_text_and_note_citations->save ("$odtDirectory/00_Bible_text_and_note_citations.odt");
  $filter_text_bible->odf_text_notes->save ("$odtDirectory/00_Bible_notes.odt");
  
  // Save to other formats.
  $filter_text_bible->html_text_standard->save ("$plainWebDirectory/00_Bible.html");
  $html_text_linked_index->save ("$linkedWebDirectory/00_index.html");
  $html_text_linked_index->save ("$linkedWebDirectory/index.html");
  $filter_text_bible->onlinebible_text->save ("$onlineBibleDirectory/bible.exp");
  $filter_text_bible->esword_text->finalize ();
  $filter_text_bible->esword_text->createModule ("$eSwordDirectory/$bible.bblx");
  
  // Create the info OpenDocument for the whole Bible.
  $filter_text_bible->produceInfoDocument ("$odtDirectory/00_Info.odt");
  
  // Create the fallout document.
  $filter_text_bible->produceFalloutDocument ("$odtDirectory/00_Fallout.odt");

}


$database_logs->log ("exports: Completed", true);


?>