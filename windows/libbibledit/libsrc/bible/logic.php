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


class Bible_Logic
{


  public static function storeChapter ($bible, $book, $chapter, $usfm)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_modifications = Database_Modifications::getInstance ();
    $database_bibleactions = Database_BibleActions::getInstance ();

    // Record data of the chapter to be stored prior to storing the new version.
    if (Filter_Client::enabled ()) {

      // Client stores Bible action.
      $oldusfm = $database_bibles->getChapter ($bible, $book, $chapter);
      $database_bibleactions->record ($bible, $book, $chapter, $oldusfm);

    } else {

      // Server stores diff data.
      $database_modifications->storeTeamDiff ($bible, $book, $chapter);

    }

    // Store the chapter in the database.
    $database_bibles->storeChapter ($bible, $book, $chapter, $usfm);
  }


  public static function deleteChapter ($bible, $book, $chapter)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_modifications = Database_Modifications::getInstance ();
    $database_bibleactions = Database_BibleActions::getInstance ();

    // Record data of the chapter to be deleted prior to deletion.
    if (Filter_Client::enabled ()) {

      // Client stores Bible action.
      $usfm = $database_bibles->getChapter ($bible, $book, $chapter);
      $database_bibleactions->record ($bible, $book, $chapter, $usfm);

    } else {

      // Server stores diff data.
      $database_modifications->storeTeamDiff ($bible, $book, $chapter);

    }

    // Delete the chapter from the database.
    $database_bibles->deleteChapter ($bible, $book, $chapter);
  }


  public static function deleteBook ($bible, $book)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_modifications = Database_Modifications::getInstance ();
    $database_bibleactions = Database_BibleActions::getInstance ();

    // Record data of the book to be deleted prior to deletion.
    if (Filter_Client::enabled ()) {

      // Client stores Bible actions.
      $chapters = $database_bibles->getChapters ($bible, $book);
      foreach ($chapters as $chapter) {
        $usfm = $database_bibles->getChapter ($bible, $book, $chapter);
        $database_bibleactions->record ($bible, $book, $chapter, $usfm);
      }

    } else {

      // Server stores diff data.
      $database_modifications->storeTeamDiffBook ($bible, $book);

    }

    // Delete the book from the database.
    $database_bibles->deleteBook ($bible, $book);
  }


  public static function deleteBible ($bible)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_modifications = Database_Modifications::getInstance ();
    $database_bibleactions = Database_BibleActions::getInstance ();

    // Record data of the Bible to be deleted prior to deletion.
    if (Filter_Client::enabled ()) {

      // Client stores Bible actions.
      $books = $database_bibles->getBooks ($bible);
      foreach ($books as $book) {
        $chapters = $database_bibles->getChapters ($bible, $book);
        foreach ($chapters as $chapter) {
          $usfm = $database_bibles->getChapter ($bible, $book, $chapter);
          $database_bibleactions->record ($bible, $book, $chapter, $usfm);
        }
      }

    } else {

      // Server stores diff data.
      $database_modifications->storeTeamDiffBible ($bible);

    }

    // Delete the Bible from the database.
    $database_bibles->deleteBible ($bible);
  }


}


?>
