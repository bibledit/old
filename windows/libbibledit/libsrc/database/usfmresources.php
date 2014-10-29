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


// Database resilience: 
// The data is stored as separate files in the filesystem.
// That is resilient enough.


class Database_UsfmResources
{
  private static $instance;
  private function __construct() {
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_UsfmResources ();
    }
    return self::$instance;
  }


  private function mainFolder ()
  {
    $folder = realpath (__DIR__ . "/../databases/usfmresources");
    return $folder;
  }
  
  
  private function resourceFolder ($name)
  {
    $folder = $this->mainFolder () . "/$name";
    return $folder;
  }
  
  
  private function bookFolder ($name, $book)
  {
    $folder = $this->resourceFolder ($name) . "/$book";
    return $folder;
  }


  private function chapterFile ($name, $book, $chapter)
  {
    $folder = $this->bookFolder ($name, $book) . "/$chapter";
    return $folder;
  }


  public function getResources ()
  {
    $resources = scandir ($this->mainFolder ());
    $resources = Filter_Folders::cleanup ($resources);
    return $resources;
  }


  public function deleteResource ($name)
  {
    $path = $this->resourceFolder ($name);
    // If a folder: Delete it.
    if (is_dir ($path)) {
      Filter_Rmdir::rmdir ($path);
    }
    // If a file: Delete it.
    if (file_exists ($path)) {
      unlink ($path);
    }
  }


  public function deleteBook ($name, $book)
  {
    $path = $this->bookFolder ($name, $book);
    // If a folder: Delete it.
    if (is_dir ($path)) {
      Filter_Rmdir::rmdir ($path);
    }
    // If a file: Delete it.
    if (file_exists ($path)) {
      unlink ($path);
    }
  }
  
  
  public function deleteChapter ($name, $book, $chapter)
  {
    $file = $this->chapterFile ($name, $book, $chapter);
    @$unlink ($file);
  }
  

  public function storeChapter ($name, $book, $chapter, $usfm)
  {
    $file = $this->chapterFile ($name, $book, $chapter);
    $folder = dirname ($file);
    if (!file_exists ($folder)) mkdir ($folder, 0777, true);
    file_put_contents ($file, $usfm);
  }


  public function getBooks ($name)
  {
    $books = array ();
    $folder = $this->resourceFolder ($name);
    if (is_dir ($folder)) {
      $books = scandir ($folder);
    }
    $books = Filter_Folders::cleanup ($books);
    sort ($books);
    return $books;
  }


  public function getChapters ($name, $book)
  {
    $chapters = array ();
    $folder = $this->bookFolder ($name, $book);
    if (is_dir ($folder)) {
      $chapters = scandir ($folder);
    }
    $chapters = Filter_Folders::cleanup ($chapters);
    sort ($chapters, SORT_NUMERIC);
    return $chapters;
  }


  public function getUsfm ($name, $book, $chapter)
  {
    $file = $this->chapterFile ($name, $book, $chapter);
    @$usfm = file_get_contents ($file);
    if ($usfm) return $usfm;
    return "";
  }


  public function getSize ($name, $book, $chapter)
  {
    $file = $this->chapterFile ($name, $book, $chapter);
    $size = 0;
    if (file_exists ($file)) $size = filesize ($file);
    return $size;
  }


}


?>
