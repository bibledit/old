/*
** Copyright (©) 2003-2013 Teus Benschop.
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


#ifndef INCLUDED_USFM_H
#define INCLUDED_USFM_H


#include "libraries.h"
#include "style.h"


class Usfm
{
public:
  Usfm (const ustring& stylesheet_in);
  ~Usfm ();
  ustring stylesheet;
  void reload ();
  vector<Style> styles;
  bool is_identifier (const ustring& usfm);
  IdentifierType identifier_get_subtype (const ustring& usfm);
  bool is_starting_paragraph (const ustring& usfm);
  ParagraphType paragraph_get_subtype (const ustring& usfm);
  bool is_inline_text (const ustring& usfm);
  bool is_chapter_number (const ustring& usfm);
  bool is_verse_number (const ustring& usfm);
  bool is_foot_endnote (const ustring& usfm);
  FootEndNoteType foot_endnote_get_subtype (const ustring& usfm);
  bool is_xref (const ustring& usfm);
  CrossreferenceType xref_get_subtype (const ustring& usfm);
  bool is_peripheral (const ustring& usfm);
  PeripheralType peripheral_get_subtype (const ustring& usfm);
  bool is_picture (const ustring& usfm);
  bool is_pagebreak (const ustring& usfm);
  bool is_table_element (const ustring& usfm);
  TableElementType table_element_get_subtype (const ustring& usfm);
  ustring name ();
  double fontsize ();
  ustring italic ();
  ustring bold ();
  ustring underline ();
  ustring smallcaps ();
  bool superscript ();
  ustring justification ();
  double spacebefore ();
  double spaceafter ();
  double leftmargin ();
  double rightmargin ();
  double firstlineindent ();
  bool spancolumns ();
  unsigned int color ();
  bool userbool1 ();
  bool userbool2 ();
  bool userbool3 ();
  int userint1 ();
private:
  ustring last_marker;
  int last_style_pointer;
  int get_style_pointer (const ustring& usfm);
};


bool usfm_is_id (const ustring& marker);
bool usfm_is_chapter (const ustring& marker);
bool usfm_is_verse (const ustring& marker);


#endif
