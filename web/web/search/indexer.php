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


// Basic variables.
//include ("/tmp/variables.php");
$document_identifier = 0;
$manualIdentifier = 0;


// Start the xml file for the sphinx indexer.
echo '<?xml version="1.0" encoding="utf-8"?>' . "\n";
echo '<sphinx:docset>' . "\n";


// Go through all the manuals in the included array.
$manuals = array ("NL", "EN", "FR", "RO", "DE");
foreach ($manuals as $manual) {
  

  
  // Path of this manual.
  $manualPath = "/path/to/$manual";
  
  
  // Use next manual identifier: Use to limit searching to one particular manual.
  $manualIdentifier++;
  


/*
  // Go through all the files in the $manualPath.
  foreach (new DirectoryIterator ($manualPath) as $fileInfo) {


    // Skip folders and non-html files.
    if ($fileInfo->isDot()) continue;
    if ($fileInfo->isDir()) continue;
    $extension = $fileInfo->getExtension();
    if ($extension != "htm") continue;


    // The indexer needs a unique document identifier.
    $document_identifier++;
    echo "<sphinx:document id=\"$document_identifier\">\n";


    // Get the filename.
    $filename = $fileInfo->getFilename();
    $basename = substr ($filename, 0, -4);
    $filepath = "$manualPath/$filename";

  
    // Get the html code.
    $contents = file_get_contents ($filepath);


    // Extract the title from the <title> tags in the html code.
    // Deal with possible empty titles.
    $title = "";
    $pattern = "/<title>(.*?)<\/title>/";
    preg_match($pattern, $contents, $matches);
    if ($matches != NULL) $title = trim ($matches[1]);
    if ($title == "") $title = "Untitled";
    $title = html_entity_decode ($title);
    $title = htmlspecialchars ($title);


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


  $document_identifier++;
  $filename = $manualPath;
  $title = "Title of manual $manual";
  $text = "The text of manual $manual\nYes, of the manual indeed\nYes, indeed, abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc abc Yes, indeed!";
  echo "<sphinx:document id=\"$document_identifier\">\n";
  echo "<manual>$manualIdentifier</manual>\n";
  echo "<url>$filename</url>\n";
  echo "<title>$title</title>\n";
  echo "<text>$text</text>\n";
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
  echo "</sphinx:document>\n";



}


// Finish the xml document for the sphinx indexer.
echo '</sphinx:docset>' . "\n";


?>
