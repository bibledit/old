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


#include "editoractions.h"
#include "gwrappers.h"
#include "editor_aids.h"


EditorAction::EditorAction(EditorActionType type_in)
{
  // The type of this EditorAction.
  type = type_in;
}


EditorAction::~EditorAction ()
{
}


void EditorAction::apply (deque <EditorAction *>& done)
{
  // Store the EditorAction on the stack of actions done.
  done.push_back (this);
}


void EditorAction::undo (deque <EditorAction *>& done, deque <EditorAction *>& undone)
{
  // Move the EditorAction from the stack of actions done to the one of actions undone.
  done.pop_back();
  undone.push_back (this);
}


void EditorAction::redo (deque <EditorAction *>& done, deque <EditorAction *>& undone)
{
  // Move the EditorAction from the stack of actions undone to the one of actions done.
  undone.pop_back();
  done.push_back (this);
}


EditorActionCreateParagraph::EditorActionCreateParagraph(GtkWidget * vbox) :
EditorAction (eatCreateParagraph)
{
  // Pointer to the GtkTextView is created on apply.
  textview = NULL;
  // Pointer to the GtkTextBuffer is created on apply.
  textbuffer = NULL;
  // The default style of the paragraph will be "unknown".
  style = unknown_style();
  // Offset of this widget at time of deletion.
  offset_at_delete = -1;
  // Pointer to the parent vertical box.
  parent_vbox = vbox;
}


EditorActionCreateParagraph::~EditorActionCreateParagraph ()
{
  if (textview) {
    gtk_widget_destroy (textview);
    textview = NULL;
  }
}


void EditorActionCreateParagraph::apply (GtkTextTagTable * texttagtable, bool editable, EditorActionCreateParagraph * focused_paragraph, GtkWidget *& to_focus)
{
  // The textbuffer uses the text tag table.
  textbuffer = gtk_text_buffer_new(texttagtable);

  // New text view to view the text buffer.
  textview = gtk_text_view_new_with_buffer(textbuffer);
  gtk_widget_show(textview);

  // Add text view to the GUI.
  gtk_box_pack_start(GTK_BOX(parent_vbox), textview, false, false, 0);

  // Set some parameters of the view.
  gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(textview), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), editable);
  gtk_text_view_set_left_margin(GTK_TEXT_VIEW(textview), 5);
  gtk_text_view_set_right_margin(GTK_TEXT_VIEW(textview), 5);
  
  // Move the widget to the right position, 
  // which is next to the currently focused paragraph.
  // This move is important since a new paragraph can be created anywhere among the current ones.
  vector <GtkWidget *> widgets = editor_get_widgets (parent_vbox);
  gint new_paragraph_offset = 0;
  if (focused_paragraph) {
    for (unsigned int i = 0; i < widgets.size(); i++) {
      if (focused_paragraph->textview == widgets[i]) {
        new_paragraph_offset = i + 1;
        break;
      }
    }
  }
  gtk_box_reorder_child (GTK_BOX(parent_vbox), textview, new_paragraph_offset);
  
  // Let the newly created textview be earmarked to grab focus
  // so that the user can type in it,
  // and the internal Editor logic knows about it.
  to_focus = textview;
}


void EditorActionCreateParagraph::undo (GtkWidget * parking_vbox, GtkWidget *& to_focus)
{
  // Remove the widget by parking it in an invisible location. It is kept alive.
  editor_park_widget (parent_vbox, textview, offset_at_delete, parking_vbox);
  // Focus textview.
  to_focus = textview;
}


void EditorActionCreateParagraph::redo (GtkWidget *& to_focus)
{
  // Restore the live widget to the editor.
  gtk_widget_reparent (textview, parent_vbox);
  gtk_box_reorder_child (GTK_BOX(parent_vbox), textview, offset_at_delete);
  // Let the restored textview be earmarked to grab focus.
  to_focus = textview;
}


EditorActionChangeParagraphStyle::EditorActionChangeParagraphStyle(const ustring& style, EditorActionCreateParagraph * parent_action) :
EditorAction (eatChangeParagraphStyle)
{
  // The EditorAction object that created the paragraph whose style it going to be set.
  paragraph = parent_action;
  // The style of the paragraph before the new style was applied.
  previous_style = parent_action->style;
  // The new style for the paragraph.
  current_style = style;
}


EditorActionChangeParagraphStyle::~EditorActionChangeParagraphStyle ()
{
}


void EditorActionChangeParagraphStyle::apply (GtkWidget *& to_focus)
{
  paragraph->style = current_style;
  set_style (paragraph->style);
  to_focus = paragraph->textview;
}


void EditorActionChangeParagraphStyle::undo (GtkWidget *& to_focus)
{
  paragraph->style = previous_style;
  set_style (paragraph->style);
  to_focus = paragraph->textview;
}


void EditorActionChangeParagraphStyle::redo (GtkWidget *& to_focus)
{
  apply (to_focus);
}


void EditorActionChangeParagraphStyle::set_style (const ustring& style)
{
  // Define the work area.
  GtkTextIter startiter;
  gtk_text_buffer_get_start_iter (paragraph->textbuffer, &startiter);
  GtkTextIter enditer;
  gtk_text_buffer_get_end_iter (paragraph->textbuffer, &enditer);
  // Apply the style in such a way that the paragraph style is always applied first, 
  // then after that the character styles.
  vector <ustring> current_character_styles = get_character_styles_between_iterators (startiter, enditer);
  gtk_text_buffer_remove_all_tags (paragraph->textbuffer, &startiter, &enditer);
  gtk_text_buffer_apply_tag_by_name (paragraph->textbuffer, style.c_str(), &startiter, &enditer);
  for (unsigned int i = 0; i < current_character_styles.size(); i++) {
    if (!current_character_styles[i].empty()) {
      gtk_text_buffer_get_iter_at_offset (paragraph->textbuffer, &startiter, i);
      enditer = startiter;
      gtk_text_iter_forward_char (&enditer);
      gtk_text_buffer_apply_tag_by_name (paragraph->textbuffer, current_character_styles[i].c_str(), &startiter, &enditer);
    }
  }
}


EditorActionInsertText::EditorActionInsertText(EditorActionCreateParagraph * parent_action, gint offset_in, const ustring& text_in) :
EditorAction (eatInsertText)
{
  // The paragraph to operate on.
  paragraph = parent_action;
  // Where to insert the text, that is, at which offset within the GtkTextBuffer.
  offset = offset_in;
  // The text to insert.
  text = text_in;
}


EditorActionInsertText::~EditorActionInsertText ()
{
}


void EditorActionInsertText::apply (GtkWidget *& to_focus)
{
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_offset (paragraph->textbuffer, &iter, offset);
  gtk_text_buffer_insert (paragraph->textbuffer, &iter, text.c_str(), -1);
  // Apply the paragraph style to the new inserted text.
  // It is important that paragraph styles are applied first, and character styles last.
  // Since this is new inserted text, there's no character style yet, 
  // so the paragraph style can be applied normally.
  GtkTextIter startiter;
  gtk_text_buffer_get_iter_at_offset (paragraph->textbuffer, &startiter, offset);
  GtkTextIter enditer;
  gtk_text_buffer_get_iter_at_offset (paragraph->textbuffer, &enditer, offset + text.length());
  gtk_text_buffer_apply_tag_by_name (paragraph->textbuffer, paragraph->style.c_str(), &startiter, &enditer);
  // Focus widget.
  to_focus = paragraph->textview;
}


void EditorActionInsertText::undo (GtkWidget *& to_focus)
{
  // Undo the insertion of text, that is, remove it again.
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_iter_at_offset (paragraph->textbuffer, &startiter, offset);
  gtk_text_buffer_get_iter_at_offset (paragraph->textbuffer, &enditer, offset + text.length());
  gtk_text_buffer_delete (paragraph->textbuffer, &startiter, &enditer);
  // Focus widget.
  to_focus = paragraph->textview;
}


void EditorActionInsertText::redo (GtkWidget *& to_focus)
{
  apply (to_focus);
}


EditorActionDeleteText::EditorActionDeleteText(EditorActionCreateParagraph * parent_action, gint offset_in, gint length_in) :
EditorAction (eatDeleteText)
{
  // The paragraph to operate on.
  paragraph = parent_action;
  // Where to start deleting the text, that is, at which offset within the GtkTextBuffer.
  offset = offset_in;
  // The length of the text to be deleted.
  length = length_in;
  // The text which was deleted will be set when this action is executed.
}


EditorActionDeleteText::~EditorActionDeleteText ()
{
}


void EditorActionDeleteText::apply (GtkWidget *& to_focus)
{
  // Limit the area.
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_iter_at_offset (paragraph->textbuffer, &startiter, offset);
  gtk_text_buffer_get_iter_at_offset (paragraph->textbuffer, &enditer, offset + length);
  // Save existing content.
  get_text_and_styles_between_iterators(&startiter, &enditer, deleted_text, deleted_styles);
  // Delete text.
  gtk_text_buffer_delete (paragraph->textbuffer, &startiter, &enditer);
  // Focus widget.
  to_focus = paragraph->textview;
}


void EditorActionDeleteText::undo (GtkWidget *& to_focus)
{
  // Undo the text deletion action, that means, re-insert the text.
  // Get initial insert position.
  gint accumulated_offset = offset;
  // Go through the text to re-insert.
  for (unsigned int i = 0; i < deleted_text.size(); i++) {
    // Get the position where to insert.
    GtkTextIter startiter;
    gtk_text_buffer_get_iter_at_offset (paragraph->textbuffer, &startiter, accumulated_offset);
    // Re-insert the text.
    gtk_text_buffer_insert (paragraph->textbuffer, &startiter, deleted_text[i].c_str(), -1);
    // Apply the paragraph style to the new inserted text.
    // It is important that paragraph styles are applied first, and character styles last.
    // Since this is new inserted text, there's no character style yet, 
    // so the paragraph style can be applied normally.
    gtk_text_buffer_get_iter_at_offset (paragraph->textbuffer, &startiter, accumulated_offset);
    GtkTextIter enditer;
    gtk_text_buffer_get_iter_at_offset (paragraph->textbuffer, &enditer, accumulated_offset + deleted_text[i].length());
    gtk_text_buffer_apply_tag_by_name (paragraph->textbuffer, paragraph->style.c_str(), &startiter, &enditer);
    // Apply the character style.
    if (!deleted_styles[i].empty()) {
      gtk_text_buffer_apply_tag_by_name (paragraph->textbuffer, deleted_styles[i].c_str(), &startiter, &enditer);
    }
    // Modify the accumulated offset for the next iteration.
    accumulated_offset += deleted_text[i].length();
  }
  // Focus widget.
  to_focus = paragraph->textview;
}


void EditorActionDeleteText::redo (GtkWidget *& to_focus)
{
  // Limit the area.
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_iter_at_offset (paragraph->textbuffer, &startiter, offset);
  gtk_text_buffer_get_iter_at_offset (paragraph->textbuffer, &enditer, offset + length);
  // Delete text.
  gtk_text_buffer_delete (paragraph->textbuffer, &startiter, &enditer);
  // Focus widget.
  to_focus = paragraph->textview;
}


EditorActionChangeCharacterStyle::EditorActionChangeCharacterStyle(EditorActionCreateParagraph * parent_action, const ustring& style_in, gint offset_in, gint length_in) :
EditorAction (eatChangeCharacterStyle)
{
  // The identifier of the paragraph to operate on.
  paragraph = parent_action;
  // The name of the style.
  style = style_in;
  // Where to start applying the style, that is, at which offset within the GtkTextBuffer.
  offset = offset_in;
  // The length of the text where the style is to be applied.
  length = length_in;
  // The previous styles are stored per character when this action is executed.
}


EditorActionChangeCharacterStyle::~EditorActionChangeCharacterStyle ()
{
}


void EditorActionChangeCharacterStyle::apply (GtkWidget *& to_focus)
{
  // Mark off the affected area.
  GtkTextIter startiter;
  GtkTextIter enditer;
  gtk_text_buffer_get_iter_at_offset (paragraph->textbuffer, &startiter, offset);
  gtk_text_buffer_get_iter_at_offset (paragraph->textbuffer, &enditer, offset + length);
  // Get the styles applied now, and store these so as to track the state of this bit of text.
  previous_styles = get_character_styles_between_iterators (startiter, enditer);
  // The new styles to apply.
  vector <ustring> new_styles;
  for (gint i = 0; i < length; i++) {
    new_styles.push_back (style);
  }
  // Change the styles.
  change_styles (previous_styles, new_styles);
  // Focus widget.
  to_focus = paragraph->textview;
}


void EditorActionChangeCharacterStyle::undo (GtkWidget *& to_focus)
{
  // The styles to remove.
  vector <ustring> styles_to_remove;
  for (gint i = 0; i < length; i++) {
    styles_to_remove.push_back (style);
  }
  // Change the styles, putting back the original ones.
  change_styles (styles_to_remove, previous_styles);
  // Focus widget.
  to_focus = paragraph->textview;
}


void EditorActionChangeCharacterStyle::redo (GtkWidget *& to_focus)
{
  // Mark off the affected area.
  GtkTextIter startiter;
  GtkTextIter enditer;
  gtk_text_buffer_get_iter_at_offset (paragraph->textbuffer, &startiter, offset);
  gtk_text_buffer_get_iter_at_offset (paragraph->textbuffer, &enditer, offset + length);
  // Get the styles applied now, and store these so as to track the state of this bit of text.
  vector <ustring> styles_to_delete = get_character_styles_between_iterators (startiter, enditer);
  // The new styles to apply.
  vector <ustring> new_styles;
  for (gint i = 0; i < length; i++) {
    new_styles.push_back (style);
  }
  // Change the styles.
  change_styles (styles_to_delete, new_styles);
  // Focus widget.
  to_focus = paragraph->textview;
}


void EditorActionChangeCharacterStyle::change_styles (const vector <ustring>& old_ones, const vector <ustring>& new_ones)
{
  // Remove old styles and apply new ones.
  for (gint i = 0; i < length; i++) {
    GtkTextIter startiter, enditer;
    gtk_text_buffer_get_iter_at_offset (paragraph->textbuffer, &startiter, offset + i);
    enditer = startiter;
    gtk_text_iter_forward_char (&enditer);
    if (!old_ones[i].empty()) {
      gtk_text_buffer_remove_tag_by_name (paragraph->textbuffer, old_ones[i].c_str(), &startiter, &enditer);
    }
    if (!new_ones[i].empty()) {
      gtk_text_buffer_apply_tag_by_name (paragraph->textbuffer, new_ones[i].c_str(), &startiter, &enditer);
    }
  }
}


EditorActionDeleteParagraph::EditorActionDeleteParagraph(EditorActionCreateParagraph * paragraph_in) :
EditorAction (eatDeleteParagraph)
{
  // The identifier of the paragraph to operate on.
  paragraph = paragraph_in;
  // Initialize the offset within the parent GtkBox.
  offset = -1;
}


EditorActionDeleteParagraph::~EditorActionDeleteParagraph ()
{
}


void EditorActionDeleteParagraph::apply (GtkWidget * parking_vbox, GtkWidget *& to_focus)
{
  // Park this widget, keeping it alive.
  GtkWidget * widget_to_park = paragraph->textview;
  if (paragraph->type == eatCreateNoteParagraph) {
    EditorActionCreateNoteParagraph * note_paragraph = static_cast <EditorActionCreateNoteParagraph *> (paragraph);
    widget_to_park = note_paragraph->hbox;
  }
  editor_park_widget (paragraph->parent_vbox, widget_to_park, offset, parking_vbox);
}


void EditorActionDeleteParagraph::undo (GtkWidget *& to_focus)
{
  // Restore the live widget to the editor.
  GtkWidget * widget_to_restore = paragraph->textview;
  if (paragraph->type == eatCreateNoteParagraph) {
    EditorActionCreateNoteParagraph * note_paragraph = static_cast <EditorActionCreateNoteParagraph *> (paragraph);
    widget_to_restore = note_paragraph->hbox;
  }
  gtk_widget_reparent (widget_to_restore, paragraph->parent_vbox);
  gtk_box_reorder_child (GTK_BOX(paragraph->parent_vbox), widget_to_restore, offset);
  // Let the restored textview be earmarked to grab focus.
  to_focus = paragraph->textview;
}


void EditorActionDeleteParagraph::redo (GtkWidget * parking_vbox, GtkWidget *& to_focus)
{
  // Park this widget, keeping it alive.
  // Don't store the offset, since we already have that value.
  GtkWidget * widget_to_park = paragraph->textview;
  if (paragraph->type == eatCreateNoteParagraph) {
    EditorActionCreateNoteParagraph * note_paragraph = static_cast <EditorActionCreateNoteParagraph *> (paragraph);
    widget_to_park = note_paragraph->hbox;
  }
  gint dummy;
  editor_park_widget (paragraph->parent_vbox, widget_to_park, dummy, parking_vbox);
}


EditorActionCreateNoteParagraph::EditorActionCreateNoteParagraph(GtkWidget * vbox, const ustring& marker_in, const ustring& caller_usfm_in, const ustring& caller_text_in, const ustring& identifier_in) :
EditorActionCreateParagraph (vbox)
{
  // Change the type to a note paragraph.
  type = eatCreateNoteParagraph;
  // Store opening and closing marker (e.g. "f" for a footnote).
  opening_closing_marker = marker_in;
  // Store USFM caller (e.g. "+" for automatic numbering).
  caller_usfm = caller_usfm_in;
  // Store caller in text (e.g. "f" for a footnote).
  caller_text = caller_text_in;
  // Store identifier. Is used as the style in the main text body.
  identifier = identifier_in;
  // Widgets will be set on initial application.
  hbox = NULL;
  eventbox = NULL;
  label = NULL;
}


EditorActionCreateNoteParagraph::~EditorActionCreateNoteParagraph ()
{
  if (hbox) {
    gtk_widget_destroy (hbox);
    hbox = NULL;
    eventbox = NULL;
    label = NULL;
    textview = NULL;
  }
}


void EditorActionCreateNoteParagraph::apply (GtkTextTagTable * texttagtable, bool editable, EditorActionCreateParagraph * focused_paragraph, GtkWidget *& to_focus)
{
  // Horizontal box to store the note.
  hbox = gtk_hbox_new (false, 0);
  gtk_widget_show (hbox);
  gtk_box_pack_start(GTK_BOX(parent_vbox), hbox, false, false, 0);

  // Eventbox to catch a few events on the caller of the note.
  eventbox = gtk_event_box_new ();
  gtk_widget_show (eventbox);
  gtk_box_pack_start(GTK_BOX(hbox), eventbox, false, false, 0);
  g_signal_connect ((gpointer) eventbox, "enter_notify_event", G_CALLBACK (on_caller_enter_notify_event), gpointer (this));
  g_signal_connect ((gpointer) eventbox, "leave_notify_event", G_CALLBACK (on_caller_leave_notify_event), gpointer (this));

  // The background of the caller is going to be grey.
  // Courier font is chosen to make the spacing of the callers equal so they line up nicely.
  label = gtk_label_new ("");
  gtk_widget_show (label);
  char *markup = g_markup_printf_escaped("<span background=\"grey\" size=\"x-small\"> </span><span background=\"grey\" face=\"Courier\">%s</span><span background=\"grey\" size=\"x-small\"> </span>", caller_text.c_str());
  gtk_label_set_markup(GTK_LABEL(label), markup);
  g_free(markup);
  gtk_container_add (GTK_CONTAINER (eventbox), label);

  // The textbuffer uses the text tag table.
  textbuffer = gtk_text_buffer_new(texttagtable);

  // Text view to view the text buffer.
  textview = gtk_text_view_new_with_buffer(textbuffer);
  gtk_widget_show(textview);
  gtk_box_pack_start(GTK_BOX(hbox), textview, true, true, 0);

  // Set some parameters of the view.
  gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(textview), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), editable);
  gtk_text_view_set_left_margin(GTK_TEXT_VIEW(textview), 5);
  gtk_text_view_set_right_margin(GTK_TEXT_VIEW(textview), 5);

  // Move the widget to the right position. To be calculated.
  /*
    vector <GtkWidget *> widgets = editor_get_widgets (parent_vbox);
    gint new_paragraph_offset = 0;
    if (focused_paragraph) {
      for (unsigned int i = 0; i < widgets.size(); i++) {
        if (focused_paragraph->textview == widgets[i]) {
          new_paragraph_offset = i + 1;
          break;
        }
      }
    }
    gtk_box_reorder_child (GTK_BOX(parent_vbox), textview, new_paragraph_offset);
  */
  
  // Let the newly created textview be earmarked to grab focus
  // so that the user can type in it,
  // and the internal Editor logic knows about it.
  to_focus = textview;
}


void EditorActionCreateNoteParagraph::undo (GtkWidget * parking_vbox, GtkWidget *& to_focus)
{
  // Remove the widget by parking it in an invisible location. It is kept alive.
  editor_park_widget (parent_vbox, textview, offset_at_delete, parking_vbox);
  // Focus textview.
  to_focus = textview;
}


void EditorActionCreateNoteParagraph::redo (GtkWidget *& to_focus)
{
  // Restore the live widget to the editor.
  gtk_widget_reparent (textview, parent_vbox);
  gtk_box_reorder_child (GTK_BOX(parent_vbox), textview, offset_at_delete);
  // Let the restored textview be earmarked to grab focus.
  to_focus = textview;
}


gboolean EditorActionCreateNoteParagraph::on_caller_enter_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data)
{
  return ((EditorActionCreateNoteParagraph *) user_data)->on_caller_enter_notify(event);
}


gboolean EditorActionCreateNoteParagraph::on_caller_enter_notify (GdkEventCrossing *event)
{
  // Set the cursor to a shape that shows that the caller can be clicked.
  GtkWidget *toplevel_widget = gtk_widget_get_toplevel(label);
  GdkWindow *gdk_window = toplevel_widget->window;
  GdkCursor *cursor = gdk_cursor_new(GDK_HAND2);
  gdk_window_set_cursor(gdk_window, cursor);
  gdk_cursor_unref (cursor);
  return false;
}


gboolean EditorActionCreateNoteParagraph::on_caller_leave_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data)
{
  return ((EditorActionCreateNoteParagraph *) user_data)->on_caller_leave_notify(event);
}


gboolean EditorActionCreateNoteParagraph::on_caller_leave_notify (GdkEventCrossing *event)
{
  // Restore the original cursor.
  GtkWidget * toplevel_widget = gtk_widget_get_toplevel(label);
  GdkWindow *gdk_window = toplevel_widget->window;
  gdk_window_set_cursor(gdk_window, NULL);
  return false;
}


