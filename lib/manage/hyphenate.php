<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

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


require_once ("../bootstrap/bootstrap");


// Security: The script runs from the cli SAPI only.
Filter_Cli::assert ();


// The databases.
$database_logs = Database_Logs::getInstance ();
$database_books = Database_Books::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_users = Database_Users::getInstance ();


$inputBible = Filter_Cli::argument (@$argv, 1);
$outputBible = "$inputBible-hyphenated";


$user = Filter_Cli::argument (@$argv, 2);


Database_Logs::log ("Reading Bible $inputBible, adding soft hyphens, putting it into Bible $outputBible");


// Get the two sets of characters as arrays.
// The /u switch treats the text as UTF8 Unicode.
preg_match_all('/./u', Database_Config_Bible::getHyphenationFirstSet ($inputBible), $firstset);
$firstset = $firstset[0];
preg_match_all('/./u', Database_Config_Bible::getHyphenationSecondSet ($inputBible), $secondset);
$secondset = $secondset[0];


// Delete and (re)create the hyphenated Bible, and grant privileges.
request->database_bibles()->deleteBible ($outputBible);
request->database_bibles()->createBible ($outputBible);
request->database_users ()->grantAccess2Bible ($user, $outputBible);


// Go through the input Bible's books and chapters.
$books = request->database_bibles()->getBooks ($inputBible);
for ($books as $book) {
  Database_Logs::log (Database_Books::getEnglishFromId ($book));
  $chapters = request->database_bibles()->getChapters ($inputBible, $book);
  for ($chapters as $chapter) {
    $data = request->database_bibles()->getChapter ($inputBible, $book, $chapter);
    $data = Filter_Hyphenate::atTransition ($firstset, $secondset, $data);
    request->database_bibles()->storeChapter ($outputBible, $book, $chapter, $data);
  }
}


Database_Logs::log ("The Bible has been hyphenated");


?>
