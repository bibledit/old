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
class Filter_Text // Todo implement / test.
{
  
  private $usfmMarkersAndText; // Array of strings alternating between USFM and text.
  private $usfmMarkersAndTextPointer;

  private $chapterUsfmMarkersAndText; // Array holding a chapter of USFM code, alternating between USFM and text.
  private $chapterUsfmMarkersAndTextPointer;
  
  private $styles; // An array holding arrays of style information.
  private $chapterMarker; // Usually this is: \c
  private $createdOdfStyles; // Array holding styles created in Odfdom.
  
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

  public $odfdom_text_standard; // Object for creating OpenDocument with main text in standard form.

  public $info; // Array with strings.
  public $fallout; // Array with strings.
  
  /**
  * Class constructor.
  */
  public function __construct ()
  {
    $this->createdOdfStyles = array ();
    $this->numberOfChaptersPerBook = array ();
    $this->runningHeaders = array ();
    $this->longTOCs = array ();
    $this->shortTOCs = array ();
    $this->bookAbbreviations = array ();
    $this->chapterLabels = array ();
    $this->publishedChapterMarkers = array ();
    $this->odfdom_text_standard = new Odfdom_Text;
    $this->info = array ();
    $this->fallout = array ();
  }
  

  /**
  * This function adds USFM code to the class.
  * $code: string or array of USFM code.
  */
  public function addUsfmCode($code)
  {
    // Get the USFM $code as a string.
    if (is_array ($code)) {
      $code = implode ("\n", $code);
    }
    $code .= "\n";
    // Sort the USFM $code out and separate it into markers and text.
    $markersAndText = Filter_Usfm::getMarkersAndText ($code);
    foreach ($markersAndText as $item) {
      $this->usfmMarkersAndText [] = $item;
    }
  }
  
  
  /**
  * This function runs the filter.
  * $standardFilename - The filename for the standard OpenDocument text.
  */
  public function run ($standardFilenameOdt)
  {
    // Get the styles.
    $this->getStyles ();

    // Preprocess.
    $this->preprocessingStage ();

    // Process data.
    $this->process ();
    
    // Finalize the documents.
    $this->odfdom_text_standard->finalize ($standardFilenameOdt);
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
      $item = $this->usfmMarkersAndText[$this->usfmMarkersAndTextPointer];
      if (!$firstLine) {
        if (trim ($item) == ("\\" . $this->chapterMarker)) {
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
    $styles_logic = Styles_Logic::getInstance (); // This is to get the relevant styles information included.
    $this->odfdom_text_standard->createPageBreakStyle ();
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
  * extracting a variety of information.
  */
  private function preprocessingStage ()
  {
    $this->usfmMarkersAndTextPointer = 0;
    while ($this->unprocessedUsfmCodeAvailable ()) {
      $this->getUsfmNextChapter ();
      for ($this->chapterUsfmMarkersAndTextPointer = 0; $this->chapterUsfmMarkersAndTextPointer < count ($this->chapterUsfmMarkersAndText); $this->chapterUsfmMarkersAndTextPointer++) {
        $currentItem = $this->chapterUsfmMarkersAndText[$this->chapterUsfmMarkersAndTextPointer];
        if (Filter_Usfm::isUsfmMarker ($currentItem)) {
          $marker = trim ($currentItem); // Change, e.g. '\id ' to '\id'.
          $marker = substr ($marker, 1); // Remove the initial backslash, e.g. '\id' becomes 'id'.
          if (Filter_Usfm::isOpeningMarker ($marker)) {
            if (array_key_exists ($marker, $this->styles)) {
              $style = $this->styles[$marker];
              switch ($style['type']) {
                case StyleTypeIdentifier:
                  switch ($style['subtype']) {
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
  * This function does the processing of the USFM code,
  * formatting the document and extracting other useful information.
  */
  private function process ()
  {
    $processedBooksCount = 0;
    $this->usfmMarkersAndTextPointer = 0;
    while ($this->unprocessedUsfmCodeAvailable ()) {
      $this->getUsfmNextChapter ();
      for ($this->chapterUsfmMarkersAndTextPointer = 0; $this->chapterUsfmMarkersAndTextPointer < count ($this->chapterUsfmMarkersAndText); $this->chapterUsfmMarkersAndTextPointer++) {
        $currentItem = $this->chapterUsfmMarkersAndText[$this->chapterUsfmMarkersAndTextPointer];
        if (Filter_Usfm::isUsfmMarker ($currentItem)) 
        {
          // Store indicator whether the marker is an opening marker. This information will be lost later on.
          $isOpeningMarker = Filter_Usfm::isOpeningMarker ($currentItem);
          // Clean up the marker, so we remain with the basic version, e.g. 'id'.
          $marker = Filter_Usfm::getMarker ($currentItem);
          if (array_key_exists ($marker, $this->styles)) 
          {
            $style = $this->styles[$marker];
            switch ($style['type']) 
            {
              case StyleTypeIdentifier:
              {
                switch ($style['subtype']) 
                {
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
                    // Throw away whatever follows the \id, e.g. 'GEN xxx xxx'.
                    Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
                    // Whether to insert a new page before the book. But never before the first book.
                    if ($style['userbool1']) {
                      if ($processedBooksCount) {
                        $this->odfdom_text_standard->newPageBreak ();
                      }
                    }
                    $processedBooksCount++;
                    break;
                  }
                  case IdentifierSubtypeEncoding:
                  {
                    $this->addToFallout ("Text encoding indicator not supported. Encoding is always in UTF8: \\$marker", true);
                    break;
                  }
                  case IdentifierSubtypeComment:
                  {
                    $this->addToInfo ("Comment: \\$marker", true);
                    break;
                  }
                  case IdentifierSubtypeRunningHeader:
                  {
                    // This information already went into the Info document during the preprocessing stage.
                    // Remove it from the USFM input stream.
                    Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
                    // Ideally this information should be inserted in the headers of the standard text document.
                    // UserBool2RunningHeaderLeft:
                    // UserBool3RunningHeaderRight:
                    break;
                  }
                  case IdentifierSubtypeLongTOC:
                  {
                    // This information already went into the Info document. Remove it from the USFM stream.
                    Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
                    break;
                  }
                  case IdentifierSubtypeShortTOC:
                  {
                    // This information already went into the Info document. Remove it from the USFM stream.
                    Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
                    break;
                  }
                  case IdentifierSubtypeBookAbbrev:
                  {
                    // This information already went into the Info document. Remove it from the USFM stream.
                    Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
                    break;
                  }
                  case IdentifierSubtypeChapterLabel: // Todo this information is used when creating the chapter text.
                  {
                    // This information is already in the object. Remove it from the USFM stream.
                    Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
                    break;
                  }
                  case IdentifierSubtypePublishedChapterMarker: // Todo this info is used when creating the chapter text.
                  {
                    // This information is already in the object. Remove it from the USFM stream.
                    Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
                    break;
                  }
                  case IdentifierSubtypeCommentWithEndmarker:
                  {
                    if ($isOpeningMarker) {
                      $this->addToInfo ("Comment: \\$marker", true);
                    }
                    break;
                  }
                  default:
                  {
                    $this->addToFallout ("Unknown markup: \\$marker", true);
                    break;
                  }
                }
                break;
              }
              case StyleTypeNotUsedComment:
              {
                $this->addToFallout ("Unknown markup: \\$marker", true);
                break;
              }
              case StyleTypeNotUsedRunningHeader:
              {
                $this->addToFallout ("Unknown markup: \\$marker", true);
                break;
              }
              case StyleTypeStartsParagraph:
              {
                switch ($style['subtype']) 
                {
                  case ParagraphSubtypeMainTitle:
                  case ParagraphSubtypeSubTitle:
                  case ParagraphSubtypeSectionHeading:
                  {
                    $this->newParagraph ($style, true);
                    break;
                  }
                  case ParagraphSubtypeNormalParagraph:
                  default:
                  {
                    $this->newParagraph ($style, false);
                    break;
                  }
                }
                break;
              }
              case StyleTypeInlineText: // Todo local styles, e.g. italics.
              {
                break;
              }
              case StyleTypeChapterNumber: // Todo output to document.
              {
                $number = Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
                $number = Filter_Numeric::integer_in_string ($number);
                $this->currentChapterNumber = $number;
                $this->numberOfChaptersPerBook[$this->currentBookIdentifier] = $number;
                $this->currentVerseNumber = "0";
                // UserBool1PrintChapterAtFirstVerse:
                // UserBool2ChapterInLeftRunningHeader:
                // UserBool3ChapterInRightRunningHeader:
                break;
              }
              case StyleTypeVerseNumber: // Todo output to document.
              {
                $number = Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
                $number = Filter_Numeric::integer_in_string ($number);
                $this->currentVerseNumber = $number;
                // UserBool1VerseRestartsParagraph: - important at times.
                break;
              }
              case StyleTypeFootEndNote: // Todo handle notes including subtypes.
              {
                switch ($style['subtype']) 
                {
                  case FootEndNoteSubtypeFootnote:
                  {
                    break;
                  }
                  case FootEndNoteSubtypeEndnote:
                  {
                    break;
                  }
                  case FootEndNoteSubtypeStandardContent:
                  {
                    break;
                  }
                  case FootEndNoteSubtypeContent:
                  {
                    break;
                  }
                  case FootEndNoteSubtypeContentWithEndmarker:
                  {
                    break;
                  }
                  case FootEndNoteSubtypeParagraph:
                  {
                    break;
                  }
                  default:
                  {
                    break;
                  }
                }
                // UserBool1NoteAppliesToApocrypha: For xref too?
                // UserInt1NoteNumbering:
                // UserInt2NoteNumberingRestart:
                // UserInt2EndnotePosition:
                // UserString1NoteNumberingSequence:
                // UserString2DumpEndnotesHere: But this one should go out.
/*
case NoteNumbering123:
{
break;
}
case NoteNumberingAbc:
{
break;
}
case NoteNumberingUser:
{
break;
}

case NoteRestartNumberingNever:
{
break;
}
case NoteRestartNumberingEveryBook:
{
break;
}
case NoteRestartNumberingEveryChapter:
{
break;
}

case EndNotePositionAfterBook:
{
break;
}
case EndNotePositionVeryEnd:
{
break;
}
case EndNotePositionAtMarker:
{
break;
}
*/                  
                break;
              }
              case StyleTypeCrossreference: // Todo handle xref.
              {
                switch ($style['subtype']) 
                {
                  case CrossreferenceSubtypeCrossreference:
                  {
                    break;
                  }
                  case CrossreferenceSubtypeStandardContent:
                  {
                    break;
                  }
                  case CrossreferenceSubtypeContent:
                  {
                    break;
                  }
                  case CrossreferenceSubtypeContentWithEndmarker:
                  {
                    break;
                  }
                  default:
                  {
                    break;
                  }
                }
                // UserInt1NoteNumbering:
                // UserInt2NoteNumberingRestart:
                // UserString1NoteNumberingSequence:
/*
case NoteNumbering123:
{
break;
}
case NoteNumberingAbc:
{
break;
}
case NoteNumberingUser:
{
break;
}

case NoteRestartNumberingNever:
{
break;
}
case NoteRestartNumberingEveryBook:
{
break;
}
case NoteRestartNumberingEveryChapter:
{
break;
}
*/                  
                break;
              }
              case StyleTypePeripheral: // Todo handle this one, let's see how exactly. Fallout? Info?
              {
                switch ($style['subtype']) 
                {
                  case PeripheralSubtypePublication:
                  {
                    break;
                  }
                  case PeripheralSubtypeTableOfContents:
                  {
                    break;
                  }
                  case PeripheralSubtypePreface:
                  {
                    break;
                  }
                  case PeripheralSubtypeIntroduction:
                  {
                    break;
                  }
                  case PeripheralSubtypeGlossary:
                  {
                    break;
                  }
                  case PeripheralSubtypeConcordance:
                  {
                    break;
                  }
                  case PeripheralSubtypeIndex:
                  {
                    break;
                  }
                  case PeripheralSubtypeMapIndex:
                  {
                    break;
                  }
                  case PeripheralSubtypeCover:
                  {
                    break;
                  }
                  case PeripheralSubtypeSpine:
                  {
                    break;
                  }
                  default:
                  {
                    break;
                  }
                }
                break;
              }
              case StyleTypePicture: // Todo Insert pictures.
              {
                break;
              }
              case StyleTypePageBreak: // Todo see how to get this in OpenDocument.
              {
                break;
              }
              case StyleTypeTableElement: // Todo create table.
              {
                switch ($style['subtype']) 
                {
                  case TableElementSubtypeRow:
                  {
                    break;
                  }
                  case TableElementSubtypeHeading:
                  {
                    break;
                  }
                  case TableElementSubtypeCell:
                  {
                    break;
                  }
                  default:
                  {
                    break;
                  }
                }
                // UserInt1TableColumnNumber:
                break;
              }
              case StyleTypeWordlistElement: // Todo see to this one.
              {
                switch ($style['subtype']) 
                {
                  case WorListElementSubtypeWordlistGlossaryDictionary:
                  {
                    break;
                  }
                  case WorListElementSubtypeHebrewWordlistEntry:
                  {
                    break;
                  }
                  case WorListElementSubtypeGreekWordlistEntry:
                  {
                    break;
                  }
                  case WorListElementSubtypeSubjectIndexEntry:
                  {
                    break;
                  }
                  default:
                  {
                    break;
                  }
                }
                // UserString1WordListEntryAddition:
                break;
              }
              default: // Todo
              {
                break;
              }
            }
          } else {
            // Here is an unknown marker. Add to fallout, plus any text that follows.
            $this->addToFallout ("Unknown marker \\$marker", true);
          }
        } else {
          // Here is no marker. Treat it as text.
          $this->odfdom_text_standard->addText ($currentItem);
        }
      }
    }
  }


  /**
  * This function produces the Java code that produces the Info Document.
  * The Info Document contains formatting information, collected from the USFM code.
  * $path: Path to the document.
  * Returns: The Java code
  */
  public function produceInfoDocument ($path)
  {
    $database_books = Database_Books::getInstance ();

    $odfdom_text = new Odfdom_Text;
    
    // Indicate the number of chapters per book.
    $odfdom_text->newHeading1 (gettext ("Number of chapters per book"));
    foreach ($this->numberOfChaptersPerBook as $book => $chapterCount) {
      $line = $database_books->getEnglishFromId ($book) . " => " . $chapterCount;
      $odfdom_text->newParagraph ();
      $odfdom_text->addText ($line);
    }
    
    // Indicate the running headers.
    $odfdom_text->newHeading1 (gettext ("Running headers"));
    foreach ($this->runningHeaders as $item) {
      $line = $database_books->getEnglishFromId ($item['book']) . " (USFM " . $item['marker'] . ") => " . $item['value'];
      $odfdom_text->newParagraph ();
      $odfdom_text->addText ($line);
    }
    
    // Indicate the Table of Contents entries.
    $odfdom_text->newHeading1 (gettext ("Long table of contents entries"));
    foreach ($this->longTOCs as $item) {
      $line = $database_books->getEnglishFromId ($item['book']) . " (USFM " . $item['marker'] . ") => " . $item['value'];
      $odfdom_text->newParagraph ();
      $odfdom_text->addText ($line);
    }
    $odfdom_text->newHeading1 (gettext ("Short table of contents entries"));
    foreach ($this->shortTOCs as $item) {
      $line = $database_books->getEnglishFromId ($item['book']) . " (USFM " . $item['marker'] . ") => " . $item['value'];
      $odfdom_text->newParagraph ();
      $odfdom_text->addText ($line);
    }

    // Indicate book abbreviations.
    $odfdom_text->newHeading1 (gettext ("Book abbreviations"));
    foreach ($this->bookAbbreviations as $item) {
      $line = $database_books->getEnglishFromId ($item['book']) . " (USFM " . $item['marker'] . ") => " . $item['value'];
      $odfdom_text->newParagraph ();
      $odfdom_text->addText ($line);
    }
        
    // Indicate the chapter specials.
    $odfdom_text->newHeading1 (gettext ("Publishing chapter labels"));
    foreach ($this->chapterLabels as $item) {
      $line = $database_books->getEnglishFromId ($item['book']) . " (USFM " . $item['marker'] . ") => " . $item['value'];
      $odfdom_text->newParagraph ();
      $odfdom_text->addText ($line);
    }
    $odfdom_text->newHeading1 (gettext ("Publishing alternate chapter numbers"));
    foreach ($this->publishedChapterMarkers as $item) {
      $line = $database_books->getEnglishFromId ($item['book']) . " (USFM " . $item['marker'] . ") => " . $item['value'];
      $odfdom_text->newParagraph ();
      $odfdom_text->addText ($line);
    }

    // Indicate the Other info.
    $odfdom_text->newHeading1 (gettext ("Other information"));
    foreach ($this->info as $line) {
      $odfdom_text->newParagraph ();
      $odfdom_text->addText ($line);
    }
    
    $odfdom_text->finalize ($path);
    return $odfdom_text->javaCode;
  }
  
  
  /**
  * This function produces the text of the current passage, e.g.: Genesis 1:1.
  * Returns: The passage text
  */
  private function getCurrentPassageText()
  {
    return Filter_Books::passageDisplay ($this->currentBookIdentifier, $this->currentChapterNumber, $this->currentVerseNumber);
  }
  

  /**
  * This function adds a string to the Info array, prefixed by the current passage.
  * $text: String to add to the Info array.
  * $next: If true, it also adds the text following the marker to the info, 
  * and removes this text from the USFM input stream.
  */
  private function addToInfo($text, $next = false)
  {
    $text = $this->getCurrentPassageText() . " " . $text;
    if ($next) {
      $text .= " " . Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
    }
    $this->info[] = $text;
  }



  /**
  * This function adds a string to the Fallout array, prefixed by the current passage.
  * $text: String to add to the Fallout array.
  * $next: If true, it also adds the text following the marker to the fallout, 
  * and removes this text from the USFM input stream.
  */
  private function addToFallout($text, $next = false)
  {
    $text = $this->getCurrentPassageText() . " " . $text;
    if ($next) {
      $text .= " " . Filter_Usfm::getTextFollowingMarker ($this->chapterUsfmMarkersAndText, $this->chapterUsfmMarkersAndTextPointer);
    }
    $this->fallout[] = $text;
  }



  /**
  * This function produces the Java code that produces the Fallout document.
  * $path: Path to the document.
  * Returns: The Java code
  */
  public function produceFalloutDocument ($path)
  {
    $odfdom_text = new Odfdom_Text;
    $odfdom_text->newHeading1 (gettext ("Fallout"));
    foreach ($this->fallout as $line) {
      $odfdom_text->newParagraph ();
      $odfdom_text->addText ($line);
    }
    $odfdom_text->finalize ($path);
    return $odfdom_text->javaCode;
  }
  
  

  /**
  * This function produces ensures that a certain paragraph style is in the OpenDocument, 
  * and then opens a paragraph with this style
  * $style: The style to use.
  * $keepWithNext: Whether to keep this paragraph with the next one.
  */
  private function newParagraph ($style, $keepWithNext)
  {
    $marker = $style["marker"];
    if (!in_array ($marker, $this->createdOdfStyles)) {
      $type = $style["type"];
      $subtype = $style["subtype"];
      $fontsize = $style["fontsize"]; // E.g.: 12 (means: 12 points).
      $italic = $style["italic"];
      $bold = $style["bold"];
      $underline = $style["underline"];
      $smallcaps = $style["smallcaps"];
      $alignment = $style["justification"];
      $spacebefore = $style["spacebefore"];
      $spaceafter = $style["spaceafter"];
      $leftmargin = $style["leftmargin"];
      $rightmargin = $style["rightmargin"];
      $firstlineindent = $style["firstlineindent"];
      // Columns are not implemented at present. Reason:
      // I failed to copy and paste sections with columns between documents in LibreOffice.
      // So why implement something that does not work.
      // If it gets implemented, then sections are used in the OpenDocument format.
      $spancolumns = $style["spancolumns"];
      $this->odfdom_text_standard->createParagraphStyle ($marker, $fontsize, $italic, $bold, $underline, $smallcaps, $alignment, $spacebefore, $spaceafter, $leftmargin, $rightmargin, $firstlineindent, $keepWithNext);
      $this->createdOdfStyles [] = $marker;
    }
    $this->odfdom_text_standard->newParagraph ($marker);
  }
  
  
  
}

?>

