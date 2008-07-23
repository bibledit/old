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

#include "text2pdf_input.h"
#include <pango/pangocairo.h>

T2PInput::T2PInput(T2PInputType type_in)
// This is the base class for any input that drives the formatter.
{
  type = type_in;
}

T2PInput::~T2PInput()
// Destructor.
{
}

T2PInputParagraph::T2PInputParagraph(int dummy) :
  T2PInput(t2pitParagraph)
// This is a class that contains a new default paragraph.
{
  font_size_points = 12;
  italic = false;
  bold = false;
  underline = false;
  small_caps = false;
  space_before_mm = 0;
  space_after_mm = 0;
  left_margin_mm = 0;
  right_margin_mm = 0;
  first_line_indent_mm = 5;
  alignment = t2patJustified;
  column_count = 2;
  keep_with_next = false;
}

T2PInputParagraph::~T2PInputParagraph()
// Destructor.
{
}

void T2PInputParagraph::add_text(const ustring& text_in)
// Adds text to the paragraph.
{
  text.append(text_in);
  maximum_text_length = text.length();
}

void T2PInputParagraph::inline_set_font_size_percentage(int percentage)
// Store a value for the font size percentage in the paragraph.
{
  if (percentage == 100) {
    for (int i = font_size_percentage_end_indices.size()-1; i >= 0; i--) {
      if (font_size_percentage_end_indices[i] == -1) {
        font_size_percentage_end_indices[i] = text.length();
      }
    }
  } else {
    font_size_percentage_values.push_back(percentage);
    font_size_percentage_start_indices.push_back(text.length());
    font_size_percentage_end_indices.push_back(-1);
  }
}

bool T2PInputParagraph::inline_get_font_size(unsigned int index, bool& in_range, int& size, int& start_index, int& end_index)
// Gets the inline font size at "index".
{
  in_range = index < font_size_percentage_values.size();
  if (!in_range) {
    return false;
  }
  int base_offset = maximum_text_length - text.length();
  start_index = font_size_percentage_start_indices[index] - base_offset;
  end_index = font_size_percentage_end_indices[index] - base_offset;
  size = font_size_points * PANGO_SCALE * font_size_percentage_values[index] / 100;
  if (start_index < 0) {
    start_index = 0;
  }
  return end_index > 0;
}
