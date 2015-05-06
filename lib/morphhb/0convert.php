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
   
   You should have received a wacopy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
   */
  
  echo "Copying Hebrew data for book:\n";
  
  $output = __DIR__ . "/../databases/morphhb.sqlite";
  @unlink ($output);
  
  $db = new PDO ("sqlite:$output");
  
  $db->exec ("PRAGMA temp_store = MEMORY;");
  $db->exec ("PRAGMA synchronous = OFF;");
  $db->exec ("PRAGMA journal_mode = OFF;");
  
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS morphhb (
                                    book integer,
                                    chapter integer,
                                    verse integer,
                                    hebrew text
                                    );
EOD;
  $db->exec ($sql);
  
  $books = array (
                  "Gen",
                  "Exod",
                  "Lev",
                  "Num",
                  "Deut",
                  "Josh",
                  "Judg",
                  "Ruth",
                  "1Sam",
                  "2Sam",
                  "1Kgs",
                  "2Kgs",
                  "1Chr",
                  "2Chr",
                  "Ezra",
                  "Neh",
                  "Esth",
                  "Job",
                  "Ps",
                  "Prov",
                  "Eccl",
                  "Song",
                  "Isa",
                  "Jer",
                  "Lam",
                  "Ezek",
                  "Dan",
                  "Hos",
                  "Joel",
                  "Amos",
                  "Obad",
                  "Jonah",
                  "Mic",
                  "Nah",
                  "Hab",
                  "Zeph",
                  "Hag",
                  "Zech",
                  "Mal"
                  );
   
  foreach ($books as $book => $osis) {
    
    $book++;
    echo "$osis ";
    
    $xml = new XMLReader();
    
    $xml->open ("../morphhb/$osis.xml");
    
    $chapter = 0;
    $verse = 0;
    $word = false;
    $hebrew = "";
    
    while ($xml->read ()) {
      
      $nodeType = $xml->nodeType;
      
      $name = $xml->name;
      
      if ($nodeType == XMLReader::ELEMENT) {
        
        if ($name == "verse") {
          $osisID = $xml->getAttribute ("osisID");
          $osisID = explode (".", $osisID);
          $chapter = intval ($osisID[1]);
          $verse = intval ($osisID[2]);
        }
        
        if ($name == "w") {
          $word = true;
        }
        
      }
      
      if ($nodeType == XMLReader::TEXT) {
        if ($word) {
          $hebrew = $xml->value;
          $hebrew = trim ($hebrew);
          $hebrew = str_replace ("/", "", $hebrew);
          $hebrew = str_replace ("'", "''", $hebrew);
          $sql = "INSERT INTO morphhb (book, chapter, verse, hebrew) VALUES (book, chapter, verse, '$hebrew');";
          $db->exec ($sql);
        }
      }
      
      if ($nodeType == XMLReader::END_ELEMENT) {
        if ($name == "w") {
          $word = false;
        }
      }
      
    }
    
    $xml->close ();
  }
  
  $output = realpath ($output);
  echo "\n";
  echo "SQLite database has been created at:\n";
  echo "$output\n";

?>
