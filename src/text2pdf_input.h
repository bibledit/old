/*
 ** Copyright (©) 2003-2008 Teus Benschop.
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
 ** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */

#ifndef INCLUDED_TEXT2PDF_TEXT_H
#define INCLUDED_TEXT2PDF_TEXT_H

#include "libraries.h"

enum T2PInputType { t2pitParagraph, t2pitText };
enum T2PAlignmentType { t2patLeft, t2patCenter, t2patRight, t2patJustified };

class T2PInput
{
public:
  T2PInput(T2PInputType type_in);
  virtual ~T2PInput();
  T2PInputType type;
  vector <T2PInput *> children;
private:
};

class T2PInputParagraph : public T2PInput
{
public:
  T2PInputParagraph(int dummy);
  virtual ~T2PInputParagraph();
  unsigned int font_size_points;
  bool italic;
  bool bold;
  bool underline;
  bool small_caps;
  T2PAlignmentType alignment;
  int space_before_mm;
  int space_after_mm;
  int left_margin_mm;
  int right_margin_mm;
  int first_line_indent_mm;
  unsigned int column_count;
  bool keep_with_next;
private:
};

class T2PInputText : public T2PInput
{
public:
  T2PInputText(T2PInputType type_in, const ustring& text_in);
  virtual ~T2PInputText();
  ustring text;
private:
};

#endif
