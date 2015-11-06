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


// Todo this one goes out, and v2 will be used, then at the end, v2 becomes the normal version.
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

  // <link rel="stylesheet" type="text/css" href="stylesheet.css">
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
  xmlDocDumpFormatMemoryEnc	(htmlDom, &xml, NULL,  "UTF-8", 0);
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
    page = page.substr (pos + 6);
    pos = page.find ("</body>");
    if (pos != string::npos) {
      page = page.substr (0, pos);
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


Html2_Text::Html2_Text (string title)
{
  currentParagraphStyle.clear();
  currentParagraphContent.clear();
  noteCount = 0;
  
  // <?xml version="1.0" encoding="UTF-8"?>
  // Todo create this another way htmlDom = xmlNewDoc (BAD_CAST "1.0");
  
  // <html>
  xml_node root_node = htmlDom.append_child ("html");
  
  // <!DOCTYPE html>
  // Todo add this another way. xmlCreateIntSubset (htmlDom, BAD_CAST "html", NULL, NULL);
  
  // <head>
  headDomNode = root_node.append_child ("head");
  
  xml_node title_node = headDomNode.append_child ("title");
  title_node.text ().set (title.c_str());
  
  // <meta http-equiv="content-type" content="text/html; charset=UTF-8" />
  xml_node meta_node = headDomNode.append_child ("meta");
  meta_node.append_attribute ("http-equiv") = "content-type";
  meta_node.append_attribute ("content") = "text/html; charset=UTF-8";
  
  // <link rel="stylesheet" type="text/css" href="stylesheet.css">
  xml_node link_node = headDomNode.append_child ("link");
  link_node.append_attribute ("rel") = "stylesheet";
  link_node.append_attribute ("type") = "text/css";
  link_node.append_attribute ("href") = "stylesheet.css";
  
  // <body>
  bodyDomNode = root_node.append_child ("body");
  
  // Optional for notes: <div> // Todo create here, when finish, move to the end, or remove altogether.
  notesDivDomNode = bodyDomNode.append_child ("div");
}


Html2_Text::~Html2_Text ()
{
}


void Html2_Text::newParagraph (string style)
{
  currentPDomElement = bodyDomNode.append_child ("p");
  if (style != "") {
    string clss = style;
    if (customClass != "") {
      clss += " " + customClass;
    }
    currentPDomElement.append_attribute ("class") = clss.c_str();
  }
  current_p_node_open = true;
  currentParagraphStyle = style;
  currentParagraphContent.clear();
}


// This function adds text to the current paragraph.
// $text: The text to add.
void Html2_Text::addText (string text)
{
  if (text != "") {
    if (!current_p_node_open) newParagraph ();
    xml_node span = currentPDomElement.append_child ("span");
    span.text().set (text.c_str());
    if (!currentTextStyle.empty ()) {
      // Take character style(s) as specified in the object.
      span.append_attribute ("class") = filter_string_implode (currentTextStyle, " ").c_str();
    }
    currentParagraphContent += text;
  }
}


// This creates a <h1> heading with contents.
// $text: Contents.
void Html2_Text::newHeading1 (string text, bool hide)
{
  newNamedHeading ("h1", text, hide);
}


// This applies a page break.
void Html2_Text::newPageBreak ()
{
  // The style is already in the css.
  newParagraph ("break");
  // Always clear the flag for the open paragraph,
  // because we don't want subsequent text to be added to this page break,
  // since it would be nearly invisible, and thus text would seem to be lost.
  current_p_node_open = false;
  currentParagraphStyle.clear();
  currentParagraphContent.clear();
}


// This opens a text style.
// $style: the array containing the style variables.
// $note: boolean: Whether this refers to notes.
// $embed: boolean: Whether to embed the new text style in an existing text style.
//                  true: add the new style to the existing style.
//                  false: close any existing text style, and open the new style.
void Html2_Text::openTextStyle (Database_Styles_Item style, bool note, bool embed)
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
void Html2_Text::closeTextStyle (bool note, bool embed)
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
void Html2_Text::addNote (string citation, string style, bool endnote)
{
  // Ensure that a paragraph is open, so that the note can be added to it.
  if (!current_p_node_open) newParagraph ();
  
  if (endnote) {};
  
  noteCount++;
  
  // Add the link with all relevant data for the note citation.
  string reference = "#note" + convert_to_string (noteCount);
  string identifier = "citation" + convert_to_string (noteCount);
  addLink (currentPDomElement, reference, identifier, "", "superscript", citation);
  
  // Open a paragraph element for the note body.
  notePDomElement = notesDivDomNode.append_child ("p");
  notePDomElement.append_attribute ("class") = style.c_str();
  note_p_node_open = true;
  
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
void Html2_Text::addNoteText (string text)
{
  if (text != "") {
    if (!note_p_node_open) addNote ("?", "");
    xml_node spanDomElement = notePDomElement.append_child ("span");
    spanDomElement.text().set (text.c_str());
    if (!currentNoteTextStyle.empty ()) {
      // Take character style as specified in this object.
      spanDomElement.append_attribute ("class") = filter_string_implode (currentNoteTextStyle, " ").c_str();
    }
  }
}


// This function closes the current footnote.
void Html2_Text::closeCurrentNote ()
{
  closeTextStyle (true, false);
  note_p_node_open = false;
}


// This adds a link.
// $domNode: The DOM node where to add the link to.
// $reference: The link's href, e.g. where it links to.
// $identifier: The link's identifier. Others can link to it.
// $title: The link's title, to make it accessible, e.g. for screenreaders.
// $style: The link text's style.
// $text: The link's text.
void Html2_Text::addLink (xml_node domNode, string reference, string identifier, string title, string style, string text)
{
  xml_node aDomElement = domNode.append_child ("a");
  aDomElement.append_attribute ("href") = reference.c_str();
  aDomElement.append_attribute ("id") = identifier.c_str();
  if (!title.empty ()) {
    aDomElement.append_attribute ("title") = title.c_str();
  }
  if (style != "") {
    aDomElement.append_attribute ("class") = style.c_str();
  }
  aDomElement.text ().set (text.c_str());
}


// This gets and then returns the html text
string Html2_Text::getHtml ()
{
  // Move any notes into place: At the end of the body.
  // Or remove empty notes container.
  // Todo and remove the empty notes div if there are no notes, so this replaces the <div></div> removal earlier.
  if (noteCount > 0) {
    bodyDomNode.append_move (notesDivDomNode);
  } else {
    bodyDomNode.remove_child (notesDivDomNode);
  }
  noteCount = 0;

  // Get the html.
  stringstream output;
  htmlDom.print (output, "", format_raw);
  string html = output.str ();
  
  return html;
}


// Returns the html text within the <body> tags, that is, without the <head> stuff.
string Html2_Text::getInnerHtml ()
{
  string page = getHtml ();
  size_t pos = page.find ("<body>");
  if (pos != string::npos) {
    page = page.substr (pos + 6);
    pos = page.find ("</body>");
    if (pos != string::npos) {
      page = page.substr (0, pos);
    }
  }
  return page;
}


// This saves the web page to file
// $name: the name of the file to save to.
void Html2_Text::save (string name)
{
  string html = getHtml ();
  filter_url_file_put_contents (name, html);
}


// This adds a new table to the html DOM.
// Returns: The new $tableElement
xml_node Html2_Text::newTable ()
{
  // Adding subsequent text should create a new paragraph.
  current_p_node_open = false;
  currentParagraphStyle.clear();
  currentParagraphContent.clear();
  // Append the table.
  xml_node tableElement = bodyDomNode.append_child ("table");
  tableElement.append_attribute ("width") = "100%";
  return tableElement;
}


// This adds a new row to an existing $tableElement.
// Returns: The new $tableRowElement.
xml_node Html2_Text::newTableRow (xml_node tableElement)
{
  xml_node tableRowElement = tableElement.append_child ("tr");
  return tableRowElement;
}


// This adds a new data cell to an existing $tableRowElement.
// Returns: The new $tableDataElement.
xml_node Html2_Text::newTableData (xml_node tableRowElement, bool alignRight)
{
  xml_node tableDataElement = tableRowElement.append_child ("td");
  if (alignRight) tableDataElement.append_attribute ("align") = "right";
  return tableDataElement;
}


// This creates a heading with styled content.
// $style: A style name.
// $text: Content.
void Html2_Text::newNamedHeading (string style, string text, bool hide)
{
  if (hide) {};
  xml_node textHDomElement = bodyDomNode.append_child (style.c_str());
  textHDomElement.text ().set (text.c_str());
  // Make paragraph null, so that adding subsequent text creates a new paragraph.
  current_p_node_open = false;
  currentParagraphStyle.clear ();
  currentParagraphContent.clear ();
}
