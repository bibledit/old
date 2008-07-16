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
  T2PInputParagraph(int first_line_indent_mm_in, T2PAlignmentType alignment_in, unsigned int column_count_in);
  T2PInputParagraph();
  virtual ~T2PInputParagraph();
  int first_line_indent_mm;
  T2PAlignmentType alignment;
  unsigned int column_count;
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
