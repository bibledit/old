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

$bibles = $database_bibles->getBibles ();
foreach ($bibles as $bible) {

  // Skip the $bible if it should not be exported.
  if (!in_array ($bible, $exportedBibles)) continue;
  
  // Files get stored in http://site.org/bibledit/downloads/exports/<Bible>
  // The user can access the files through the browser.
  $baseUrl = "/downloads/exports/" . $bible;
  $bibleDirectory = dirname (dirname (__FILE__)) . $baseUrl;
  @mkdir ($bibleDirectory, 0777, true);
  
  // USFM files go into the USFM folder.
  $usfmDirectory = $bibleDirectory . "/USFM";
  @mkdir ($usfmDirectory, 0777, true);

  // OpenDocument files to into the OpenDocument folder.
  $odtDirectory = $bibleDirectory . "/OpenDocument";
  @mkdir ($odtDirectory, 0777, true);
  
  // There is also a file that holds the USFM code of the entire Bible.
  $bibleUsfmData = "";

  // Go through the Bible books.
  $books = $database_bibles->getBooks ($bible);
  foreach ($books as $book) {
    
    // Empty the USFM data for the current book.
    $bookUsfmData = "";

    // Go through the chapters in this book.
    $chapters = $database_bibles->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      
      // Get the USFM code for the current chapter.
      $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
      $chapter_data = trim ($chapter_data);

      // Add the chapter USFM code to the book's USFM code.
      $bookUsfmData .= $chapter_data;
      $bookUsfmData .= "\n";
    }

    // Store the USFM code for the book to disk.
    $baseBookFileName = sprintf("%0" . 2 . "d", $book) . "_" . $database_books->getEnglishFromId ($book);
    file_put_contents ("$usfmDirectory/$baseBookFileName.usfm", $bookUsfmData);

    // Add the book's USFM code to the whole Bible's USFM code.
    $bibleUsfmData .= $bookUsfmData;
    
  }

  // Store the USFM code for the whole Bible to disk.
  file_put_contents ("$usfmDirectory/00_Bible.usfm", $bibleUsfmData);

  // Create OpenDocument files related to the whole Bible. Todo
  $filter_text_bible = new Filter_Text;
  $filter_text_bible->addUsfmCode ($bibleUsfmData);
  $filter_text_bible->run ();
  $javaCode = $filter_text_bible->produceInfoDocument ("$odtDirectory/00_Info.odt");
  $dir = Filter_Java::compile ($javaCode, array (Odfdom_Class::path (), Filter_Java::xercesClassPath()));
  Filter_Java::run ($dir, array (Odfdom_Class::path (), Filter_Java::xercesClassPath()), "odt");

}
$database_logs->log (gettext ("The Bibles have been exported"), true);

?>
