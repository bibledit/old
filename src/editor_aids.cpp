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


#include "editor_aids.h"
#include "settings.h"
#include "styles.h"
#include "utilities.h"
#include "usfmtools.h"
#include "tiny_utilities.h"
#include "spelling.h"
#include "stylesheetutils.h"
#include "gwrappers.h"
#include "bible.h"


void marker_get_type_and_subtype(const ustring & project, const ustring & marker, StyleType & type, int &subtype)
/*
 Given a "project", and a "marker", this function gives the "type" and the 
 "subtype" of the style of that marker.
 */
{
  // Code for speeding up the lookup process.
  static ustring speed_project;
  static ustring speed_marker;
  static StyleType speed_type = stNotUsedComment;
  static int speed_subtype = 0;
  if (project == speed_project) {
    if (marker == speed_marker) {
      type = speed_type;
      subtype = speed_subtype;
      return;
    }
  }
  
  // Store both keys in the speedup system.
  speed_project = project;
  speed_marker = marker;

  // Lookup the values.  
  ustring stylesheet = stylesheet_get_actual ();
  extern Styles *styles;
  Usfm *usfm = styles->usfm(stylesheet);
  type = stIdentifier;
  subtype = itComment;
  for (unsigned int i = 0; i < usfm->styles.size(); i++) {
    if (marker == usfm->styles[i].marker) {
      // Values found.
      type = usfm->styles[i].type;
      subtype = usfm->styles[i].subtype;
      // Store values in the speedup system.
      speed_type = type;
      speed_subtype = subtype;
      break;
    }
  }
}


bool style_get_plaintext(StyleType type, int subtype)
/*
 Returns the property "plain text" of a certain style when displayed in the Editor.
 Plain text means that this style is to be displayed in the editor using
 plain text.
 type: the type of this style.
 subtype: the subtype of this style.
 */
{
  // Set default value.
  bool plaintext = true;
  // Set value depending on the type of the marker, and the subtype.
  // Only set value if it differs from the default.
  switch (type) {
    case stIdentifier:
    {
      if (subtype == itCommentWithEndmarker)
        plaintext = false;
      break;
    }
    case stNotUsedComment:
    case stNotUsedRunningHeader:
    {
      break;
    }
    case stStartsParagraph:
    case stInlineText:
    case stChapterNumber:
    case stVerseNumber:
    case stFootEndNote:
    case stCrossreference:
    {
      plaintext = false;
      break;
    }
    case stPeripheral:
    {
      break;
    }
    case stPicture:
    {
      break;
    }
    case stPageBreak:
    {
      break;
    }
    case stTableElement:
    {
      break;
    }
    case stWordlistElement:
    {
      plaintext = false;
      break;
    }
  }
  // Return the value.
  return plaintext;
}


bool style_get_paragraph(StyleType type, int subtype)
/*
 Returns true if the combination of the "type" and the"subtype" is
 a paragraph style, as opposed to a character style.

 Note that there is another function, "style_get_starts_new_line_in_editor", 
 which has a slightly different use. Let these two not be confused.

 This function says whether a style is a paragraph style, and the other 
 function says whether a style ought to start a new line in the formatted view.
 */
{
  bool paragraph_style = true;
  // Set value depending on the type of the marker, and the subtype.
  // Only set value if it differs from the default.
  switch (type) {
  case stIdentifier:
    {
      if (subtype == itCommentWithEndmarker)
        paragraph_style = false;
      break;
    }
  case stNotUsedComment:
  case stNotUsedRunningHeader:
    {
      break;
    }
  case stStartsParagraph:
    {
      break;
    }
  case stInlineText:
    {
      paragraph_style = false;
      break;
    }
  case stChapterNumber:
    {
      break;
    }
  case stVerseNumber:
    {
      paragraph_style = false;
      break;
    }
  case stFootEndNote:
    {
      if ((subtype != fentParagraph) && (subtype != fentStandardContent))
        paragraph_style = false;
      break;
    }
  case stCrossreference:
    {
      if (subtype != ctStandardContent)
        paragraph_style = false;
      break;
    }
  case stPeripheral:
    {
      break;
    }
  case stPicture:
    {
      break;
    }
  case stPageBreak:
    {
      break;
    }
  case stTableElement:
    {
      break;
    }
  case stWordlistElement:
    {
      paragraph_style = false;
      break;
    }
  }
  return paragraph_style;
}


bool style_get_starts_new_line_in_editor(StyleType type, int subtype)
// Returns true if the combination of the "type" and the"subtype" starts
// a new line in the formatted view.
{
  // Set default value to starting a new line.
  bool starts_new_line = true;
  // Set value depending on the type of the marker, and the subtype.
  // Only set value if it differs from the default.
  switch (type) {
  case stIdentifier:
    {
      if (subtype == itCommentWithEndmarker)
        starts_new_line = false;
      break;
    }
  case stNotUsedComment:
  case stNotUsedRunningHeader:
    {
      break;
    }
  case stStartsParagraph:
    {
      break;
    }
  case stInlineText:
    {
      starts_new_line = false;
      break;
    }
  case stChapterNumber:
    {
      break;
    }
  case stVerseNumber:
    {
      starts_new_line = false;
      break;
    }
  case stFootEndNote:
    {
      if (subtype != fentParagraph)
        starts_new_line = false;
      break;
    }
  case stCrossreference:
    {
      starts_new_line = false;
      break;
    }
  case stPeripheral:
    {
      break;
    }
  case stPicture:
    {
      break;
    }
  case stPageBreak:
    {
      break;
    }
  case stTableElement:
    {
      break;
    }
  case stWordlistElement:
    {
      starts_new_line = false;
      break;
    }
  }
  // Return the outcome.
  return starts_new_line;
}


bool style_get_starts_new_line_in_usfm(StyleType type, int subtype)
// Returns true if the combination of the "type" and the"subtype" starts
// a new line in the usfm code.
{
  // Set default value to starting a new line.
  bool starts_new_line = true;
  // Set value depending on the type of the marker, and the subtype.
  // Only set value if it differs from the default.
  switch (type) {
  case stIdentifier:
    {
      if (subtype == itCommentWithEndmarker)
        starts_new_line = false;
      break;
    }
  case stNotUsedComment:
  case stNotUsedRunningHeader:
    {
      break;
    }
  case stStartsParagraph:
    {
      break;
    }
  case stInlineText:
    {
      starts_new_line = false;
      break;
    }
  case stChapterNumber:
    {
      break;
    }
  case stVerseNumber:
    {
      break;
    }
  case stFootEndNote:
    {
      starts_new_line = false;
      break;
    }
  case stCrossreference:
    {
      starts_new_line = false;
      break;
    }
  case stPeripheral:
    {
      break;
    }
  case stPicture:
    {
      break;
    }
  case stPageBreak:
    {
      break;
    }
  case stTableElement:
    {
      break;
    }
  case stWordlistElement:
    {
      starts_new_line = false;
      break;
    }
  }
  // Return the outcome.
  return starts_new_line;
}


bool style_get_displays_marker(StyleType type, int subtype)
// Returns true if the combination of the "type" and the"subtype" should display
// the marker in the formatted view.
{
  // Set default value to displaying the style.
  bool display_marker = true;
  // Set value depending on the type of the marker, and the subtype.
  // Only set value if it differs from the default.
  switch (type) {
    case stIdentifier:
    {
      if (subtype == itCommentWithEndmarker)
        display_marker = false;
      break;
    }
    case stNotUsedComment:
    case stNotUsedRunningHeader:
    {
      break;
    }
    case stStartsParagraph:
    {
      display_marker = false;
      break;
    }
    case stInlineText:
    {
      display_marker = false;
      break;
    }
    case stChapterNumber:
    {
      display_marker = false;
      break;
    }
    case stVerseNumber:
    {
      display_marker = false;
      break;
    }
    case stFootEndNote:
    {
      display_marker = false;
      break;
    }
    case stCrossreference:
    {
      display_marker = false;
      break;
    }
    case stPeripheral:
    {
      break;
    }
    case stPicture:
    {
      break;
    }
    case stPageBreak:
    {
      break;
    }
    case stTableElement:
    {
      break;
    }
    case stWordlistElement:
    {
      display_marker = false;
      break;
    }
  }
  // Return the outcome.
  return display_marker;

}


bool style_get_starts_character_style(StyleType type, int subtype)
// Returns true if the combination of the "type" and the"subtype" starts
// (or ends, of course) a character style in the formatted view.
{
  // Set default value to not starting a character style.
  bool starts_character_style = false;
  // Set value depending on the type of the marker, and the subtype.
  // Only set value if it differs from the default.
  switch (type) {
  case stIdentifier:
    {
      if (subtype == itCommentWithEndmarker)
        starts_character_style = true;
      break;
    }
  case stNotUsedComment:
  case stNotUsedRunningHeader:
    {
      break;
    }
  case stStartsParagraph:
    {
      break;
    }
  case stInlineText:
    {
      starts_character_style = true;
      break;
    }
  case stChapterNumber:
    {
      break;
    }
  case stVerseNumber:
    {
      break;
    }
  case stFootEndNote:
    {
      if (subtype == fentContentWithEndmarker)
        starts_character_style = true;
      break;
    }
  case stCrossreference:
    {
      if (subtype == ctContentWithEndmarker)
        starts_character_style = true;
      break;
    }
  case stPeripheral:
    {
      break;
    }
  case stPicture:
    {
      break;
    }
  case stPageBreak:
    {
      break;
    }
  case stTableElement:
    {
      break;
    }
  case stWordlistElement:
    {
      starts_character_style = true;
      break;
    }
  }
  // Return the outcome.
  return starts_character_style;
}

bool style_get_starts_verse_number(StyleType type, int subtype)
// Returns true if the combination of the "type" and the"subtype" starts
// a verse number.
{
  // Set default value to not starting a a verse number.
  bool starts_verse_number = false;
  // Set value depending on the type of the marker, and the subtype.
  // Only set value if it differs from the default.
  switch (type) {
  case stIdentifier:
  case stNotUsedComment:
  case stNotUsedRunningHeader:
  case stStartsParagraph:
  case stInlineText:
  case stChapterNumber:
    {
      break;
    }
  case stVerseNumber:
    {
      starts_verse_number = true;
      break;
    }
  case stFootEndNote:
  case stCrossreference:
  case stPeripheral:
  case stPicture:
  case stPageBreak:
  case stTableElement:
  case stWordlistElement:
    {
      break;
    }
  }
  // Return the outcome.
  return starts_verse_number;
}

ustring style_get_verse_marker(const ustring & project)
// Gets the verse marker, normally the "v".
{
  ustring stylesheet = stylesheet_get_actual ();
  extern Styles *styles;
  Usfm *usfm = styles->usfm(stylesheet);
  ustring style = "v";
  for (unsigned int i = 0; i < usfm->styles.size(); i++) {
    if (style_get_starts_verse_number(usfm->styles[i].type, usfm->styles[i].subtype)) {
      style = usfm->styles[i].marker;
      break;
    }
  }
  return style;
}


bool style_get_starts_footnote(StyleType type, int subtype)
// Returns true if the combination of the "type" and the"subtype" starts
// a footnote.
{
  switch (type) {
    case stIdentifier:
    case stNotUsedComment:
    case stNotUsedRunningHeader:
    case stStartsParagraph:
    case stInlineText:
    case stChapterNumber:
    case stVerseNumber:
    {
      break;
    }
    case stFootEndNote:
    {
      if (subtype == fentFootnote)
        return true;
      break;
    }
    case stCrossreference:
    case stPeripheral:
    case stPicture:
    case stPageBreak:
    case stTableElement:
    case stWordlistElement:
    {
      break;
    }
  }
  return false;
}


bool style_get_starts_endnote(StyleType type, int subtype)
// Returns true if the combination of the "type" and the"subtype" starts
// an endnote.
{
  switch (type) {
    case stIdentifier:
    case stNotUsedComment:
    case stNotUsedRunningHeader:
    case stStartsParagraph:
    case stInlineText:
    case stChapterNumber:
    case stVerseNumber:
    {
      break;
    }
    case stFootEndNote:
    {
      if (subtype == fentEndnote)
        return true;
      break;
    }
    case stCrossreference:
    case stPeripheral:
    case stPicture:
    case stPageBreak:
    case stTableElement:
    case stWordlistElement:
    {
      break;
    }
  }
  return false;
}


bool style_get_starts_crossreference(StyleType type, int subtype)
// Returns true if the combination of the "type" and the"subtype" starts
// a crossreference.
{
  switch (type) {
    case stIdentifier:
    case stNotUsedComment:
    case stNotUsedRunningHeader:
    case stStartsParagraph:
    case stInlineText:
    case stChapterNumber:
    case stVerseNumber:
    case stFootEndNote:
    {
      break;
    }
    case stCrossreference:
    {
      if (subtype == ctCrossreference)
        return true;
      break;
    }
    case stPeripheral:
    case stPicture:
    case stPageBreak:
    case stTableElement:
    case stWordlistElement:
    {
      break;
    }
  }
  return false;
}


bool style_get_starts_note_content(StyleType type, int subtype)
// Returns true if the combination of the "type" and the"subtype" starts
// note content.
{
  bool note_content = false;
  switch (type) {
    case stIdentifier:
    case stNotUsedComment:
    case stNotUsedRunningHeader:
    case stStartsParagraph:
    case stInlineText:
    case stChapterNumber:
    case stVerseNumber:
    {
      break;
    }
    case stFootEndNote:
    {
      if ((subtype == fentContent) || (subtype == fentStandardContent))
        note_content = true;
      break;
    }
    case stCrossreference:
    {
      if ((subtype == ctContent) || (subtype == ctStandardContent))
        note_content = true;
      break;
    }
    case stPeripheral:
    case stPicture:
    case stPageBreak:
    case stTableElement:
    case stWordlistElement:
    {
      break;
    }
  }
  return note_content;
}


ustring style_get_default_note_style(const ustring & project, EditorNoteType type)
{
  ustring stylesheet = stylesheet_get_actual ();
  extern Styles *styles;
  Usfm *usfm = styles->usfm(stylesheet);
  ustring style;
  switch (type) {
    case entFootnote:
    case entEndnote:
    {
      style = "ft";
      break;
    }
    case entCrossreference:
    {
      style = "xt";
      break;
    }
  }
  for (unsigned int i = 0; i < usfm->styles.size(); i++) {
    switch (type) {
      case entFootnote:
      case entEndnote:
      {
        if (usfm->styles[i].type == stFootEndNote)
          if (usfm->styles[i].subtype == fentStandardContent)
            style = usfm->styles[i].marker;
        break;
      }
      case entCrossreference:
      {
        if (usfm->styles[i].type == stCrossreference)
          if (usfm->styles[i].subtype == ctStandardContent)
            style = usfm->styles[i].marker;
        break;
      }
    }
  }
  return style;
}


ustring style_get_paragraph_note_style(const ustring & project)
// Gets the style that starts a new paragraph in a footnote or endnote.
{
  ustring stylesheet = stylesheet_get_actual ();
  extern Styles *styles;
  Usfm *usfm = styles->usfm(stylesheet);
  ustring style("fp");
  for (unsigned int i = 0; i < usfm->styles.size(); i++) {
    if (usfm->styles[i].type == stFootEndNote)
      if (usfm->styles[i].subtype == fentParagraph)
        style = usfm->styles[i].marker;
  }
  return style;
}


bool style_get_starts_table_row(StyleType type, int subtype)
{
  bool starts_row = false;
  switch (type) {
  case stIdentifier:
  case stNotUsedComment:
  case stNotUsedRunningHeader:
  case stStartsParagraph:
  case stInlineText:
  case stChapterNumber:
  case stVerseNumber:
  case stFootEndNote:
  case stCrossreference:
  case stPeripheral:
  case stPicture:
  case stPageBreak:
    {
      break;
    }
  case stTableElement:
    {
      if (subtype == tetRow)
        starts_row = true;
      break;
    }
  case stWordlistElement:
    {
      break;
    }
  }
  return starts_row;
}

ustring style_get_table_row_marker(const ustring & project)
// Get the marker that starts a new row in a table.
{
  ustring stylesheet = stylesheet_get_actual ();
  extern Styles *styles;
  Usfm *usfm = styles->usfm(stylesheet);
  ustring style = "tr";
  for (unsigned int i = 0; i < usfm->styles.size(); i++) {
    if (style_get_starts_table_row(usfm->styles[i].type, usfm->styles[i].subtype)) {
      style = usfm->styles[i].marker;
      break;
    }
  }
  return style;
}

bool style_get_starts_table_cell(StyleType type, int subtype)
{
  bool starts_cell = false;
  switch (type) {
  case stIdentifier:
  case stNotUsedComment:
  case stNotUsedRunningHeader:
  case stStartsParagraph:
  case stInlineText:
  case stChapterNumber:
  case stVerseNumber:
  case stFootEndNote:
  case stCrossreference:
  case stPeripheral:
  case stPicture:
  case stPageBreak:
    {
      break;
    }
  case stTableElement:
    {
      if (subtype != tetRow)
        starts_cell = true;
      break;
    }
  case stWordlistElement:
    {
      break;
    }
  }
  return starts_cell;
}

ustring style_get_table_cell_marker(const ustring & project, int column)
// Get the marker that starts a cell in a table in "column".
// Column starts with 1 for the first column.
{
  ustring stylesheet = stylesheet_get_actual ();
  extern Styles *styles;
  Usfm *usfm = styles->usfm(stylesheet);
  ustring style = "tc" + convert_to_string(column);
  for (unsigned int i = 0; i < usfm->styles.size(); i++) {
    if (style_get_starts_table_cell(usfm->styles[i].type, usfm->styles[i].subtype)) {
      if (usfm->styles[i].subtype == tetCell) {
        if (usfm->styles[i].userint1 == column) {
          style = usfm->styles[i].marker;
          break;
        }
      }
    }
  }
  return style;
}


EditorNoteType note_type_get(const ustring & project, const ustring & marker)
// Gets the type of the note, e.g. a footnote.
{
  EditorNoteType notetype = entFootnote;
  ustring stylesheet = stylesheet_get_actual ();
  extern Styles *styles;
  Usfm *usfm = styles->usfm(stylesheet);
  for (unsigned int i = 0; i < usfm->styles.size(); i++) {
    if (usfm->styles[i].marker == marker) {
      if (usfm->styles[i].type == stFootEndNote) {
        if (usfm->styles[i].subtype == fentFootnote) {
          notetype = entFootnote;
        }
        if (usfm->styles[i].subtype == fentEndnote) {
          notetype = entFootnote;
        }
      }
      if (usfm->styles[i].type == stCrossreference) {
        notetype = entCrossreference;
      }
    }
  }
  return notetype;
}

NoteNumberingType note_numbering_type_get(const ustring & project, const ustring & marker)
/*
 Gets the numbering type of a note, for example, the numbering could be numerical
 or alphabethical.
 */
{
  NoteNumberingType numbering = nntNumerical;
  ustring stylesheet = stylesheet_get_actual ();
  extern Styles *styles;
  Usfm *usfm = styles->usfm(stylesheet);
  for (unsigned int i = 0; i < usfm->styles.size(); i++) {
    if (usfm->styles[i].marker == marker) {
      numbering = (NoteNumberingType) usfm->styles[i].userint1;
    }
  }
  return numbering;
}


ustring note_numbering_user_sequence_get(const ustring & project, const ustring & marker)
// Gets the sequence of characters from which the note caller should be taken.
{
  ustring sequence;
  ustring stylesheet = stylesheet_get_actual ();
  extern Styles *styles;
  Usfm *usfm = styles->usfm(stylesheet);
  for (unsigned int i = 0; i < usfm->styles.size(); i++) {
    if (usfm->styles[i].marker == marker) {
      sequence = usfm->styles[i].userstring1;
    }
  }
  return sequence;
}


gint table_get_n_rows(GtkTable * table)
{
  gint n_rows = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(table), "n_rows"));
  return n_rows;
}


gint table_get_n_columns(GtkTable * table)
{
  gint n_columns = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(table), "n_columns"));
  return n_columns;
}


void usfm_internal_add_text(ustring & text, const ustring & addition)
// This is an internal function that adds an addition to already existing
// USFM text.
{
  // Special handling for adding an end-of-line.
  if (addition == "\n") {
    // No end-of-line is to be added at the beginning of the text.
    if (text.empty())
      return;
    // Ensure that no white-space exists before the end-of-line.
    text = trim(text);
  }
  // Add text.
  text.append(addition);
}


void usfm_internal_get_text_close_character_style(ustring & text, const ustring & project, const ustring & style)
// Adds the USFM code for a character style that closes.
{
  // Get the type and the subtype.
  StyleType type;
  int subtype;
  marker_get_type_and_subtype(project, style, type, subtype);
  // A verse number, normally the \v, does not have a closing marker.
  if (!style_get_starts_verse_number(type, subtype)) {
    usfm_internal_add_text(text, usfm_get_full_closing_marker(style));
  }
}


ustring usfm_get_note_text(GtkTextIter startiter, GtkTextIter enditer, const ustring & project)
{
  // Variable to hold the note text.
  ustring notetext;

  // Initialize the iterator.
  GtkTextIter iter = startiter;

  // Paragraph and character styles.
  ustring previous_paragraph_style;
  ustring previous_character_style;
  bool paragraph_initialized = false;

  // Iterate through the text.  
  while (gtk_text_iter_compare(&iter, &enditer) < 0) {

    // Get the new paragraph and character style.
    // This is done by getting the names of the styles at this iterator.
    // With the way the styles are applied currently, the first 
    // style is a paragraph style, and the second style is optional 
    // and would be a character style.
    ustring new_paragraph_style;
    ustring new_character_style;
    get_styles_at_iterator(iter, new_paragraph_style, new_character_style);

    // Get the text at the iterator, and whether this is a linebreak.
    ustring new_character;
    bool line_break;
    {
      gunichar unichar = gtk_text_iter_get_char(&iter);
      gchar buf[7];
      gint length = g_unichar_to_utf8(unichar, (gchar *) & buf);
      buf[length] = '\0';
      new_character = buf;
      line_break = (new_character.find_first_of("\n\r") == 0);
      if (line_break) {
        new_character.clear();
        previous_paragraph_style.clear();
      }
    }

    /*
       How to get the usfm code of a note.
       Generally speaking, this is the way to do it:
       If a character marker of the type "note content" appears or changes,
       then that is the code to be inserted as an opening marker.
       At the moment that such a character marker disappears,
       then insert the then prevailing paragraph marker.
       If a character marker of type "note content with endmarker" appears,
       then insert the opening code of it, 
       and if such a marker disappears, insert the closing code.
     */

    bool note_content_opening = false;
    bool note_content_closing = false;
    bool note_content_with_endmarker_opening = false;
    bool note_content_with_endmarker_closing = false;
    if (new_character_style != previous_character_style) {
      StyleType type;
      int subtype;
      if (new_character_style.empty()) {
        marker_get_type_and_subtype(project, previous_character_style, type, subtype);
        if (style_get_starts_note_content(type, subtype)) {
          note_content_closing = true;
        }
        if (style_get_starts_character_style(type, subtype)) {
          note_content_with_endmarker_closing = true;
        }
      } else {
        marker_get_type_and_subtype(project, new_character_style, type, subtype);
        if (style_get_starts_note_content(type, subtype)) {
          note_content_opening = true;
        }
        if (style_get_starts_character_style(type, subtype)) {
          note_content_with_endmarker_opening = true;
        }
      }
    }

    if (new_paragraph_style != previous_paragraph_style) {
      if (paragraph_initialized) {
        if (!new_paragraph_style.empty()) {
          usfm_internal_add_text(notetext, usfm_get_full_opening_marker(new_paragraph_style));
        }
      }
    }
    // Determine the usfm code to be inserted.
    ustring usfm_code;
    if (note_content_opening || note_content_with_endmarker_opening) {
      usfm_code = usfm_get_full_opening_marker(new_character_style);
    } else if (note_content_closing) {
      usfm_code = usfm_get_full_opening_marker(new_paragraph_style);
    } else if (note_content_with_endmarker_closing) {
      usfm_code = usfm_get_full_closing_marker(previous_character_style);
    }
    // Store all styles and flags for next iteration.
    previous_paragraph_style = new_paragraph_style;
    previous_character_style = new_character_style;
    paragraph_initialized = true;

    /*
       Store the possible code and the character.
       We have had cases that such code was produced:
       \f + \fr v1:\ft  one \fdc two\fdc* three.\f*
       This was coming from v1: one two three.
       But the fr style was only applied to "v1:", thus giving a space after the \ft marker.
       The "\ft " marker itself already has a space. 
       We then find that case that one space follows the other.
       Two consecusitive spaces in USFM count as only one space, hence this space
       was removed. And that again results in this faulty text:
       "v1:one two three".
       A special routine solves that, putting the space before the "\ft " marker.
     */
    bool swap_code_and_text = false;
    if (paragraph_initialized) {
      if (new_character == " ") {
        if (trim(usfm_code) != usfm_code) {
          swap_code_and_text = true;
        }
      }
    }
    if (swap_code_and_text) {
      usfm_internal_add_text(notetext, new_character);
      usfm_internal_add_text(notetext, usfm_code);
    } else {
      usfm_internal_add_text(notetext, usfm_code);
      usfm_internal_add_text(notetext, new_character);
    }

    // Next iteration.
    gtk_text_iter_forward_char(&iter);
  }

  // Return what it got.
  return notetext;
}


void get_styles_at_iterator(GtkTextIter iter, ustring & paragraph_style, ustring & character_style)
{
  // Get the applicable styles.
  // This is done by getting the names of the styles at the iterator.
  // The first named tag is the paragraph style, 
  // and the second named one is the character style.
  paragraph_style.clear();
  character_style.clear();
  GSList *tags = NULL, *tagp = NULL;
  tags = gtk_text_iter_get_tags(&iter);
  for (tagp = tags; tagp != NULL; tagp = tagp->next) {
    GtkTextTag *tag = (GtkTextTag *) tagp->data;
    gchar *strval;
    g_object_get(G_OBJECT(tag), "name", &strval, NULL);
    if (strval) {
      if (strlen(strval)) {
        // Skip the tag for a misspelled word.
        if (strcmp(strval, spelling_tag_name()) != 0) {
          // First store the paragraph style, then the character style.
          // This works because the editing code takes care when applying the tags,
          // to first apply the paragraph style, and then the character style.
          if (paragraph_style.empty()) {
            paragraph_style = strval;
          } else {
            character_style = strval;
          }
        }
      }
      g_free(strval);
    }
  }
  if (tags) {
    g_slist_free(tags);
  }
}


vector <ustring> get_character_styles_between_iterators (GtkTextIter startiter, GtkTextIter enditer)
// Gets the character styles between two iterators given.
// To do this properly, it is assumed that the first style encountered will always be the paragraph style,
// and the second the character style.
{
  vector <ustring> styles;
  if (!gtk_text_iter_equal (&startiter, &enditer)) {
    GtkTextIter iter = startiter;
    do {
      ustring paragraphstyle, characterstyle;
      get_styles_at_iterator(iter, paragraphstyle, characterstyle);
      styles.push_back (characterstyle);
      gtk_text_iter_forward_char(&iter);
    } while (gtk_text_iter_in_range(&iter, &startiter, &enditer));
  }
  return styles;
}


bool get_verse_number_at_iterator_internal (GtkTextIter iter, const ustring & verse_marker, ustring& verse_number)
// This function looks at the iterator for the verse number.
// If a verse number is not found, it iterates back till one is found.
// If the iterator can't go back any further, and no verse number was found, it returns false.
// If a verse number is found, it returns true and stores it in parameter "verse_number".
{
  // Look for the v style while iterating backward.
  bool verse_style_found = false;
  do {
    ustring paragraph_style, character_style;
    get_styles_at_iterator(iter, paragraph_style, character_style);
    if (character_style == verse_marker) {
      verse_style_found = true;
    }
  } while (!verse_style_found && gtk_text_iter_backward_char(&iter));
  // If the verse style is not in this textbuffer, bail out.
  if (!verse_style_found) {
    return false;
  }
  // The verse number may consist of more than one character.
  // Therefore iterate back to the start of the verse style.
  // For convenience, it iterates back to the start of any style.
  while (!gtk_text_iter_begins_tag (&iter, NULL)) {
    gtk_text_iter_backward_char (&iter);
  }
  // Extract the verse number.
  GtkTextIter enditer = iter;
  gtk_text_iter_forward_chars(&enditer, 10);
  verse_number = gtk_text_iter_get_slice(&iter, &enditer);
  size_t position = verse_number.find(" ");
  position = CLAMP(position, 0, verse_number.length());
  verse_number.erase (position, 10);
  // Indicate that a verse number was found.
  return true;
}


ustring get_verse_number_at_iterator(GtkTextIter iter, const ustring & verse_marker, const ustring & project, GtkWidget * parent_box)
/* 
This function returns the verse number at the iterator.
It also takes into account a situation where the cursor is on a heading.
The user expects a heading to belong to the next verse. 
*/
{
  // Get the paragraph style at the iterator, in case it is in a heading.
  ustring paragraph_style_at_cursor;
  {
    ustring dummy;
    get_styles_at_iterator(iter, paragraph_style_at_cursor, dummy);
  }
  
  // Verse-related variables.
  ustring verse_number = "0";
  bool verse_number_found = false;
  GtkWidget * textview = NULL;

  do {
    // Try to find a verse number in the GtkTextBuffer the "iter" points to.
    verse_number_found = get_verse_number_at_iterator_internal (iter, verse_marker, verse_number);
    // If the verse number was not found, look through the previous GtkTextBuffer.
    if (!verse_number_found) {
      // If the "textview" is not yet set, look for the current one.
      if (textview == NULL) {
        GtkTextBuffer * textbuffer = gtk_text_iter_get_buffer (&iter);
        vector <GtkWidget *> widgets = editor_get_widgets (parent_box);
        for (unsigned int i = 0; i < widgets.size(); i++) {
          if (textbuffer == gtk_text_view_get_buffer (GTK_TEXT_VIEW (widgets[i]))) {
            textview = widgets[i];
            break;
          }
        }
      }
      // Look for the previous GtkTextView.
      textview = editor_get_previous_textview (parent_box, textview);
      // Start looking at the end of that textview.
      if (textview) {
        GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
        gtk_text_buffer_get_end_iter (textbuffer, &iter);
      }
    }
  } while (!verse_number_found && textview);

  // Optional: If the cursor is on a title/heading, increase verse number.
  if (!project.empty()) {
    StyleType type;
    int subtype;
    marker_get_type_and_subtype(project, paragraph_style_at_cursor, type, subtype);
    if (type == stStartsParagraph) {
      switch (subtype) {
        case ptMainTitle:
        case ptSubTitle:
        case ptSectionHeading:
        {
          unsigned int vs = convert_to_int (verse_number);
          vs++;
          verse_number = convert_to_string (vs);
          break;
        }
        case ptNormalParagraph:
        {
          break;
        }
      }
    }
  }

  // Return the verse number found.
  return verse_number;
}


bool get_iterator_at_verse_number (const ustring& verse_number, const ustring& verse_marker, GtkWidget * parent_box, GtkTextIter & iter, GtkWidget *& textview, bool deep_search)
// This returns the iterator and textview where "verse_number" starts.
// Returns true if the verse was found, else false.
{
  // Go through all textviews.
  vector <GtkWidget *> textviews = editor_get_widgets (parent_box);
  for (unsigned int i = 0; i < textviews.size(); i++) {
    // Handle this textview.
    textview = textviews[i];
    // Search from start of buffer.
    GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
    gtk_text_buffer_get_start_iter (textbuffer, &iter);
    // Verse 0 or empty: beginning of chapter.
    if ((verse_number == "0") || (verse_number.empty())) {
      return true;
    }
    // Go through the buffer and find out about the verse.
    do {
      ustring paragraph_style, character_style, verse_at_iter;
      get_styles_at_iterator(iter, paragraph_style, character_style);
      if (character_style == verse_marker) {
        verse_at_iter = get_verse_number_at_iterator(iter, verse_marker, "", parent_box);
        if (verse_number == verse_at_iter) {
          return true;
        }
      }
      // Optionally do a deep search: whether the verse is in a sequence or range of verses.
      if (deep_search && !verse_at_iter.empty()) {
        unsigned int verse_int = convert_to_int(verse_at_iter);
        vector <unsigned int> combined_verses = verse_range_sequence(verse_number);
        for (unsigned int i2 = 0; i2 < combined_verses.size(); i2++) {
          if (verse_int == combined_verses[i2]) {
            return true;
          }
        }
      }
    } while (gtk_text_iter_forward_char(&iter));
  }
  // If we haven't done the deep search yet, do it now.
  if (!deep_search) {
    if (get_iterator_at_verse_number (verse_number, verse_marker, parent_box, iter, textview, true)) {
      return true;
    }
  }
  // Verse was not found.
  return false;
}


const gchar *unknown_style()
// Gives the name of the style that is for markers that are not in the stylesheet.
{
  return "unknown";
}


const gchar * note_starting_style ()
// Gives the name of the style that a note caller in the text starts with.
{
  return "note";
}


const gchar * usfm_clipboard_code ()
// Gives the clipboard text that signifies that no text was copied, but USFM code only.
{
  return "<USFM>";
}


void get_next_note_caller_and_style (EditorNoteType type, ustring& caller, ustring& style, bool restart)
// Gets the next note caller style.
// Since note callers have sequential numbers, it needs another one for each note.
{
  static unsigned int last_offset = 0;
  if (restart) {
    last_offset = 0;
    return;
  }
  switch (type) {
    case entFootnote:       caller = "f"; break;
    case entEndnote:        caller = "e"; break;
    case entCrossreference: caller = "x"; break;
  }
  last_offset++;
  style = note_starting_style ();
  style.append (convert_to_string (last_offset));
}


void textbuffer_apply_named_tag(GtkTextBuffer * buffer, const ustring & name, const GtkTextIter * start, const GtkTextIter * end)
// Applies the tag on the textbuffer, if the tag exists.
// Else applies the "unknown" style.
{
  GtkTextTagTable *table = gtk_text_buffer_get_tag_table(buffer);
  GtkTextTag *tag = gtk_text_tag_table_lookup(table, name.c_str());
  if (tag)
    gtk_text_buffer_apply_tag_by_name(buffer, name.c_str(), start, end);
  else
    gtk_text_buffer_apply_tag_by_name(buffer, unknown_style(), start, end);
}


void textbuffer_insert_with_named_tags(GtkTextBuffer * buffer, GtkTextIter * iter, const ustring & text, ustring first_tag_name, ustring second_tag_name)
// Inserts text into the buffer applying one or two named tags at the same time.
// If a tag does not exist, it applies the "unknown" style instead.
{
  GtkTextTagTable *table = gtk_text_buffer_get_tag_table(buffer);
  GtkTextTag *tag = gtk_text_tag_table_lookup(table, first_tag_name.c_str());
  if (!tag)
    first_tag_name = unknown_style();
  if (!second_tag_name.empty()) {
    tag = gtk_text_tag_table_lookup(table, second_tag_name.c_str());
    if (!tag)
      second_tag_name = unknown_style();
  }
  if (second_tag_name.empty()) {
    gtk_text_buffer_insert_with_tags_by_name(buffer, iter, text.c_str(), -1, first_tag_name.c_str(), NULL);
  } else {
    gtk_text_buffer_insert_with_tags_by_name(buffer, iter, text.c_str(), -1, first_tag_name.c_str(), second_tag_name.c_str(), NULL);
  }
}


void clear_and_destroy_editor_actions (deque <EditorAction *>& actions)
{
  for (unsigned int i = 0; i < actions.size(); i++) {
    EditorAction * action = actions[i];
    delete action;
  }
  actions.clear();
}


void on_container_tree_callback_destroy (GtkWidget *widget, gpointer user_data)
{
  gtk_widget_destroy (widget);
}


void editor_text_append(GtkTextBuffer * textbuffer, const ustring & text, const ustring & paragraph_style, const ustring & character_style)
// This function appends text to the textbuffer.
// It inserts the text at the cursor.
{
  // Get the iterator at the text insertion point.
  GtkTextIter insertiter;
  gtk_text_buffer_get_iter_at_mark(textbuffer, &insertiter, gtk_text_buffer_get_insert(textbuffer));
  // Insert text together with the style(s).
  textbuffer_insert_with_named_tags(textbuffer, &insertiter, text, paragraph_style, character_style);
}


gint editor_paragraph_insertion_point_get_offset (EditorActionCreateParagraph * paragraph_action)
{
  gint offset = 0;
  if (paragraph_action) {
    GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (paragraph_action->textview));
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(textbuffer, &iter, gtk_text_buffer_get_insert(textbuffer));
    offset = gtk_text_iter_get_offset (&iter);
  }
  return offset;
}


void editor_paragraph_insertion_point_set_offset (EditorActionCreateParagraph * paragraph_action, gint offset)
{
  if (paragraph_action) {
    GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (paragraph_action->textview));
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_offset (textbuffer, &iter, offset);
    gtk_text_buffer_place_cursor (textbuffer, &iter);
  }
}


EditorActionDeleteText * paragraph_delete_last_character_if_space(EditorActionCreateParagraph * paragraph_action)
// Creates an action for deleting text for the last character in the text buffer if it is a space.
{
  if (paragraph_action) {
    GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (paragraph_action->textview));
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter (textbuffer, &iter);
    bool text_available = gtk_text_iter_backward_char(&iter);
    if (text_available) {
      gunichar last_character = gtk_text_iter_get_char(&iter);
      if (g_unichar_isspace(last_character)) {
        gint offset = gtk_text_iter_get_offset (&iter);
        return new EditorActionDeleteText (paragraph_action, offset, 1);
      }
    }
  }
  return NULL;
}


bool text_starts_paragraph (const ustring& project, ustring& line, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype(project, marker, type, subtype);
        if (style_get_starts_new_line_in_editor(type, subtype)) {
          line.erase(0, marker_length);
          return true;
        }
      }
    }
  }
  return false;
}


bool text_starts_verse (const ustring& project, ustring& line, const ustring& marker_text, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype(project, marker_text, type, subtype);
        if (style_get_starts_verse_number(type, subtype)) {
          line.erase (0, marker_length);
          return true;
        }
      }
    }
  }
  return false;  
}


void on_editor_get_widgets_callback (GtkWidget *widget, gpointer user_data)
{
  vector <GtkWidget *> * widgets = static_cast < vector <GtkWidget *> * > (user_data);
  widgets->push_back (widget);
}


vector <GtkWidget *> editor_get_widgets (GtkWidget * vbox)
{
  vector <GtkWidget *> widgets;
  gtk_container_foreach(GTK_CONTAINER(vbox), on_editor_get_widgets_callback, gpointer(&widgets));
  return widgets;  
}


GtkWidget * editor_get_next_textview (GtkWidget * vbox, GtkWidget * textview)
{
  vector <GtkWidget *> widgets = editor_get_widgets (vbox);
  for (unsigned int i = 0; i < widgets.size(); i++) {
    if (textview == widgets[i])
      if (i < widgets.size() - 1)
        return widgets[i+1];
  }
  return NULL;
}


GtkWidget * editor_get_previous_textview (GtkWidget * vbox, GtkWidget * textview)
// Gets the textview that precedes the "current" one in the Editor object.
{
  vector <GtkWidget *> widgets = editor_get_widgets (vbox);
  for (unsigned int i = 0; i < widgets.size(); i++) {
    if (textview == widgets[i])
      if (i)
        return widgets[i-1];
  }
  return NULL;
}


EditorActionDeleteText * paragraph_get_text_and_styles_after_insertion_point(EditorActionCreateParagraph * paragraph, vector <ustring>& text, vector <ustring>& styles)
// This function accepts a paragraph, and gives a list of text and their associated character styles
// from the insertion point to the end of the buffer.
// It returns the EditorAction that would be required to erase that text from the paragraph.
// Note that this EditorAction needs to be applied for the effect to be obtained.
// If it is not applied, it should then be destroyed.
{
  GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (paragraph->textview));
  gint start_offset = editor_paragraph_insertion_point_get_offset (paragraph);
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_iter_at_offset (textbuffer, &startiter, start_offset);
  gtk_text_buffer_get_end_iter (textbuffer, &enditer);
  gint end_offset = gtk_text_iter_get_offset (&enditer);
  get_text_and_styles_between_iterators(&startiter, &enditer, text, styles);
  EditorActionDeleteText * delete_action = NULL;
  if (end_offset > start_offset) {
    delete_action = new EditorActionDeleteText(paragraph, start_offset, end_offset - start_offset);
  }
  return delete_action;
}


void get_text_and_styles_between_iterators(GtkTextIter * startiter, GtkTextIter * enditer, vector <ustring>& text, vector <ustring>& styles)
// This function gives a list of the text and character styles between two iterators.
// The "text" variable contains a chunks of text with the same style.
{
  text.clear();
  styles.clear();
  ustring previous_style;
  ustring accumulated_text;
  GtkTextIter iter = *startiter;
  do {
    ustring paragraph_style, character_style;
    get_styles_at_iterator(iter, paragraph_style, character_style);
    GtkTextIter iter2 = iter;
    gtk_text_iter_forward_char (&iter2);
    ustring character = gtk_text_iter_get_text(&iter, &iter2);
    if (character_style != previous_style) {
      if (!accumulated_text.empty()) {
        text.push_back (accumulated_text);
        styles.push_back (previous_style);
        accumulated_text.clear();
      }
    }
    previous_style = character_style;    
    accumulated_text.append (character);
    gtk_text_iter_forward_char(&iter);
  } while (gtk_text_iter_in_range(&iter, startiter, enditer));
  if (!accumulated_text.empty()) {
    text.push_back (accumulated_text);
    styles.push_back (previous_style);
  }
}


void editor_park_widget (GtkWidget * vbox, GtkWidget * widget, gint& offset, GtkWidget * parking)
// Do the administration of parking a widget.
{
  // Look for the widget's offset within its parent.
  vector <GtkWidget *> widgets = editor_get_widgets (vbox);
  for (unsigned int i = 0; i < widgets.size(); i++) {
    if (widget == widgets[i]) {
      offset = i;
    }
  }
  // Transfer the widget to the parking lot. It is kept alive.
  gtk_widget_reparent (widget, parking);
}


bool iterator_includes_note_caller (GtkTextIter iter)
// Check whether the iter points right after a note caller.
{
  if (!gtk_text_iter_backward_char (&iter))
    return false;
  ustring paragraph_style, character_style;
  get_styles_at_iterator(iter, paragraph_style, character_style);
  if (character_style.find (note_starting_style ()) == string::npos)
    return false;
  return true;
}


bool move_end_iterator_before_note_caller_and_validate (GtkTextIter startiter, GtkTextIter enditer, GtkTextIter & moved_enditer)
// If the iterator is after a note caller, it moves the iterator till it is before the note caller.
// Return true if the two iterators contain some text.
{
  // Initialize the iterator that migth be moved.
  moved_enditer = enditer;
  // Finite loop prevention.
  unsigned int finite_loop = 0;
  // Keep moving the iterator for as long as it contains a note caller.
  while (iterator_includes_note_caller (moved_enditer) && finite_loop < 10) {
    gtk_text_iter_backward_char (&moved_enditer);
    finite_loop++;
  }
  // Check whether the end iterator is bigger than the start iterator.
  return (gtk_text_iter_compare (&moved_enditer, &startiter) > 0);
}


