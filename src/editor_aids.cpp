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


EditorNote::EditorNote(int dummy)
// This object is for storing data related to the different bits of a Bible note.
// By storing them all in one object, the relationship between the different
// bits can be seen easily.
{
  textview_allocated_height = 0;
  label_caller_note_allocated_width = 0;
  textview = NULL;
}


EditorNote::~EditorNote()
{
}


EditorTable::EditorTable(int dummy)
// This object is for storing data related to the different bits of a table.
// The data is stored row by row.
{
}


EditorTable::~EditorTable()
{
}


EditorSnapshot::EditorSnapshot(int dummy)
// This object stores a snapshot of the editor.
{
  insert = 0;
  scroll = 0;
}


EditorSnapshot::~EditorSnapshot()
{
}


PreventEditorUndo::PreventEditorUndo(int *flag)
/*
 Preventing recording of undo-able actions.
 This approach has been chosen so that, if a function creates this object to
 prevent undo-able action from being recorded, there is no need to enable
 these actions again in the code. As soon as this object goes out of scope,
 it enables the recording again. 
 Embedded calls to this object are allowed.
 */
{
  flagpointer = flag;
  (*flagpointer)--;
}


PreventEditorUndo::~PreventEditorUndo()
{
  (*flagpointer)++;
}


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
      plaintext = false;
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
      display_marker = false;
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


void textbuffer_erase_character_before_text_insertion_point_if_space(GtkTextBuffer * textbuffer)
// Erases the character before the text insertion point if that is a space.
{
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_mark(textbuffer, &iter, gtk_text_buffer_get_insert(textbuffer));
  bool text_available = gtk_text_iter_backward_char(&iter);
  if (!text_available)
    return;
  gunichar last_character = gtk_text_iter_get_char(&iter);
  if (!g_unichar_isspace(last_character))
    return;
  GtkTextIter iter2 = iter;
  gtk_text_iter_forward_char(&iter2);
  gtk_text_buffer_delete(textbuffer, &iter, &iter2);
}


void textbuffer_erase_white_space_at_end(GtkTextBuffer * textbuffer)
// Erases the character at the end of the textbuffer if that is white space.
{
  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(textbuffer, &iter);
  bool text_available = gtk_text_iter_backward_char(&iter);
  if (!text_available)
    return;
  gunichar last_character = gtk_text_iter_get_char(&iter);
  if (!g_unichar_isspace(last_character))
    return;
  GtkTextIter iter2 = iter;
  gtk_text_iter_forward_char(&iter2);
  gtk_text_buffer_delete(textbuffer, &iter, &iter2);
}


void textbuffer_erase_child_anchor(GtkTextBuffer * textbuffer, GtkTextChildAnchor * anchor)
{
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_child_anchor(textbuffer, &iter, anchor);
  GtkTextIter iter2 = iter;
  gtk_text_iter_forward_char(&iter2);
  gtk_text_buffer_delete(textbuffer, &iter, &iter2);
}


bool textbuffer_empty(GtkTextBuffer * textbuffer)
{
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_start_iter(textbuffer, &startiter);
  gtk_text_buffer_get_end_iter(textbuffer, &enditer);
  return gtk_text_iter_equal(&startiter, &enditer);
}


void textbuffers_set_unmodified(GtkTextBuffer * mainbuffer, vector < EditorNote > &editornotes, vector < EditorTable > &editortables)
// This clears the "modified" flag of the main textbuffer and all the 
// embedded ones,
{
  gtk_text_buffer_set_modified(mainbuffer, false);
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    gtk_text_buffer_set_modified(editornotes[i].textbuffer, false);
  }
  for (unsigned int i = 0; i < editortables.size(); i++) {
    for (unsigned int row = 0; row < editortables[i].textbuffers.size(); row++) {
      for (unsigned int column = 0; column < editortables[i].textbuffers[row].size(); column++) {
        GtkTextBuffer *textbuffer = table_cell_get_buffer(editortables[i], row, column);
        gtk_text_buffer_set_modified(textbuffer, false);
      }
    }
  }
}

bool textbuffers_get_modified(GtkTextBuffer * mainbuffer, vector < EditorNote > &editornotes, vector < EditorTable > &editortables)
// This returns true if any if the textbuffers is modified.
{
  if (gtk_text_buffer_get_modified(mainbuffer))
    return true;
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    if (gtk_text_buffer_get_modified(editornotes[i].textbuffer))
      return true;
  }
  for (unsigned int i = 0; i < editortables.size(); i++) {
    for (unsigned int row = 0; row < editortables[i].textbuffers.size(); row++) {
      for (unsigned int column = 0; column < editortables[i].textbuffers[row].size(); column++) {
        GtkTextBuffer *textbuffer = table_cell_get_buffer(editortables[i], row, column);
        if (gtk_text_buffer_get_modified(textbuffer))
          return true;
      }
    }
  }
  return false;
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

GtkTextBuffer *table_cell_get_buffer(const EditorTable & editortable, gint row, gint column)
{
  GtkTextBuffer *textbuffer = NULL;
  if (row < (gint) editortable.textbuffers.size())
    if (column < (gint) editortable.textbuffers[row].size())
      textbuffer = editortable.textbuffers[row][column];
  return textbuffer;
}

GtkWidget *table_cell_get_view(const EditorTable & editortable, gint row, gint column)
{
  GtkWidget *textview = NULL;
  if (row < (gint) editortable.textviews.size())
    if (column < (gint) editortable.textviews[row].size())
      textview = editortable.textviews[row][column];
  return textview;
}

void table_resize(EditorTable & editortable, GtkTextTagTable * texttagtable, gint n_rows, gint n_columns, bool editable)
/*
 This function supposes that there is an already existing table. 
 It may have zero or more columns and/or rows.
 Information regarding this table is in variable "editortable".
 It resizes the table so as to include the required number of rows and columns.
 The "texttagtable" is an existing table with the tags that are needed.
 */
{
  // Get table pointer.
  GtkTable *table = GTK_TABLE(editortable.table);

  // Get current rows and columns.
  gint current_n_rows = table_get_n_rows(table) / 3;
  gint current_n_columns = table_get_n_columns(table) / 3;

  // Resize the table.
  gtk_table_resize(table, n_rows * 3, n_columns * 3);

  // Add the required amount of rows and columns and store them in the EditorTable object.
  for (gint row = current_n_rows; row < n_rows; row++) {
    vector < GtkWidget * >widgets;
    editortable.textviews.push_back(widgets);
    vector < GtkTextBuffer * >buffers;
    editortable.textbuffers.push_back(buffers);
    for (gint column = current_n_columns; column < n_columns; column++) {
      GtkWidget *textview;
      GtkTextBuffer *textbuffer;
      table_create_cell(table, texttagtable, textview, textbuffer, row, column, editable);
      editortable.textviews[row].push_back(textview);
      editortable.textbuffers[row].push_back(textbuffer);
    }
  }
}

void table_create_cell(GtkTable * table, GtkTextTagTable * texttagtable, GtkWidget * &textview, GtkTextBuffer * &textbuffer, gint row, gint column, bool editable)
/*
 This prepares one cell in the table. It assumes that the table has enough space
 for this cell. It writes the borders, and writes the GtkTextView and the 
 GtkTextBuffer to the EditorTable object.
 Each data cell of the table if surrounded by lines on all four sides.
 Translated into Gtk, this means that each GtkTextView is being surrounded
 by one GtkHSeparator above it, and one below it, and one GtkVSeparator
 at the left and one at the right. With the proper spacing these separators
 give the impression of table borders.
 */
{
  // Get the zero-point, the point in the table from where to start building
  // the widgets.
  gint zero_row = 3 * row;
  gint zero_column = 3 * column;

  // Write the four borders around the cell. 
  // These consist of two times a GtkHSeparator and two times a GtkVSeparator.
  GtkWidget *hseparator1;
  GtkWidget *vseparator1;
  GtkWidget *hseparator2;
  GtkWidget *vseparator2;

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_table_attach(GTK_TABLE(table), hseparator1, zero_column + 1, zero_column + 2, zero_row + 0, zero_row + 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_FILL), 0, 0);

  vseparator1 = gtk_vseparator_new();
  gtk_widget_show(vseparator1);
  gtk_table_attach(GTK_TABLE(table), vseparator1, zero_column + 0, zero_column + 1, zero_row + 1, zero_row + 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_FILL), 0, 0);

  hseparator2 = gtk_hseparator_new();
  gtk_widget_show(hseparator2);
  gtk_table_attach(GTK_TABLE(table), hseparator2, zero_column + 1, zero_column + 2, zero_row + 2, zero_row + 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_FILL), 0, 0);

  vseparator2 = gtk_vseparator_new();
  gtk_widget_show(vseparator2);
  gtk_table_attach(GTK_TABLE(table), vseparator2, zero_column + 2, zero_column + 3, zero_row + 1, zero_row + 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_FILL), 0, 0);

  textbuffer = gtk_text_buffer_new(texttagtable);

  textview = gtk_text_view_new_with_buffer(textbuffer);
  gtk_widget_show(textview);
  gtk_table_attach(GTK_TABLE(table), textview, zero_column + 1, zero_column + 2, zero_row + 1, zero_row + 2, (GtkAttachOptions) (0), (GtkAttachOptions) (0), 0, 0);
  gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(textview), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD);
}


void usfm_get_text(GtkTextBuffer * textbuffer, GtkTextIter startiter, GtkTextIter enditer, vector < EditorNote > *editornotes, vector < EditorTable > *editortables, const ustring & project, ustring & text, bool verse_restarts_paragraph)
/*
 Gets the USFM text from the main textbuffer between the two offsets.
 Calls routines so as to include the text of notes and tables.
 */
{
  // Initialize the iterator.
  GtkTextIter iter = startiter;

  // Paragraph and character styles.
  ustring previous_paragraph_style;
  ustring previous_character_style;

  // Store any note text.
  ustring note_text;

  // Iterate through the text.
  unsigned int iterations = 0;
  while (gtk_text_iter_compare(&iter, &enditer) < 0) {

    // Get the new paragraph and character style.
    // This is done by getting the names of the styles at this iterator.
    // With the way the styles are applied currently, the first 
    // style is a paragraph style, and the second style is optional 
    // and would be a character style.
    ustring new_paragraph_style;
    ustring new_character_style;
    get_styles_at_iterator(iter, new_paragraph_style, new_character_style);

    // Omit the starting paragraph marker except when at the start of a line.
    if (iterations == 0) {
      if (!gtk_text_iter_starts_line(&iter)) {
        previous_paragraph_style = new_paragraph_style;
      }
    }

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
      if (line_break)
        new_character.clear();
    }

    // Flags for whether styles are opening or closing.
    bool character_style_closing = false;
    bool paragraph_style_closing = false;
    bool paragraph_style_opening = false;
    bool character_style_opening = false;

    // Look whether there is a child anchor here.
    GtkTextChildAnchor *childanchor = gtk_text_iter_get_child_anchor(&iter);
    if (childanchor) {

      // See if this child anchor is a note.
      if (editornotes) {
        for (unsigned int i = 0; i < editornotes->size(); i++) {
          EditorNote editornote = editornotes->at(i);
          if (childanchor == editornote.childanchor_caller_text) {
            // A note should not have a character style. 
            // This fixes a few bugs related to character styles applied to a note.
            new_character_style.clear();
            // Extract note text.
            GtkTextIter startiter, enditer;
            gtk_text_buffer_get_start_iter(editornote.textbuffer, &startiter);
            gtk_text_buffer_get_end_iter(editornote.textbuffer, &enditer);
            usfm_get_note_text(editornote, startiter, enditer, project, note_text);
          }
        }
      }
      // See if this child anchor is a table.
      if (editortables) {
        for (unsigned int i = 0; i < editortables->size(); i++) {
          EditorTable editortable = editortables->at(i);
          if (childanchor == editortable.childanchor) {
            usfm_get_table_text(editortable, project, text);
          }
        }
      }
      // The textbuffer would have the character for the unknown unicode character. 
      // This must be discarded, so that no actual text is going to be stored.
      new_character.clear();

    } else {

      // Proceed with the case that there is no child anchor.
      // This means that we have normal text here.

      // Paragraph style closing.
      if (new_paragraph_style != previous_paragraph_style) {
        if (!previous_paragraph_style.empty()) {
          paragraph_style_closing = true;
        }
      }
      // If a new line is encountered, then the paragraph closes.
      if (line_break)
        paragraph_style_closing = true;
      // If the paragraph closes, then the character style, if open, should close too.
      if (paragraph_style_closing) {
        new_character_style.clear();
      }
      // Character style closing. 
      if (new_character_style != previous_character_style)
        if (!previous_character_style.empty())
          character_style_closing = true;
      // Paragraph style opening.
      if (new_paragraph_style != previous_paragraph_style)
        if (!new_paragraph_style.empty())
          paragraph_style_opening = true;
      // Character style opening.
      if (new_character_style != previous_character_style)
        if (!new_character_style.empty())
          character_style_opening = true;

      // Handle possible character style closing.
      if (character_style_closing) {
        usfm_internal_get_text_close_character_style(text, project, previous_character_style);
      }
      // USFM doesn't need anything if a paragraph style is closing.
      if (paragraph_style_closing) {
      }
      // Handle possible paragraph style opening.
      if (paragraph_style_opening) {
        usfm_internal_add_text(text, "\n");
        // We would need to add the USFM code to the text.
        // But in some cases the code is already in the text,
        // e.g. in the case of "\id JHN".
        // In such cases the code is fine already, so it does not need to be added anymore.
        // Accomodate cases such as \toc
        // These don't have the full marker as "\toc ", but only without the last space.
        ustring usfm_code = usfm_get_full_opening_marker(new_paragraph_style);
        GtkTextIter iter2 = iter;
        gtk_text_iter_forward_chars(&iter2, usfm_code.length());
        ustring usfm_code_in_text = gtk_text_iter_get_slice(&iter, &iter2);
        replace_text(usfm_code_in_text, "\n", " ");
        if (usfm_code_in_text.length() < usfm_code.length())
          usfm_code_in_text.append(" ");
        if (usfm_code != usfm_code_in_text) {
          // A space after an opening marker gets erased in USFM: move it forward.
          if (new_character == " ") {
            usfm_internal_add_text(text, new_character);
            new_character.clear();
          }
          // Don't insert the unknown style
          if (new_paragraph_style != unknown_style())
            usfm_internal_add_text(text, usfm_code);
        }
      }
      // Handle possible character style opening.
      if (character_style_opening) {
        // Get the type and the subtype.
        StyleType type;
        int subtype;
        marker_get_type_and_subtype(project, new_character_style, type, subtype);
        // Normally a character style does not start a new line, but a verse (\v) does.
        if (style_get_starts_new_line_in_usfm(type, subtype)) {
          usfm_internal_add_text(text, "\n");
        }
        // A space after an opening marker gets erased in USFM: move it forward.
        if (new_character == " ") {
          usfm_internal_add_text(text, new_character);
          new_character.clear();
        }
        usfm_internal_add_text(text, usfm_get_full_opening_marker(new_character_style));
      }
      // Store all styles for next iteration.
      previous_paragraph_style = new_paragraph_style;
      previous_character_style = new_character_style;
      if (paragraph_style_closing)
        previous_paragraph_style.clear();
      if (character_style_closing)
        previous_character_style.clear();

      // Store any note text that we may have collected.
      text.append (note_text);
      note_text.clear();

      // Store this character.
      usfm_internal_add_text(text, new_character);

    }

    // Next iteration.
    gtk_text_iter_forward_char(&iter);
    iterations++;
  }

  // If a character style has been applied to the last character or word 
  // in the buffer, the above code would not add the closing marker.
  // Thus we may be found to have text like \p New paragraph with \add italics
  // The \add* marker is missing. This violates the USFM standard.
  // The code below fixes that.
  if (!previous_character_style.empty()) {
    usfm_internal_get_text_close_character_style(text, project, previous_character_style);
  }
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

void usfm_get_note_text(const EditorNote & editornote, GtkTextIter startiter, GtkTextIter enditer, const ustring & project, ustring & text)
{
  // Add the note opener.
  usfm_internal_add_text(text, usfm_get_full_opening_marker(editornote.marker));

  // Add the caller.
  usfm_internal_add_text(text, editornote.caller + " ");

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

  // Add the note bit to the main text.
  usfm_internal_add_text(text, notetext);

  // Add the note closer.
  usfm_internal_add_text(text, usfm_get_full_closing_marker(editornote.marker));
}

void usfm_get_table_text(const EditorTable & editortable, const ustring & project, ustring & text)
{
  // Go through all the rows of this table.
  for (unsigned int row = 0; row < editortable.textbuffers.size(); row++) {

    // New row: Add the appropriate USFM code.
    usfm_internal_add_text(text, "\n");
    usfm_internal_add_text(text, usfm_get_full_opening_marker(style_get_table_row_marker(project)));

    // Go through all the columns of this row.
    for (unsigned int column = 0; column < editortable.textbuffers[row].size(); column++) {

      // Get the USFM code of this cell.
      ustring celltext;
      GtkTextBuffer *textbuffer = table_cell_get_buffer(editortable, row, column);
      GtkTextIter startiter, enditer;
      gtk_text_buffer_get_start_iter(textbuffer, &startiter);
      gtk_text_buffer_get_end_iter(textbuffer, &enditer);
      usfm_get_text(textbuffer, startiter, enditer, NULL, NULL, project, celltext, false);

      // Add this USFM code to the main text.
      usfm_internal_add_text(text, celltext);
    }
  }
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
          // Store the paragraph style, or, if the paragraph has been found 
          // already, the character style.
          // This works because the editing code takes care when applying the
          // tags, to first apply the paragraph style, and then the character
          // style.
          if (paragraph_style.empty())
            paragraph_style = strval;
          else
            character_style = strval;
        }
      }
      g_free(strval);
    }
  }
  if (tags) {
    g_slist_free(tags);
  }
}

GtkTextIter editor_get_iter_for_note(GtkTextBuffer * textbuffer, const vector < EditorNote > &editornotes, unsigned int offset, unsigned int function)
/*
 This function gets the iterator where to insert a note in the "textbuffer". 
 It gives this iterator for a note of "editornotes", with offset "offset".
 "Function" indicates what the function of the iterator is going to be:
 0: Inserting the new line to make space for the note.
 1: Inserting the GtkLabel for the note caller.
 2: Inserting the GtkTextChildAnchor for the note body.
 */
{
  // The text iterator.
  GtkTextIter iter;

  // By default the iterator is at the end of the buffer.
  gtk_text_buffer_get_end_iter(textbuffer, &iter);

  /*
     If the first note is not rendered yet, the location will be the one before
     the first next note that has been rendered.
   */
  if (offset == 0) {
    bool iterfound = false;
    for (unsigned int i = 1; i < editornotes.size(); i++) {
      if (iterfound)
        continue;
      if (editornotes[i].textview) {
        gtk_text_buffer_get_iter_at_child_anchor(textbuffer, &iter, editornotes[i].childanchor_caller_note);
        if (function)
          gtk_text_iter_backward_char(&iter);
        iterfound = true;
      }
    }
  }

  /*
     For the second note, and up, it is right after the previous note.
     This assumes that any previous note has been rendered already. With the 
     current mechanism this assumption is always true.
   */
  if (offset > 0) {
    gtk_text_buffer_get_iter_at_child_anchor(textbuffer, &iter, editornotes[offset - 1].childanchor_textview);
    gtk_text_iter_forward_chars(&iter, ++function);
  }
  // Return the iterator.
  return iter;
}

ustring get_verse_number_at_iterator(GtkTextIter iter, const ustring & verse_marker, const ustring & project)
/* This function returns the verse number at the iterator.
 * It also takes into account a situation where the cursor is on a heading.
 * The user expects a heading to belong to the next verse. 
 */
{
  // Variables.
  ustring verse = "0";
  bool verse_marker_found = false;
  ustring paragraph_style_at_cursor;
  GtkTextIter iter_at_cursor = iter;

  // Keep iterating backward till we have found the v style applied.
  // The text, starting from this iterator and forward, is the verse number.
  do {
    ustring paragraph_style;
    ustring character_style;
    get_styles_at_iterator(iter, paragraph_style, character_style);
    if (paragraph_style_at_cursor.empty())
      paragraph_style_at_cursor = paragraph_style;
    if (character_style == verse_marker) {
      verse_marker_found = true;
    }
    if (verse_marker_found) {
      if (character_style != verse_marker) {
        GtkTextIter startiter = iter;
        gtk_text_iter_forward_char(&startiter);
        GtkTextIter enditer = startiter;
        gtk_text_iter_forward_chars(&enditer, 10);
        verse = gtk_text_iter_get_slice(&startiter, &enditer);
        size_t position = verse.find(" ");
        position = CLAMP(position, 0, verse.length());
        verse = verse.substr(0, position);
        break;
      }
    }
  } while (gtk_text_iter_backward_char(&iter));

  // Optionally do the optimization cycle so that if the cursor is on a title/heading, it takes the next verse.
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
          // At this stage the cursor is on a title / heading.
          // We look one line ahead and take the verse number it gives.
          if (gtk_text_iter_forward_line(&iter_at_cursor)) {
            verse = get_verse_number_at_iterator(iter_at_cursor, verse_marker, project);
          }
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
  return verse;
}


const gchar *unknown_style()
// Gives the name of the style that is for markers that are not in the stylesheet.
{
  return "unknown";
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


void textview_apply_paragraph_style(GtkWidget *textview, const ustring& oldstyle, const ustring& newstyle)
// Removes the old style and applies the new to the whole "textview".
{
  GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
  GtkTextIter start;
  gtk_text_buffer_get_start_iter (textbuffer, &start);
  GtkTextIter end;
  gtk_text_buffer_get_end_iter (textbuffer, &end);
  if (!oldstyle.empty()) {
    gtk_text_buffer_remove_tag_by_name (textbuffer, oldstyle.c_str(), &start, &end);
  }
  gtk_text_buffer_apply_tag_by_name (textbuffer, newstyle.c_str(), &start, &end);
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


GtkWidget *textview_note_get_another(GtkTextBuffer * mainbuffer, GtkWidget * currentview, vector < EditorNote > &editornotes, EditorMovementType movement)
{
  // Variable for the next textview to go to.
  GtkWidget *anotherview = NULL;

  // See whether to get the next or previous textview.
  bool nextview = false;
  switch (movement) {
  case emtForward:
  case emtDown:
    nextview = true;
    break;
  case emtBack:
  case emtUp:
    nextview = false;
    break;
  }

  // Get offset of current textview.
  gint currentoffset = 0;
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    if (currentview == editornotes[i].textview) {
      GtkTextIter iter;
      gtk_text_buffer_get_iter_at_child_anchor(mainbuffer, &iter, editornotes[i].childanchor_textview);
      currentoffset = gtk_text_iter_get_offset(&iter);
    }
  }

  // Only proceed if we found a current offset.
  if (currentoffset > 0) {

    // Variables.
    gint minimum_difference = G_MAXINT;

    // Go through all notes.
    for (unsigned int i = 0; i < editornotes.size(); i++) {

      // Get the offset of the note, and ...
      GtkTextIter iter;
      gtk_text_buffer_get_iter_at_child_anchor(mainbuffer, &iter, editornotes[i].childanchor_textview);
      gint textviewoffset = gtk_text_iter_get_offset(&iter);
      bool proceed = false;
      if (nextview) {
        proceed = textviewoffset > currentoffset;
      } else {
        proceed = textviewoffset < currentoffset;
      }

      // ... proceed if the it is bigger than the current offset.
      if (proceed) {

        // Look for the textview nearest the current one.
        gint difference = ABS(textviewoffset - currentoffset);
        if (difference < minimum_difference) {
          anotherview = editornotes[i].textview;
          minimum_difference = difference;
        }

      }
    }
  }
  // Return the next view, if any.
  return anotherview;
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


bool create_editor_objects_for_text_table_raw(const ustring& project, GtkWidget * textview, ustring& line, ustring& paragraph_mark, ustring& character_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
// This function loads the raw text of a table.
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype(project, marker, type, subtype);
        // Proceed if this bit starts with any table-related marker.
        if (style_get_starts_table_row(type, subtype) || style_get_starts_table_cell(type, subtype)) {
          // Get the marker that ends the table bits.
          ustring endmarker;
          vector < ustring > markers = usfm_get_all_markers(line);
          for (unsigned int i = 0; i < markers.size(); i++) {
            StyleType type2;
            int subtype2;
            marker_get_type_and_subtype(project, markers[i], type2, subtype2);
            if (type2 != stTableElement) {
              endmarker = markers[i];
              break;
            }
          }
          // Proceed with the text till the next non-table marker or the end of the line.
          ustring rawtable;
          if (endmarker.empty()) {
            rawtable = line;
            line.clear();
          } else {
            size_t pos1, pos2;
            pos1 = line.find(usfm_get_full_opening_marker(endmarker));
            pos2 = line.find(usfm_get_full_closing_marker(endmarker));
            size_t pos = MIN(pos1, pos2);
            rawtable = line.substr(0, pos);
            line.erase(0, pos);
          }
          /*
          // Display the table
          GtkTextIter iter;
          gtk_text_buffer_get_iter_at_mark(textbuffer, &iter, gtk_text_buffer_get_insert(textbuffer));
          display_table(rawtable, iter);
          */
          // The information was processed: return true.
          return true;
        }
      }
    }
  }
  return false;
}


bool create_editor_objects_for_text_note_raw(const ustring& project, GtkWidget * textview, ustring& line, ustring& paragraph_mark, ustring& character_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
/*
 This function loads the raw text of a footnote, an endnote, or a 
 crossreference.
 */
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype(project, marker, type, subtype);
        if (style_get_starts_footnote(type, subtype) || style_get_starts_endnote(type, subtype) || style_get_starts_crossreference(type, subtype)) {
          // Proceed if the endmarker is in the text too.
          ustring endmarker = usfm_get_full_closing_marker(marker);
          size_t endmarkerpos = line.find(endmarker);
          if (endmarkerpos != string::npos) {
            // Get raw note text and erase it from the input buffer.
            ustring rawnote(line.substr(marker_length, endmarkerpos - endmarker.length()));
            line.erase(0, endmarkerpos + endmarker.length());
            // Insert the note.
            //insert_note(marker, rawnote, false);
            // The information was processed: return true.
            return true;
          }
        }
      }
    }
  }
  return false;
}


gint editor_paragraph_insertion_point_get_offset (EditorActionCreateParagraph * paragraph_action)
{
  gint offset = 0;
  if (paragraph_action) {
    GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (paragraph_action->widget));
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(textbuffer, &iter, gtk_text_buffer_get_insert(textbuffer));
    offset = gtk_text_iter_get_offset (&iter);
  }
  return offset;
}


EditorActionDeleteText * paragraph_delete_character_before_text_insertion_point_if_space(EditorActionCreateParagraph * paragraph_action)
// Creates an action for deleting text for the character before the text insertion point if that is a space.
{
  if (paragraph_action) {
    GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (paragraph_action->widget));
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(textbuffer, &iter, gtk_text_buffer_get_insert(textbuffer));
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

