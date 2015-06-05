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


#ifndef INCLUDED_EDITOR_EXPORT_H
#define INCLUDED_EDITOR_EXPORT_H


#include <config/libraries.h>
#include <libxml/tree.h>
#include <database/styles.h>


class Editor_Export
{
public:
  Editor_Export (void * webserver_request_in);
  ~Editor_Export ();
  void load (string html);
  void stylesheet (string stylesheet);
  void run ();
  string get ();
private:
  void * webserver_request;
  static void error_handler (void *ctx, const char *msg, ...);
  xmlDocPtr document = NULL; // DOMDocument holding the html.
  map <string, Database_Styles_Item> styles; // Style information.
  vector <string> output; // Output USFM.
  string currentLine; // Growing current USFM line.
  bool mono; // Monospace font.
  set <string> suppressEndMarkers; // Markers which should not have endmarkers, e.g. \v does not have \v*
  set <string> noteOpeners;
  vector <string> characterStyles; // Active character styles.
  bool processingNote = false; // Note processing flag.
  void preprocess ();
  void flushLine ();
  void postprocess ();
  void process ();
  void processNode (xmlNodePtr node);
  void openElementNode (xmlNodePtr node);
  void processNodeChildren (xmlNodePtr node);
  void closeElementNode (xmlNodePtr node);
  void processAttributeNode (xmlNodePtr node);
  void processTextNode (xmlNodePtr node);
  void openInline (string className);
  void processNoteCitation (xmlNodePtr node);
  string cleanUSFM (string usfm);
  xmlNodePtr get_note_pointer (xmlNodePtr node, string id);
};


string editor_export_verse (void * webserver_request, string stylesheet, string html);


#endif
