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
$database_books = Database_Books::getInstance ();


$inputBible = $argv[1];
$outputBible = "$inputBible-hyphenated";
$database_logs->log ("Reading Bible $inputBible, adding soft hyphens, putting it into Bible $outputBible");


// Security: The script runs from the cli SAPI only.
if (php_sapi_name () != "cli") {
  $database_logs->log ("Fatal: This only runs through the cli Server API");
  die;
}


// The relevant databases.
$database_config_general = Database_Config_General::getInstance ();
$database_bibles = Database_Bibles::getInstance ();


// Get the two sets of characters as arrays.
// The /u switch treats the text as UTF8 Unicode.
preg_match_all('/./u', $database_config_general->getHyphenationFirstSet (), $firstset);
$firstset = $firstset[0];
preg_match_all('/./u', $database_config_general->getHyphenationSecondSet (), $secondset);
$secondset = $secondset[0];


// Delete and (re)create the hyphenated Bible.
$database_bibles->deleteBible ($outputBible);
$database_bibles->createBible ($outputBible);


// Go through the input Bible's books and chapters.
$books = $database_bibles->getBooks ($inputBible);
foreach ($books as $book) {
  $database_logs->log ($database_books->getEnglishFromId ($book));
  $chapters = $database_bibles->getChapters ($inputBible, $book);
  foreach ($chapters as $chapter) {
    $data = $database_bibles->getChapter ($inputBible, $book, $chapter);
    $data = Filter_Hyphenate::atTransition ($firstset, $secondset, $data);
    $database_bibles->storeChapter ($outputBible, $book, $chapter, $data);
  }
}


$database_logs->log ("The Bible has been hyphenated");

?>
