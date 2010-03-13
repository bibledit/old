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


#ifndef INCLUDED_HTMLWRITER_H
#define INCLUDED_HTMLWRITER_H


#include "libraries.h"
#include <libxml/xmlwriter.h>
#include "types.h"


class HtmlWriter
{
public:
  HtmlWriter (const ustring& title, bool include_java_scripts, bool include_bibledit_css, bool include_bar_graph_css);
  ~HtmlWriter ();
  void heading (unsigned int level, const ustring& text);
  void paragraph (const ustring& text);
  void bargraph (const ustring& header, const vector <ustring>& texts, const vector <unsigned int>& percentages, const ustring& footer);
  void table (const ustring& main_header, const vector <ustring>& column_headers, const vector <VectorUstring>& cell_texts, const ustring& footer, vector <bool> * centers, int header_font_size);
  void hyperlinkedparagraph (const ustring& text, const ustring& hyperlink);
private:
  xmlBufferPtr buffer;
  xmlTextWriterPtr writer;
};


#endif
