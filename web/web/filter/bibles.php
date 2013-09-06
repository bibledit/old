<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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


class Filter_Bibles
{

  // Function to safely store a chapter.
  // It saves the chapter if the new USFM does not differ too much from the existing USFM.
  public static function safeStoreChapter ($bible, $book, $chapter, $usfm)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_logs = Database_Logs::getInstance ();
    $database_books = Database_Books::getInstance ();

    // Existing chapter contents.
    $existing = $database_bibles->getChapter ($bible, $book, $chapter);

    // The length of the new USFM code should not differ more than 20% from the existing USFM code.
    $existingLength = strlen ($existing);
    $newLength = strlen ($usfm);
    $percentage = 100 * ($newLength - $existingLength) / $existingLength;
    $percentage = abs ($percentage);
    $percentage = round ($percentage);
    if ($percentage > 20) {
      $database_logs->log ("The chapter was not saved for safety reasons. The length differs $percentage% from the existing chapter. Make minor changes and save often.");
      $database_logs->log ("$bible " . $database_books->getEnglishFromId ($book) . " $chapter");
      $database_logs->log ($usfm);
      return;
    }
    
    // The text of the new chapter should not differ more than 20% from the existing text.
    similar_text ($existing, $usfm, $percentage);
    $percentage = abs ($percentage);
    $percentage = 100 - $percentage;
    $percentage = round ($percentage);
    if ($percentage > 20) {
      $database_logs->log ("The chapter was not saved for safety reasons. The new text differs $percentage% from the existing text. Make minor changes and save often.");
      $database_logs->log ("$bible " . $database_books->getEnglishFromId ($book) . " $chapter");
      $database_logs->log ($usfm);
      return;
    }
    
    // Safety checks have passed: Save chapter.
    $database_bibles->storeChapter ($bible, $book, $chapter, $usfm);
  }


}

?>
