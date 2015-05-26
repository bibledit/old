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


#ifndef INCLUDED_HTML_TEXT_H
#define INCLUDED_HTML_TEXT_H


#include <config/libraries.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <database/styles.h>


class Html_Text
{
public:
  Html_Text (string title);
  ~Html_Text ();
  void newParagraph (string style = "");
  void addText (string text, xmlNodePtr domNode = NULL);
  string getHtml ();
  string getInnerHtml ();
  void newHeading1 (string text, bool hide = false);
  void newPageBreak ();
  void openTextStyle (Database_Styles_Item style, bool note, bool embed);
  void closeTextStyle (bool note, bool embed);
  void addNote (string citation, string style, bool endnote = false);
  void addNoteText (string text);
  void closeCurrentNote ();
  void addLink (xmlNodePtr domNode, string reference, string identifier, string title, string style, string text);
  xmlNodePtr newTable ();
  xmlNodePtr newTableRow (xmlNodePtr tableElement);
  xmlNodePtr newTableData (xmlNodePtr tableRowElement, bool alignRight = false);
  void save (string name);
  xmlNodePtr currentPDomElement; // The current text:p DOMElement.
  string currentParagraphStyle;
  string currentParagraphContent;
  vector <string> currentTextStyle;
  string customClass = ""; // This class to be added to each paragraph. The class to be defined in the stylesheet.css.
private:
  xmlDocPtr htmlDom;
  xmlNodePtr headDomNode;
  xmlNodePtr bodyDomNode;
  xmlNodePtr notesDivDomNode;
  int noteCount;
  xmlNodePtr notePDomElement; // The text:p DOMElement of the current footnote, if any.
  vector <string> currentNoteTextStyle;
  void newNamedHeading (string style, string text, bool hide = false);
};


#endif

