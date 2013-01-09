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
$sphinxPort = 9312;

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

  
  // Folder for the rich web data.
  $richWebDirectory = $bibleDirectory . "/Web";
  mkdir ($richWebDirectory);

  
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

  
  // Rich web main index file. 
  $html_text_rich_bible_index = new Html_Text ($bible); 
  // On top are the breadcrumbs, starting with a clickable Bible name.
  $htmlHeader = new Html_Header ($html_text_rich_bible_index);
  $htmlHeader->searchBackLink (Filter_Paths::htmlFileNameBible (), gettext ("Go back to Bible"));
  $htmlHeader->create (array (array ($bible, Filter_Paths::htmlFileNameBible ()))); // Todo
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
    $htmlHeader->create (array (array ($bible, Filter_Paths::htmlFileNameBible ()), array ($database_books->getEnglishFromId ($book), Filter_Paths::htmlFileNameBible ()) )); // Todo
    unset ($htmlHeader);
    $html_text_rich_book_index->newParagraph ("navigationbar");
    $html_text_rich_book_index->addText ("|");

    // Empty the USFM data for the current book.
    $bookUsfmData = "";

    // OpenDocument and Web data for one Bible book.
    $filter_text_book = new Filter_Text ($bible);

    // Go through the chapters in this book.
    $chapters = $database_bibles->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      
      // Rich web for one chapter.
      $filter_text_chapter = new Filter_Text ($bible);

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
      $filter_text_chapter->addUsfmCode ($chapter_data);

      // Create breadcrumbs for the chapter.
      $htmlHeader = new Html_Header ($filter_text_chapter->html_text_linked);
      $htmlHeader->create (array (array ($bible, Filter_Paths::htmlFileNameBible ()),
                                  array ($database_books->getEnglishFromId ($book), Filter_Paths::htmlFileNameBible ()),
                                  array ($chapter, Filter_Paths::htmlFileNameBible ("", $book))
                                 )); // Todo
      unset ($htmlHeader);
      
      // Create rich html for the chapter.
      $filter_text_chapter->run ($stylesheet);
      $filter_text_chapter->html_text_linked->save (Filter_Paths::htmlFileNameBible ($richWebDirectory, $book, $chapter));
      
      $html_text_rich_book_index->addLink ($html_text_rich_book_index->currentPDomElement, Filter_Paths::htmlFileNameBible ("", $book, $chapter), "", $chapter, "", " " . $chapter . " ");
      $html_text_rich_book_index->addText ("|");

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
    $filter_text_book->html_text_standard->save (Filter_Paths::htmlFileNameBible ($plainWebDirectory, $book));

    // Add the book's USFM code to the whole Bible's USFM code.
    $bibleUsfmData .= $bookUsfmData;
    
    // Add this book to the main index.
    $html_text_rich_bible_index->addLink ($html_text_rich_bible_index->currentPDomElement,  Filter_Paths::htmlFileNameBible ("", $book), "", $database_books->getEnglishFromId ($book), "", " " . $database_books->getEnglishFromId ($book) . " ");
    $html_text_rich_bible_index->addText ("|");

    // Save the book index.
    $html_text_rich_book_index->save (Filter_Paths::htmlFileNameBible ("$richWebDirectory", $book));
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
  $filter_text_bible->html_text_standard->save ("$plainWebDirectory/00-Bible.html");
  $html_text_rich_bible_index->save ("$richWebDirectory/index.html");
  $html_text_rich_bible_index->save ("$richWebDirectory/00_index.html");
  $filter_text_bible->onlinebible_text->save ("$onlineBibleDirectory/bible.exp");
  $filter_text_bible->esword_text->finalize ();
  $filter_text_bible->esword_text->createModule ("$eSwordDirectory/$bible.bblx");
  
  // Create the info OpenDocument for the whole Bible.
  $filter_text_bible->produceInfoDocument ("$odtDirectory/00_Info.odt");
  
  // Create the fallout document.
  $filter_text_bible->produceFalloutDocument ("$odtDirectory/00_Fallout.odt");

  // Web indexer support files.
  // For each subsequent Bible, sphinxsearch uses a higher TCP port number.
  $sphinxPort++;
  $sphinxPidFilename = sys_get_temp_dir () . "/" . $bible . "-sphinx.pid";
  $bibleditPath = dirname (dirname (__FILE__));

  $contents = file_get_contents ("../webbible/sphinx.conf");
  $contents = str_replace ('web_directory', $richWebDirectory, $contents);
  $contents = str_replace ('9312', $sphinxPort, $contents);
  $contents = str_replace ('sphinx.pid', $sphinxPidFilename, $contents);
  file_put_contents ("$richWebDirectory/sphinx.conf", $contents);

  $contents = file_get_contents ("../webbible/indexer.php");
  $contents = str_replace ('bibleditPath', $bibleditPath, $contents);
  $contents = str_replace ('_bible_', $bible, $contents);
  file_put_contents ("$richWebDirectory/indexer.php", $contents);

  // Run the Sphinx indexer.
  $success = true;
  //$command = "cd \"$richWebDirectory\"; indexer --rotate --all --config sphinx.conf 2>&1";
  $command = "cd \"$richWebDirectory\"; indexer --all --config sphinx.conf 2>&1";
  $database_logs->log ("exports: $command");
  unset ($result);
  exec ($command, $result, $exit_code);
  if ($exit_code != 0) $success = false;
  foreach ($result as $line) {
    if ($line == "") continue;
    $database_logs->log ("exports: $line");
  }
  $database_logs->log ("exports: Exit code $exit_code");

  // Kill whatever deamon is listening on the TCP port that searchd needs.
  $command = "lsof -i";
  unset ($result);
  exec ($command, $result, $exit_code);
  unset ($command);
  foreach ($result as $line) {
    $database_logs->log ("exports: $line");
    if (strpos ($line, ":$sphinxPort") !== false) {
      $pos = strpos ($line, " ");
      $line = substr ($line, $pos);
      $pid = (int) $line;
      $command = "kill -9 $pid 2>&1";
    }
  }
  if (isset ($command)) {
    $database_logs->log ("exports: $command");
    unset ($result);
    exec ($command, $result, $exit_code);
    foreach ($result as $line) {
      $database_logs->log ("exports: $line");
    }
  }
  $database_logs->log ("exports: Exit code $exit_code");

  // Start the Sphinx daemon..
  $command = "cd \"$richWebDirectory\"; searchd --config sphinx.conf 2>&1";
  $database_logs->log ("exports: $command");
  unset ($result);
  exec ($command, $result, $exit_code);
  foreach ($result as $line) {
    if ($line == "") continue;
    $database_logs->log ("exports: $line");
  }
  $database_logs->log ("exports: Exit code $exit_code");

  // Supporting files for displaying doing the search plus displaying the results.
  copy ("../webbible/lens.png", "$richWebDirectory/lens.png");

  $contents = file_get_contents ("../webbible/sphinxapi.php");
  $contents = str_replace ('9312', $sphinxPort, $contents);
  file_put_contents ("$richWebDirectory/sphinxapi.php", $contents);

  $contents = file_get_contents ("../webbible/search.php");
  $contents = str_replace ('9312', $sphinxPort, $contents);
  file_put_contents ("$richWebDirectory/search.php", $contents);

}


$database_logs->log ("exports: Completed", true);


?>
