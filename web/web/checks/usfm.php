<?php
/**
* @package bibledit
*/
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


class Checks_Usfm // Todo
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


  public function __construct ()
  {
    $database_config_general = Database_Config_General::getInstance ();
    $stylesheet = $database_config_general->getExportStylesheet ();
  }
  

  public function initialize ()
  {
    $this->checkingResults = array ();
    $this->usfmMarkersAndText = array ();
    $this->usfmMarkersAndTextPointer = 0;
    $this->verseNumber = 0;
  }


  public function check ($usfm) // Todo
  {
    $this->newLineInUsfm ($usfm);

    $this->usfmMarkersAndText = Filter_Usfm::getMarkersAndText ($usfm);
    for ($this->usfmMarkersAndTextPointer = 0; $this->usfmMarkersAndTextPointer < count ($this->usfmMarkersAndText); $this->usfmMarkersAndTextPointer++) {
      $this->usfmItem = $this->usfmMarkersAndText [$this->usfmMarkersAndTextPointer];
      if (Filter_Usfm::isUsfmMarker ($this->usfmItem)) {
 
        // Current verse number for diagnostics.
        if ($this->usfmItem == '\v ') {
          $usfm = Filter_Usfm::peekTextFollowingMarker ($this->usfmMarkersAndText, $this->usfmMarkersAndTextPointer);
          $this->verseNumber = Filter_Usfm::peekVerseNumber ($usfm);
        }

        $this->malformedVerseNumber ();

      } else {
      }
    }
  }


  private function malformedVerseNumber ()
  {
    if ($this->usfmItem == '\v ') {
      $usfm = Filter_Usfm::peekTextFollowingMarker ($this->usfmMarkersAndText, $this->usfmMarkersAndTextPointer);
      $cleanVerseNumber = Filter_Usfm::peekVerseNumber ($usfm);
      $dirtyVerseNumber = explode (" ", $usfm);
      $dirtyVerseNumber = $dirtyVerseNumber [0];
      if ($cleanVerseNumber != $dirtyVerseNumber) {
        $this->addResult ("Malformed verse number", Checks_Usfm::displayFull);
      }
    }
  }


  private function newLineInUsfm ($usfm) // Todo
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
      $this->addResult ("New line within USFM: " . $bit, Checks_Usfm::displayNothing); // Todo
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
