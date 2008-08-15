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

T2PInputParagraph::T2PInputParagraph(const ustring& font, unsigned int line_spacing_in, bool right_to_left_in) :
  T2PInput(t2pitParagraph)
// This is a class that contains a new default paragraph.
{
  font_name = font;
  font_size_points = 12;
  line_spacing = line_spacing_in;
  right_to_left = right_to_left_in;
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
  maximum_text_length = 0;
  italic_values_completed = false;
  bold_values_completed = false;
  underline_values_completed = false;
  small_caps_values_completed = false;
  strike_through_values_completed = false;
  intrusion = NULL;
}

T2PInputParagraph::~T2PInputParagraph()
// Destructor.
{
  for (unsigned int i = 0; i < note_pointers.size(); i++) {
    delete note_pointers[i];
  }
}

void T2PInputParagraph::add_text(const string& text_in)
// Adds text to the paragraph.
{
  text.append(text_in);
  maximum_text_length = text.length();
}

void T2PInputParagraph::inline_set_font_size_percentage(int percentage)
// Store a value for the inline font size percentage.
{
  if (percentage == 100) {
    for (int i = font_size_percentage_end_indices.size()-1; i >= 0; i--) {
      if (font_size_percentage_end_indices[i] == -1) {
        font_size_percentage_end_indices[i] = maximum_text_length;
      }
    }
  } else {
    font_size_percentage_values.push_back(percentage);
    font_size_percentage_start_indices.push_back(maximum_text_length);
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
  end_index = font_size_percentage_end_indices[index];
  if (end_index == -1)
    end_index = maximum_text_length;
  end_index -= base_offset;
  size = font_size_points * PANGO_SCALE * font_size_percentage_values[index] / 100;
  if (start_index < 0) {
    start_index = 0;
  }
  return end_index > 0;
}

void T2PInputParagraph::inline_set_italic(T2PMarkupType italic, bool cleanup_only)
// Store a value for the inline italic markup.
{
  inline_set_value(italic_values, italic_start_indices, italic_end_indices, italic, cleanup_only);
}

bool T2PInputParagraph::inline_get_italic(unsigned int index, bool& in_range, bool& value, int& start_index, int& end_index)
// Gets the inline italic markup at "index".
{
  return inline_get_value(italic_values, italic_start_indices, italic_end_indices, italic_values_completed, italic, index, in_range, value, start_index, end_index);
}

void T2PInputParagraph::inline_set_bold(T2PMarkupType bold, bool cleanup_only)
// Store a value for the inline bold markup.
{
  inline_set_value(bold_values, bold_start_indices, bold_end_indices, bold, cleanup_only);
}

bool T2PInputParagraph::inline_get_bold(unsigned int index, bool& in_range, bool& value, int& start_index, int& end_index)
// Gets the inline bold markup at "index".
{
  return inline_get_value(bold_values, bold_start_indices, bold_end_indices, bold_values_completed, bold, index, in_range, value, start_index, end_index);
}

void T2PInputParagraph::inline_set_underline(T2PMarkupType underline, bool cleanup_only)
// Store a value for the inline underline markup.
{
  inline_set_value(underline_values, underline_start_indices, underline_end_indices, underline, cleanup_only);
}

bool T2PInputParagraph::inline_get_underline(unsigned int index, bool& in_range, bool& value, int& start_index, int& end_index)
// Gets the inline underline markup at "index".
{
  return inline_get_value(underline_values, underline_start_indices, underline_end_indices, underline_values_completed, underline, index, in_range, value, start_index, end_index);
}

void T2PInputParagraph::inline_set_small_caps(T2PMarkupType small_caps, bool cleanup_only)
// Store a value for the inline small caps markup.
{
  inline_set_value(small_caps_values, small_caps_start_indices, small_caps_end_indices, small_caps, cleanup_only);
}

bool T2PInputParagraph::inline_get_small_caps(unsigned int index, bool& in_range, bool& value, int& start_index, int& end_index)
// Gets the inline small caps markup at "index".
{
  return inline_get_value(small_caps_values, small_caps_start_indices, small_caps_end_indices, small_caps_values_completed, small_caps, index, in_range, value, start_index, end_index);
}

void T2PInputParagraph::inline_set_value(vector <int>& values, vector <int>& start_indices, vector <int>& end_indices, T2PMarkupType value, bool cleanup_only)
// Store a value for the inline markup.
{
  // If the last end index is there, it will have been set to a value of -1.
  // Set it to the proper value.
  if (!end_indices.empty()) {
    end_indices[end_indices.size()-1] = maximum_text_length;
  }
  // Fill up any missing values till the current offset of the text.
  int values_filled_offset = 0;
  if (!end_indices.empty()) {
    values_filled_offset = end_indices[end_indices.size()-1];
  }
  if ((int)maximum_text_length > values_filled_offset) {
    values.push_back(t2pmtInherit);
    start_indices.push_back(values_filled_offset);
    end_indices.push_back(maximum_text_length);
  }
  // Store the current values, leaving the end index undefined as its value is not yet known at this stage.
  if (!cleanup_only) {
    values.push_back(value);
    start_indices.push_back(maximum_text_length);
    end_indices.push_back(-1);
  }
}

bool T2PInputParagraph::inline_get_value(vector <int>& values, vector <int>& start_indices, vector <int>& end_indices, bool& values_completed, bool paragraph_value, unsigned int index, bool& in_range, bool& value, int& start_index, int& end_index)
// Gets the inline markup value at "index".
{
  // If needed complete the values. 
  if (!values_completed) {
    inline_set_value(values, start_indices, end_indices, t2pmtInherit, true);
    for (unsigned int i = 0; i < values.size(); i++) {
      if (values[i] == t2pmtInherit) {
        values[i]= (paragraph_value ? t2pmtOn : t2pmtOff);
      } else if (values[i] == t2pmtToggle) {
        values[i]= (paragraph_value ? t2pmtOff : t2pmtOn);
      }
    }
    values_completed = true;
  }

  // Get the value at the index.  
  in_range = index < values.size();
  if (!in_range) {
    return false;
  }
  int base_offset = maximum_text_length - text.length();
  start_index = start_indices[index] - base_offset;
  end_index = end_indices[index] - base_offset;
  value = values[index] == t2pmtOn;
  if (start_index < 0) {
    start_index = 0;
  }
  return end_index > 0;
}

void T2PInputParagraph::inline_set_superscript(bool superscript) {
  if (superscript) {
    superscript_values.push_back(superscript);
    superscript_start_indices.push_back(maximum_text_length);
    superscript_end_indices.push_back(-1);
  } else {
    for (int i = superscript_end_indices.size()-1; i >= 0; i--) {
      if (superscript_end_indices[i] == -1) {
        superscript_end_indices[i] = maximum_text_length;
      }
    }
  }
}

bool T2PInputParagraph::inline_get_superscript(unsigned int index, bool& in_range, bool& superscript, int& start_index, int& end_index, size_t text_length) {
  in_range = index < superscript_values.size();
  if (!in_range) {
    return false;
  }
  int base_offset = maximum_text_length - text_length;
  start_index = superscript_start_indices[index] - base_offset;
  end_index = superscript_end_indices[index];
  if (end_index == -1)
    end_index = maximum_text_length;
  end_index -= base_offset;
  superscript = superscript_values[index];
  if (start_index < 0) {
    start_index = 0;
  }
  return end_index > 0;
}

void T2PInputParagraph::inline_set_colour(int colour)
// Store a value for the inline colour.
{
  if (colour == 0) {
    for (int i = colour_end_indices.size()-1; i >= 0; i--) {
      if (colour_end_indices[i] == -1) {
        colour_end_indices[i] = maximum_text_length;
      }
    }
  } else {
    colour_values.push_back(colour);
    colour_start_indices.push_back(maximum_text_length);
    colour_end_indices.push_back(-1);
  }
}

bool T2PInputParagraph::inline_get_colour(unsigned int index, bool& in_range, int& value, int& start_index, int& end_index)
// Gets the inline colour at "index".
{
  in_range = index < colour_values.size();
  if (!in_range) {
    return false;
  }
  int base_offset = maximum_text_length - text.length();
  start_index = colour_start_indices[index] - base_offset;
  end_index = colour_end_indices[index];
  if (end_index == -1)
    end_index = maximum_text_length;
  end_index -= base_offset;
  value = colour_values[index];
  if (start_index < 0) {
    start_index = 0;
  }
  return end_index > 0;
}

void T2PInputParagraph::inline_set_strike_through(T2PMarkupType strike_through, bool cleanup_only)
// Store a value for the inline strike-through markup.
{
  inline_set_value(strike_through_values, strike_through_start_indices, strike_through_end_indices, strike_through, cleanup_only);
}

bool T2PInputParagraph::inline_get_strike_through(unsigned int index, bool& in_range, bool& value, int& start_index, int& end_index)
// Gets the inline strike-through markup at "index".
{
  return inline_get_value(strike_through_values, strike_through_start_indices, strike_through_end_indices, strike_through_values_completed, t2pmtOff, index, in_range, value, start_index, end_index);
}

void T2PInputParagraph::add_note(T2PInputParagraph * note)
// Adds a note to this paragraph of text.
{
  note_pointers.push_back(note);
  note_offsets.push_back(maximum_text_length);
}

vector <T2PInputParagraph *> T2PInputParagraph::get_notes(size_t text_length_before_fitting, size_t text_length_after_fitting)
// Gets the notes between the start and end index.
{
  vector <T2PInputParagraph *> notes;
  size_t start_offset = maximum_text_length - text_length_before_fitting;
  size_t end_offset = start_offset + text_length_after_fitting;
  for (size_t i = 0; i < note_offsets.size(); i++) {
    if (note_offsets[i] >= start_offset) {
      if (note_offsets[i] <= end_offset) {
        notes.push_back(note_pointers[i]);
      }
    }
  }
  return notes;
}

void T2PInputParagraph::set_intrusion(T2PInputParagraph * intrusion_in) // Todo 
// Sets the intrusion of this paragraph.
{
  intrusion = intrusion_in;
}

