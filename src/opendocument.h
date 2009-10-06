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


#ifndef INCLUDED_OPENDOCUMENT_H
#define INCLUDED_OPENDOCUMENT_H


#include "libraries.h"
#include "settings.h"
#include "usfm.h"
#include "progresswindow.h"
#include "odtutils.h"
#include "usfm-inline-markers.h"
#include "odtnotes.h"
#include <libxml/xmlwriter.h>


class OpenDocument
{
public:
  OpenDocument(const ustring& project, const ustring& filename, set<unsigned int> * selection);
  ~OpenDocument();
  void note_unformatted_markers (vector <ustring>& markers);
private:
  ustring myproject;
  ProjectConfiguration * projectconfig;
  Usfm * usfm;
  UsfmInlineMarkers * usfm_inline_markers;
  OdtFootnote * odtfootnote;
  OdtEndnote * odtendnote;
  OdtXref * odtxref;
  ProgressWindow * progresswindow;
  ustring workingdirectory;
  vector <unsigned int> books;
  vector<ustring> odtlines;
  OdtTextParagraph * odttextparagraph;
  ustring fontname;
  void unpack_template();
  void cover();
  void format_general(vector <ustring>& lines);
  void zip(const ustring filename);
  unsigned int anchor_book, anchor_chapter;
  void generate_styles_xml(bool right_to_left);
  void generate_styles(xmlTextWriterPtr writer);
  void paragraph_style(xmlTextWriterPtr writer, const ustring& marker, const ustring& name, const ustring& fontname, double fontsize, int lineheight, const ustring& italic, const ustring& bold, const ustring& underline, const ustring& smallcaps, ustring justification, double spacebefore, double spaceafter, double leftmargin, double rightmargin, double firstlineindent, bool spancolumns, bool startpage);
  void span_style(xmlTextWriterPtr writer, const ustring& marker, const ustring& name, const ustring& fontname, double fontpercentage, ustring italic, ustring bold, ustring underline, ustring smallcaps, bool superscript, unsigned int color);
  set <ustring> unformatted_markers;
};


#endif
