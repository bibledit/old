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


#ifndef INCLUDED_EDITOR_IMPORT_H
#define INCLUDED_EDITOR_IMPORT_H


#include <config/libraries.h>
#include <libxml/tree.h>
#include <database/styles.h>


class Editor_Usfm2Html
{
public:
  Editor_Usfm2Html (void * webserver_request_in);
  ~Editor_Usfm2Html ();
  void load (string usfm);
  void stylesheet (string stylesheet);
  void run ();
  string get ();
  size_t textLength;
  map <int, int> verseStartOffsets;
private:
  void * webserver_request;

  vector <string> markersAndText; // Strings alternating between USFM and text.
  unsigned int markersAndTextPointer = 0;
  
  map <string, Database_Styles_Item> styles; // All the style information.
  
  // Html DOMNodes.
  xmlDocPtr htmlDom;
  xmlNodePtr bodyDomNode;
  xmlNodePtr notesDomNode;
  
  // Standard content markers for notes.
  string standardContentMarkerFootEndNote;
  string standardContentMarkerCrossReference;
  
  xmlNodePtr currentPDomElement; // The current p DOMElement.
  string currentParagraphStyle;
  string currentParagraphContent;
  vector <string> currentTextStyles;
  
  int noteCount = 0;
  xmlNodePtr notePDomElement; // The text:p DOMElement of the current footnote, if any.
  vector <string> currentNoteTextStyles;
  
  // Whether note is open.
  bool noteOpened = false;
  
  // Lengths and offsets.
  int paragraphCount = 0;
  
  void preprocess ();
  void process ();
  void postprocess ();
  void outputAsIs (string marker, bool isOpeningMarker);
  xmlNodePtr newElement (string element);
  void newParagraph (string style = "");
  void openTextStyle (Database_Styles_Item & style, bool note, bool embed);
  void closeTextStyle (bool note, bool embed);
  void addText (string text);
  void addNote (string citation, string style, bool endnote = false);
  void addNoteText (string text);
  void closeCurrentNote ();
  void addLink (xmlNodePtr domNode, string reference, string identifier, string style, string text);
};


#endif

