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


require_once ("../bootstrap/bootstrap.php");
$database_logs = Database_Logs::getInstance ();



// Security: The script runs from the cli SAPI only.
if (php_sapi_name () != "cli") {
  $database_logs->log ("Fatal: This only runs through the cli Server API", true);
  die;
}


$database_logs->log ("Start importing Psalmen van Datheen", true);


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


// Read and convert the "Psalmen van Datheen".
// The Bible in Bibledit-Web is going to be called as below.
$outputBible = "Psalmen van Datheen"; 


// Delete and (re)create the "Psalmen van Datheen".
$database_bibles->deleteBible ($outputBible);
$database_bibles->createBible ($outputBible);


// Book identifier for the Psalms.
$book = $database_books->getIdFromEnglish ("Psalms");


// Chapter 0.
$usfm = <<<'EOD'
\id PSA
\mt1 DE CL PSALMEN
\mt3 DES PROFETEN
\mt2 DAVIDS
\mt4 Met enige andere Lofzangen,
\mt4 Uit het Franse in Nederlandse dicht overgezet door
\mt3 PETRUS DATHENUS
EOD;
$database_bibles->storeChapter ($outputBible, $book, 0, $usfm);


// Go through the 150 chapters.
for ($chapter = 1; $chapter <= 150; $chapter++) {


  // Start building the USFM for the chapter.
  $usfm = "\\c $chapter\n";
  $usfm .= "\\cl Psalm $chapter\n";


  // Get the musical indication.
  $query = "SELECT aanwijzing_tekst FROM psalmen_datheen WHERE psalm_nummer = $chapter AND psalm_vers = 1;";
  $result = $mysqli->query ($query);
  if ($result === false) {
    $message = "Reading verses from Datheen: " . stripslashes ($mysqli->error);
    $database_logs->log ($message, true);
    echo $message . "\n";
    die;
  }
  $row = $result->fetch_row ();
  $indication = $row[0];
  if ($indication != "") {
    $usfm .= "\\q1 $indication\n";
    $usfm .= "\\b\n";
  }


  // Get the verses in the chapter.
  $verses = array ();
  $query = "SELECT DISTINCT psalm_vers FROM psalmen_datheen WHERE psalm_nummer = $chapter ORDER BY psalm_vers ASC;";
  $result = $mysqli->query ($query);
  for ($i = 0; $i < $result->num_rows; $i++) {
    $row = $result->fetch_row();
    $verses[] = $row[0];
  }
  
  
  // Go through the verses.
  foreach ($verses as $verse) {
    $query = "SELECT vers_tekst FROM psalmen_datheen WHERE psalm_nummer = $chapter AND psalm_vers = $verse;";
    $result = $mysqli->query ($query);
    $row = $result->fetch_row ();
    $text = $row[0];
    $text = utf8_encode ($text); // Convert from latin1 to utf8.
    $lines = explode ("<br />", $text);
    $lines[0] = "\n\\v $verse " . $lines[0];
    foreach ($lines as &$line) {
      $line = "\\q1 " . $line;
    }
    $text = implode ("\n", $lines) . "\n";
    $usfm .= $text;
    $usfm .= "\\b\n";
  }
  
  // Store USFM data for this chapter.
  $database_bibles->storeChapter ($outputBible, $book, $chapter, $usfm);
  

}


$database_logs->log ("Ready importing Psalmen van Datheen", true);

?>
