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


#include "note_editor.h"
#include "gwrappers.h"
#include "tiny_utilities.h"
#include "html.h"


NoteEditor::NoteEditor(int dummy)
{
}


NoteEditor::~NoteEditor()
{
}


void NoteEditor::store_original_data(const ustring & data)
// Stores the data before editing.
{
  original_data = data;
}


void NoteEditor::receive_data_from_html_editor(const char *data)
// Receive data from the html note editor.
{
  edited_data =  data;
}


ustring NoteEditor::clean_edited_data () // Todo working here.
// Clean the data from the html note editor.
{
  edited_data = trim (edited_data);
  return edited_data;
}


bool NoteEditor::data_was_edited()
// Returns whether the note data was edited.
{
  return (edited_data != original_data);
}


vector < ustring > note_editor_font_size_names_list()
// This gives a list of possible font sizes.
{
  vector < ustring > list;
  // Todo for (int i = GTK_HTML_FONT_STYLE_SIZE_1; i <= GTK_HTML_FONT_STYLE_SIZE_7; i++) {
  // Todo   list.push_back(note_editor_font_size_enum_to_name(GtkHTMLFontStyle(i)));
  // Todo }
  return list;
}


/*
// Todo 
GtkHTMLFontStyle note_editor_font_size_name_to_enum(const ustring & style)
// Todo // This accepts a name of a font size and returns the style enum.
{
  GtkHTMLFontStyle enumeration = GTK_HTML_FONT_STYLE_DEFAULT;
  for (int i = GTK_HTML_FONT_STYLE_DEFAULT; i <= GTK_HTML_FONT_STYLE_SIZE_7; i++) {
    if (style == note_editor_font_size_enum_to_name(GtkHTMLFontStyle(i)))
      enumeration = GtkHTMLFontStyle(i);
  }
  return enumeration;
}
*/

/*
// Todo 
ustring note_editor_font_size_enum_to_name(GtkHTMLFontStyle style)
// Accepts a font size enumeration and returns the name of it.
{
  style = GtkHTMLFontStyle(style & GTK_HTML_FONT_STYLE_SIZE_MASK);
  if (style == GTK_HTML_FONT_STYLE_DEFAULT)
    return "+0";
  else if (style == GTK_HTML_FONT_STYLE_SIZE_1)
    return "-2";
  else if (style == GTK_HTML_FONT_STYLE_SIZE_2)
    return "-1";
  else if (style == GTK_HTML_FONT_STYLE_SIZE_3)
    return "+0";
  else if (style == GTK_HTML_FONT_STYLE_SIZE_4)
    return "+1";
  else if (style == GTK_HTML_FONT_STYLE_SIZE_5)
    return "+2";
  else if (style == GTK_HTML_FONT_STYLE_SIZE_6)
    return "+3";
  else if (style == GTK_HTML_FONT_STYLE_SIZE_7)
    return "+4";
  return "Unknown";
}
*/


vector < ustring > note_editor_paragraph_style_names_list()
// This gives a list of possible paragraph styles.
{
  vector < ustring > list;
  // Todo for (int i = 0; i < GTK_HTML_PARAGRAPH_STYLE_ITEMALPHA; i++) {
    // Todo list.push_back(note_editor_paragraph_style_enum_to_name(GtkHTMLParagraphStyle(i)));
  // Todo }
  return list;
}


/*
// Todo 
GtkHTMLParagraphStyle note_editor_paragraph_style_name_to_enum(const ustring & style)
// This accepts the name of a paragraph style, and returns the style enum.
{
  GtkHTMLParagraphStyle enumeration = GTK_HTML_PARAGRAPH_STYLE_NORMAL;
  for (int i = 0; i < GTK_HTML_PARAGRAPH_STYLE_ITEMALPHA; i++) {
    if (style == note_editor_paragraph_style_enum_to_name(GtkHTMLParagraphStyle(i)))
      enumeration = GtkHTMLParagraphStyle(i);
  }
  return enumeration;
}
*/


/*
// Todo 
ustring note_editor_paragraph_style_enum_to_name(GtkHTMLParagraphStyle style)
// Accepts a paragraph style enumeration and returns the name of it.
{
  switch (style) {
  case GTK_HTML_PARAGRAPH_STYLE_NORMAL:
    return "Normal";
  case GTK_HTML_PARAGRAPH_STYLE_H1:
    return "Header 1";
  case GTK_HTML_PARAGRAPH_STYLE_H2:
    return "Header 2";
  case GTK_HTML_PARAGRAPH_STYLE_H3:
    return "Header 3";
  case GTK_HTML_PARAGRAPH_STYLE_H4:
    return "Header 4";
  case GTK_HTML_PARAGRAPH_STYLE_H5:
    return "Header 5";
  case GTK_HTML_PARAGRAPH_STYLE_H6:
    return "Header 6";
  case GTK_HTML_PARAGRAPH_STYLE_ADDRESS:
    return "Address";
  case GTK_HTML_PARAGRAPH_STYLE_PRE:
    return "Preformatted";
  case GTK_HTML_PARAGRAPH_STYLE_ITEMDOTTED:
    return "Bulleted list";
  case GTK_HTML_PARAGRAPH_STYLE_ITEMROMAN:
    return "Roman list";
  case GTK_HTML_PARAGRAPH_STYLE_ITEMDIGIT:
    return "Latin list";
  case GTK_HTML_PARAGRAPH_STYLE_ITEMALPHA:
    return "Alphabetical list";
  }
  return "";
}
*/

