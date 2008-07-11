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
 ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 **  
 */

#include "text2pdf_text.h"

T2PInput::T2PInput(T2PInputType type_in)
// This is the base class for any input that drives the formatter.
{
  type = type_in;
}

T2PInput::~T2PInput()
// Destructor.
{
  for (unsigned int i = 0; i < children.size(); i++) {
    delete children[i];
  }
}

T2PInputText::T2PInputText(T2PInputType type_in, const ustring& text_in) :
  T2PInput(type_in)
// This is a class that contains input text.
{
  type = type_in;
  text = text_in;
}

T2PInputText::~T2PInputText()
// Destructor.
{
}

T2PInputParagraph::T2PInputParagraph(int first_line_indent_mm_in) :
  T2PInput(tp2itParagraph)
// This is a class that contains a new paragraph.
{
  type = tp2itParagraph;
  first_line_indent_mm = first_line_indent_mm_in;
}

T2PInputParagraph::T2PInputParagraph() :
  T2PInput(tp2itParagraph)
// This is a class that contains a new paragraph.
{
  type = tp2itParagraph;
  first_line_indent_mm = 5;
}

T2PInputParagraph::~T2PInputParagraph()
// Destructor.
{
}

/*

Tests.

A default paragraph.

A paragraph with positive indentation, and one with negative indentation.



*/