<?php

/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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


/**
* This class filters USFM text, converting it into other formats.
*/
class Filter_Text // Todo implement and test.
{
  
  private $inputUsfm; // Input USFM as a string.
  private $usfmMarkersAndText; // Array of strings alternating between USFM and text.
  private $usfmMarkersAndTextPointer;

  private $chapterUsfmMarkersAndText; // Array holding a chapter of USFM code, alternating between USFM and text.
  private $chapterUsfmMarkersAndTextPointer;
  
  private $styles; // An array holding arrays of style information.
  private $chapterMarker; // Usually this is: \c
  
  private $currentBookIdentifier; // Book identifier, e.g. 1, 2, 3, and so on.
  private $currentChapterNumber; // Chapter number, e.g. 1, 2, 3, etc.
  private $currentVerseNumber; // Verse number, e.g. "0", "1", "2", and so on.
  public $numberOfChaptersPerBook; // Array of (book, chapter number).
  
  public $runningHeaders; // Array with numerical keys, and values like array (book, chapter, verse, marker, header value).
  public $longTOCs; // Array with numerical keys, and values like array (book, chapter, verse, marker, TOC value).
  public $shortTOCs; // Array with numerical keys, and values like array (book, chapter, verse, marker, TOC value).
  public $bookAbbreviations; // Array with numerical keys, and values like array (book, chapter, verse, marker, abbreviation value).
  
  public $chapterLabels; // Array with numerical keys, and values like array (book, chapter, verse, marker, label value).
  public $publishedChapterMarkers; // Array with numerical keys, and values like array (book, chapter, verse, marker, marker value).

  
  /**
  * Class constructor.
  */
  public function __construct ()
  {
    $this->numberOfChaptersPerBook = array ();
    $this->runningHeaders = array ();
    $this->longTOCs = array ();
    $this->shortTOCs = array ();
    $this->bookAbbreviations = array ();
    $this->chapterLabels = array ();
    $this->publishedChapterMarkers = array ();
  }
  

  /**
  * This function adds USFM code to the class.
  * $code: string or array of USFM code.
  */
  public function addUsfmCode($code)
  {
    if (is_array ($code)) {
      $this->inputUsfm .= implode ("\n", $code);
    }
    if (is_string ($code)) {
      $this->inputUsfm .= $code;
    }
    $this->inputUsfm .= "\n";
  }
  
  
  /**
  * This function runs the filter.
  */
  public function run ()
  {
    // Sort the USFM code out and separate it into markers and text.
    $this->usfmMarkersAndText = Filter_Usfm::getMarkersAndText ($this->inputUsfm);
    unset ($this->inputUsfm);
    
    // Get the styles.
    $this->getStyles ();

    // Preprocess.
    $this->preprocessingStage ();
    
  }


  /**
  * This function return true when there is still unprocessed USFM code available.
  */
  private function unprocessedUsfmCodeAvailable ()
  {
    return (($this->usfmMarkersAndTextPointer) < count ($this->usfmMarkersAndText));
  }
  
  
  /**
  * This function stores data in the class: 
  * the next chapter from the unprocessed USFM code.
  */
  private function getUsfmNextChapter ()
  {
    // Initialization.
    $this->chapterUsfmMarkersAndText = array ();
    $this->chapterUsfmMarkersAndTextPointer = 0;
    $firstLine = true;
    
    // Obtain the standard marker for the chapter number.
    // Deal with the unlikely case that the chapter marker is non-standard.
    if (!isset ($this->chapterMarker)) {
      $this->chapterMarker = "c";
      foreach ($this->styles as $style) {
        if ($style["type"] == StyleTypeChapterNumber) {
          $this->chapterMarker = $style["marker"];
          break;
        }
      }
    }

    while ($this->unprocessedUsfmCodeAvailable ()) {
      // Trim white space for various purposes.
      $item = trim ($this->usfmMarkersAndText[$this->usfmMarkersAndTextPointer]);
      if (!$firstLine) {
        if ($item == ("\\" . $this->chapterMarker)) {
          return;
        }
      }
      $this->chapterUsfmMarkersAndText [] = $item;
      $firstLine = false;
      $this->usfmMarkersAndTextPointer++;
    }
  }
  

  /**
  * This function gets the styles from the database, 
  * and stores them in the object for quicker access.
  */
  private function getStyles ()
  {
    $this->styles = array ();
    $styles_logic = Styles_Logic::getInstance (); // This is to get the relevant styles information include.
    $database_styles = Database_Styles::getInstance ();
    $database_config_user = Database_Config_User::getInstance ();
    $stylesheet = $database_config_user->getStylesheet ();
    $markers = $database_styles->getMarkers ($stylesheet);
    foreach ($markers as $marker) {
      $this->styles [$marker] = $database_styles->getMarkerData ($stylesheet, $marker);
    }
  }


  /**
  * This function does the preprocessing of the USFM code 
  * extracting a variety of information from it
  */
  private function preprocessingStage ()
  {
    $this->usfmMarkersAndTextPointer = 0;
    while ($this->unprocessedUsfmCodeAvailable ()) {
      $this->getUsfmNextChapter ();
      for ($this->chapterUsfmMarkersAndTextPointer = 0; $this->chapterUsfmMarkersAndTextPointer < count ($this->chapterUsfmMarkersAndText); $this->chapterUsfmMarkersAndTextPointer++) {
        $currentItem = $this->chapterUsfmMarkersAndText[$this->chapterUsfmMarkersAndTextPointer];
        if (Filter_Usfm::isUsfmMarker ($currentItem)) {
          if (Filter_Usfm::isOpeningMarker ($currentItem)) {
            $marker = substr ($currentItem, 1); // Remove the initial backslash, e.g. \id becomes 'id'.
            if (array_key_exists ($marker, $this->styles)) {
              switch ($this->styles[$marker]['type']) {
                case StyleTypeIdentifier:
                  switch ($this->styles[$marker]['subtype']) {
                    case IdentifierSubtypeBook:
                    {
                      // Get book number.
                      $s = Filter_Usfm::getBookIdentifier ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
                      $database_books = Database_Books::getInstance ();
                      $this->currentBookIdentifier = $database_books->getIdFromUsfm ($s);
                      // Reset chapter and verse numbers.
                      $this->currentChapterNumber = 0;
                      $this->numberOfChaptersPerBook[$this->currentBookIdentifier] = 0;
                      $this->currentVerseNumber = "0";
                      break;
                    }
                    case IdentifierSubtypeRunningHeader:
                    {
                      $runningHeader = Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
                      $this->runningHeaders [] = array ('book' => $this->currentBookIdentifier, 'chapter' => $this->currentChapterNumber, 'verse' => $this->currentVerseNumber, 'marker' => $marker, 'value' => $runningHeader);
                      break;
                    }
                    case IdentifierSubtypeLongTOC:
                    {
                      $longTOC = Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
                      $this->longTOCs [] = array ('book' => $this->currentBookIdentifier, 'chapter' => $this->currentChapterNumber, 'verse' => $this->currentVerseNumber, 'marker' => $marker, 'value' => $longTOC);
                      break;
                    }
                    case IdentifierSubtypeShortTOC:
                    {
                      $shortTOC = Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
                      $this->shortTOCs [] = array ('book' => $this->currentBookIdentifier, 'chapter' => $this->currentChapterNumber, 'verse' => $this->currentVerseNumber, 'marker' => $marker, 'value' => $shortTOC);
                      break;
                    }
                    case IdentifierSubtypeBookAbbrev:
                    {
                      $bookAbbreviation = Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
                      $this->bookAbbreviations [] = array ('book' => $this->currentBookIdentifier, 'chapter' => $this->currentChapterNumber, 'verse' => $this->currentVerseNumber, 'marker' => $marker, 'value' => $bookAbbreviation);
                      break;
                    }
                    case IdentifierSubtypeChapterLabel:
                    {
                      $chapterLabel = Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
                      $this->chapterLabels [] = array ('book' => $this->currentBookIdentifier, 'chapter' => $this->currentChapterNumber, 'verse' => $this->currentVerseNumber, 'marker' => $marker, 'value' => $chapterLabel);
                      break;
                    }
                    case IdentifierSubtypePublishedChapterMarker:
                    {
                      $publishedChapterMarker = Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
                      $this->publishedChapterMarkers [] = array ('book' => $this->currentBookIdentifier, 'chapter' => $this->currentChapterNumber, 'verse' => $this->currentVerseNumber, 'marker' => $marker, 'value' => $publishedChapterMarker);
                      break;
                    }
                  }
                  break;
                case StyleTypeChapterNumber:
                {
                  $number = Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
                  $number = Filter_Numeric::integer_in_string ($number);
                  $this->currentChapterNumber = $number;
                  $this->numberOfChaptersPerBook[$this->currentBookIdentifier] = $number;
                  $this->currentVerseNumber = "0";
                  break;
                }
                case StyleTypeVerseNumber:
                {
                  $number = Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
                  $number = Filter_Numeric::integer_in_string ($number);
                  $this->currentVerseNumber = $number;
                  break;
                }
              }
            }
          }
        }
      }
    }
  }


  /**
  * This function produces the Java program that can produce the Info Document.
  * The Info Document contains formatting information, collected from the USFM code.
  * $path: Path to the document.
  * Returns: The Java code
  */
  public function produceInfoDocument ($path)
  {
    $database_books = Database_Books::getInstance ();

    $odfdom_text = new Odfdom_Text;
    $odfdom_text->initialize ();
    
    // Indicate the number of chapters per book.
    $odfdom_text->newParagraph ();
    $odfdom_text->addText (gettext ("Number of chapters per book"));
    foreach ($this->numberOfChaptersPerBook as $book => $chapterCount) {
      $line = $database_books->getEnglishFromId ($book) . " => " . $chapterCount;
      $odfdom_text->newParagraph ();
      $odfdom_text->addText ($line);
    }
    $odfdom_text->newParagraph ();
    
    // Indicate the running headers.
    $odfdom_text->newParagraph ();
    $odfdom_text->addText (gettext ("Running headers"));
    foreach ($this->runningHeaders as $item) {
      $line = $database_books->getEnglishFromId ($item['book']) . " (USFM " . $item['marker'] . ") => " . $item['value'];
      $odfdom_text->newParagraph ();
      $odfdom_text->addText ($line);
    }
    $odfdom_text->newParagraph ();
    
    // Indicate the Table of Contents entries.
    $odfdom_text->newParagraph ();
    $odfdom_text->addText (gettext ("Long table of contents entries"));
    foreach ($this->longTOCs as $item) {
      $line = $database_books->getEnglishFromId ($item['book']) . " (USFM " . $item['marker'] . ") => " . $item['value'];
      $odfdom_text->newParagraph ();
      $odfdom_text->addText ($line);
    }
    $odfdom_text->newParagraph ();
    $odfdom_text->newParagraph ();
    $odfdom_text->addText (gettext ("Short table of contents entries"));
    foreach ($this->shortTOCs as $item) {
      $line = $database_books->getEnglishFromId ($item['book']) . " (USFM " . $item['marker'] . ") => " . $item['value'];
      $odfdom_text->newParagraph ();
      $odfdom_text->addText ($line);
    }
    $odfdom_text->newParagraph ();

    // Indicate book abbreviations.
    $odfdom_text->newParagraph ();
    $odfdom_text->addText (gettext ("Book abbreviations"));
    foreach ($this->bookAbbreviations as $item) {
      $line = $database_books->getEnglishFromId ($item['book']) . " (USFM " . $item['marker'] . ") => " . $item['value'];
      $odfdom_text->newParagraph ();
      $odfdom_text->addText ($line);
    }
    $odfdom_text->newParagraph ();
        
    // Indicate the chapter specials.
    $odfdom_text->newParagraph ();
    $odfdom_text->addText (gettext ("Publishing chapter labels"));
    foreach ($this->chapterLabels as $item) {
      $line = $database_books->getEnglishFromId ($item['book']) . " (USFM " . $item['marker'] . ") => " . $item['value'];
      $odfdom_text->newParagraph ();
      $odfdom_text->addText ($line);
    }
    $odfdom_text->newParagraph ();
    $odfdom_text->newParagraph ();
    $odfdom_text->addText (gettext ("Publishing alternate chapter numbers"));
    foreach ($this->publishedChapterMarkers as $item) {
      $line = $database_books->getEnglishFromId ($item['book']) . " (USFM " . $item['marker'] . ") => " . $item['value'];
      $odfdom_text->newParagraph ();
      $odfdom_text->addText ($line);
    }
    $odfdom_text->newParagraph ();
    
    $odfdom_text->finalize ($path);
/*

  

  
  public $; // Array with numerical keys, and values like array (book, chapter, verse, marker, label value).
  public $; // Array with numerical keys, and values like array (book, chapter, verse, marker, marker value).
*/
    return $odfdom_text->javaCode;
  }


  
  
}

?>

