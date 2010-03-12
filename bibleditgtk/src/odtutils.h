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


#ifndef INCLUDED_ODTUTILS_H
#define INCLUDED_ODTUTILS_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>


class OdtTextParagraph
{
public:
  OdtTextParagraph (vector<ustring> * lines, const ustring& stylename);
  ~OdtTextParagraph ();
  void newline ();
  void plaintext (const ustring& line);
  void spannedtext (const ustring& line, const ustring& stylename);
private:
  ustring myline;
  vector<ustring> * mylines;
  size_t linelength;
  bool mynewline;
};


ustring odt_content_xml_filename (const ustring& directory);
void odt_set_font (const ustring& directory, const ustring& fontname);
void odt_insert_content (const ustring& directory, const vector <ustring>& text);
void odt_insert_styles (const ustring& directory, const vector <ustring>& styles, bool right_to_left);


#endif
