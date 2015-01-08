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
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include <editor/import.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/usfm.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <styles/logic.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/HTMLparser.h>
#include <database/logs.h>


Editor_Import::Editor_Import (void * webserver_request_in)
{
  webserver_request = webserver_request_in;
}


Editor_Import::~Editor_Import ()
{

  //xmlDocDump (stdout, document);
  if (htmlDom) xmlFreeDoc (htmlDom);
  //htmlFreeParserCtxt (context);
}


void Editor_Import::load (string usfm)
{
  // Clean up.
  usfm = filter_string_trim (usfm);
  usfm.append ("\n");
  // Separate it into markers and text.
  // Load it into the object.
  markersAndText = usfm_get_markers_and_text (usfm);
  markersAndTextPointer = 0;
}


void Editor_Import::stylesheet (string stylesheet)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  styles.clear();
  vector <string> markers = request->database_styles()->getMarkers (stylesheet);
  // Load the style information into the object.
  for (auto & marker : markers) {
    Database_Styles_Item style = request->database_styles()->getMarkerData (stylesheet, marker);
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


void Editor_Import::run ()
{
  preprocess ();
  /* Todo
  process ();
   */
  postprocess ();
}


string Editor_Import::get ()
{
  // If there are notes, add the notes <div> after everything else.
  // (It has the <hr> as a child).
  if (xmlChildElementCount (notesDomNode) > 1) {
    xmlAddChild (bodyDomNode, notesDomNode);
  }
  
  // Get the entire html code, including head.
  xmlChar * contents;
  int size;
  xmlDocDumpFormatMemory (htmlDom, &contents, &size, 1);
  string html;
  if (contents) {
    html = (char *) contents;
    xmlFree (contents);
  }
  
  // Remain with the stuff within the <body> elements.
  size_t pos = html.find ("<body>");
  if (pos != string::npos) {
    html = html.substr (pos + 7);
    pos = html.find ("</body>");
    if (pos != string::npos) {
      html = html.substr (0, pos - 1);
    }
  }
  
  return html;
}


void Editor_Import::preprocess ()
{
  currentParagraphStyle = "";
  currentParagraphContent = "";
  currentTextStyles.clear();
  noteCount = 0;
  currentNoteTextStyles.clear();
  paragraphCount = 0;
  textLength = 0;
  verseStartOffsets = { make_pair (0, 0) };
  currentPDomElement = NULL;
  notePDomElement = NULL;
  
  htmlParserCtxtPtr context = htmlNewParserCtxt();
  
  // Load the template.
  string html_template =
  "<!DOCTYPE html>\n"
  "<html>\n"
  "<head>\n"
  "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n"
  "</head>\n"
  "<body>\n"
  "</body>\n"
  "</html>\n";
  htmlDom = htmlCtxtReadMemory (context, html_template.c_str(), html_template.length(), "", "UTF-8", HTML_PARSE_RECOVER);
  
  // Get the "body" node.
  xmlXPathContextPtr xpathCtx = xmlXPathNewContext (htmlDom);
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression (BAD_CAST "//body", xpathCtx);
  xmlNodeSetPtr nodes = xpathObj->nodesetval;
  bodyDomNode = nodes->nodeTab [0];
  xmlXPathFreeObject(xpathObj);
  xmlXPathFreeContext(xpathCtx);

  // Create notes node.
  notesDomNode = newElement ("div");
  xmlNewProp (notesDomNode, BAD_CAST "id", BAD_CAST "notes");
  xmlNodePtr node = newElement ("hr");
  xmlAddChild (notesDomNode, node);
}


/*
class Editor_Import // Todo port it.

 
  private function process ()
  {
    $this->markersAndTextPointer = 0;
    $markersAndTextCount = count ($this->markersAndText);
    for ($this->markersAndTextPointer = 0; $this->markersAndTextPointer < $markersAndTextCount; $this->markersAndTextPointer++) {
      $currentItem = $this->markersAndText[$this->markersAndTextPointer];
      if (usfm_is_usfm_marker ($currentItem))
      {
        // Store indicator whether the marker is an opening marker.
        $isOpeningMarker = usfm_is_opening_marker ($currentItem);
        $isEmbeddedMarker = usfm_is_embedded_marker ($currentItem);
        // Clean up the marker, so we remain with the basic version, e.g. 'id'.
        $marker = usfm_get_marker ($currentItem);
        if (array_key_exists ($marker, $this->styles))
        {
          $style = $this->styles[$marker];
          switch ($style['type'])
          {
            case StyleTypeIdentifier:
            case StyleTypeNotUsedComment:
            case StyleTypeNotUsedRunningHeader:
            {
              $this->closeTextStyle (false, false);
              $this->outputAsIs ($marker, $isOpeningMarker);
              break;
            }
            case StyleTypeStartsParagraph:
            {
              $this->closeTextStyle (false, false);
              $this->newParagraph ($marker);
              break;
            }
            case StyleTypeInlineText:
            {
              if ($isOpeningMarker) {
                $this->openTextStyle ($style, false, $isEmbeddedMarker);
              } else {
                $this->closeTextStyle (false, $isEmbeddedMarker);
              }
              break;
            }
            case StyleTypeChapterNumber:
            {
              $this->closeTextStyle (false, false);
              $this->newParagraph ($marker);
              break;
            }
            case StyleTypeVerseNumber:
            {
              // Close any existing text style.
              $this->closeTextStyle (false, false);
              // Output the space before the verse number in case the paragraph already has some text.
              if ($this->currentParagraphContent != "") {
                $this->addText (" ");
              }
              // Open verse style, record verse/length, add verse number, close style again, and add a space.
              $this->openTextStyle ($style, false, false);
              $textFollowingMarker = usfm_get_text_following_marker ($this->markersAndText, $this->markersAndTextPointer);
              $number = usfm_peek_verse_number ($textFollowingMarker);
              $this->verseStartOffsets [intval ($number)] = $this->textLength;
              $this->addText ($number);
              $this->closeTextStyle (false, false);
              $this->addText (" ");
              // If there was any text following the \v marker, remove the verse number,
              // put the remainder back into the object, and update the pointer.
              if ($textFollowingMarker != "") {
                $pos = strpos ($textFollowingMarker, $number);
                if ($pos !== false) {
                  $textFollowingMarker = substr ($textFollowingMarker, $pos + strlen ($number));
                }
                $textFollowingMarker = ltrim ($textFollowingMarker);
                $this->markersAndText [$this->markersAndTextPointer] = $textFollowingMarker;
                $this->markersAndTextPointer--;
              }
              break;
            }
            case StyleTypeFootEndNote:
            {
              switch ($style['subtype'])
              {
                case FootEndNoteSubtypeFootnote:
                case FootEndNoteSubtypeEndnote:
                {
                  $this->closeTextStyle (false, false);
                  if ($isOpeningMarker) {
                    $this->noteOpened = true;
                    $caller = $this->noteCount % 9 + 1;
                    $this->addNote ($caller, $marker, false);
                  } else {
                    $this->closeCurrentNote ();
                    $this->noteOpened = false;
                  }
                  break;
                }
                case FootEndNoteSubtypeStandardContent:
                case FootEndNoteSubtypeContent:
                case FootEndNoteSubtypeContentWithEndmarker:
                {
                  if ($isOpeningMarker) {
                    $this->openTextStyle ($style, true, $isEmbeddedMarker);
                  } else {
                    $this->closeTextStyle (true, $isEmbeddedMarker);
                  }
                  break;
                }
                case FootEndNoteSubtypeParagraph:
                default:
                {
                  $this->closeTextStyle (false, false);
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
                  $this->closeTextStyle (false, false);
                  if ($isOpeningMarker) {
                    $this->noteOpened = true;
                    $caller = ($this->noteCount) % 9 + 1;
                    $this->addNote ($caller, $marker, false);
                  } else {
                    $this->closeCurrentNote ();
                    $this->noteOpened = false;
                  }
                  break;
                }
                case CrossreferenceSubtypeContent:
                case CrossreferenceSubtypeContentWithEndmarker:
                {
                  if ($isOpeningMarker) {
                    $this->openTextStyle ($style, true, $isEmbeddedMarker);
                  } else {
                    $this->closeTextStyle (true, $isEmbeddedMarker);
                  }
                  break;
                }
                case CrossreferenceSubtypeStandardContent:
                default:
                {
                  // The style of the standard content is already used in the note's body.
                  // Clear the text style to get the correct style for the note paragraph.
                  $this->closeTextStyle (false, false);
                  break;
                }
              }
              break;
            }
            case StyleTypePeripheral:
            {
              $this->closeTextStyle (false, false);
              $this->outputAsIs ($marker, $isOpeningMarker);
              break;
            }
            case StyleTypePicture:
            {
              $this->closeTextStyle (false, false);
              $this->outputAsIs ($marker, $isOpeningMarker);
              break;
            }
            case StyleTypePageBreak:
            {
              $this->closeTextStyle (false, false);
              $this->outputAsIs ($marker, $isOpeningMarker);
              break;
            }
            case StyleTypeTableElement:
            {
              $this->closeTextStyle (false, false);
              switch ($style['subtype'])
              {
                case TableElementSubtypeRow:
                {
                  $this->outputAsIs ($marker, $isOpeningMarker);
                  break;
                }
                case TableElementSubtypeHeading:
                case TableElementSubtypeCell:
                {
                  $this->openTextStyle ($style, false, false);
                  break;
                }
                default:
                {
                  $this->openTextStyle ($style, false, false);
                  break;
                }
              }
              break;
            }
            case StyleTypeWordlistElement:
            {
              if ($isOpeningMarker) {
                $this->openTextStyle ($style, false, false);
              } else {
                $this->closeTextStyle (false, false);
              }
              break;
            }
            default:
            {
              // This marker is known in the stylesheet, but not yet implemented here.
              $this->closeTextStyle (false, false);
              $this->outputAsIs ($marker, $isOpeningMarker);
              break;
            }
          }
        } else {
          // This is a marker unknown in the stylesheet.
          $this->closeTextStyle (false, false);
          $this->outputAsIs ($marker, $isOpeningMarker);
        }
      } else {
        // Here is no marker. Treat it as text.
        if ($this->noteOpened) {
          $this->addNoteText ($currentItem);
        } else {
          $this->addText ($currentItem);
        }
      }
    }
  }
 */

  
void Editor_Import::postprocess ()
{
}


void Editor_Import::outputAsIs (string marker, bool isOpeningMarker)
{
  // Output the marker in monospace font.
  if (isOpeningMarker) {
    // Add opening marker as it is.
    newParagraph ("mono");
    addText (usfm_get_opening_usfm (marker));
  } else {
    // Add closing marker to existing paragraph.
    addText (usfm_get_closing_usfm (marker));
  }
}


xmlNodePtr Editor_Import::newElement (string element)
{
  xmlNodePtr node = xmlNewNode (NULL, BAD_CAST element.c_str());
  return node;
}


void Editor_Import::newParagraph (string style)
{
  currentPDomElement = newElement ("p");
  if (!style.empty()) {
    xmlNewProp (currentPDomElement, BAD_CAST "class", BAD_CAST style.c_str());
  }
  xmlAddChild (bodyDomNode, currentPDomElement);
  currentParagraphStyle = style;
  currentParagraphContent.clear();
  paragraphCount++;
  if (paragraphCount > 1) {
    textLength++;
  }
}


// This opens a text style.
// $style: the array containing the style variables.
// $note: boolean: Whether this refers to a note.
// $embed: boolean: Whether to open embedded / nested style.
void Editor_Import::openTextStyle (Database_Styles_Item & style, bool note, bool embed)
{
  string marker = style.marker;
  if (note) {
    if (!embed) currentNoteTextStyles.clear();
    currentNoteTextStyles.push_back (marker);
  } else {
    if (!embed) currentTextStyles.clear();
    currentTextStyles.push_back (marker);
  }
}


// This closes any open text style.
// $note: Whether this refers to a note.
// $embed: boolean: Whether to close embedded character style.
void Editor_Import::closeTextStyle (bool note, bool embed)
{
  if (note) {
    if (!currentNoteTextStyles.empty ()) currentNoteTextStyles.pop_back ();
    if (embed) currentNoteTextStyles.clear ();
  } else {
    if (!currentTextStyles.empty()) currentTextStyles.pop_back();
    if (!embed) currentTextStyles.clear ();
  }
}


// This function adds text to the current paragraph.
// $text: The text to add.
void Editor_Import::addText (string text)
{
  if (text != "") {
    if (!currentPDomElement) {
      newParagraph ();
    }
    xmlNodePtr spanDomElement = newElement ("span");
    xmlNodePtr textnode = xmlNewText (BAD_CAST filter_string_sanitize_html (text).c_str());
    xmlAddChild (spanDomElement, textnode);
    xmlAddChild (currentPDomElement, spanDomElement);
    if (!currentTextStyles.empty ()) {
      // Take character style(s) as specified in this object.
      xmlNewProp (spanDomElement, BAD_CAST "class", BAD_CAST filter_string_implode (currentTextStyles, " ").c_str());
    }
    currentParagraphContent.append (text);
  }
  textLength += unicode_string_length (text);
}


// This function adds a note to the current paragraph.
// $citation: The text of the note citation.
// $style: Style name for the paragraph in the note body.
// $endnote: Whether this is a footnote and cross reference (false), or an endnote (true).
void Editor_Import::addNote (string citation, string style, bool endnote)
{
  // Ensure that a paragraph is open, so that the note can be added to it.
  if (!currentPDomElement) {
    newParagraph ();
  }
  
  noteCount++;
  
  // Add the link with all relevant data for the note citation.
  string reference = "#note" + convert_to_string (noteCount);
  string identifier = "citation" + convert_to_string (noteCount);
  addLink (currentPDomElement, reference, identifier, "superscript", citation);
  
  // Open a paragraph element for the note body.
  notePDomElement = newElement ("p");
  xmlAddChild (notesDomNode, notePDomElement);
  xmlNewProp (notePDomElement, BAD_CAST "class", BAD_CAST style.c_str());
  
  closeTextStyle (true, false);
  
  // Add the link with all relevant data for the note body.
  reference = "#citation" + convert_to_string (noteCount);
  identifier = "note" + convert_to_string (noteCount);
  addLink (notePDomElement, reference, identifier, "", citation);
  
  // Add a space.
  addNoteText (" ");
  
  // Update the text length of the text body, excluding the note.
  textLength += unicode_string_length (citation);
}


// This function adds text to the current footnote.
// $text: The text to add.
void Editor_Import::addNoteText (string text)
{
  if (text != "") {
    if (!notePDomElement) {
      addNote ("?", "");
    }
    xmlNodePtr spanDomElement = newElement ("span");
    xmlNodePtr textnode = xmlNewText (BAD_CAST filter_string_sanitize_html (text).c_str());
    xmlAddChild (spanDomElement, textnode);
    xmlAddChild (notePDomElement, spanDomElement);
    if (!currentNoteTextStyles.empty()) {
      // Take character style as specified in this object.
      xmlNewProp (spanDomElement, BAD_CAST "class", BAD_CAST filter_string_implode (currentNoteTextStyles, " ").c_str());
    }
  }
}


// This function closes the current footnote.
void Editor_Import::closeCurrentNote ()
{
  closeTextStyle (true, false);
  notePDomElement = NULL;
}


// This adds a link.
// $domNode: The DOM node where to add the link to.
// $reference: The link's href, e.g. where it links to.
// $identifier: The link's identifier. Others can link to it.
// $style: The link text's style.
// $text: The link's text.
void Editor_Import::addLink (xmlNodePtr domNode, string reference, string identifier, string style, string text)
{
  xmlNodePtr aDomElement = newElement ("a");
  xmlAddChild (domNode, aDomElement);
  xmlNewProp (aDomElement, BAD_CAST "href", BAD_CAST reference.c_str());
  xmlNewProp (aDomElement, BAD_CAST "id", BAD_CAST identifier.c_str());
  // The link itself, for the notes, is not editable, so it can be clicked.
  // It was disabled again due to Chrome removing content on backspace.
  // $aDomElement->setAttribute ("contenteditable", "false");
  if (style != "") xmlNewProp (aDomElement, BAD_CAST "class", BAD_CAST style.c_str());
  xmlNodePtr textnode = xmlNewText(BAD_CAST filter_string_sanitize_html (text).c_str());
  xmlAddChild (aDomElement, textnode);
}
