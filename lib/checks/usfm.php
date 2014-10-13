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


class Checks_Usfm
{

  // USFM and text.
  private $usfmMarkersAndText;
  private $usfmMarkersAndTextPointer;
  private $usfmItem;
  private $verseNumber;

  // Results of the checks.
  private $checkingResults;
  const displayNothing = 0;
  const displayCurrent = 1;
  const displayNext = 2;
  const displayFull = 3;

  // Stylesheet.
  private $markersStylesheet;

  // Matching markers.
  private $markersRequiringEndmarkers;
  private $openMatchingMarkers;


  public function __construct ($bible)
  {
    $database_config_bible = Database_Config_Bible::getInstance ();
    $database_styles = Database_Styles::getInstance ();
    $stylesheet = $database_config_bible->getExportStylesheet ($bible);
    $this->markersStylesheet = $database_styles->getMarkers ($stylesheet);
    $styles_logic = Styles_Logic::getInstance ();
    foreach ($this->markersStylesheet as $marker) {
      $style = $database_styles->getMarkerData ($stylesheet, $marker);
      $requiredEndmarker = false;
      $styleType = $style['type'];
      $styleSubtype = $style['subtype'];
      if ($styleType == StyleTypeFootEndNote) {
        if (($styleSubtype == FootEndNoteSubtypeFootnote) || ($styleSubtype == FootEndNoteSubtypeEndnote)) {
          $requiredEndmarker = true;
        }
      }
      if ($styleType == StyleTypeCrossreference) {
        if ($styleSubtype == CrossreferenceSubtypeCrossreference) {
          $requiredEndmarker = true;
        }
      }
      if ($styleType == StyleTypeInlineText) $requiredEndmarker = true;
      if ($styleType == StyleTypeWordlistElement) $requiredEndmarker = true;
      if ($requiredEndmarker) {
        $this->markersRequiringEndmarkers [] = $marker;
      }
    }
  }


  public function initialize ()
  {
    $this->checkingResults = array ();
    $this->usfmMarkersAndText = array ();
    $this->usfmMarkersAndTextPointer = 0;
    $this->verseNumber = 0;
    $this->openMatchingMarkers = array ();
  }


  public function finalize ()
  {
    if (count ($this->openMatchingMarkers) > 0) {
      $this->addResult ("Unclosed markers" . ": " . implode (" ", $this->openMatchingMarkers), Checks_Usfm::displayNothing);
    }
  }


  public function check ($usfm)
  {
    $this->newLineInUsfm ($usfm);

    $this->forwardSlash ($usfm);

    $this->usfmMarkersAndText = Filter_Usfm::getMarkersAndText ($usfm);
    for ($this->usfmMarkersAndTextPointer = 0; $this->usfmMarkersAndTextPointer < count ($this->usfmMarkersAndText); $this->usfmMarkersAndTextPointer++) {
      $this->usfmItem = $this->usfmMarkersAndText [$this->usfmMarkersAndTextPointer];
      if (Filter_Usfm::isUsfmMarker ($this->usfmItem)) {

        // Get the current verse number.
        if ($this->usfmItem == '\v ') {
          $verseCode = Filter_Usfm::peekTextFollowingMarker ($this->usfmMarkersAndText, $this->usfmMarkersAndTextPointer);
          $this->verseNumber = Filter_Usfm::peekVerseNumber ($verseCode);
        }

        $this->malformedVerseNumber ();

        $this->markerInStylesheet ();

        $this->malformedId ();

        $this->widowBackSlash ();

        $this->matchingEndmarker ();

      } else {
      }
    }
  }


  private function malformedVerseNumber ()
  {
    if ($this->usfmItem == '\v ') {
      $code = Filter_Usfm::peekTextFollowingMarker ($this->usfmMarkersAndText, $this->usfmMarkersAndTextPointer);
      $cleanVerseNumber = Filter_Usfm::peekVerseNumber ($code);
      $dirtyVerseNumber = explode (" ", $code);
      $dirtyVerseNumber = $dirtyVerseNumber [0];
      if ($cleanVerseNumber != $dirtyVerseNumber) {
        $this->addResult ("Malformed verse number", Checks_Usfm::displayFull);
      }
    }
  }


  private function newLineInUsfm ($usfm)
  {
    $position = false;
    $pos = strpos ($usfm, "\\\n");
    if ($pos !== false) {
      $position = $pos;
    }
    $pos = strpos ($usfm, "\\ \n");
    if ($pos !== false) {
      $position = $pos;
    }
    if ($position !== false) {
      $bit = substr ($usfm, $position - 1, 10);
      $bit = str_replace ("\n", " ", $bit);
      $this->addResult ("New line within USFM: " . $bit, Checks_Usfm::displayNothing);
    }
  }


  private function markerInStylesheet ()
  {
    $marker = substr ($this->usfmItem, 1, 100);
    $marker = trim ($marker);
    if (!Filter_Usfm::isOpeningMarker ($marker)) {
      $marker = substr ($marker, 0, -1);
    }
    if ($marker == "") return;
    if (in_array ($marker, $this->markersStylesheet)) return;
    $this->addResult ("Marker not in stylesheet", Checks_Usfm::displayCurrent);
  }


  private function malformedId ()
  {
    $item = substr ($this->usfmItem, 0, 3);
    if ($item == '\id') {
      $code = Filter_Usfm::peekTextFollowingMarker ($this->usfmMarkersAndText, $this->usfmMarkersAndTextPointer);
      $id =  substr ($code, 0, 3);
      $id = explode (" ", $code);
      $id = $id [0];
      $database_books = Database_Books::getInstance ();
      $book = $database_books->getIdFromUsfm ($id);
      if ($book == 0) {
        $this->addResult ("Unknown ID", Checks_Usfm::displayFull);
      } else {
        if (strtoupper ($id) != $id) {
          $this->addResult ("ID is not in uppercase", Checks_Usfm::displayFull);
        }
      }
    }
  }


  private function forwardSlash ($usfm)
  {
    $code = str_replace ("\n", " ", $usfm);
    $pos = strpos ($code, "/");
    if ($pos !== false) {
      $pos2 = strpos ($code, " ", $pos);
      if ($pos2 !== false) {
        $bit = substr ($code, $pos, $pos2 - $pos);
      } else {
        $bit = substr ($code, $pos, 100);
      }
      $pos2 = strpos ($bit, "*");
      if ($pos2 !== false) {
        $bit = substr ($bit, 0, $pos2);
      }
      $marker = substr ($bit, 1, 100);
      if (in_array ($marker, $this->markersStylesheet)) {
        $this->addResult ("Forward slash instead of backslash: " . $bit, Checks_Usfm::displayNothing);
      }
    }
  }


  private function widowBackSlash ()
  {
    $marker = $this->usfmItem;
    $marker = trim ($marker);
    if (strlen ($marker) == 1) {
      $this->addResult ("Widow backslash", Checks_Usfm::displayCurrent);
    }
  }


  private function matchingEndmarker ()
  {
    $marker = $this->usfmItem;
    // Remove the initial backslash, e.g. '\add' becomes 'add'.
    $marker = substr ($marker, 1);
    $marker = trim ($marker);
    $isOpener = Filter_Usfm::isOpeningMarker ($marker);
    if (!$isOpener) $marker = substr ($marker, 0, -1);
    if (!in_array ($marker, $this->markersRequiringEndmarkers)) return;
    if ($isOpener) {
      if (in_array ($marker, $this->openMatchingMarkers)) {
        $this->addResult ("Repeating opening marker", Checks_Usfm::displayCurrent);
      } else {
        $this->openMatchingMarkers [] = $marker;
      }
    } else {
      if (in_array ($marker, $this->openMatchingMarkers)) {
        $this->openMatchingMarkers = array_diff ($this->openMatchingMarkers, array ($marker));
      } else {
        $this->addResult ("Closing marker does not match opening marker" . " " . implode (" ", $this->openMatchingMarkers), Checks_Usfm::displayCurrent);
      }
      $this->previousMarkerForEndmarker = "";
    }
  }


  public function getResults ()
  {
    return $this->checkingResults;
  }


  private function addResult ($text, $modifier)
  {
    $current = $this->usfmItem;
    $next = Filter_Usfm::peekTextFollowingMarker ($this->usfmMarkersAndText, $this->usfmMarkersAndTextPointer);
    $next = substr ($next, 0, 20);
    switch ($modifier) {
      case Checks_Usfm::displayNothing:
        break;
      case Checks_Usfm::displayCurrent:
        $text .= ": " . $current;
        break;
      case Checks_Usfm::displayNext:
        $text .= ": " . $next;
        break;
      case Checks_Usfm::displayFull:
        $text .= ": " . $current . $next;
        break;
    }
    $this->checkingResults [] = array ((int) ($this->verseNumber) => $text);
  }


}


?>
