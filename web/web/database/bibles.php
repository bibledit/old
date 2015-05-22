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


// This database stores its data in files in the filesystem.
// This is a rugged and reliable system of storing data.
// Because no real database is used, no database can get corrupted.


class Database_Bibles
{
  private static $instance;
  private function __construct() {
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Bibles ();
    }
    return self::$instance;
  }


  public function optimize ()
  {
    // Go through all chapters in all books and all Ḃibles.
    $bibles = $this->getBibles ();
    foreach ($bibles as $bible) {
      $books = $this->getBooks ($bible);
      foreach ($books as $book) {
        $chapters = $this->getChapters ($bible, $book);
        foreach ($chapters as $chapter) {
          $folder = $this->chapterFolder ($bible, $book, $chapter);
          // Read the files in the folder.
          $files = scandir ($folder);
          // Remove the "." and ".." entries.
          // Because scandir sorts the files, those entries are at the start of the array.
          array_shift ($files);
          array_shift ($files);
          // Remove files with 0 size. so that in case a chapter was emptied by accident, 
          // it is removed now, effectually reverting the chapter to an earlier version.
          foreach ($files as $offset => $file) {
            if (filesize ("$folder/$file") == 0) {
              unlink ("$folder/$file");
              unset ($files [$offset]);
            }
          }
          // Remove the three most recent files from the array, so they don't get deleted.
          // Because scandir sorts the files, the files to be removed are at the end.
          array_pop ($files);
          array_pop ($files);
          array_pop ($files);
          // Remove the remaining files. These are the older versions.
          foreach ($files as $file) {
            unlink ("$folder/$file");
          }
        }
      }
    }
  }
  

  private function mainFolder ()
  {
    $folder = realpath (__DIR__ . "/../bibles");
    return $folder;
  }
  
  
  private function bibleFolder ($bible)
  {
    $folder = $this->mainFolder () . "/" . $bible;
    return $folder;
  }
  
  
  private function bookFolder ($bible, $book)
  {
    $folder = $this->bibleFolder ($bible) . "/" . $book;
    return $folder;
  }
  
  
  private function chapterFolder ($bible, $book, $chapter)
  {
    $folder = $this->bookFolder ($bible, $book) . "/" . $chapter;
    return $folder;
  }


  // Returns an array with the available Bibles.
  public function getBibles ()
  {
    $bibles = scandir ($this->mainFolder ());
    unset ($bibles [0]); // Remove folder ".".
    unset ($bibles [1]); // Remove folder "..".
    unset ($bibles [2]); // Remove file ".htaccess";
    $bibles = array_values ($bibles);
    return $bibles;
  }


  // Returns the ID for a named Bible.
  public function getID ($name)
  {
    $file = $this->bibleFolder ($name) . "/id";
    @$id = file_get_contents ($file);
    if ($id) return $id;
    return 0;
  }


  // Returns the Bible name for a Bible ID.
  public function getName ($id)
  {
    $bibles = $this->getBibles ();
    foreach ($bibles as $bible) {
      $bibleID = $this->getID ($bible);
      if ($id == $bibleID) return $bible;
    }
    return "Unknown";
  }


  // Creates a new empty Bible. Returns its ID.
  public function createBible ($name)
  {
    // If the Bible already exists, return its ID.
    $id = $this->getID ($name);
    if ($id > 0) {
      return $id;
    }

    // Get IDs in use.
    $ids = array ();
    $bibles = $this->getBibles ();
    foreach ($bibles as $bible) {
      $ids [] = $this->getID ($bible);
    }
    
    // Get the first free ID.
    $id = 1;
    while (in_array ($id, $ids)) $id++;

    // Create the empty system.
    $folder = $this->bibleFolder ($name);
    mkdir ($folder);
    file_put_contents ("$folder/id", $id);

    // Return new ID.
    return $id;
  }


  // Deletes a Bible.
  public function deleteBible ($name)
  {
    Filter_Rmdir::rmdir ($this->bibleFolder ($name));
  }


  // Stores data of one chapter in Bible $name,
  public function storeChapter ($name, $book, $chapter_number, $chapter_text)
  {
    $folder = $this->chapterFolder ($name, $book, $chapter_number);
    if (!file_exists ($folder)) mkdir ($folder, 0777, true);

    // Increate the chapter identifier, and store the chapter data.
    $id = $this->getChapterId ($name, $book, $chapter_number);
    $id++;
    $file = "$folder/$id";
    file_put_contents ($file, $chapter_text);

    // Update search fields.
    $this->updateSearchFields ($name, $book, $chapter_number);
  }


  public function updateSearchFields ($name, $book, $chapter)
  {
    $database_search = Database_Search::getInstance ();
    $database_search->updateSearchFields ($name, $book, $chapter);
  }


  // Returns an array with the available books in a Bible.
  public function getBooks ($bible)
  {
    // Read the books from the database.
    $folder = $this->bibleFolder ($bible);
    @$books = scandir ($folder);
    if ($books == false) return array ();
    foreach ($books as $offset => $book) {
      if (!is_numeric ($book)) unset ($books [$offset]);
    }

    // Sort the books according to the sequence defined in the books database.
    $sequence = array ();
    $database_books = Database_Books::getInstance ();
    foreach ($books as $book) {
      $sequence [] = $database_books->getSequenceFromId ($book);
    }
    array_multisort ($sequence, $books);

    // Result.
    return $books;
  }


  public function deleteBook ($bible, $book)
  {
    $folder = $this->bookFolder ($bible, $book);
    Filter_Rmdir::rmdir ($folder);
  }


  // Returns an array with the available chapters in a $book in a Bible.
  public function getChapters ($bible, $book)
  {
    // Read the chapters from the database.
    $folder = $this->bookFolder ($bible, $book);
    @$chapters = scandir ($folder);
    if ($chapters) {
      foreach ($chapters as $offset => $chapter) {
        if (!is_numeric ($chapter)) unset ($chapters [$offset]);
      }
      sort ($chapters, SORT_NUMERIC);
      return $chapters;
    }
    return array ();
  }


  public function deleteChapter ($bible, $book, $chapter)
  {
    $folder = $this->chapterFolder ($bible, $book, $chapter);
    Filter_Rmdir::rmdir ($folder);
  }


  // Gets the chapter data as a string.
  public function getChapter ($bible, $book, $chapter)
  {
    $folder = $this->chapterFolder ($bible, $book, $chapter);
    @$files = scandir ($folder);
    if ($files) {
      $count = count ($files);
      if ($count > 2) {
        $file = $files [$count - 1];
        $data = file_get_contents ("$folder/$file");
        return $data;
      }
    }
    return "";
  }


  // Gets the chapter id.
  public function getChapterId ($bible, $book, $chapter)
  {
    if (is_numeric ($bible)) $bible = $this->getName ($bible);
    $folder = $this->chapterFolder ($bible, $book, $chapter);
    $files = scandir ($folder);
    $count = count ($files);
    if ($count > 2) {
      $file = $files [$count - 1];
      return $file;
    }
    return 100000000;
  }


}


?>
