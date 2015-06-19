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


#include <editor/usfm2html.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/usfm.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <styles/logic.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <database/logs.h>


Editor_Usfm2Html::Editor_Usfm2Html (void * webserver_request_in)
{
  webserver_request = webserver_request_in;
}


Editor_Usfm2Html::~Editor_Usfm2Html ()
{
  //xmlDocDump (stdout, htmlDom);
  if (htmlDom) xmlFreeDoc (htmlDom);
}


void Editor_Usfm2Html::load (string usfm)
{
  // Clean up.
  usfm = filter_string_trim (usfm);
  usfm.append ("\n");
  // Separate it into markers and text.
  // Load it into the object.
  markersAndText = usfm_get_markers_and_text (usfm);
  markersAndTextPointer = 0;
}


void Editor_Usfm2Html::stylesheet (string stylesheet)
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


void Editor_Usfm2Html::run ()
{
  preprocess ();
  process ();
  postprocess ();
}


string Editor_Usfm2Html::get ()
{
  // If there are notes, add the notes <div> after everything else.
  // (It has the <hr> as a child).
  if (xmlChildElementCount (notesDomNode) > 1) {
    xmlAddChild (bodyDomNode, notesDomNode);
  }
  
  // Get the entire html code, including head.
  xmlChar * contents;
  int size;
  //xmlDocDumpFormatMemory (htmlDom, &contents, &size, 1);
  xmlDocDumpFormatMemory (htmlDom, &contents, &size, 0);
  string html;
  if (contents) {
    html = (char *) contents;
    xmlFree (contents);
  }
  
  // Remain with the stuff within the <body> elements.
  size_t pos = html.find ("<body>");
  if (pos != string::npos) {
    // html.erase (0, pos + 7);
    html.erase (0, pos + 6);
    pos = html.find ("</body>");
    if (pos != string::npos) {
      html.erase  (pos);
    }
  }

  return html;
}


void Editor_Usfm2Html::preprocess ()
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

  /*
  // Load and parse the template.
  htmlParserCtxtPtr context = htmlNewParserCtxt();
  
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
  
  // Get the "body" node through XPath.
  xmlXPathContextPtr xpathCtx = xmlXPathNewContext (htmlDom);
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression (BAD_CAST "//body", xpathCtx);
  xmlNodeSetPtr nodes = xpathObj->nodesetval;
  bodyDomNode = nodes->nodeTab [0];
  xmlXPathFreeObject(xpathObj);
  xmlXPathFreeContext(xpathCtx);
  
  // Done parsing.
  htmlFreeParserCtxt (context);
  */

  // XPath crashes on Android with libxml2 2.9.2 compiled through the Android NDK.
  // It crashes here: bodyDomNode = nodes->nodeTab [0];
  // Therefore use another method: Build the document from scratch.
  htmlDom = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr rootnode = xmlNewNode (NULL, BAD_CAST "html");
  xmlDocSetRootElement (htmlDom, rootnode);
  xmlNodePtr headnode = xmlNewNode (NULL, BAD_CAST "head");
  xmlAddChild (rootnode, headnode);
  xmlNodePtr metanode = xmlNewNode  (NULL, BAD_CAST "meta");
  xmlAddChild (headnode, metanode);
  xmlNewProp (metanode, BAD_CAST "http-equiv", BAD_CAST "content-type");
  xmlNewProp (metanode, BAD_CAST "http-equiv", BAD_CAST "text/html; charset=UTF-8");
  bodyDomNode = xmlNewNode (NULL, BAD_CAST "body");
  xmlAddChild (rootnode, bodyDomNode);
  
  // Create notes node.
  // It will not yet be inserted into the html document.
  notesDomNode = newElement ("div");
  xmlNewProp (notesDomNode, BAD_CAST "id", BAD_CAST "notes");
  xmlNodePtr node = newElement ("hr");
  xmlAddChild (notesDomNode, node);
}

 
void Editor_Usfm2Html::process ()
{
  markersAndTextPointer = 0;
  unsigned int markersAndTextCount = markersAndText.size();
  for (markersAndTextPointer = 0; markersAndTextPointer < markersAndTextCount; markersAndTextPointer++) {
    string currentItem = markersAndText[markersAndTextPointer];
    if (usfm_is_usfm_marker (currentItem))
    {
      // Store indicator whether the marker is an opening marker.
      bool isOpeningMarker = usfm_is_opening_marker (currentItem);
      bool isEmbeddedMarker = usfm_is_embedded_marker (currentItem);
      // Clean up the marker, so we remain with the basic version, e.g. 'id'.
      string marker = usfm_get_marker (currentItem);
      if (styles.count (marker))
      {
        Database_Styles_Item style = styles [marker];
        switch (style.type)
        {
          case StyleTypeIdentifier:
          case StyleTypeNotUsedComment:
          case StyleTypeNotUsedRunningHeader:
          {
            closeTextStyle (false, false);
            outputAsIs (marker, isOpeningMarker);
            break;
          }
          case StyleTypeStartsParagraph:
          {
            closeTextStyle (false, false);
            newParagraph (marker);
            break;
          }
          case StyleTypeInlineText:
          {
            if (isOpeningMarker) {
              openTextStyle (style, false, isEmbeddedMarker);
            } else {
              closeTextStyle (false, isEmbeddedMarker);
            }
            break;
          }
          case StyleTypeChapterNumber:
          {
            closeTextStyle (false, false);
            newParagraph (marker);
            break;
          }
          case StyleTypeVerseNumber:
          {
            // Close any existing text style.
            closeTextStyle (false, false);
            // Output the space before the verse number in case the paragraph already has some text.
            if (currentParagraphContent != "") {
              addText (" ");
            }
            // Open verse style, record verse/length, add verse number, close style again, and add a space.
            openTextStyle (style, false, false);
            string textFollowingMarker = usfm_get_text_following_marker (markersAndText, markersAndTextPointer);
            string number = usfm_peek_verse_number (textFollowingMarker);
            verseStartOffsets [convert_to_int (number)] = textLength;
            addText (number);
            closeTextStyle (false, false);
            addText (" ");
            // If there was any text following the \v marker, remove the verse number,
            // put the remainder back into the object, and update the pointer.
            if (textFollowingMarker != "") {
              size_t pos = textFollowingMarker.find (number);
              if (pos != string::npos) {
                textFollowingMarker = textFollowingMarker.substr (pos + number.length());
              }
              textFollowingMarker = filter_string_ltrim (textFollowingMarker);
              markersAndText [markersAndTextPointer] = textFollowingMarker;
              markersAndTextPointer--;
            }
            break;
          }
          case StyleTypeFootEndNote:
          {
            switch (style.subtype)
            {
              case FootEndNoteSubtypeFootnote:
              case FootEndNoteSubtypeEndnote:
              {
                closeTextStyle (false, false);
                if (isOpeningMarker) {
                  noteOpened = true;
                  int caller = noteCount % 9 + 1;
                  addNote (convert_to_string (caller), marker, false);
                } else {
                  closeCurrentNote ();
                  noteOpened = false;
                }
                break;
              }
              case FootEndNoteSubtypeStandardContent:
              case FootEndNoteSubtypeContent:
              case FootEndNoteSubtypeContentWithEndmarker:
              {
                if (isOpeningMarker) {
                  openTextStyle (style, true, isEmbeddedMarker);
                } else {
                  closeTextStyle (true, isEmbeddedMarker);
                }
                break;
              }
              case FootEndNoteSubtypeParagraph:
              default:
              {
                closeTextStyle (false, false);
                break;
              }
            }
            break;
          }
          case StyleTypeCrossreference:
          {
            switch (style.subtype)
            {
              case CrossreferenceSubtypeCrossreference:
              {
                closeTextStyle (false, false);
                if (isOpeningMarker) {
                  noteOpened = true;
                  int caller = (noteCount) % 9 + 1;
                  addNote (convert_to_string (caller), marker, false);
                } else {
                  closeCurrentNote ();
                  noteOpened = false;
                }
                break;
              }
              case CrossreferenceSubtypeContent:
              case CrossreferenceSubtypeContentWithEndmarker:
              {
                if (isOpeningMarker) {
                  openTextStyle (style, true, isEmbeddedMarker);
                } else {
                  closeTextStyle (true, isEmbeddedMarker);
                }
                break;
              }
              case CrossreferenceSubtypeStandardContent:
              default:
              {
                // The style of the standard content is already used in the note's body.
                // Clear the text style to get the correct style for the note paragraph.
                closeTextStyle (false, false);
                break;
              }
            }
            break;
          }
          case StyleTypePeripheral:
          {
            closeTextStyle (false, false);
            outputAsIs (marker, isOpeningMarker);
            break;
          }
          case StyleTypePicture:
          {
            closeTextStyle (false, false);
            outputAsIs (marker, isOpeningMarker);
            break;
          }
          case StyleTypePageBreak:
          {
            closeTextStyle (false, false);
            outputAsIs (marker, isOpeningMarker);
            break;
          }
          case StyleTypeTableElement:
          {
            closeTextStyle (false, false);
            switch (style.subtype)
            {
              case TableElementSubtypeRow:
              {
                outputAsIs (marker, isOpeningMarker);
                break;
              }
              case TableElementSubtypeHeading:
              case TableElementSubtypeCell:
              {
                openTextStyle (style, false, false);
                break;
              }
              default:
              {
                openTextStyle (style, false, false);
                break;
              }
            }
            break;
          }
          case StyleTypeWordlistElement:
          {
            if (isOpeningMarker) {
              openTextStyle (style, false, false);
            } else {
              closeTextStyle (false, false);
            }
            break;
          }
          default:
          {
            // This marker is known in the stylesheet, but not yet implemented here.
            closeTextStyle (false, false);
            outputAsIs (marker, isOpeningMarker);
            break;
          }
        }
      } else {
        // This is a marker unknown in the stylesheet.
        closeTextStyle (false, false);
        outputAsIs (marker, isOpeningMarker);
      }
    } else {
      // Here is no marker. Treat it as text.
      if (noteOpened) {
        addNoteText (currentItem);
      } else {
        addText (currentItem);
      }
    }
  }
}


void Editor_Usfm2Html::postprocess ()
{
}


void Editor_Usfm2Html::outputAsIs (string marker, bool isOpeningMarker)
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


xmlNodePtr Editor_Usfm2Html::newElement (string element)
{
  xmlNodePtr node = xmlNewNode (NULL, BAD_CAST element.c_str());
  return node;
}


void Editor_Usfm2Html::newParagraph (string style)
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
void Editor_Usfm2Html::openTextStyle (Database_Styles_Item & style, bool note, bool embed)
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
void Editor_Usfm2Html::closeTextStyle (bool note, bool embed)
{
  if (note) {
    if (!currentNoteTextStyles.empty ()) currentNoteTextStyles.pop_back ();
    if (!embed) currentNoteTextStyles.clear ();
  } else {
    if (!currentTextStyles.empty()) currentTextStyles.pop_back();
    if (!embed) currentTextStyles.clear ();
  }
}


// This function adds text to the current paragraph.
// $text: The text to add.
void Editor_Usfm2Html::addText (string text)
{
  if (text != "") {
    if (!currentPDomElement) {
      newParagraph ();
    }
    xmlNodePtr spanDomElement = newElement ("span");
    xmlNodePtr textnode = xmlNewText (BAD_CAST text.c_str());
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
void Editor_Usfm2Html::addNote (string citation, string style, bool endnote)
{
  // Ensure that a paragraph is open, so that the note can be added to it.
  if (!currentPDomElement) {
    newParagraph ();
  }
  
  // Not used:
  if (endnote) {};
  
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
void Editor_Usfm2Html::addNoteText (string text)
{
  if (text != "") {
    if (!notePDomElement) {
      addNote ("?", "");
    }
    xmlNodePtr spanDomElement = newElement ("span");
    xmlNodePtr textnode = xmlNewText (BAD_CAST text.c_str());
    xmlAddChild (spanDomElement, textnode);
    xmlAddChild (notePDomElement, spanDomElement);
    if (!currentNoteTextStyles.empty()) {
      // Take character style as specified in this object.
      xmlNewProp (spanDomElement, BAD_CAST "class", BAD_CAST filter_string_implode (currentNoteTextStyles, " ").c_str());
    }
  }
}


// This function closes the current footnote.
void Editor_Usfm2Html::closeCurrentNote ()
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
void Editor_Usfm2Html::addLink (xmlNodePtr domNode, string reference, string identifier, string style, string text)
{
  xmlNodePtr aDomElement = newElement ("a");
  xmlAddChild (domNode, aDomElement);
  xmlNewProp (aDomElement, BAD_CAST "href", BAD_CAST reference.c_str());
  xmlNewProp (aDomElement, BAD_CAST "id", BAD_CAST identifier.c_str());
  // The link itself, for the notes, is not editable, so it can be clicked.
  // It was disabled again due to Chrome removing content on backspace.
  // $aDomElement->setAttribute ("contenteditable", "false");
  if (style != "") xmlNewProp (aDomElement, BAD_CAST "class", BAD_CAST style.c_str());
  xmlNodePtr textnode = xmlNewText (BAD_CAST text.c_str());
  xmlAddChild (aDomElement, textnode);
}
