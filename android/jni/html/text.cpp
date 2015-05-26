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


#include <html/text.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/url.h>
#include <database/books.h>


// Class for creating Html text documents.


Html_Text::Html_Text (string title)
{
  currentParagraphStyle.clear();
  currentParagraphContent.clear();
  noteCount = 0;
  currentPDomElement = NULL;

  xmlNodePtr node = NULL;
  xmlNodePtr textnode = NULL;

  // <?xml version="1.0" encoding="UTF-8"?>
  htmlDom = xmlNewDoc (BAD_CAST "1.0");
  
  // <html>
  xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "html");
  xmlDocSetRootElement (htmlDom, root_node);

  // <!DOCTYPE html>  
  xmlCreateIntSubset (htmlDom, BAD_CAST "html", NULL, NULL);

  // <head>    
  headDomNode = xmlNewNode (NULL, BAD_CAST "head");
  xmlAddChild (root_node, headDomNode);

  node = xmlNewNode (NULL, BAD_CAST "title");
  xmlAddChild (headDomNode, node);
  textnode = xmlNewText(BAD_CAST title.c_str());
  xmlAddChild (node, textnode);

  // <meta http-equiv="content-type" content="text/html; charset=UTF-8" />
  node = xmlNewNode (NULL, BAD_CAST "meta");
  xmlAddChild (headDomNode, node);
  xmlNewProp(node, BAD_CAST "http-equiv", BAD_CAST "content-type");
  xmlNewProp(node, BAD_CAST "content", BAD_CAST "text/html; charset=UTF-8");

  // <link rel="stylesheet" type="text/css" href="stylesheet.css"></link>
  node = xmlNewNode (NULL, BAD_CAST "link");
  xmlAddChild (headDomNode, node);
  xmlNewProp (node, BAD_CAST "rel", BAD_CAST "stylesheet");
  xmlNewProp (node, BAD_CAST "type", BAD_CAST "text/css");
  xmlNewProp (node, BAD_CAST "href", BAD_CAST "stylesheet.css");

  // <body>
  bodyDomNode = xmlNewNode (NULL, BAD_CAST "body");
  xmlAddChild (root_node, bodyDomNode);
  
  // Optional for notes: <div>
  notesDivDomNode = xmlNewNode (NULL, BAD_CAST "div");
}


Html_Text::~Html_Text ()
{
  xmlFreeDoc (htmlDom);
}


void Html_Text::newParagraph (string style)
{
  currentPDomElement = xmlNewNode (NULL, BAD_CAST "p");
  if (style != "") {
    string clss = style;
    if (customClass != "") {
      clss += " " + customClass;
    }
    xmlNewProp (currentPDomElement, BAD_CAST "class", BAD_CAST clss.c_str());
  }
  xmlAddChild (bodyDomNode, currentPDomElement);
  currentParagraphStyle = style;
  currentParagraphContent.clear();
}


// This function adds text to the current paragraph.
// $text: The text to add.
void Html_Text::addText (string text, xmlNodePtr domNode)
{
  if (domNode) {};
  if (text != "") {
    if (!currentPDomElement) newParagraph ();
    xmlNodePtr spanDomElement = xmlNewNode (NULL, BAD_CAST "span");
    xmlNodePtr textnode = xmlNewText(BAD_CAST text.c_str());
    xmlAddChild (spanDomElement, textnode);
    xmlAddChild (currentPDomElement, spanDomElement);
    if (!currentTextStyle.empty ()) {
      // Take character style(s) as specified in the object.
      xmlNewProp (spanDomElement, BAD_CAST "class", BAD_CAST filter_string_implode (currentTextStyle, " ").c_str());
    }
    currentParagraphContent += text;
  }
}


// This creates a <h1> heading with contents.
// $text: Contents.
void Html_Text::newHeading1 (string text, bool hide)
{
  newNamedHeading ("h1", text, hide);
}


// This applies a page break.
void Html_Text::newPageBreak ()
{
  // The style is already in the css.
  newParagraph ("break");
  // Always make the paragraph null, because we don't want subsequent text to be added to this page break,
  // since it would be nearly invisible, and thus text would seem to get lost.
  currentPDomElement = NULL;
  currentParagraphStyle.clear();
  currentParagraphContent.clear();
}


// This opens a text style.
// $style: the array containing the style variables.
// $note: boolean: Whether this refers to notes.
// $embed: boolean: Whether to embed the new text style in an existing text style.
//                  true: add the new style to the existing style.
//                  false: close any existing text style, and open the new style.
void Html_Text::openTextStyle (Database_Styles_Item style, bool note, bool embed)
{
  string marker = style.marker;
  if (note) {
    if (!embed) currentNoteTextStyle.clear();
    currentNoteTextStyle.push_back (marker);
  } else {
    if (!embed) currentTextStyle.clear();
    currentTextStyle.push_back (marker);
  }
}


// This closes any open text style.
// $note: boolean: Whether this refers to notes.
// $embed: boolean: Whether to embed the new text style in an existing text style.
//                  true: add the new style to the existing style.
//                  false: close any existing text style, and open the new style.
void Html_Text::closeTextStyle (bool note, bool embed)
{
  if (note) {
    if (!currentNoteTextStyle.empty()) currentNoteTextStyle.pop_back ();
    if (!embed) currentNoteTextStyle.clear();
  } else {
    if (!currentTextStyle.empty()) currentTextStyle.pop_back ();
    if (!embed) currentTextStyle.clear ();
  }
}


// This function adds a note to the current paragraph.
// $citation: The text of the note citation.
// $style: Style name for the paragraph in the note body.
// $endnote: Whether this is a footnote and cross reference (false), or an endnote (true).
void Html_Text::addNote (string citation, string style, bool endnote)
{
  // Ensure that a paragraph is open, so that the note can be added to it.
  if (!currentPDomElement) newParagraph ();

  if (endnote) {};
  
  noteCount++;

  // Add the link with all relevant data for the note citation.
  string reference = "#note" + convert_to_string (noteCount);
  string identifier = "citation" + convert_to_string (noteCount);
  addLink (currentPDomElement, reference, identifier, "", "superscript", citation);

  // Open a paragraph element for the note body.
  notePDomElement = xmlNewNode (NULL, BAD_CAST "p");
  xmlAddChild (notesDivDomNode, notePDomElement);
  xmlNewProp (notePDomElement, BAD_CAST "class", BAD_CAST style.c_str());

  closeTextStyle (true, false);

  // Add the link with all relevant data for the note body.
  reference = "#citation" + convert_to_string (noteCount);
  identifier = "note" + convert_to_string (noteCount);
  addLink (notePDomElement, reference, identifier, "", "", citation);

  // Add a space.
  addNoteText (" ");
}


// This function adds text to the current footnote.
// $text: The text to add.
void Html_Text::addNoteText (string text)
{
  if (text != "") {
    if (!notePDomElement) addNote ("?", "");
    xmlNodePtr spanDomElement = xmlNewNode (NULL, BAD_CAST "span");
    xmlNodePtr textnode = xmlNewText(BAD_CAST text.c_str());
    xmlAddChild (spanDomElement, textnode);
    xmlAddChild (notePDomElement, spanDomElement);
    if (!currentNoteTextStyle.empty ()) {
      // Take character style as specified in this object.
      xmlNewProp (spanDomElement, BAD_CAST "class", BAD_CAST filter_string_implode (currentNoteTextStyle, " ").c_str());
    }
  }
}


// This function closes the current footnote.
void Html_Text::closeCurrentNote ()
{
  closeTextStyle (true, false);
  notePDomElement = NULL;
}


// This adds a link.
// $domNode: The DOM node where to add the link to.
// $reference: The link's href, e.g. where it links to.
// $identifier: The link's identifier. Others can link to it.
// $title: The link's title, to make it accessible, e.g. for screenreaders.
// $style: The link text's style.
// $text: The link's text.
void Html_Text::addLink (xmlNodePtr domNode, string reference, string identifier, string title, string style, string text)
{
  xmlNodePtr aDomElement = xmlNewNode (NULL, BAD_CAST "a");
  xmlAddChild (domNode, aDomElement);
  xmlNewProp (aDomElement, BAD_CAST "href", BAD_CAST reference.c_str());
  xmlNewProp (aDomElement, BAD_CAST "id", BAD_CAST identifier.c_str());
  if (!title.empty ()) xmlNewProp (aDomElement, BAD_CAST "title", BAD_CAST title.c_str());
  if (style != "") xmlNewProp (aDomElement, BAD_CAST "class", BAD_CAST style.c_str());
  xmlNodePtr textnode = xmlNewText(BAD_CAST text.c_str());
  xmlAddChild (aDomElement, textnode);
}


// This gets and then returns the html text
string Html_Text::getHtml ()
{
  // Add possible notes.
  if (noteCount > 0) {
    xmlAddChild (bodyDomNode, notesDivDomNode);
    noteCount = 0;
  }
  // Get the html.
  xmlChar * xml;
  xmlDocDumpFormatMemoryEnc	(htmlDom, &xml, NULL,  "UTF-8", 1);
  string html = (char *) xml;
  xmlFree (xml);
  return html;
}


// Returns the html text within the <body> tags, that is, without the <head> stuff.
string Html_Text::getInnerHtml ()
{
  string page = getHtml ();
  size_t pos = page.find ("<body>");
  if (pos != string::npos) {
    page = page.substr (pos + 7);
    pos = page.find ("</body>");
    if (pos != string::npos) {
      page = page.substr (0, pos - 1);
    }
  }
  // Remove the empty <div> element at the end.
  pos = page.find ("<div></div>");
  if (pos != string::npos) {
    page = page.substr (0, pos);
  }
  return page;
}


// This saves the web page to file
// $name: the name of the file to save to.
void Html_Text::save (string name)
{
  string html = getHtml ();
  filter_url_file_put_contents (name, html);
}


// This adds a new table to the html DOM.
// Returns: The new $tableElement
xmlNodePtr Html_Text::newTable ()
{
  // Adding subsequent text should create a new paragraph.
  currentPDomElement = NULL;
  currentParagraphStyle.clear();
  currentParagraphContent.clear();
  // Append the table.
  xmlNodePtr tableElement = xmlNewNode (NULL, BAD_CAST "table");
  xmlAddChild (bodyDomNode, tableElement);
  xmlNewProp (tableElement, BAD_CAST "width", BAD_CAST "100%");
  return tableElement;
}


// This adds a new row to an existing $tableElement.
// Returns: The new $tableRowElement.
xmlNodePtr Html_Text::newTableRow (xmlNodePtr tableElement)
{
  xmlNodePtr tableRowElement = xmlNewNode (NULL, BAD_CAST "tr");
  xmlAddChild (tableElement, tableRowElement);
  return tableRowElement;
}


// This adds a new data cell to an existing $tableRowElement.
// Returns: The new $tableDataElement.
xmlNodePtr Html_Text::newTableData (xmlNodePtr tableRowElement, bool alignRight)
{
  xmlNodePtr tableDataElement = xmlNewNode (NULL, BAD_CAST "td");
  if (alignRight) xmlNewProp (tableDataElement, BAD_CAST "align", BAD_CAST "right");
  xmlAddChild (tableRowElement, tableDataElement);
  return tableDataElement;
}


// This creates a heading with styled content.
// $style: A style name.
// $text: Content.
void Html_Text::newNamedHeading (string style, string text, bool hide)
{
  if (hide) {};
  xmlNodePtr textHDomElement = xmlNewNode (NULL, BAD_CAST style.c_str());
  xmlAddChild (bodyDomNode, textHDomElement);
  xmlNodePtr textnode = xmlNewText(BAD_CAST text.c_str());
  xmlAddChild (textHDomElement, textnode);
  // Make paragraph null, so that adding subsequent text creates a new paragraph.
  currentPDomElement = NULL;
  currentParagraphStyle.clear ();
  currentParagraphContent.clear ();
}


