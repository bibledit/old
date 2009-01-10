/* Copyright (©) 2003-2009 Teus Benschop.
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

#include "utilities.h"
#include <glib.h>
#include "usfmview.h"
#include "usfm.h"
#include <gdk/gdkkeysyms.h>
#include "projectutils.h"
#include "settings.h"
#include "screen.h"
#include "referenceutils.h"
#include "books.h"
#include "gwrappers.h"
#include "biblenotes.h"
#include "color.h"
#include "bible.h"
#include "stylesheetutils.h"
#include "styles.h"
#include "usfmtools.h"
#include "directories.h"
#include "notecaller.h"
#include "gtkwrappers.h"
#include "keyboard.h"
#include "tiny_utilities.h"
#include "git.h"

USFMView::USFMView(GtkWidget * vbox, const ustring & project_in):
current_reference(0, 1000, "")
{
  // Save and initialize variables.
  project = project_in;
  book = 0;
  chapter = 0;

  // The scrolled window that contains the sourceview.
  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow);
  gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, true, true, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  // A sourcebuffer to store all text.
  sourcebuffer = gtk_source_buffer_new (NULL);

  // A sourceview to display the buffer.
  sourceview = gtk_source_view_new_with_buffer(sourcebuffer);
  gtk_widget_show(sourceview);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), sourceview);
  gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(sourceview), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(sourceview), GTK_WRAP_WORD);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(sourceview), FALSE);
  gtk_text_view_set_left_margin(GTK_TEXT_VIEW(sourceview), 5);
  gtk_source_view_set_highlight_current_line (GTK_SOURCE_VIEW (sourceview), true);

  /*
  do_not_process_child_anchors_being_deleted = false;
  textview_allocated_width = 0;
  texttagtable = NULL;
  record_undo_level = 0;
  previous_hand_cursor = false;
  highlight = NULL;
  event_id_show_quick_references = 0;
  texview_to_textview_old = NULL;
  texview_to_textview_new = NULL;
  textview_to_textview_offset = 0;
  start_verse_tracker_event_id = 0;
  verse_tracker_event_id = 0;
  verse_tracker_on = false;

  // Create data that is needed for any of the possible formatted views.
  create_or_update_formatting_data();

  // The buffer's signal handlers.
  g_signal_connect(G_OBJECT(textbuffer), "insert-text", G_CALLBACK(on_buffer_insert_text_before), gpointer(this));
  g_signal_connect_after(G_OBJECT(textbuffer), "insert-text", G_CALLBACK(on_buffer_insert_text_after), gpointer(this));
  g_signal_connect(G_OBJECT(textbuffer), "delete-range", G_CALLBACK(on_buffer_delete_range_before), gpointer(this));
  g_signal_connect_after(G_OBJECT(textbuffer), "delete-range", G_CALLBACK(on_buffer_delete_range_after), gpointer(this));
  g_signal_connect(G_OBJECT(textbuffer), "apply-tag", G_CALLBACK(on_buffer_apply_tag), gpointer(this));
  g_signal_connect(G_OBJECT(textbuffer), "remove-tag", G_CALLBACK(on_buffer_remove_tag), gpointer(this));
  g_signal_connect(G_OBJECT(textbuffer), "insert-child-anchor", G_CALLBACK(on_buffer_insert_child_anchor), gpointer(this));
  g_signal_connect(G_OBJECT(textbuffer), "insert-pixbuf", G_CALLBACK(on_buffer_insert_pixbuf), gpointer(this));
  g_signal_connect(G_OBJECT(textbuffer), "changed", G_CALLBACK(on_textbuffer_changed), gpointer(this));
  g_signal_connect(G_OBJECT(textbuffer), "modified-changed", G_CALLBACK(on_textbuffer_modified_changed), gpointer(this));

  // The view's signal handlers.
  spellingchecker->attach(textview);
  g_signal_connect_after((gpointer) textview, "move_cursor", G_CALLBACK(on_textview_move_cursor), gpointer(this));
  g_signal_connect_after((gpointer) textview, "grab_focus", G_CALLBACK(on_textview_grab_focus), gpointer(this));
  g_signal_connect((gpointer) textview, "motion-notify-event", G_CALLBACK(on_text_motion_notify_event), gpointer(this));
  g_signal_connect((gpointer) textview, "event-after", G_CALLBACK(on_text_event_after), gpointer(this));
  g_signal_connect((gpointer) textview, "key-press-event", G_CALLBACK(text_key_press_event_before), gpointer(this));
  g_signal_connect_after((gpointer) textview, "key-press-event", G_CALLBACK(text_key_press_event_after), gpointer(this));
  g_signal_connect((gpointer) textview, "visibility-notify-event", G_CALLBACK(screen_visibility_notify_event), gpointer(this));
  g_signal_connect((gpointer) textview, "button_press_event", G_CALLBACK(on_textview_button_press_event), gpointer(this));
  g_signal_connect((gpointer) textview, "size-allocate", G_CALLBACK(on_related_widget_size_allocated), gpointer(this));

  // Initialize the last focused textview to the main textview.
  last_focused_widget = textview;

  // Buttons to give signals.
  new_verse_signal = gtk_button_new();
  new_styles_signal = gtk_button_new();
  word_double_clicked_signal = gtk_button_new();
  reload_signal = gtk_button_new();
  changed_signal = gtk_button_new();
  quick_references_button = gtk_button_new();

  // Initialize a couple of event ids.
  textview_cursor_moved_delayer_event_id = 0;
  grab_focus_event_id = 0;
  undo_redo_event_id = 0;
  save_timeout_event_id = 0;
  highlight_timeout_event_id = 0;
  spelling_timeout_event_id = 0;

  // Tag for highlighting search words.
  // Note that for convenience the GtkTextBuffer function is called. 
  // But this adds the reference to the GtkTextTagTable, so making it available
  // to any other buffer that uses the same text tag table.
  reference_tag = gtk_text_buffer_create_tag(textbuffer, NULL, "background", "khaki", NULL);

  // Highlighting searchwords timeout.
  highlight_timeout_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 500, GSourceFunc(on_highlight_timeout), gpointer(this), NULL);

  // Automatic saving of the file, periodically.
  save_timeout_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 60000, GSourceFunc(on_save_timeout), gpointer(this), NULL);

  // Fonts.
  set_font();

  // Grab focus.
  focus_programmatically_being_grabbed = false;
  gtk_widget_grab_focus(textview);
  */
}

USFMView::~USFMView()
{
  // Save the chapter.
  chapter_save();

  /*

  // Destroy a couple of timeout sources.
  gw_destroy_source(textview_cursor_moved_delayer_event_id);
  gw_destroy_source(grab_focus_event_id);
  gw_destroy_source(undo_redo_event_id);
  gw_destroy_source(save_timeout_event_id);
  gw_destroy_source(highlight_timeout_event_id);
  gw_destroy_source(spelling_timeout_event_id);
  gw_destroy_source(event_id_show_quick_references);
  gw_destroy_source(start_verse_tracker_event_id);
  gw_destroy_source(verse_tracker_event_id);

  // Clear a few flags.
  verse_tracker_on = false;

  // Destroy the signalling buttons.
  gtk_widget_destroy(new_verse_signal);
  new_verse_signal = NULL;
  gtk_widget_destroy(new_styles_signal);
  new_styles_signal = NULL;
  gtk_widget_destroy(word_double_clicked_signal);
  gtk_widget_destroy(reload_signal);
  gtk_widget_destroy(changed_signal);
  gtk_widget_destroy(quick_references_button);

  // Destroy the texttag tables.
  g_object_unref(texttagtable);

  */
  // Destroy the sourceview.
  gtk_widget_destroy(scrolledwindow);

  // Destroy possible highlight object.
  // Todo if (highlight)
    // Todo delete highlight;
}

void USFMView::book_set(unsigned int book_in)
{
  book = book_in;
}


void USFMView::chapter_load(unsigned int chapter_in, vector < ustring > *lines_in)
// Loads a chapter with the number "chapter_in".
// If "lines_in" exists, it load these instead of getting the chapter.
{
  // Clear the undo buffer.
  gtk_source_buffer_set_max_undo_levels (sourcebuffer, 0);
  gtk_source_buffer_set_max_undo_levels (sourcebuffer, -1);

  // No recording of undoable actions.
  gtk_source_buffer_begin_not_undoable_action (sourcebuffer);

  // Restart the verse tracker.
  // Todo restart_verse_tracker();

  // Get rid of possible previous text.
  gtk_text_buffer_set_text(GTK_TEXT_BUFFER (sourcebuffer), "", -1);

  // Save chapter number.
  chapter = chapter_in;

  // Load text in memory.
  vector <ustring> lines;
  if (lines_in)
    lines = *lines_in;
  else
    lines = project_retrieve_chapter(project, book, chapter);

  // Settings.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);

  // Deal with (non-)editable.
  bool editable = true;
  if (lines.empty())
    editable = false;
  if (!projectconfig->editable_get())
    editable = false;
  if (lines_in)
    editable = false;
  gtk_text_view_set_editable(GTK_TEXT_VIEW(sourceview), editable);

  // Make one string containing the whole chapter.
  ustring line;
  for (unsigned int i = 0; i < lines.size(); i++) {
    line.append(lines[i]);
    line.append("\n");
  }

  // Load text in the view.
  // Load it at the position of the text insertion point marker.
  // Because the buffer has just been cleared, this marker will always be at the end.
  gtk_text_buffer_set_text (GTK_TEXT_BUFFER (sourcebuffer), line.c_str(), -1);

  // Set the buffer non-modified.
  gtk_text_buffer_set_modified(GTK_TEXT_BUFFER (sourcebuffer), false);

  // Place cursor at start of buffer and scroll to it.
  GtkTextIter iter;
  gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER (sourcebuffer), &iter);
  gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER (sourcebuffer), &iter);
  screen_scroll_to_iterator(GTK_TEXT_VIEW(sourceview), &iter);
  
  // Enable undo manager again.
  gtk_source_buffer_end_not_undoable_action (sourcebuffer);
}


void USFMView::chapter_save()
// Handles saving the chapters.
{
  // Set variables.
  reload_chapter_number = chapter;

  // If the text is not editable, bail out.
  if (!gtk_text_view_get_editable(GTK_TEXT_VIEW(sourceview)))
    return;

  // If the text was not changed, bail out.
  if (!gtk_text_buffer_get_modified(GTK_TEXT_BUFFER (sourcebuffer)))
    return;

  // If the project is empty, bail out.
  if (project.empty())
    return;

  // Get the USFM text.
  ustring chaptertext = get_chapter();

  // If the chapter text is completely empty, 
  // that means that the user has deleted everything.
  // This is interpreted as to mean that the user wishes to delete this chapter.
  // After asking for confirmation, delete the chapter.
  bool reload = false;
  bool save_action_is_over = false;
  if (chaptertext.empty()) {
    if (gtkw_dialog_question(NULL, "The chapter is empty.\n" "Do you wish to delete this chapter?", GTK_RESPONSE_YES) == GTK_RESPONSE_YES) {
      project_remove_chapter(project, book, chapter);
      save_action_is_over = true;
      reload = true;
      if (chapter > 0)
        reload_chapter_number = chapter - 1;
    }
  }

  // If the text has not yet been dealt with, save it.  
  if (!save_action_is_over) {
    // Clean it up a bit and divide it into lines.
    ParseLine parseline(trim(chaptertext));
    // Add verse information.
    CategorizeChapterVerse ccv(parseline.lines);
    /*
       We have noticed people editing Bibledit's data directly. 
       If this was done with OpenOffice, and when saving it as text again, 
       three special bytes were added right at the beginning of the file, 
       making the \c x marker not being parsed correctly.
       It would then look as if this is chapter 0.
       In addition to this, the user could have edited the chapter number.
       If a change in the chapter number is detected, ask the user what to do.
     */
    unsigned int chapter_in_text = chapter;
    for (unsigned int i = 0; i < ccv.chapter.size(); i++) {
      if (ccv.chapter[i] != chapter) {
        chapter_in_text = ccv.chapter[i];
      }
    }
    // Ask what to do if the chapter number in the text differs from the 
    // chapter that has been loaded.
    if (chapter_in_text != chapter) {
      unsigned int confirmed_chapter_number;
      ustring message;
      message = "Chapter " + convert_to_string(chapter) + " was loaded, " "but it appears that the chapter number has been changed to " + convert_to_string(chapter_in_text) + ".\n" "Do you wish to save the text as a different chapter, that is, as chapter " + convert_to_string(chapter_in_text) + "?";
      if (gtkw_dialog_question(NULL, message.c_str()) == GTK_RESPONSE_YES) {
        confirmed_chapter_number = chapter_in_text;
        reload = true;
        reload_chapter_number = chapter_in_text;
      } else {
        confirmed_chapter_number = chapter;
      }
      for (unsigned int i = 0; i < ccv.chapter.size(); i++) {
        ccv.chapter[i] = confirmed_chapter_number;
      }
      // Check whether the new chapter number already exists.
      if (confirmed_chapter_number != chapter) {
        vector < unsigned int >chapters = project_get_chapters(project, book);
        set < unsigned int >chapter_set(chapters.begin(), chapters.end());
        if (chapter_set.find(confirmed_chapter_number) != chapter_set.end()) {
          message = "The new chapter number already exists\n" "Do you wish to overwrite it?";
          if (gtkw_dialog_question(NULL, message.c_str()) == GTK_RESPONSE_NO) {
            gtkw_dialog_info(NULL, "The changes have been discarded");
            save_action_is_over = true;
            reload = true;
          }
        }
      }
    }
    // Store chapter in database.
    if (!save_action_is_over)
      project_store_chapter(project, book, ccv);
    save_action_is_over = true;
  }
  // Set the buffer non-modified.
  gtk_text_buffer_set_modified(GTK_TEXT_BUFFER (sourcebuffer), false);

  // Possible reload signal.
  if (reload) {
    // Todo gtk_button_clicked(GTK_BUTTON(reload_signal));
  }

}

ustring USFMView::get_chapter()
{
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER (sourcebuffer), &startiter);
  gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER (sourcebuffer), &enditer);
  gchar *text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER (sourcebuffer), &startiter, &enditer, true);
  ustring chaptertext (text);
  g_free(text);
  replace_text(chaptertext, "  ", " ");
  return chaptertext;
}

