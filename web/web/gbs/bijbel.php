<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2012 Teus Benschop.
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


function markup ($text)
{
  $text = str_replace ('<I>', '\it ', $text);
  $text = str_replace ('</I>', '\it*', $text);
  $text = str_replace ('<B>', '\bd ', $text);
  $text = str_replace ('</B>', '\bd*', $text);
  $text = str_replace ("\n", " ", $text);
  $text = str_replace ("  ", " ", $text);
  return $text;
}



// Security: The script runs from the cli SAPI only.
if (php_sapi_name () != "cli") {
  $database_logs->log ("Fatal: This only runs through the cli Server API", true);
  die;
}


$database_logs->log ("Start importing Statenbijbel", true);


// The relevant databases.
$database_config_general = Database_Config_General::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_books = Database_Books::getInstance ();


// Connect to the GBS database.
include ("database/credentials.php");
$mysqli = new mysqli($database_host, $database_user, $database_pass);
if ($mysqli->connect_errno) {
  $database_logs->log ($mysqli->connect_error, true);
  echo $mysqli->connect_error . "\n";
  die;
}
if (!$mysqli->select_db ("gbs")) {
  $message = "Fatal error: Could not select database gbs";
  $database_logs->log ($message, true);
  echo $message . "\n";
  die;
}


// Read and convert the Statenbijbel.
// The Bible in Bibledit-Web is going to be called as below.
$outputBible = "Statenbijbel"; 


// Delete and (re)create the Statenbijbel.
$database_bibles->deleteBible ($outputBible);
$database_bibles->createBible ($outputBible);


// Go through the 66 books of the Bible.
for ($book = 1; $book <= 66; $book++) {

  
  // Get the Dutch name of the book.
  $query = "SELECT DISTINCT bijbelboek FROM hoofdstukken WHERE bijbelboek_id = $book;";
  $result = $mysqli->query ($query);
  if ($result === false) {
    $message = "Reading book name: " . stripslashes ($mysqli->error);
    $database_logs->log ($message, true);
    echo $message . "\n";
  }
  $row = $result->fetch_row ();
  // Convert from latin1 to utf8, e.g. TIMOTHEÜS.
  $bookname = utf8_encode ($row[0]); 
  $database_logs->log ($bookname, true);


  // Chapter 0 data. This includes the introductions to the book.
  $usfm = '\id ' . $database_books->getUsfmFromId ($book) . "\n";
  $usfm .= '\h ' . $bookname . "\n";
  $usfm .= '\toc2 ' . $bookname . "\n";
  $usfm .= '\rem The document is free. It is copyrighted (©) 2011 by The Author. This work is licensed under the GNU Free Documentation License. See www.gnu.org slash licenses slash fdl.html' . "\n";

  $query = "SELECT titel FROM bijbelboek WHERE bijbelboek_id = $book;";
  $result = $mysqli->query ($query);
  if ($result === false) {
    $message = "Reading book title: " . stripslashes ($mysqli->error);
    $database_logs->log ($message, true);
    echo $message . "\n";
  }
  $row = $result->fetch_row ();
  $title = utf8_encode ($row[0]); 
  $title = explode ("\n", $title);
  foreach ($title as $line) {
    $line = trim ($line);
    if ($line != "") {
      $usfm .= '\mt ' . $line . "\n";
    }
  }

  $query = "SELECT inhoud FROM bijbelboek WHERE bijbelboek_id = $book;";
  $result = $mysqli->query ($query);
  if ($result === false) {
    $message = "Reading book introduction: " . stripslashes ($mysqli->error);
    $database_logs->log ($message, true);
    echo $message . "\n";
  }
  $row = $result->fetch_row ();
  $introduction = utf8_encode ($row[0]); 
  $introduction = markup ($introduction);
  $introduction = explode ('<br />', $introduction);
  foreach ($introduction as $line) {
    $usfm .= '\ip ' . $line . "\n";
  }

  $database_bibles->storeChapter ($outputBible, $book, 0, $usfm);
  
  
  // Number of chapters in this book.
  $query = "SELECT aantal_hoofdstukken FROM bijbelboek WHERE bijbelboek_id = $book;";
  $result = $mysqli->query ($query);
  if ($result === false) {
    $message = "Reading chapter count: " . stripslashes ($mysqli->error);
    $database_logs->log ($message, true);
    echo $message . "\n";
  }
  $row = $result->fetch_row ();
  $chaptercount = utf8_encode ($row[0]); 
  

  // Go through all the chapters.
  for ($chapter = 1; $chapter <= $chaptercount; $chapter++) {
    

    // Start building the USFM for the chapter.
    $usfm = "\\c $chapter\n";


    // Chapter title and introduction.
    $query = "SELECT titel, hoofdstukintro FROM hoofdstukken WHERE bijbelboek_id = $book AND hoofdstuknummer = $chapter;";
    $result = $mysqli->query ($query);
    if ($result === false) {
      $message = "Reading chapter title: " . stripslashes ($mysqli->error);
      $database_logs->log ($message, true);
      echo $message . "\n";
    }
    $row = $result->fetch_row ();
    $title = utf8_encode ($row[0]); 
    $introduction = utf8_encode ($row[1]); 
    $title = markup ($title);
    $usfm .= "\\cl $title\n";
    // Remove the new lines that the database contains.
    $introduction = str_replace ("\n", "", $introduction);
    $introduction = markup ($introduction);
    $usfm .= "\\ip $introduction\n";


    // Get the verses in the chapter.
    $verses = array ();
    $query = "SELECT DISTINCT tekstnummer FROM teksten WHERE bijbelboek_id = $book AND hoofdstuk = $chapter ORDER BY tekstnummer ASC;";
    $result = $mysqli->query ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row();
      $verses[] = $row[0];
    }


    // Go through the verses.
    foreach ($verses as $verse) {
      
      
      // The section heading.
      $query = "SELECT perikoop FROM teksten WHERE bijbelboek_id = $book AND hoofdstuk = $chapter AND tekstnummer = $verse;";
      $result = $mysqli->query ($query);
      $row = $result->fetch_row ();
      $sectionheading = utf8_encode ($row[0]);
      $sectionheading = trim ($sectionheading);
      if ($sectionheading != "") {
        $usfm .= "\\s $sectionheading\n";
      }


      // Get the notes.
      $notes = array ();
      $query = "SELECT kanttekeningnummer, kanttekeningtekst FROM kanttekeningen WHERE bijbelboek_id = $book AND hoofdstuk = $chapter AND tekstnummer = $verse;";
      $result = $mysqli->query ($query);
      for ($i = 0; $i < $result->num_rows; $i++) {
        $row = $result->fetch_row();
        $notes[$row[0]] = markup (utf8_encode ($row[1]));
      }
            
      
      // Verse text.
      $query = "SELECT tekst FROM teksten WHERE bijbelboek_id = $book AND hoofdstuk = $chapter AND tekstnummer = $verse;";
      // E.g. 1 IN den <SUP>1</SUP> <SUP>a</SUP>beginne <SUP>2</SUP>schiep God den <SUP>3</SUP>hemel en de aarde.
      $result = $mysqli->query ($query);
      $row = $result->fetch_row ();
      $text = $row[0];
      $text = utf8_encode ($text); // Convert from latin1 to utf8.
      $lines = explode ("</SUP>", $text);
      foreach ($lines as &$line) {
        $lines2 = explode ("<SUP>", $line);
        $line = $lines2[0];
        if (sizeof ($lines2) > 1) {
          $notecaller = $lines2[1];
          $notetext = "";
          if (isset ($notes [$notecaller])) {
            $notetext = $notes [$notecaller];
          }
          unset ($notes [$notecaller]);
          $footnote = is_numeric ($notecaller);
          if ($footnote) {
            $line .= '\f';
          } else {
            $line .= '\x';
          }
          $line .= " ";
          $line .= $notecaller;
          if ($notetext != "") {
            $line .= " ";
            if ($footnote) {
              $line .= '\ft';
            } else {
              $line .= '\xt';
            }
            $line .= " ";
            $line .= $notetext;
          }
          if ($footnote) {
            $line .= '\f*';
          } else {
            $line .= '\x*';
          }
        }
      }
      $text = implode ("", $lines);
      $text = markup ($text);
      $text = str_replace ("<br />", "\n\\p ", $text);
      $usfm .= "\\p\n";
      $usfm .= "\\v $text\n";
     

    }


    // Store USFM data for this chapter.
    $database_bibles->storeChapter ($outputBible, $book, $chapter, $usfm);

    
  }
  
  
}


$database_logs->log ("Ready importing Statenbijbel", true);


?>
