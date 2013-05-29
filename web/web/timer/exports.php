<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2013 Teus Benschop.
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


ignore_user_abort (true);
set_time_limit (0);


$database_config_general = Database_Config_General::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_books = Database_Books::getInstance ();


$exportedBibles = $database_config_general->getExportedBibles ();
$stylesheet = $database_config_general->getExportStylesheet ();


unset ($database_config_general);


// Where to store the exported Bibles.
include ("paths/paths.php");
$exportsDirectory = "$localStatePath/$location/exports";


$bibles = $database_bibles->getBibles ();
foreach ($bibles as $bible) {


  // Where to store this Bible to be exported.
  // Remove this directory just in case something has changed in the data or the settings.
  $bibleDirectory = "$exportsDirectory/$bible";
  Filter_Rmdir::rmdir ($bibleDirectory);
  // Skip the $bible if it should not be exported.
  if (!in_array ($bible, $exportedBibles)) continue;
  // The user can access the files through the browser at the directory.
  mkdir ($bibleDirectory);
  // Folder where the USFM files go.
  $usfmDirectory = $bibleDirectory . "/usfm";
  mkdir ($usfmDirectory);
  // Folder for the OpenDocument files.
  $odtDirectory = $bibleDirectory . "/opendocument";
  mkdir ($odtDirectory);
  // Folder where the plain web files go.
  $plainWebDirectory = $bibleDirectory . "/plainweb";
  mkdir ($plainWebDirectory);
  // Folder for the interlinked web files.
  $richWebDirectory = $bibleDirectory . "/web";
  mkdir ($richWebDirectory);
  // Folder for the Online Bible file.
  $onlineBibleDirectory = $bibleDirectory . "/onlinebible";
  mkdir ($onlineBibleDirectory);
  // Folder for the eSword module.
  $eSwordDirectory = $bibleDirectory . "/esword";
  mkdir ($eSwordDirectory);
  // Folder for the clear text export.
  $clearTextDirectory = $bibleDirectory . "/text";
  mkdir ($clearTextDirectory);
  // Folder for the information files.
  $infoDirectory = $bibleDirectory . "/info";
  mkdir ($infoDirectory);


  // Back link path.
  $backLinkPath = "../downloads/exports/$bible/web/";

  // Rich web main index file. 
  $html_text_rich_bible_index = new Html_Text ($bible); 
  // On top are the breadcrumbs, starting with a clickable Bible name.
  $htmlHeader = new Html_Header ($html_text_rich_bible_index);
  $htmlHeader->searchBackLink ($backLinkPath . Filter_Paths::htmlFileNameBible (), gettext ("Go back to Bible"));
  $htmlHeader->create (array (array ($bible, Filter_Paths::htmlFileNameBible ())));
  unset ($htmlHeader);
  // Prepare for the list of books in de html index file.
  $html_text_rich_bible_index->newParagraph ("navigationbar");
  $html_text_rich_bible_index->addText (" |");


  // Go through the Bible books.
  $books = $database_bibles->getBooks ($bible);
  foreach ($books as $book) {
    
    // E.g.: Ndebele Genesis
    $bibleBookText = $bible . " " . $database_books->getEnglishFromId ($book);
    
    $database_logs->log ("exports: " . $bibleBookText, true);

    // Rich web index file per book. 
    $html_text_rich_book_index = new Html_Text ($bibleBookText);
    $htmlHeader = new Html_Header ($html_text_rich_book_index);
    $htmlHeader->searchBackLink ($backLinkPath . Filter_Paths::htmlFileNameBible ("", $book), gettext ("Go back to") . " " . $bibleBookText);
    $htmlHeader->create (array (array ($bible, Filter_Paths::htmlFileNameBible ()), array ($database_books->getEnglishFromId ($book), Filter_Paths::htmlFileNameBible ()) ));
    $html_text_rich_book_index->newParagraph ("navigationbar");
    $html_text_rich_book_index->addText ("|");

    // The USFM data of the current book.
    $bookUsfmData = "";

    // The text converter per book.
    $filter_text_book = new Filter_Text ($bible);
    $filter_text_book->html_text_standard = new Html_Text (gettext ("Bible"));
    $filter_text_book->text_text = new Text_Text ();
    $filter_text_book->odf_text_standard = new Odf_Text;
    $filter_text_book->odf_text_text_only = new Odf_Text;
    $filter_text_book->odf_text_text_and_note_citations = new Odf_Text;
    $filter_text_book->odf_text_notes = new Odf_Text;

    // Go through the chapters in this book.
    $chapters = $database_bibles->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      
      // Interlinked web data for one chapter.
      $filter_text_chapter = new Filter_Text ($bible);
      $filter_text_chapter->html_text_linked = new Html_Text (gettext ("Bible"));

      // Get the USFM code for the current chapter.
      $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
      $chapter_data = trim ($chapter_data);

      // Add the chapter USFM code to the book's USFM code.
      $bookUsfmData .= $chapter_data;
      $bookUsfmData .= "\n";
      
      // Add the chapter's USFM code to the Text_* filter for the book, and for the chapter.
      // Use small chunks of USFM at a time. This provides much better performance.
      $filter_text_book->addUsfmCode ($chapter_data);
      $filter_text_chapter->addUsfmCode ($chapter_data);

      // Create breadcrumbs for the chapter.
      $htmlHeader = new Html_Header ($filter_text_chapter->html_text_linked);
      $htmlHeader->searchBackLink ($backLinkPath . Filter_Paths::htmlFileNameBible ("", $book, $chapter), gettext ("Go back to") . " " . $bibleBookText . " " . $chapter);
      $htmlHeader->create (array (array ($bible, Filter_Paths::htmlFileNameBible ()),
                                  array ($database_books->getEnglishFromId ($book), Filter_Paths::htmlFileNameBible ()),
                                  array ($chapter, Filter_Paths::htmlFileNameBible ("", $book))
                                 ));
      
      // Create interlinked html for the chapter.
      $filter_text_chapter->run ($stylesheet);
      $filter_text_chapter->html_text_linked->save (Filter_Paths::htmlFileNameBible ($richWebDirectory, $book, $chapter));
      
      $html_text_rich_book_index->addLink ($html_text_rich_book_index->currentPDomElement, Filter_Paths::htmlFileNameBible ("", $book, $chapter), "", $chapter, "", " " . $chapter . " ");
      $html_text_rich_book_index->addText ("|");

    }

    // Save the USFM code of the book to disk.
    $baseBookFileName = sprintf("%0" . 2 . "d", $book) . "_" . $database_books->getEnglishFromId ($book);
    file_put_contents ("$usfmDirectory/$baseBookFileName.usfm", $bookUsfmData);

    // Create OpenDocument and Web files for the Bible book.
    $filter_text_book->run ($stylesheet);
    $filter_text_book->odf_text_standard->save ("$odtDirectory/$baseBookFileName" . "_standard.odt");
    $filter_text_book->odf_text_text_only->save ("$odtDirectory/$baseBookFileName" . "_text_only.odt");
    $filter_text_book->odf_text_text_and_note_citations->save ("$odtDirectory/$baseBookFileName" . "_text_and_note_citations.odt");
    $filter_text_book->odf_text_notes->save ("$odtDirectory/$baseBookFileName" . "_notes.odt");
    $filter_text_book->html_text_standard->save (Filter_Paths::htmlFileNameBible ($plainWebDirectory, $book));

    // Add this book to the main web index.
    $html_text_rich_bible_index->addLink ($html_text_rich_bible_index->currentPDomElement,  Filter_Paths::htmlFileNameBible ("", $book), "", $database_books->getEnglishFromId ($book), "", " " . $database_books->getEnglishFromId ($book) . " ");
    $html_text_rich_bible_index->addText ("|");

    // Save the book index.
    $html_text_rich_book_index->save (Filter_Paths::htmlFileNameBible ("$richWebDirectory", $book));
    
    // Save the clear text export.
    $filter_text_book->text_text->save ("$clearTextDirectory/$baseBookFileName.txt");
    
    // Unset the variables used for the separate books.
    unset ($bibleBookText);
    unset ($htmlHeader);
    unset ($bookUsfmData);
    unset ($filter_text_book->html_text_standard);
    unset ($filter_text_book->text_text);
    unset ($filter_text_book->odf_text_standard);
    unset ($filter_text_book->odf_text_text_only);
    unset ($filter_text_book->odf_text_text_and_note_citations);
    unset ($filter_text_book->odf_text_notes);
    unset ($filter_text_book);
    unset ($chapters);
    unset ($chapter);
    unset ($filter_text_chapter);
    unset ($chapter_data);
    unset ($baseBookFileName);
    
  }


  // Save index file for the interlinked web export.
  $database_logs->log ("exports: Create index file for interlinked Web", true);
  $html_text_rich_bible_index->save ("$richWebDirectory/index.html");
  $html_text_rich_bible_index->save ("$richWebDirectory/00_index.html");
  unset ($html_text_rich_bible_index);


  // This is the end of the conversion of separate books.
  // Continue with the conversion of the entire Bible. 


  // Save the USFM code for the whole Bible.
  // Go through the Bible books and chapters.
  // Add the chapter's USFM code to the exporter filter for the whole Bible.
  // Use small chunks of USFM at a time for much better performance.
  $database_logs->log ("exports: Save entire Bible to USFM", true);
  $bibleUsfmData = "";
  $books = $database_bibles->getBooks ($bible);
  foreach ($books as $book) {
    $chapters = $database_bibles->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
      $chapter_data = trim ($chapter_data);
      $bibleUsfmData .= $chapter_data;
      $bibleUsfmData .= "\n";
    }
  }
  file_put_contents ("$usfmDirectory/00_Bible.usfm", $bibleUsfmData);
  unset ($bibleUsfmData);


  // Save Bible to the plain Web format.
  $database_logs->log ("exports: Save entire Bible to plain Web", true);
  $filter_text_bible = new Filter_Text ($bible);
  $filter_text_bible->html_text_standard = new Html_Text (gettext ("Bible"));
  // Use small chunks of USFM at a time for much better performance.
  $books = $database_bibles->getBooks ($bible);
  foreach ($books as $book) {
    $chapters = $database_bibles->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
      $chapter_data = trim ($chapter_data);
      $filter_text_bible->addUsfmCode ($chapter_data);
    }
  }
  $filter_text_bible->run ($stylesheet);
  $filter_text_bible->html_text_standard->save ("$plainWebDirectory/00-Bible.html");
  unset ($filter_text_bible->html_text_standard);
  unset ($filter_text_bible);


  // Save to Online Bible format.
  $database_logs->log ("exports: Save entire Bible to Online Bible", true);
  $filter_text_bible = new Filter_Text ($bible);
  $filter_text_bible->onlinebible_text = new Onlinebible_Text ();
  $books = $database_bibles->getBooks ($bible);
  foreach ($books as $book) {
    $chapters = $database_bibles->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
      $chapter_data = trim ($chapter_data);
      $filter_text_bible->addUsfmCode ($chapter_data);
    }
  }
  $filter_text_bible->run ($stylesheet);
  $filter_text_bible->onlinebible_text->save ("$onlineBibleDirectory/bible.exp");
  unset ($filter_text_bible->onlinebible_text);
  unset ($filter_text_bible);


  // Save to eSword module.
  $database_logs->log ("exports: Save entire Bible to eSword", true);
  $filter_text_bible = new Filter_Text ($bible);
  $filter_text_bible->esword_text = new Esword_Text ($bible);
  $books = $database_bibles->getBooks ($bible);
  foreach ($books as $book) {
    $chapters = $database_bibles->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
      $chapter_data = trim ($chapter_data);
      $filter_text_bible->addUsfmCode ($chapter_data);
    }
  }
  $filter_text_bible->run ($stylesheet);
  $filter_text_bible->esword_text->finalize ();
  $filter_text_bible->esword_text->createModule ("$eSwordDirectory/$bible.bblx");
  unset ($filter_text_bible->esword_text);
  unset ($filter_text_bible);


  // Export to OpenDocument in standard Bible format: Text plus footnotes.
  $database_logs->log ("exports: Save entire Bible to OpenDocument: Text plus footnotes", true);
  $filter_text_bible = new Filter_Text ($bible);
  $filter_text_bible->odf_text_standard = new Odf_Text;
  $books = $database_bibles->getBooks ($bible);
  foreach ($books as $book) {
    $chapters = $database_bibles->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
      $chapter_data = trim ($chapter_data);
      $filter_text_bible->addUsfmCode ($chapter_data);
    }
  }
  $filter_text_bible->run ($stylesheet);
  $filter_text_bible->odf_text_standard->save ("$odtDirectory/00_Bible_standard.odt");

  // Create the document with information about the Bible.
  $filter_text_bible->produceInfoDocument ("$infoDirectory/information.html");

  // Create the document with the formatting fallout.
  $filter_text_bible->produceFalloutDocument ("$infoDirectory/fallout.html");
  
  // Clear memory.
  unset ($filter_text_bible->odf_text_standard);
  unset ($filter_text_bible);


  // Export to OpenDocument in text-only format.
  $database_logs->log ("exports: Save entire Bible to OpenDocument: Text only", true);
  $filter_text_bible = new Filter_Text ($bible);
  $filter_text_bible->odf_text_text_only = new Odf_Text;
  $books = $database_bibles->getBooks ($bible);
  foreach ($books as $book) {
    $chapters = $database_bibles->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
      $chapter_data = trim ($chapter_data);
      $filter_text_bible->addUsfmCode ($chapter_data);
    }
  }
  $filter_text_bible->run ($stylesheet);
  $filter_text_bible->odf_text_text_only->save ("$odtDirectory/00_Bible_text_only.odt");
  unset ($filter_text_bible->odf_text_text_only);
  unset ($filter_text_bible);


  // Export to OpenDocument in text plus note citations format.
  $database_logs->log ("exports: Save entire Bible to OpenDocument: Text plus note citations", true);
  $filter_text_bible = new Filter_Text ($bible);
  $filter_text_bible->odf_text_text_and_note_citations = new Odf_Text;
  $books = $database_bibles->getBooks ($bible);
  foreach ($books as $book) {
    $chapters = $database_bibles->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
      $chapter_data = trim ($chapter_data);
      $filter_text_bible->addUsfmCode ($chapter_data);
    }
  }
  $filter_text_bible->run ($stylesheet);
  $filter_text_bible->odf_text_text_and_note_citations->save ("$odtDirectory/00_Bible_text_and_note_citations.odt");
  unset ($filter_text_bible->odf_text_text_and_note_citations);
  unset ($filter_text_bible);


  // Export to OpenDocument in notes-only format.
  $database_logs->log ("exports: Save entire Bible to OpenDocument: Notes only", true);
  $filter_text_bible = new Filter_Text ($bible);
  $filter_text_bible->odf_text_notes = new Odf_Text;
  $books = $database_bibles->getBooks ($bible);
  foreach ($books as $book) {
    $chapters = $database_bibles->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
      $chapter_data = trim ($chapter_data);
      $filter_text_bible->addUsfmCode ($chapter_data);
    }
  }
  $filter_text_bible->run ($stylesheet);
  $filter_text_bible->odf_text_notes->save ("$odtDirectory/00_Bible_notes.odt");
  unset ($filter_text_bible->odf_text_notes);
  unset ($filter_text_bible);

  // Lens image supporting search.
  copy ("../webbible/lens.png", "$richWebDirectory/lens.png");

}


$database_logs->log ("exports: Completed", true);


?>
