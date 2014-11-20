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


#include <filter/text.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/usfm.h>
#include <styles/logic.h>


Filter_Text_Passage_Marker_Value::Filter_Text_Passage_Marker_Value (int book_in, int chapter_in, int verse_in, string marker_in, string value_in)
{
  book = book_in;
  chapter = chapter_in;
  verse = verse_in;
  marker = marker_in;
  value = value_in;
}


// This class filters USFM text, converting it into other formats.


Filter_Text::Filter_Text (string bible_in)
{
  bible = bible_in;
  currentBookIdentifier = 0;
  currentChapterNumber = 0;
  heading_started = false;
  text_started = false;
  odf_text_standard = NULL;
  odf_text_text_only = NULL;
  odf_text_text_and_note_citations = NULL;
  odf_text_notes = NULL;
  html_text_standard = NULL;
  html_text_linked = NULL;
  onlinebible_text = NULL;
  esword_text = NULL;
  text_text = NULL;  
}


Filter_Text::~Filter_Text ()
{
  if (odf_text_standard) delete odf_text_standard;
  if (odf_text_text_only) delete odf_text_text_only;
  if (odf_text_text_and_note_citations) delete odf_text_text_and_note_citations;
  if (odf_text_notes) delete odf_text_notes;
  if (html_text_standard) delete html_text_standard;
  if (html_text_linked) delete html_text_linked;
  if (onlinebible_text) delete onlinebible_text;
  if (esword_text) delete esword_text;
  if (text_text) delete esword_text;
}


/**
* This function adds USFM code to the class.
* $code: USFM code.
*/
void Filter_Text::addUsfmCode (string usfm)
{
  // Clean the USFM.
  usfm = filter_string_trim (usfm);
  usfm += "\n";
  // Sort the USFM code out and separate it into markers and text.
  vector <string> markersAndText = usfm_get_markers_and_text (usfm);
  // Add the USFM to the object.
  usfmMarkersAndText.insert (usfmMarkersAndText.end(), markersAndText.begin(), markersAndText.end());
}


/**
* This function runs the filter.
* $stylesheet - The stylesheet to use.
*/
void Filter_Text::run (string stylesheet)
{
  // Get the styles.
  getStyles (stylesheet);

  // Preprocess.
  // Todo preprocessingStage ();

  // Process data.
  // Todo processUsfm ();

  // Clear USFM and styles.
  usfmMarkersAndText.clear();
  usfmMarkersAndTextPointer = 0;
  chapterUsfmMarkersAndText.clear();
  chapterUsfmMarkersAndTextPointer = 0;
  styles.clear();
  chapterMarker.clear();
  createdStyles.clear();
}


/**
* This function return true when there is still unprocessed USFM code available.
*/
bool Filter_Text::unprocessedUsfmCodeAvailable ()
{
  return (usfmMarkersAndTextPointer < usfmMarkersAndText.size());
}


/**
* This function stores data in the class:
* the next chapter from the unprocessed USFM code.
*/
void Filter_Text::getUsfmNextChapter ()
{
  // Initialization.
  chapterUsfmMarkersAndText.clear();
  chapterUsfmMarkersAndTextPointer = 0;
  bool firstLine = true;

  // Obtain the standard marker for the chapter number.
  // Deal with the unlikely case that the chapter marker is non-standard.
  if (chapterMarker.empty()) {
    chapterMarker = "c";
    for (const auto & style : styles) {
      if (style.second.type == StyleTypeChapterNumber) {
        chapterMarker = style.second.marker;
        break;
      }
    }
  }

  // Load the USFM code till the next chapter marker.
  while (unprocessedUsfmCodeAvailable ()) {
    string item = usfmMarkersAndText [usfmMarkersAndTextPointer];
    if (!firstLine) {
      if (filter_string_trim (item) == ("\\" + chapterMarker)) {
        return;
      }
    }
    chapterUsfmMarkersAndText.push_back (item);
    firstLine = false;
    usfmMarkersAndTextPointer++;
  }
}


/**
* This function gets the styles from the database,
* and stores them in the object for quicker access.
*/
void Filter_Text::getStyles (string stylesheet)
{
  styles.clear();
  // Get the relevant styles information included.
  if (odf_text_standard) odf_text_standard->createPageBreakStyle ();
  if (odf_text_text_only) odf_text_text_only->createPageBreakStyle ();
  if (odf_text_text_and_note_citations) odf_text_text_and_note_citations->createPageBreakStyle ();
  if (odf_text_text_and_note_citations) odf_text_text_and_note_citations->createSuperscriptStyle ();
  Database_Styles database_styles = Database_Styles ();
  vector <string> markers = database_styles.getMarkers (stylesheet);
  for (auto marker : markers) {
    Database_Styles_Item style = database_styles.getMarkerData (stylesheet, marker);
    styles [marker] = style;
    if (style.type == StyleTypeFootEndNote) {
      if (style.subtype == FootEndNoteSubtypeStandardContent) {
        standardContentMarkerFootEndNote = style.marker;
      }
    }
    if (style.type == StyleTypeCrossreference) {
      if (style.subtype == CrossreferenceSubtypeStandardContent) {
        standardContentMarkerCrossReference = style.marker;
      }
    }
  }
}


/**
* This function does the preprocessing of the USFM code
* extracting a variety of information, creating note citations, etc.
*/
void Filter_Text::preprocessingStage ()
{
  usfmMarkersAndTextPointer = 0;
  while (unprocessedUsfmCodeAvailable ()) {
    getUsfmNextChapter ();
    for (chapterUsfmMarkersAndTextPointer = 0; chapterUsfmMarkersAndTextPointer < chapterUsfmMarkersAndText.size(); chapterUsfmMarkersAndTextPointer++) {
      string currentItem = chapterUsfmMarkersAndText[chapterUsfmMarkersAndTextPointer];
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
                    $s = str_replace (get_soft_hyphen (), "", $s); // Remove possible soft hyphen.
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
              case StyleTypeFootEndNote:
              {
                switch ($style['subtype'])
                {
                  case FootEndNoteSubtypeFootnote:
                  case FootEndNoteSubtypeEndnote:
                  {
                    $this->createNoteCitation ($style);
                    break;
                  }
                  case FootEndNoteSubtypeStandardContent:
                  case FootEndNoteSubtypeContent:
                  case FootEndNoteSubtypeContentWithEndmarker:
                  case FootEndNoteSubtypeParagraph:
                  {
                    break;
                  }
                }
                break;
              }
              case StyleTypeCrossreference:
              {
                switch ($style['subtype'])
                {
                  case CrossreferenceSubtypeCrossreference:
                  {
                    $this->createNoteCitation ($style);
                    break;
                  }
                  case CrossreferenceSubtypeStandardContent:
                  case CrossreferenceSubtypeContent:
                  case CrossreferenceSubtypeContentWithEndmarker:
                  {
                    break;
                  }
                }
                break;
              }
            }
          }
        }
      }
    }
  }
}


