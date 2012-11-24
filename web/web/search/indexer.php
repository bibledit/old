<?php
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

/*
This script goes through all html files in all manuals.
It prepares the html files for the indexer.
It echoes the content to stdout as an xml file fit for Sphinx.
It adds the URLs of the individual web pages,
as well as other attributes.
Sphinx will index this xml file.

*/


require_once ("../bootstrap/bootstrap.php");


// Basic variables.
//include ("/tmp/variables.php");
$document_identifier = 0;
$manualIdentifier = 0;


// Start the xml file for the sphinx indexer.
echo '<?xml version="1.0" encoding="utf-8"?>' . "\n";
echo '<sphinx:docset>' . "\n";


$database_config_general = Database_Config_General::getInstance ();
$database_notes = Database_Notes::getInstance ();


$siteUrl = $database_config_general->getSiteURL ();


// Go through the identifiers of all consultation notes.
$identifiers = $database_notes->getIdentifiers ();
foreach ($identifiers as $noteIdentifier) {


  // Assemble the title.
  $summary = $database_notes->getSummary ($noteIdentifier);
  $verses = Filter_Books::passagesDisplayInline ($database_notes->getPassages ($noteIdentifier));
  $title = "$summary | $verses";
  $title = Filter_Html::sanitize ($title);


  // Assemble the text.
  $text = $database_notes->getContents ($noteIdentifier);
  $text = Filter_Html::html2text ($text);
  $text = Filter_Html::sanitize ($text);


  $document_identifier++;
  echo "<sphinx:document id=\"$document_identifier\">\n";

  echo "<manual>$manualIdentifier</manual>\n";

  $url = "$siteUrl/consultations/notes.php?consultationnote=$noteIdentifier";
  echo "<url>$url</url>\n";

  echo "<title>$title</title>\n";

  echo "<text>$text</text>\n";

  // Output the content.
  echo "<content>\n";
  // The title is repeated in the context to give it a higher weight.
  echo "$title\n";
  echo "$title\n";
  echo "$title\n";
  echo "$title\n";
  echo $text;
  echo "</content>\n";

  echo "</sphinx:document>\n";

}


// Go through all the manuals in the included array.
$manuals = array ();
foreach ($manuals as $manual) {
  

  
  


/*
    // Get the html code.
    $contents = file_get_contents ($filepath);




    // Convert the html file to clean text.
    $text = "";
    unset ($output);
    $command = "html2text $filepath 2>&1";
    exec ($command, $output, $return_var);
    if ($return_var == 0) {
      $text = implode (" ", $output);
      $text = str_replace ("  ", " ", $text);
      $text = trim ($text);
      $text = htmlspecialchars ($text);
    }


    // Echo the search attributes.
    // The manual attribute specifies which manual to search in.
    // The system uses one Sphinx search daemon for all manuals on the site.
    // Without using the manual attribute, a search operation would look through all the manuals.
    // The manual attribute restricts the search to one manual.
    echo "<manual>$manualIdentifier</manual>\n";
    echo "<url>$filename</url>\n";
    echo "<title>$title</title>\n";
    echo "<text>$text</text>\n";


    // Echo the content. Give a higher weight to the title by repeating it.
    echo "<content>\n";
    echo "$title\n";
    echo "$title\n";
    echo "$title\n";
    echo "$title\n";
    echo "$title\n";
    echo "$title\n";
    echo "$title\n";
    echo "$title\n";
    echo "$title\n";
    echo "$title\n";
    echo $text;
    echo "</content>\n";

  
    // End of this document.
    echo "</sphinx:document>\n";


  }
*/



}


// Finish the xml document for the sphinx indexer.
echo '</sphinx:docset>' . "\n";


?>
