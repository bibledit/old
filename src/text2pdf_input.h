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


#ifndef INCLUDED_TEXT2PDF_TEXT_H
#define INCLUDED_TEXT2PDF_TEXT_H


#include "libraries.h"


enum T2PInputType {t2pitParagraph, t2pitOpenKeepTogether, t2pitCloseKeepTogether, t2pitNewPage, t2pitNewOddPage};
enum T2PAlignmentType {t2patLeft, t2patCenter, t2patRight, t2patJustified};
enum T2PMarkupType {t2pmtOff, t2pmtOn, t2pmtInherit, t2pmtToggle};
enum T2PHeaderType {t2phtFixed};


class T2PInput
{
public:
  T2PInput(T2PInputType type_in);
  virtual ~T2PInput();
  T2PInputType type;
private:
};


class T2PInputParagraph : public T2PInput
{
public:
  T2PInputParagraph(const ustring& font, unsigned int line_spacing_in, bool right_to_left_in);
  virtual ~T2PInputParagraph();
  ustring font_name;
  float font_size_points;
  unsigned int line_spacing;
  bool right_to_left;
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
  void add_text(const string& text_in);
  string text;
  void inline_set_font_size_percentage(int percentage);
  bool inline_get_font_size(unsigned int index, bool& in_range, int& size, int& start_index, int& end_index);
  void inline_set_italic(T2PMarkupType italic, bool cleanup_only = false);
  bool inline_get_italic(unsigned int index, bool& in_range, bool& value, int& start_index, int& end_index);
  void inline_set_bold(T2PMarkupType bold, bool cleanup_only = false);
  bool inline_get_bold(unsigned int index, bool& in_range, bool& value, int& start_index, int& end_index);
  void inline_set_underline(T2PMarkupType underline, bool cleanup_only = false);
  bool inline_get_underline(unsigned int index, bool& in_range, bool& value, int& start_index, int& end_index);
  void inline_set_small_caps(T2PMarkupType small_caps, bool cleanup_only = false);
  bool inline_get_small_caps(unsigned int index, bool& in_range, bool& value, int& start_index, int& end_index);
  void inline_set_superscript(bool superscript);
  bool inline_get_superscript(unsigned int index, bool& in_range, bool& superscript, int& start_index, int& end_index, size_t text_length);
  void inline_set_colour(int colour);
  bool inline_get_colour(unsigned int index, bool& in_range, int& value, int& start_index, int& end_index);
  void inline_set_strike_through(T2PMarkupType strike_through, bool cleanup_only = false);
  bool inline_get_strike_through(unsigned int index, bool& in_range, bool& value, int& start_index, int& end_index);
  void add_note(T2PInputParagraph * note);
  vector <T2PInputParagraph *> get_notes(size_t text_length_before_fitting, size_t text_length_after_fitting);
  void set_intrusion(T2PInputParagraph * intrusion_in);
  T2PInputParagraph * get_intrusion();
  ustring left_running_header;
  ustring right_running_header;
  unsigned int left_running_chapter;
  unsigned int right_running_chapter;
  bool suppress_header;
  ustring reference;
  ustring referent;
private:
  size_t maximum_text_length;
  vector <int> font_size_percentage_values;
  vector <int> font_size_percentage_start_indices;
  vector <int> font_size_percentage_end_indices;
  void inline_set_value(vector <int>& values, vector <int>& start_indices, vector <int>& end_indices, T2PMarkupType value, bool cleanup_only);
  bool inline_get_value(vector <int>& values, vector <int>& start_indices, vector <int>& end_indices, bool& values_completed, bool paragraph_value, unsigned int index, bool& in_range, bool& value, int& start_index, int& end_index);
  vector <int> italic_values;
  vector <int> italic_start_indices;
  vector <int> italic_end_indices;
  bool italic_values_completed;
  vector <int> bold_values;
  vector <int> bold_start_indices;
  vector <int> bold_end_indices;
  bool bold_values_completed;
  vector <int> underline_values;
  vector <int> underline_start_indices;
  vector <int> underline_end_indices;
  bool underline_values_completed;
  vector <int> small_caps_values;
  vector <int> small_caps_start_indices;
  vector <int> small_caps_end_indices;
  bool small_caps_values_completed;
  vector <bool> superscript_values;
  vector <int> superscript_start_indices;
  vector <int> superscript_end_indices;
  vector <int> colour_values;
  vector <int> colour_start_indices;
  vector <int> colour_end_indices;
  vector <int> strike_through_values;
  vector <int> strike_through_start_indices;
  vector <int> strike_through_end_indices;
  bool strike_through_values_completed;
  vector <T2PInputParagraph *> note_pointers;
  vector <size_t> note_offsets;
  T2PInputParagraph * intrusion;
  void add_text_internal(const string& text);
};


#endif
