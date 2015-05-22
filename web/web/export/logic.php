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


require_once ("bootstrap/bootstrap.php");


class Export_Logic
{


  // Schedule all Bibles for exports.
  static public function scheduleAll ()
  {
    Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/index.php"));

  }


  // Schedule a Bible book for export to text and basic USFM format.
  // $bible: Bible.
  // $book: book.
  // $force: Whether to force an update, or else just to check the existence.
  static public function scheduleTextAndBasicUsfm ($bible)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $books = $database_bibles->getBooks ($bible);
    foreach ($books as $book) {
      Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/textusfm.php", $bible, $book));
    }
  }
  
  
  // Schedule a Bible for export to USFM format.
  static public function scheduleUsfm ($bible)
  {
    Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/usfm.php", $bible));
  }
  
  
  // Schedule export to OpenDocument.
  // $bible: Bible.
  static public function scheduleOpenDocument ($bible)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $books = $database_bibles->getBooks ($bible);
    foreach ($books as $book) {
      Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/odt.php", $bible, $book));
    }
  }


  // Schedule creation info documents.
  // $bible: Bible.
  static public function scheduleInfo ($bible)
  {
    Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/info.php", $bible));
  }


  // Schedule export to html.
  // $bible: Bible.
  static public function scheduleHtml ($bible)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $books = $database_bibles->getBooks ($bible);
    foreach ($books as $book) {
      Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/html.php", $bible, $book));
    }
  }


  // Schedule export to web.
  // $bible: Bible.
  static public function scheduleWeb ($bible)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $books = $database_bibles->getBooks ($bible);
    foreach ($books as $book) {
      Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/web.php", $bible, $book));
    }
  }


  // Schedule export to web index.
  // $bible: Bible.
  static public function scheduleWebIndex ($bible)
  {
    Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/web0.php", $bible));
  }


  static public function scheduleOnlineBible ($bible)
  {
    Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/onlinebible.php", $bible));
  }
  

  static public function scheduleESword ($bible)
  {
    Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/esword.php", $bible));
  }
  

  // The main exports directory.
  static public function mainDirectory ()
  {
    return realpath ("../exports");
  }
  

  // A Bible's export directory.
  static public function bibleDirectory ($bible)
  {
    return self::mainDirectory () . "/" . $bible;
  }


  // Directory for the USFM.
  // $type:
  // 0: directory for the full USFM.
  // 1: directory for the basic USFM.
  // 2: root USFM directory.
  static public function USFMdirectory ($bible, $type)
  {
    $directory = self::bibleDirectory ($bible);
    $directory .= "/usfm";
    switch ($type) {
      case 0: $directory .= "/full"; break;
      case 1: $directory .= "/basic"; break;
      default: break;
    }
    return $directory;
  }
  
  
  static public function webDirectory ($bible)
  {
    $directory = self::bibleDirectory ($bible);
    $directory .= "/web";
    return $directory;
  }
  
  
  static public function webBackLinkDirectory ($bible)
  {
    $directory = "../exports/$bible/web/";
    return $directory;
  }
  
  
  // Provides the base book file name, e.g. 01_Genesis.
  // Or 00_Bible for an entire Bible when $book = 0;
  static public function baseBookFileName ($book)
  {
    if ($book) {
      $database_books = Database_Books::getInstance ();
      $filename = sprintf("%0" . 2 . "d", $book) . "_" . $database_books->getEnglishFromId ($book);
    } else {
      $filename = "00_Bible";
    }
    return $filename;
  }


  // Provides the OSIS directory.
  // $bible: The Bible.
  static public function osisDirectory ($bible)
  {
    $directory = self::bibleDirectory ($bible);
    $directory .= "/osis";
    return $directory;
  }
  
  
  // $interpreter: 'py' (Python) or 'pl' (Perl).
  // $complexity: 1 (basic USFM) or 2 (full USFM)
  static public function osisSwordVariantDirectory ($interpreter, $complexity)
  {
    $directory = "basic";
    if ($complexity != 1) $directory = "full";
    $directory = "/$directory-$interpreter";
    return $directory;
  }


}


?>
