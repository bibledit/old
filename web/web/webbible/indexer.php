<?php
/*
** Copyright (©) 2003-2013 Teus Benschop.
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
This script goes through a Bible.
It prepares the data for the indexer.
It echoes the content to stdout as an xml file fit for Sphinx.
It adds the URLs of the individual web pages as well as other attributes.
Sphinx will index this xml file.
*/


require_once ("bibleditPath/bootstrap/bootstrap.php");


// Start the xml file for the sphinx indexer.
echo '<?xml version="1.0" encoding="utf-8"?>' . "\n";
echo '<sphinx:docset>' . "\n";


$database_config_general = Database_Config_General::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_styles = Database_Styles::getInstance ();
$styles_logic = Styles_Logic::getInstance ();
$document_identifier = 0;
$siteUrl = $database_config_general->getSiteURL ();


// Style information.
$paragraphMarkers = array ();
$noteOpeners = array ();
$noteClosers = array ();
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
      $noteOpeners [] = $marker;
      $noteClosers [] = "$marker*";
    }
    if ($style['subtype'] == FootEndNoteSubtypeEndnote) {
      $noteOpeners [] = $marker;
      $noteClosers [] = "$marker*";
    }
  }
  if ($style['type'] == StyleTypeCrossreference) {
    if ($style['subtype'] == CrossreferenceSubtypeCrossreference) {
      $noteOpeners [] = $marker;
      $noteClosers [] = "$marker*";
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


// Go through the Bible.
$bible = '_bible_';
$books = $database_bibles->getBooks ($bible);
foreach ($books as $book) {
  $chapters = $database_bibles->getChapters ($bible, $book);
  foreach ($chapters as $chapter) {
    $chapterText = $database_bibles->getChapter ($bible, $book, $chapter);
    $verses = Filter_Usfm::getVerseNumbers ($chapterText);
    foreach ($verses as $verse) {
      $textLine = "";
      $textLines = array ();
      $processingNote = false;
      $noteLine = "";
      $noteLines = array ();
      $verseText = Filter_Usfm::getVerseText ($chapterText, $verse);
      $markersAndText = Filter_Usfm::getMarkersAndText ($verseText);
      foreach ($markersAndText as $markerOrText) {
        if (substr ($markerOrText, 0, 1) == "\\") {
          $marker = substr ($markerOrText, 1);
          $marker = trim ($marker);
          if (in_array ($marker, $paragraphMarkers)) {
            if ($textLine != "") $textLines [] = $textLine;
            $textLine = "";
          }
          if (in_array ($marker, $noteOpeners)) {
            $processingNote = true;
          }
          if (in_array ($marker, $noteClosers)) {
            $processingNote = false;
            $noteLines [] = $noteLine;
            $noteLine = "";
          }
        } else {
          if ($processingNote) {
            $noteLine .= $markerOrText;
          } else {
            $textLine .= $markerOrText;
          }
        }
      }
      if ($textLine != "") $textLines [] = $textLine;
      if ($noteLine != "") $noteLines [] = $textLine;
      $url = Filter_Paths::htmlFileNameBible ("", $book, $chapter);
      $title = Filter_Books::passageDisplay ($book, $chapter, $verse);
      $text = implode ("\n", $textLines) . "\n" . implode ("\n", $noteLines);
      $text = trim ($text);
      
      $document_identifier++;
      echo "<sphinx:document id=\"$document_identifier\">\n";
      $url = Filter_Html::sanitize ($url);
      echo "<url>$url</url>\n";
      $title = Filter_Html::sanitize ($title);
      echo "<title>$title</title>\n";
      $text = Filter_Html::sanitize ($text);
      echo "<text>$text</text>\n";
      echo "<content>\n";
      echo $text;
      echo "</content>\n";
      echo "</sphinx:document>\n";
    }
  }
}


// Finish the xml document for the sphinx indexer.
echo '</sphinx:docset>' . "\n";


?>
