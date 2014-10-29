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
$database_books = Database_Books::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();


// The Bible.
$bible = Filter_Cli::argument (@$argv, 1);


ignore_user_abort (true);
set_time_limit (0);


// Root USFM directory, plus info file.
$usfmDirectory = Export_Logic::USFMdirectory ($bible, 2);
if (!file_exists ($usfmDirectory)) mkdir ($usfmDirectory, 0777, true);
copy (__DIR__ . "/usfm.html", "$usfmDirectory/readme.html");


// USFM directories
$usfmDirectoryFull = Export_Logic::USFMdirectory ($bible, 0);
if (!file_exists ($usfmDirectoryFull)) mkdir ($usfmDirectoryFull, 0777, true);


// Remove possible secured zip file.
@unlink ("$usfmDirectoryFull/bible.zip");


// Take the USFM from the Bible database.
// Generate one USFM file per book.


$books = $database_bibles->getBooks ($bible);
foreach ($books as $book) {


  // The USFM data of the current book.
  $bookUsfmDataFull = "";


  // Collect the USFM for all chapters in this book.
  $chapters = $database_bibles->getChapters ($bible, $book);
  foreach ($chapters as $chapter) {
    // Get the USFM code for the current chapter.
    $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
    $chapter_data = trim ($chapter_data);
    // Add the chapter USFM code to the book's USFM code.
    $bookUsfmDataFull .= $chapter_data;
    $bookUsfmDataFull .= "\n";
  }


  // The filename for the USFM for this book.
  $filename = Export_Logic::baseBookFileName ($book);
  $path = "$usfmDirectoryFull/$filename.usfm";
  
  
  // Save.
  file_put_contents ($path, $bookUsfmDataFull);


}


// Compress USFM files into one zip file.
$zipfile = "$usfmDirectoryFull/" . Export_Logic::baseBookFileName (0) . ".zip";
@unlink ($zipfile);
$archive = Filter_Archive::zip ($usfmDirectoryFull);
rename ($archive, $zipfile);


if ($database_config_bible->getSecureUsfmExport ($bible)) {
  // Securing the full USFM export means that there will be one zip file secured with a password.
  // This zip file contains all exported USFM data.
  // All other files will be removed.
  // It uses the external zip binary.
  // PHP 5.6 supports password protected archives: ZipArchive::setPassword ($password).
  $files = scandir ($usfmDirectoryFull);
  $files = Filter_Folders::cleanup ($files);
  $basefile = basename ($zipfile);
  foreach ($files as $file) {
    if ($file != $basefile) unlink ("$usfmDirectoryFull/$file");
  }
  $directory = escapeshellarg ($usfmDirectoryFull);
  $password = escapeshellarg ($database_config_bible->getExportPassword ($bible));
  $command = "cd $directory; zip -P $password bible.zip $basefile; rm $basefile";
  Tasks_Logic::save ($command);
}


$database_logs->log ($bible . ": " . Locale_Translate::_("Exported to USFM"), Filter_Roles::TRANSLATOR_LEVEL);


?>
