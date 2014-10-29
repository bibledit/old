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
$database_search = Database_Search::getInstance ();


// Health check on the database.
$recreate = $database_search->checkup ();
if ($recreate) {
  $database_logs->log ("Recreating damaged search database" , Filter_Roles::MANAGER_LEVEL);
}


$database_search->optimize ();


// This script checks whether the data in the search index matches the data in the Bibles.
// Under normal circumstances the search index matches the data in the Bibles.
// But it may happen that a record could not be updated in the search index, or that another error occurred.
// Then the search index no longer is in sync with the Bible data.
// Therefore below it is going to check whether everything matches.
// It removes superfluous data, and checks that all Bible data is present in the search database.


// Check for and delete extra Bibles from the search database.
$database_logs->log ("Trimming search database" , Filter_Roles::MANAGER_LEVEL);
$bibles = $database_bibles->getBibles ();
$searchBibles = $database_search->getBibles ();
foreach ($searchBibles as $searchBible) {
  if (!in_array ($searchBible, $bibles)) {
    $database_search->deleteBible ($searchBible);
  } else {

    // Check for and delete extra books from the search database.
    $books = $database_bibles->getBooks ($searchBible);
    $searchBooks = $database_search->getBooks ($searchBible);
    foreach ($searchBooks as $searchBook) {
      if (!in_array ($searchBook, $books)) {
        $database_search->deleteBook ($searchBible, $searchBook);
      } else {
        
        // Check for and delete extra chapters from the search database.
        $chapters = $database_bibles->getChapters ($searchBible, $searchBook);
        $searchChapters = $database_search->getChapters ($searchBible, $searchBook);
        foreach ($searchChapters as $searchChapter) {
          if (!in_array ($searchChapter, $chapters)) {
            $database_search->deleteChapter ($searchBible, $searchBook, $searchChapter);
          } else {
            
            // Check for extra verses in the search database, and take corrective action.
            $usfm = $database_bibles->getChapter ($searchBible, $searchBook, $searchChapter);
            $verses = Filter_Usfm::getVerseNumbers ($usfm);
            $searchVerses = $database_search->getVerses ($searchBible, $searchBook, $searchChapter);
            foreach ($searchVerses as $searchVerse) {
              if (!in_array ($searchVerse, $verses)) {
                $database_search->deleteChapter ($searchBible, $searchBook, $searchChapter);
              }
            }
          }
        }
      }
    }
  }
}


// Updating the search indexes if there's a difference with the data in the Bibles.
// There's a difference if the raw USFM differs. 
// When the raw USFM matches for a chapter, than this chapter is considered okay.
$bibles = $database_bibles->getBibles ();
foreach ($bibles as $bible) {
  $database_logs->log ("Updating search index for Bible $bible" , Filter_Roles::MANAGER_LEVEL);
  $books = $database_bibles->getBooks ($bible);
  foreach ($books as $book) {
    $chapters = $database_bibles->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      $usfm = $database_bibles->getChapter ($bible, $book, $chapter);
      $verses = Filter_Usfm::getVerseNumbers ($usfm);
      $updateChapter = false;
      foreach ($verses as $verse) {
        if (!$updateChapter) {
          $verseUSFM = Filter_Usfm::getVerseText ($usfm, $verse);
          $searchUSFM = $database_search->getBibleVerseUsfm ($bible, $book, $chapter, $verse);
          if ($searchUSFM != $verseUSFM) {
            $updateChapter = true;
          }
        }
      }
      if ($updateChapter) {
        $database_search->updateSearchFields ($bible, $book, $chapter);
      }
    }
  }
}


$database_logs->log ("Recreating Bible indexes ready", Filter_Roles::MANAGER_LEVEL);


?>
