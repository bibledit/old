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
$database_bibles = Database_Bibles::getInstance ();
$database_styles = Database_Styles::getInstance ();
$styles_logic = Styles_Logic::getInstance ();


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


// Style information.
$noteMarkers = array ();
$paragraphMarkers = array ();
$database_styles->ensureOneSheet ();
$stylesheet = $database_styles->getSheets ();
if (in_array ("Standard", $stylesheet)) {
  $stylesheet = "Standard";
} else {
  $stylesheet = $stylesheet [0];
}
$markers = $database_styles->getMarkers ($stylesheet);
foreach ($markers as $marker) {
  $style = $database_styles->getMarkerData ($stylesheet, $marker);
  if ($style['type'] == StyleTypeFootEndNote) {
    if ($style['subtype'] == FootEndNoteSubtypeFootnote) {
      $noteMarkers [] = $marker;
    }
    if ($style['subtype'] == FootEndNoteSubtypeEndnote) {
      $noteMarkers [] = $marker;
    }
  }
  if ($style['type'] == StyleTypeCrossreference) {
    if ($style['subtype'] == CrossreferenceSubtypeCrossreference) {
      $noteMarkers [] = $marker;
    }
  }
  if ($style['type'] == StyleTypeIdentifier) {
    $paragraphMarkers [] = $marker;
  }
  if ($style['type'] == StyleTypeStartsParagraph) {
    $paragraphMarkers [] = $marker;
  }
  if ($style['type'] == StyleTypeChapterNumber) {
    $paragraphMarkers [] = $marker;
  }
  if ($style['type'] == StyleTypePeripheral) {
    $paragraphMarkers [] = $marker;
  }
}


// Go through all Bibles.
$bibles = $database_bibles->getBibles ();
foreach ($bibles as $bible) {
  $books = $database_bibles->getBooks ($bible);
  foreach ($books as $book) {
    $chapters = $database_bibles->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      $chapterText = $database_bibles->getChapter ($bible, $book, $chapter);
      $verses = Filter_Usfm::getVerseNumbers ($chapterText);
      foreach ($verses as $verse) {
        $lines = array ();
        $line = "";
        $verseText = Filter_Usfm::getVerseText ($chapterText, $verse);
        var_dump ($verseText);
        $markersAndText = Filter_Usfm::getMarkersAndText ($verseText);
        foreach ($markersAndText as $markerOrText) { // Todo working here filtering USFM.
          if (substr ($markerOrText, 0, 1) !== false) {
            $marker = substr ($markerOrText, 1);
            $marker = trim ($marker);
            if (in_array ($marker, $paragraphMarkers)) {
              if ($line != "") $lines [] = $line;
              $line = "";
            }
          } else {
            $line .= $markerOrText;
          }
        }
        if ($line != "") $lines [] = $line;
        var_dump ($lines);
      }
    }
  }
}



// Finish the xml document for the sphinx indexer.
echo '</sphinx:docset>' . "\n";


?>
