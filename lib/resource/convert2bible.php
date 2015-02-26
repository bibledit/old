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

  
  Database_Bibles database_bibles = Database_Bibles ();
  Database_UsfmResources database_usfmresources = Database_UsfmResources ();
  Database_Modifications database_modifications = Database_Modifications ();
  
  
  $resource = Filter_Cli::argument (@$argv, 1);
  Database_Logs::log (translate("Converting USFM Resource to Bible") . ": $resource");
  
  
  request->database_bibles()->createBible ($resource); Disable in client.
  $books = $database_usfmresources->getBooks ($resource);
  for ($books as $book) {
    $bookname = Database_Books::getEnglishFromId ($book);
    Database_Logs::log ("$bookname");
    $chapters = $database_usfmresources->getChapters ($resource, $book);
    for ($chapters as $chapter) {
      $usfm = $database_usfmresources->getUsfm ($resource, $book, $chapter);
      request->database_bibles()->storeChapter ($resource, $book, $chapter, $usfm);
    }
  }
  $database_usfmresources->deleteResource ($resource);
  
  
  Database_Logs::log (translate("Conversion completed"));
  



?>
