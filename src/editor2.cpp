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
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


#include "utilities.h"
#include <glib.h>
#include "editor2.h"
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
#include "progresswindow.h"
#include "dialogyesnoalways.h"


/*


Text editor with a formatted view and undo and redo.


The following elements of text have their own textview and textbuffer:
- every paragraph
- every cell of a table.
- every note.

While the user edits text, signal handlers are connected so it can be tracked what is being edited.
The signal handlers call routines that make EditorActions out of this, and optionally correct the text being typed.
The EditorActions, when played back, will have the same effect as if the user typed.
If Undo is done, then the last EditorAction is undone. It can also be Redone.

*/


Editor2::Editor2(GtkWidget * vbox_in, const ustring & project_in):
current_reference(0, 1000, "")
{
  // Save and initialize variables.
  project = project_in;
  do_not_process_child_anchors_being_deleted = false;
  texttagtable = NULL;
  record_undo_level = 0;
  previous_hand_cursor = false;
  highlight = NULL;
  editable = false;
  event_id_show_quick_references = 0;
  book = 0;
  chapter = 0;
  texview_to_textview_old = NULL;
  texview_to_textview_new = NULL;
  textview_to_textview_offset = 0;
  start_verse_tracker_event_id = 0;
  verse_tracker_event_id = 0;
  verse_tracker_on = false;
  verse_restarts_paragraph = false;
  textbuffer_changed_event_id = 0;
  redo_counter = 0;
  focused_textview_identifier = 0;
    
  // Create data that is needed for any of the possible formatted views.
  create_or_update_formatting_data();

  // Spelling checker.
  spellingchecker = new SpellingChecker(texttagtable);
  g_signal_connect((gpointer) spellingchecker->check_signal, "clicked", G_CALLBACK(on_button_spelling_recheck_clicked), gpointer(this));
  load_dictionaries();

  // The basic GUI, which actually is empty until text will be loaded in it.
  scrolledwindow_v2 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow_v2);
  gtk_box_pack_start(GTK_BOX(vbox_in), scrolledwindow_v2, true, true, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow_v2), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  viewport_v2 = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport_v2);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_v2), viewport_v2);
  
  vbox_v2 = gtk_vbox_new (false, 0);
  gtk_widget_show(vbox_v2);
  gtk_container_add (GTK_CONTAINER (viewport_v2), vbox_v2);

  last_focused_widget = vbox_v2;






  /*
  // The scrolled window that contains the main formatted view.
  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow);
  gtk_box_pack_start(GTK_BOX(vbox_in), scrolledwindow, true, true, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  // A textbuffer to store all text.
  textbuffer = gtk_text_buffer_new(texttagtable);

  // The buffer's signal handlers.
  // g_signal_connect(G_OBJECT(textbuffer), "insert-text", G_CALLBACK(on_buffer_insert_text_before), gpointer(this));
  //g_signal_connect_after(G_OBJECT(textbuffer), "insert-text", G_CALLBACK(on_buffer_insert_text_after), gpointer(this));
  //g_signal_connect(G_OBJECT(textbuffer), "delete-range", G_CALLBACK(on_buffer_delete_range_before), gpointer(this));
  //g_signal_connect_after(G_OBJECT(textbuffer), "delete-range", G_CALLBACK(on_buffer_delete_range_after), gpointer(this));
  //g_signal_connect(G_OBJECT(textbuffer), "apply-tag", G_CALLBACK(on_buffer_apply_tag), gpointer(this));
  //g_signal_connect(G_OBJECT(textbuffer), "remove-tag", G_CALLBACK(on_buffer_remove_tag), gpointer(this));
  //g_signal_connect(G_OBJECT(textbuffer), "insert-child-anchor", G_CALLBACK(on_buffer_insert_child_anchor), gpointer(this));
  //g_signal_connect(G_OBJECT(textbuffer), "insert-pixbuf", G_CALLBACK(on_buffer_insert_pixbuf), gpointer(this));
  //g_signal_connect(G_OBJECT(textbuffer), "changed", G_CALLBACK(on_textbuffer_changed), gpointer(this));

  // A text view to display the buffer.
  textview = gtk_text_view_new_with_buffer(textbuffer);
  gtk_widget_show(textview);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), textview);
  gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(textview), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), FALSE);
  gtk_text_view_set_left_margin(GTK_TEXT_VIEW(textview), 5);

  // The view's signal handlers.
  spellingchecker->attach(textview);
  g_signal_connect_after((gpointer) textview, "move_cursor", G_CALLBACK(on_textview_move_cursor), gpointer(this));
  g_signal_connect((gpointer) textview, "motion-notify-event", G_CALLBACK(on_text_motion_notify_event), gpointer(this));
  g_signal_connect((gpointer) textview, "event-after", G_CALLBACK(on_text_event_after), gpointer(this));
  g_signal_connect((gpointer) textview, "key-press-event", G_CALLBACK(text_key_press_event_before), gpointer(this));
  g_signal_connect_after((gpointer) textview, "key-press-event", G_CALLBACK(text_key_press_event_after), gpointer(this));
  g_signal_connect((gpointer) textview, "visibility-notify-event", G_CALLBACK(screen_visibility_notify_event), gpointer(this));
  g_signal_connect((gpointer) textview, "button_press_event", G_CALLBACK(on_textview_button_press_event), gpointer(this));
  g_signal_connect((gpointer) textview, "size-allocate", G_CALLBACK(on_related_widget_size_allocated), gpointer(this));

  // Initialize the last focused textview to the main textview.
  //last_focused_widget = textview;
  */

  // Buttons to give signals.
  new_verse_signal = gtk_button_new();
  new_styles_signal = gtk_button_new();
  word_double_clicked_signal = gtk_button_new();
  reload_signal = gtk_button_new();
  changed_signal = gtk_button_new();
  quick_references_button = gtk_button_new();
  spelling_checked_signal = gtk_button_new ();

  // Initialize a couple of event ids.
  textview_cursor_moved_delayer_event_id = 0;
  grab_focus_event_id = 0;
  spelling_timeout_event_id = 0;

  // Tag for highlighting search words.
  // Note that for convenience the GtkTextBuffer function is called. 
  // But this adds the reference to the GtkTextTagTable, so making it available
  // to any other buffer that uses the same text tag table.
  //reference_tag = gtk_text_buffer_create_tag(textbuffer, NULL, "background", "khaki", NULL);

  // Highlighting searchwords timeout.
  highlight_timeout_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 500, GSourceFunc(on_highlight_timeout), gpointer(this), NULL);

  // Automatic saving of the file, periodically.
  save_timeout_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 60000, GSourceFunc(on_save_timeout), gpointer(this), NULL);

  // Fonts.
  //set_font();

  // Grab focus.
  focus_programmatically_being_grabbed = false;
  //gtk_widget_grab_focus(textview);
}


Editor2::~Editor2()
{
  // Save the chapter.
  chapter_save();

  // Destroy a couple of timeout sources.
  gw_destroy_source(textview_cursor_moved_delayer_event_id);
  gw_destroy_source(grab_focus_event_id);
  gw_destroy_source(save_timeout_event_id);
  gw_destroy_source(highlight_timeout_event_id);
  gw_destroy_source(spelling_timeout_event_id);
  gw_destroy_source(event_id_show_quick_references);
  gw_destroy_source(start_verse_tracker_event_id);
  gw_destroy_source(verse_tracker_event_id);
  gw_destroy_source(textbuffer_changed_event_id);

  // Clear a few flags.
  verse_tracker_on = false;

  // Delete speller.
  delete spellingchecker;

  // Destroy the signalling buttons.
  gtk_widget_destroy(new_verse_signal);
  new_verse_signal = NULL;
  gtk_widget_destroy(new_styles_signal);
  new_styles_signal = NULL;
  gtk_widget_destroy(word_double_clicked_signal);
  gtk_widget_destroy(reload_signal);
  gtk_widget_destroy(changed_signal);
  gtk_widget_destroy(quick_references_button);
  gtk_widget_destroy (spelling_checked_signal);

  // Destroy the texttag tables.
  g_object_unref(texttagtable);

  // Destroy the text area.
  gtk_widget_destroy(scrolledwindow_v2);

  // Destroy possible highlight object.
  if (highlight)
    delete highlight;
    
  // Destroy any editor actions.
  clear_and_destroy_editor_actions (actions_done);
  clear_and_destroy_editor_actions (actions_redoable);
  
}


void Editor2::book_set(unsigned int book_in)
{
  book = book_in;
}


void Editor2::chapter_load(unsigned int chapter_in)
// Loads a chapter with the number "chapter_in".
{
  // No recording of undoable actions while this object is alive.
  //PreventEditorUndo preventundo(&record_undo_level);

  // Clear the stacks of actions done and redoable.
  clear_and_destroy_editor_actions (actions_done);
  clear_and_destroy_editor_actions (actions_redoable);

  // Restart the verse tracker.
  restart_verse_tracker();

  // Save chapter number.
  chapter = chapter_in;

  // Settings.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);

  // Load text in memory.
  vector <ustring> lines = project_retrieve_chapter(project, book, chapter);

  // Whether chapter is editable.
  editable = true;
  if (lines.empty())
    editable = false;
  if (!projectconfig->editable_get())
    editable = false;

  // Get rid of possible previous widgets with their data.
  gtk_container_foreach(GTK_CONTAINER(vbox_v2), on_container_tree_callback_destroy, gpointer(this));
  focused_textview_identifier = 0;

  // Make one long line containing the whole chapter.
  // This is done so as to exclude any possibility that the editor does not
  // properly load a possible chapter that has line-breaks at unexpected places.
  // Add one space to the end so as to accomodate situation such as that the 
  // last marker is "\toc". Without that extra space it won't see this marker,
  // because the formatter looks for "\toc ". The space will solve the issue.
  ustring line;
  for (unsigned int i = 0; i < lines.size(); i++) {
    if (!line.empty())
      line.append(" ");
    line.append(lines[i]);
  }
  line.append(" ");

  // Load in editor.
  text_load (line);

  // Clear undo buffer.
  //snapshots.clear();
  
  // Set the buffer(s) non-modified.
  //textbuffers_set_unmodified(textbuffer, editornotes, editortables);

  // Place cursor at the start and scroll it onto the screen.
  //GtkTextIter iter;
  //gtk_text_buffer_get_start_iter(textbuffer, &iter);
  //gtk_text_buffer_place_cursor(textbuffer, &iter);
  //scroll_cursor_on_screen ();
}


void Editor2::text_load (ustring text) // Todo
{
  // Clean away possible new lines.
  replace_text (text, "\n", " ");

  // If there's no textview to start with create a new one.
  if (focused_textview_identifier == 0) {
    EditorActionCreateParagraph * action = new EditorActionCreateParagraph (0);
    apply_editor_action (action);
  }

  // Create editor actions out of this text and apply these.
  ustring character_mark;
  ustring marker;
  size_t marker_pos;
  size_t marker_length;
  bool is_opener;
  bool marker_found;
  while (!text.empty()) {
    vector <EditorAction *> editoractions;
    marker_found = usfm_search_marker(text, marker, marker_pos, marker_length, is_opener);
    bool handled = false;
    /*
    if (!handled) {
      if (create_editor_objects_for_text_table_raw                (project, last_focused_textview_v2, text, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found)) {
        handled = true;
      }
    }
    */
    if (!handled) {
      if (create_editor_objects_starting_new_paragraph (editoractions, text, character_mark, marker, marker_pos, marker_length, is_opener, marker_found)) {
        handled = true;
      }
    }
    /*
    if (!handled) {
      if (create_editor_objects_for_text_starting_character_style (project, last_focused_textview_v2, text, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found)) {
        handled = true;
      }
    }
    if (!handled) {
      if (create_editor_objects_for_text_ending_character_style   (project, last_focused_textview_v2, text, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found)) {
        handled = true;
      }
    }
    if (!handled) {
      if (create_editor_objects_for_text_verse_number             (editoractions, project, last_focused_textview_v2, text, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found)) {
        handled = true;
      }
    }
    if (!handled) {
      if (create_editor_objects_for_text_note_raw                 (project, last_focused_textview_v2, text, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found)) {
        handled = true;
      }
    }
    */
    if (!handled) {
      create_editor_objects_fallback (editoractions, text, character_mark, marker_pos, marker_found);
    }
    for (unsigned int i = 0; i < editoractions.size(); i++) {
      apply_editor_action (editoractions[i]);
    }
  }

  // Update gui for styles.
  signal_if_styles_changed();

  // Trigger a spelling check.
  spelling_trigger();
}


void Editor2::chapter_save()
// Handles saving the chapters.
{
  /*
  // Set variables.
  reload_chapter_number = chapter;

  // If the text is not editable, bail out.
  if (!gtk_text_view_get_editable(GTK_TEXT_VIEW(textview)))
    return;

  // If the text was not changed, bail out.
  if (!textbuffers_get_modified(textbuffer, editornotes, editortables))
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
  // Temporal code to compare editor's input and output.
  */
  /*
     {
     chaptertext.append ("\n");
     ustring filename = directories_get_temp () + "/chapter-save.txt";
     g_file_set_contents (filename.c_str (), chaptertext.c_str (), -1, NULL);
     cout << "start compare" << endl;
     GwSpawn spawn ("diff");
     spawn.arg (project_data_filename_chapter (project, book, chapter, false));
     spawn.arg (directories_get_temp () + "/chapter-save.txt");
     spawn.run ();
     cout << "end compare" << endl;
     if (spawn.exitstatus != 0) {
     cout << "start of text" << endl;
     cout << chaptertext << endl;
     cout << "end of text" << endl;
     }
     }
   */
  /*
  // If the text has not yet been dealt with, save it.  
  if (!save_action_is_over) {
    // Clean it up a bit and divide it into lines.
    ParseLine parseline(trim(chaptertext));
    // Add verse information.
    CategorizeChapterVerse ccv(parseline.lines);
    */
    /*
       We have noticed people editing Bibledit's data directly. 
       If this was done with OpenOffice, and then saving it as text again, 
       three special bytes were added right at the beginning of the file, 
       making the \c x marker not being parsed correctly. 
       It would then look as if this is chapter 0.
       In addition to this, the user could have edited the chapter number.
       If a change in the chapter number is detected, ask the user what to do.
     */
    /*
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
    // Store chapter.
    if (!save_action_is_over)
      project_store_chapter(project, book, ccv);
    save_action_is_over = true;
  }
  // Set the buffer(s) non-modified.
  textbuffers_set_unmodified(textbuffer, editornotes, editortables);

  // Possible reload signal.
  if (reload) {
    gtk_button_clicked(GTK_BUTTON(reload_signal));
  }
  */
}


ustring Editor2::text_get_selection()
// Retrieves the selected text from the textview that has the focus, 
// and returns it as USFM code.
{
  // Variable to hold the USFM text that is going to be produced.
  ustring text;

  /*
  // Get the iterators at the selection bounds of the main textview or note or 
  // table, whichever has the focus. Get the text too.
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_selection_bounds(textbuffer, &startiter, &enditer);
  if (gtk_widget_is_focus(textview)) {
    gtk_text_buffer_get_selection_bounds(textbuffer, &startiter, &enditer);
    usfm_get_text(textbuffer, startiter, enditer, &editornotes, &editortables, project, text, verse_restarts_paragraph);
  }
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    if (gtk_widget_is_focus(editornotes[i].textview)) {
      GtkTextBuffer *buffer = editornotes[i].textbuffer;
      gtk_text_buffer_get_selection_bounds(buffer, &startiter, &enditer);
      usfm_get_note_text(editornotes[i], startiter, enditer, project, text);
    }
  }
  for (unsigned int i = 0; i < editortables.size(); i++) {
    for (unsigned int row = 0; row < editortables[i].textviews.size(); row++) {
      for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
        if (gtk_widget_is_focus(table_cell_get_view(editortables[i], row, column))) {
          GtkTextBuffer *buffer = table_cell_get_buffer(editortables[i], row, column);
          gtk_text_buffer_get_selection_bounds(buffer, &startiter, &enditer);
          usfm_get_text(buffer, startiter, enditer, NULL, NULL, project, text, false);
        }
      }
    }
  }
  */
  
  // Return the text.
  return text;
}


void Editor2::text_erase_selection()
// Erases the selected text from the textview that has the focus.
{
  /*
  // Get the iterators at the selection bounds of the main textview or note or 
  // table, whichever has the focus. Erase that bit.
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_selection_bounds(textbuffer, &startiter, &enditer);
  if (gtk_widget_is_focus(textview)) {
    gtk_text_buffer_get_selection_bounds(textbuffer, &startiter, &enditer);
    gtk_text_buffer_delete(textbuffer, &startiter, &enditer);
  }
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    if (gtk_widget_is_focus(editornotes[i].textview)) {
      GtkTextBuffer *buffer = editornotes[i].textbuffer;
      gtk_text_buffer_get_selection_bounds(buffer, &startiter, &enditer);
      gtk_text_buffer_delete(buffer, &startiter, &enditer);
    }
  }
  for (unsigned int i = 0; i < editortables.size(); i++) {
    for (unsigned int row = 0; row < editortables[i].textviews.size(); row++) {
      for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
        if (gtk_widget_is_focus(table_cell_get_view(editortables[i], row, column))) {
          GtkTextBuffer *buffer = table_cell_get_buffer(editortables[i], row, column);
          gtk_text_buffer_get_selection_bounds(buffer, &startiter, &enditer);
          gtk_text_buffer_delete(buffer, &startiter, &enditer);
        }
      }
    }
  }
  */
}


void Editor2::text_insert(ustring text)
// This inserts plain or USFM text at the cursor location of the focused textview.
// If text is selected, this is erased first.
{
  /*
  // If the text is not editable, bail out.
  if (!editable)
    return;

  // Store scrollled window's position.
  GtkAdjustment * adjustment = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolledwindow));
  unsigned int scroll_position = gtk_adjustment_get_value (adjustment);

  // Get the textbuffer that is focused.
  GtkTextBuffer *buffer = textbuffer;
  if (gtk_widget_is_focus(textview)) {
    buffer = textbuffer;
  }
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    if (gtk_widget_is_focus(editornotes[i].textview)) {
      buffer = editornotes[i].textbuffer;
    }
  }
  for (unsigned int i = 0; i < editortables.size(); i++) {
    for (unsigned int row = 0; row < editortables[i].textviews.size(); row++) {
      for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
        if (gtk_widget_is_focus(table_cell_get_view(editortables[i], row, column))) {
          buffer = table_cell_get_buffer(editortables[i], row, column);
        }
      }
    }
  }

  // Erase selected text.
  gtk_text_buffer_delete_selection(buffer, true, editable);

  // Move the cursor to the insert position.
  GtkTextIter insert_iter;
  gtk_text_buffer_get_iter_at_mark(buffer, &insert_iter, gtk_text_buffer_get_insert(buffer));
  gtk_text_buffer_place_cursor(buffer, &insert_iter);

  // If makes a difference if pasting into the main textbuffer, or into a table or note.
  if (buffer == textbuffer) {
    
    // Inserting text into the main text buffer.
    // Formatting can be complex. For that reason the raw USFM code is processed and reloaded.
    
    // Get the offset of the cursor in the editor.
    gint cursor_offset = gtk_text_iter_get_offset (&insert_iter);  

    // Join the inserted text with the existing text.
    character_style_on_start_typing.clear ();
    style_to_be_applied_at_cursor.clear ();
    PreventEditorUndo * preventundo = new PreventEditorUndo (&record_undo_level);
    #define ANCHOR "_ANCHOR_"
    gtk_text_buffer_insert_at_cursor (buffer, ANCHOR, -1);
    ustring text2 = get_chapter();
    size_t pos = text2.find (ANCHOR);
    if (pos != string::npos) {
      text2.erase (pos, strlen (ANCHOR));
      text2.insert (pos, text);
    }
    #undef ANCHOR
    text_load (text2);
    delete preventundo;
    trigger_undo_redo_recording ();
   
    // Restore cursor position.
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_offset (textbuffer, &iter, cursor_offset);
    gtk_text_buffer_place_cursor (textbuffer, &iter);

    // Restore scrolled window's position.
    gtk_adjustment_set_value (adjustment, scroll_position);
    
  } else {
    
    // Inserting text into a table or note.
    // This one simply inserts the plain text at the cursor. No formatting used.
    // This prevents scrolling and cursor positioning problems that would otherwise occur.
    
    // Remove all markers to prevent trouble with markup.
    if (buffer != textbuffer) {
      ustring marker;
      size_t marker_pos;
      size_t marker_length;
      bool is_opener;
      while (usfm_search_marker(text, marker, marker_pos, marker_length, is_opener)) {
        text.erase(marker_pos, marker_length);
      }
    }
    
    // Insert the text.
    gtk_text_buffer_insert_at_cursor (buffer, text.c_str(), -1);
    
  }
  */
}

void Editor2::show_quick_references()
// Starts the process to show the quick references.
// A delaying routine is used to make the program more responsive.
// That is, the quick references are not shown at each change,
// but only shortly after. 
// Without this pasting a long text in the footnote takes a lot of time.
{
  gw_destroy_source(event_id_show_quick_references);
  event_id_show_quick_references = g_timeout_add_full(G_PRIORITY_DEFAULT, 100, GSourceFunc(show_quick_references_timeout), gpointer(this), NULL);
}

bool Editor2::show_quick_references_timeout(gpointer user_data)
{
  ((Editor2 *) user_data)->show_quick_references_execute();
  return false;
}


void Editor2::show_quick_references_execute()
// Takes the text of the references in the note that has the cursor,
// and shows that text in the quick reference area.
{
  // Clear the event id.
  event_id_show_quick_references = 0;
  
  // If we're not in a note, bail out.
  if (last_focused_type() != etvtNote)
    return;

  // Get the text of the focused note.
  GtkTextBuffer *note_buffer = last_focused_textbuffer();
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_start_iter(note_buffer, &startiter);
  gtk_text_buffer_get_end_iter(note_buffer, &enditer);
  ustring note_text = gtk_text_buffer_get_text(note_buffer, &startiter, &enditer, true);

  // Get the language of the project.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  ustring language = projectconfig->language_get();

  // Extract references.
  ReferencesScanner refscanner(language, book, note_text);

  // If there are no references, bail out.
  if (refscanner.references.empty()) {
    return;
  }

  // Ask whether the references should be made available.
  if (yes_no_always_dialog ("This note has references.\nWould you like to load these in the references list?", ynadtLoadReferences, false, true)) {
    // Make the references available and fire a signal.
    quick_references = refscanner.references;
    gtk_button_clicked(GTK_BUTTON(quick_references_button));
  }
}


void Editor2::on_textview_move_cursor(GtkTextView * textview, GtkMovementStep step, gint count, gboolean extend_selection, gpointer user_data)
{
  ((Editor2 *) user_data)->on_textview_cursor_moved_delayer(textview, step, count);
}

void Editor2::on_textview_cursor_moved_delayer(GtkTextView * textview, GtkMovementStep step, gint count)
{
  // Clear the character style that was going to be applied when the user starts typing.
  character_style_on_start_typing.clear();
  // Keep postponing the actual handler if a new cursor movement was detected before the previous one was processed.
  gw_destroy_source(textview_cursor_moved_delayer_event_id);
  textview_cursor_moved_delayer_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 100, GSourceFunc(on_textview_cursor_moved_delayer_handler), gpointer(this), NULL);
  // Store data about the move for finding out whether to move to another textview.
  texview_to_textview_new = textview;
  textview_to_textview_steptype = step;
  textview_to_textview_stepcount = count;
}

bool Editor2::on_textview_cursor_moved_delayer_handler(gpointer user_data)
{
  ((Editor2 *) user_data)->on_textview_cursor_moved();
  return false;
}

void Editor2::on_textview_cursor_moved()
// Handle the administration if the cursor moved.
{
  textview_cursor_moved_delayer_event_id = 0;
  signal_if_styles_changed();
  check_move_textview_to_textview();
}

ustring Editor2::verse_number_get()
// Returns the verse number the cursor is in.
{
  return "0";
  /*
  // Get an iterator at the cursor location of the main textview.
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_mark(textbuffer, &iter, gtk_text_buffer_get_insert(textbuffer));
  if (gtk_widget_is_focus(textview)) {
    gtk_text_buffer_get_iter_at_mark(textbuffer, &iter, gtk_text_buffer_get_insert(textbuffer));
  } else {
    // If the cursor is in a footnote, or in a table, 
    // then the iterator is retrieved from another location in the main textview.
    // This location corresponds to the verse that the footnote refers to or that the table is in.
    for (unsigned int i = 0; i < editornotes.size(); i++) {
      if (gtk_widget_is_focus(editornotes[i].textview)) {
        gtk_text_buffer_get_iter_at_child_anchor(textbuffer, &iter, editornotes[i].childanchor_caller_text);
      }
    }
    for (unsigned int i = 0; i < editortables.size(); i++) {
      for (unsigned int row = 0; row < editortables[i].textviews.size(); row++) {
        for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
          if (gtk_widget_is_focus(table_cell_get_view(editortables[i], row, column))) {
            gtk_text_buffer_get_iter_at_child_anchor(textbuffer, &iter, editortables[i].childanchor);
          }
        }
      }
    }
  }

  // Get and return the verse.
  ustring verse_marker = style_get_verse_marker(project);
  return get_verse_number_at_iterator(iter, verse_marker, project);
  */
}


void Editor2::on_textview_grab_focus(GtkWidget * widget, gpointer user_data)
{
  ((Editor2 *) user_data)->textview_grab_focus(widget);
}


void Editor2::textview_grab_focus(GtkWidget * widget)
{
  // Retrieve the identifier that belongs to the widget, and store it.
  focused_textview_identifier = textview2identifier (widget);
  /*
  // Clear the character style that was going to be applied when the user starts typing.
  character_style_on_start_typing.clear();
  // Further processing of the focus grab is done with a delay.
  gw_destroy_source(grab_focus_event_id);
  grab_focus_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 10, GSourceFunc(on_grab_focus_delayer_timeout), gpointer(this), NULL);
  */
}


bool Editor2::on_grab_focus_delayer_timeout(gpointer data)
{
  ((Editor2 *) data)->on_grab_focus_delayed_handler();
  return false;
}


void Editor2::on_grab_focus_delayed_handler()
/*
 If the user clicks in the editor window, 
 and straight after that the position of the cursor is requested, 
 we get the position where the cursor was prior to clicking. 
 This delayed handler solves that.
 */
{
  /*
  grab_focus_event_id = 0;
  signal_if_styles_changed();
  if (recording_undo_actions()) {
    show_quick_references();
  }
  // If the user clicks on a note caller, then the focus will move to or from 
  // that note. This functionality is processed below.
  if (child_anchor_clicked) {
    // Go through the available notes and if the anchor agrees, 
    // focus the appropriate textview and scroll to it.
    for (unsigned int i = 0; i < editornotes.size(); i++) {
      if (child_anchor_clicked == editornotes[i].childanchor_caller_text) {
        gtk_widget_grab_focus(editornotes[i].textview);
        GtkTextIter iter;
        gtk_text_buffer_get_start_iter(editornotes[i].textbuffer, &iter);
        gtk_text_buffer_place_cursor(editornotes[i].textbuffer, &iter);
        gtk_text_buffer_get_iter_at_child_anchor(textbuffer, &iter, editornotes[i].childanchor_caller_note);
        gtk_text_buffer_place_cursor(textbuffer, &iter);
        scroll_cursor_on_screen ();
      }
      if (child_anchor_clicked == editornotes[i].childanchor_caller_note) {
        gtk_widget_grab_focus(textview);
        GtkTextIter iter;
        gtk_text_buffer_get_iter_at_child_anchor(textbuffer, &iter, editornotes[i].childanchor_caller_text);
        gtk_text_buffer_place_cursor(textbuffer, &iter);
        scroll_cursor_on_screen ();
      }
    }
    child_anchor_clicked = NULL;
  }
  */
}


void Editor2::programmatically_grab_focus(GtkWidget * widget)
{
  focus_programmatically_being_grabbed = true;
  gtk_widget_grab_focus(widget);
  focus_programmatically_being_grabbed = false;
}


void Editor2::undo()
{
  // Calculate snapshot to revert to.
  // The last snapshot always has the current state.
  // So it needs to revert to the last but one snapshot.
  int snapshot_pointer = snapshots.size() - 2 -  redo_counter;

  // Bail out if nothing to undo.
  if (snapshot_pointer < 0)
    return;

  // Restore the snapshot.
  restore_snapshot (snapshot_pointer, true);

  // It undid one snapshot, so the user can redo it again if needed.
  redo_counter++;  
}


void Editor2::redo()
{
  // It's going to redo one snapshot.
  if (redo_counter)
    redo_counter--;
  
  // Calculate snapshot to re-apply.
  int snapshot_pointer = snapshots.size() - 1 - redo_counter;
  
  // Bail out if nothing to redo.
  if (snapshot_pointer < 0)
    return;
  if ((unsigned int) snapshot_pointer >= snapshots.size())
    return;
    
  // Restore the snapshot.
  restore_snapshot (snapshot_pointer, false);
}


bool Editor2::can_undo()
{
  int snapshot_pointer = snapshots.size() - 2 -  redo_counter;
  return (snapshot_pointer >= 0);
}


bool Editor2::can_redo()
{
  return redo_counter;
}


void Editor2::set_font()
{
  /*
  // Get a list of all textviews that make up the editor.
  vector < GtkWidget * >textviews;
  textviews.push_back(textview);
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    if (editornotes[i].textview) {
      textviews.push_back(editornotes[i].textview);
    }
  }
  for (unsigned int i = 0; i < editortables.size(); i++) {
    for (unsigned int row = 0; row < editortables[i].textviews.size(); row++) {
      for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
        textviews.push_back(table_cell_get_view(editortables[i], row, column));
      }
    }
  }

  // Set font.
  PangoFontDescription *font_desc = NULL;
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  if (!projectconfig->editor_font_default_get()) {
    font_desc = pango_font_description_from_string(projectconfig->editor_font_name_get().c_str());
  }
  for (unsigned int i = 0; i < textviews.size(); i++) {
    gtk_widget_modify_font(textviews[i], font_desc);
  }
  if (font_desc)
    pango_font_description_free(font_desc);

  // Set the colours.
  for (unsigned int i = 0; i < textviews.size(); i++) {
    if (projectconfig->editor_default_color_get()) {
      color_widget_default(textviews[i]);
    } else {
      color_widget_set(textviews[i], projectconfig->editor_normal_text_color_get(), projectconfig->editor_background_color_get(), projectconfig->editor_selected_text_color_get(), projectconfig->editor_selection_color_get());
    }
  }

  // Set predominant text direction.
  for (unsigned int i = 0; i < textviews.size(); i++) {
    if (projectconfig->right_to_left_get()) {
      gtk_widget_set_direction(textviews[i], GTK_TEXT_DIR_RTL);
    } else {
      gtk_widget_set_direction(textviews[i], GTK_TEXT_DIR_LTR);
    }
  }
  */
}


bool Editor2::on_save_timeout(gpointer data)
{
  return ((Editor2 *) data)->save_timeout();
}


bool Editor2::save_timeout()
{
  chapter_save();
  return true;
}


gboolean Editor2::on_text_motion_notify_event(GtkWidget * textview, GdkEventMotion * event, gpointer user_data)
{
  return ((Editor2 *) user_data)->text_motion_notify_event(textview, event);
}


gboolean Editor2::text_motion_notify_event(GtkWidget * textview, GdkEventMotion * event)
// Update the cursor image if the pointer moved. 
{
  gint x, y;
  gtk_text_view_window_to_buffer_coords(GTK_TEXT_VIEW(textview), GTK_TEXT_WINDOW_WIDGET, gint(event->x), gint(event->y), &x, &y);
  GtkTextIter iter;
  gtk_text_view_get_iter_at_location(GTK_TEXT_VIEW(textview), &iter, x, y);
  GtkTextChildAnchor *anchor = gtk_text_iter_get_child_anchor(&iter);
  bool hand_cursor = false;
  if (anchor) {
    for (unsigned int i = 0; i < editornotes.size(); i++) {
      if (anchor == editornotes[i].childanchor_caller_text)
        hand_cursor = true;
      if (anchor == editornotes[i].childanchor_caller_note)
        hand_cursor = true;
    }
  }
  if (hand_cursor != previous_hand_cursor) {
    GdkCursor *cursor;
    if (hand_cursor) {
      cursor = gdk_cursor_new(GDK_HAND2);
    } else {
      cursor = gdk_cursor_new(GDK_XTERM);
    }
    gdk_window_set_cursor(gtk_text_view_get_window(GTK_TEXT_VIEW(textview), GTK_TEXT_WINDOW_TEXT), cursor);
  }
  previous_hand_cursor = hand_cursor;
  gdk_window_get_pointer(textview->window, NULL, NULL, NULL);
  return false;
}

void Editor2::on_text_event_after(GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
  ((Editor2 *) user_data)->text_event_after(widget, event);
}

void Editor2::text_event_after(GtkWidget * textview, GdkEvent * ev)
/* Links can also be activated by clicking.
 */
{
  GtkTextIter start, end, iter;
  GtkTextBuffer *buffer;
  GdkEventButton *event;
  gint x, y;
  if (ev->type != GDK_BUTTON_RELEASE)
    return;
  event = (GdkEventButton *) ev;
  if (event->button != 1)
    return;
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
  // We shouldn't follow a link if the user has selected something.
  gtk_text_buffer_get_selection_bounds(buffer, &start, &end);
  if (gtk_text_iter_get_offset(&start) != gtk_text_iter_get_offset(&end))
    return;
  gtk_text_view_window_to_buffer_coords(GTK_TEXT_VIEW(textview), GTK_TEXT_WINDOW_WIDGET, gint(event->x), gint(event->y), &x, &y);
  gtk_text_view_get_iter_at_location(GTK_TEXT_VIEW(textview), &iter, x, y);
  text_edit_if_link(textview, &iter);
}

void Editor2::text_edit_if_link(GtkWidget * textview, GtkTextIter * iter)
/* Looks at all tags covering the position of iter in the text view, 
 * and if one of them is a link, follow it by showing the page identified
 * by the data attached to it.
 */
{
  GSList *tags = NULL, *tagp = NULL;
  tags = gtk_text_iter_get_tags(iter);
  for (tagp = tags; tagp != NULL; tagp = tagp->next) {
    GtkTextTag *tag = (GtkTextTag *) tagp->data;
    gint id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(tag), "id"));
    if (id != 0) {
    }
  }
  if (tags)
    g_slist_free(tags);
}


gboolean Editor2::text_key_press_event_before(GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  return ((Editor2 *) user_data)->on_text_key_press_event_before(widget, event);
}


gboolean Editor2::on_text_key_press_event_before(GtkWidget * widget, GdkEventKey * event)
// Preprocessing of the keyboard events in the text editors.
{
  /*
  if (keyboard_enter_pressed(event)) {
    // No <Enter> in a crossreference.
    if (last_focused_type() == etvtNote) {
      GtkWidget *textview = last_focused_textview();
      for (unsigned int i = 0; i < editornotes.size(); i++) {
        if (textview == editornotes[i].textview) {
          EditorNoteType notetype = note_type_get(project, editornotes[i].marker);
          if (notetype == entCrossreference) {
            return true;
          } else {
            // If <Enter> has been given in a foot/end note, 
            // apply the paragraph continuation style.
            // This is done with a delay to allow the keypress to be fully processed.
            // It was thought at first to use the keypress handler after the
            // event, but it appears that the event is handled and no after event
            // is called anymore.
            style_to_be_applied_at_cursor = style_get_paragraph_note_style(project);
            g_timeout_add(1, GSourceFunc(on_apply_style_at_cursor), gpointer(this));
          }
        }
      }
    }
    // No <Enter> in a table cell.
    if (last_focused_type() == etvtTable) {
      return true;
    }
  }
  // Pressing Page Up while the cursor is in the footnote brings the user
  // to the note caller in the text.
  if (keyboard_page_up_pressed(event)) {
    if (last_focused_type() == etvtNote) {
      GtkTextBuffer *buffer = last_focused_textbuffer();
      for (unsigned int i = 0; i < editornotes.size(); i++) {
        if (buffer == editornotes[i].textbuffer) {
          programmatically_grab_focus(textview);
          GtkTextIter iter;
          gtk_text_buffer_get_iter_at_child_anchor(textbuffer, &iter, editornotes[i].childanchor_caller_text);
          gtk_text_buffer_place_cursor(textbuffer, &iter);
          scroll_cursor_on_screen ();
          break;
        }
      }
    }
  }
  */
  return false;
}

gboolean Editor2::text_key_press_event_after(GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  return ((Editor2 *) user_data)->on_text_key_press_event_after(widget, event);
}

gboolean Editor2::on_text_key_press_event_after(GtkWidget * widget, GdkEventKey * event)
// Postprocessing of the keyboard events in the text editors.
{
  return false;
}

void Editor2::on_textbuffer_footnotes_changed(GtkEditable * editable, gpointer user_data)
{
  ((Editor2 *) user_data)->on_textbuffer_footnotes();
}

void Editor2::on_textbuffer_footnotes()
{
  if (recording_undo_actions()) {
    show_quick_references();
  }
}


gboolean Editor2::on_textview_button_press_event(GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
  ((Editor2 *) user_data)->on_texteditor_click(widget, event);
  return false;
}


void Editor2::on_texteditor_click(GtkWidget * widget, GdkEventButton * event)
{
  /*
  // Double-clicking sends the word to Toolbox.
  if (event->type == GDK_2BUTTON_PRESS) {

    // Get the textbuffer.
    GtkTextBuffer *textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));

    // Get the word.
    GtkTextIter enditer;
    GtkTextIter startiter;
    gtk_text_buffer_get_iter_at_mark(textbuffer, &enditer, gtk_text_buffer_get_insert(textbuffer));
    if (!gtk_text_iter_ends_word(&enditer))
      gtk_text_iter_forward_word_end(&enditer);
    startiter = enditer;
    gtk_text_iter_backward_word_start(&startiter);
    word_double_clicked_text = gtk_text_buffer_get_text(textbuffer, &startiter, &enditer, false);

    // Signal to have it sent to Toolbox.
    gtk_button_clicked(GTK_BUTTON(word_double_clicked_signal));
  }
  // See whether the user clicked on a note caller.
  child_anchor_clicked = NULL;
  if (widget == textview) {
    if (event->type == GDK_BUTTON_PRESS) {
      // Get iterator at clicking location.
      GtkTextIter iter;
      gint x, y;
      gtk_text_view_window_to_buffer_coords(GTK_TEXT_VIEW(widget), GTK_TEXT_WINDOW_WIDGET, gint(event->x), gint(event->y), &x, &y);
      gtk_text_view_get_iter_at_location(GTK_TEXT_VIEW(widget), &iter, x, y);
      // Get the child anchor at the iterator, if there is one.
      child_anchor_clicked = gtk_text_iter_get_child_anchor(&iter);
      // Further processing of this child anchor is done in the focus grabbed handler.
    }
  }
  */
}

void Editor2::create_or_update_formatting_data()
// Create and fill the text tag table for all the formatted views.
// If already there, update it.
{
  // If there is no text tag table, create a new one.
  if (!texttagtable) {
    texttagtable = gtk_text_tag_table_new();
  }

  // Get the stylesheet.
  ustring stylesheet = stylesheet_get_actual ();

  // Get the font size multiplication factor.
  double font_size_multiplier = 1;
  PangoFontDescription *font_desc = NULL;
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  if (!projectconfig->editor_font_default_get()) {
    font_desc = pango_font_description_from_string(projectconfig->editor_font_name_get().c_str());
    gint fontsize = pango_font_description_get_size(font_desc) / PANGO_SCALE;
    font_size_multiplier = (double)fontsize / 12;
    pango_font_description_free(font_desc);
  }
  // Create the unknown style.
  {
    Style style("", unknown_style(), false);
    create_or_update_text_style(&style, false, true, font_size_multiplier);
  }

  // Get all the Styles.
  extern Styles *styles;
  Usfm *usfm = styles->usfm(stylesheet);

  // Handle the known styles.
  // The priorities of the character styles should be higher than those of the 
  // paragraph styles. 
  // Therefore paragraph styles are created first, then the other ones.
  for (unsigned int i = 0; i < usfm->styles.size(); i++) {
    // Get the properties of this style.
    bool paragraphstyle = style_get_paragraph(usfm->styles[i].type, usfm->styles[i].subtype);
    bool plaintext = style_get_plaintext(usfm->styles[i].type, usfm->styles[i].subtype);
    // Create a text style, only paragraph styles.
    if (paragraphstyle)
      create_or_update_text_style(&(usfm->styles[i]), true, plaintext, font_size_multiplier);
  }
  for (unsigned int i = 0; i < usfm->styles.size(); i++) {
    // Get the properties of this style.
    bool paragraphstyle = style_get_paragraph(usfm->styles[i].type, usfm->styles[i].subtype);
    bool plaintext = style_get_plaintext(usfm->styles[i].type, usfm->styles[i].subtype);
    // Create a text style, the non-paragraph styles.
    if (!paragraphstyle)
      create_or_update_text_style(&(usfm->styles[i]), false, plaintext, font_size_multiplier);
  }

  // Special handling for the verse style, whether it restarts the paragraph.
  for (unsigned int i = 0; i < usfm->styles.size(); i++) {
    if (usfm->styles[i].type == stVerseNumber) {
      verse_restarts_paragraph = usfm->styles[i].userbool1;
    }
  }
}

void Editor2::create_or_update_text_style(Style * style, bool paragraph, bool plaintext, double font_multiplier)
// This creates or updates a GtkTextTag with the data stored in "style".
// The fontsize of the style is calculated by the value as stored in "style", and multiplied by "font_multiplier".
{
  // Take the existing tag, or create a new one and add it to the tagtable.
  GtkTextTag *tag = gtk_text_tag_table_lookup(texttagtable, style->marker.c_str());
  if (!tag) {
    tag = gtk_text_tag_new(style->marker.c_str());
    gtk_text_tag_table_add(texttagtable, tag);
    g_object_unref(tag);
  }
  // Optionally handle plain-text style and return.
  if (plaintext) {
    GValue gvalue = { 0, };
    g_value_init(&gvalue, G_TYPE_STRING);
    int fontsize = (int)(12 * font_multiplier);
    ustring font = "Courier " + convert_to_string(fontsize);
    g_value_set_string(&gvalue, font.c_str());
    g_object_set_property(G_OBJECT(tag), "font", &gvalue);
    g_value_unset(&gvalue);
    return;
  }
  // Fontsize.
  if (paragraph) {
    GValue gvalue = { 0, };
    g_value_init(&gvalue, G_TYPE_DOUBLE);
    double fontsize = style->fontsize * font_multiplier;
    g_value_set_double(&gvalue, fontsize);
    g_object_set_property(G_OBJECT(tag), "size-points", &gvalue);
    g_value_unset(&gvalue);
  }
  // Italic, bold, underline, smallcaps can be ON, OFF, INHERIT, or TOGGLE.
  // Right now, INHERIT is taken as OFF, and TOGGLE is interpreted as ON.
  // Improvements might be needed.
  {
    PangoStyle pangostyle = PANGO_STYLE_NORMAL;
    if ((style->italic == ON) || (style->italic == TOGGLE))
      pangostyle = PANGO_STYLE_ITALIC;
    GValue gvalue = { 0, };
    g_value_init(&gvalue, PANGO_TYPE_STYLE);
    g_value_set_enum(&gvalue, pangostyle);
    g_object_set_property(G_OBJECT(tag), "style", &gvalue);
    g_value_unset(&gvalue);
  }
  {
    PangoWeight pangoweight = PANGO_WEIGHT_NORMAL;
    if ((style->bold == ON) || (style->bold == TOGGLE))
      pangoweight = PANGO_WEIGHT_BOLD;
    GValue gvalue = { 0, };
    g_value_init(&gvalue, PANGO_TYPE_WEIGHT);
    g_value_set_enum(&gvalue, pangoweight);
    g_object_set_property(G_OBJECT(tag), "weight", &gvalue);
    g_value_unset(&gvalue);
  }
  {
    PangoUnderline pangounderline = PANGO_UNDERLINE_NONE;
    if ((style->underline == ON) || (style->underline == TOGGLE))
      pangounderline = PANGO_UNDERLINE_SINGLE;
    GValue gvalue = { 0, };
    g_value_init(&gvalue, PANGO_TYPE_UNDERLINE);
    g_value_set_enum(&gvalue, pangounderline);
    g_object_set_property(G_OBJECT(tag), "underline", &gvalue);
    g_value_unset(&gvalue);
  }
  {
    /*
       The small caps variant has not yet been implemented in Pango.
       PangoVariant pangovariant = PANGO_VARIANT_NORMAL;
       if ((style->smallcaps == ON) || (style->smallcaps == TOGGLE))
       pangovariant = PANGO_VARIANT_NORMAL;
       GValue gvalue = {0,};
       g_value_init (&gvalue, PANGO_TYPE_VARIANT);
       g_value_set_enum (&gvalue, pangovariant);
       g_object_set_property (G_OBJECT (tag), "variant", &gvalue);
       g_value_unset (&gvalue);
     */
    if ((style->smallcaps == ON) || (style->smallcaps == TOGGLE)) {
      double percentage = (double)0.6 * font_multiplier;
      GValue gvalue = { 0, };
      g_value_init(&gvalue, G_TYPE_DOUBLE);
      g_value_set_double(&gvalue, percentage);
      g_object_set_property(G_OBJECT(tag), "scale", &gvalue);
      g_value_unset(&gvalue);
    }
  }

  /*
     Superscript.
     Make size of verse or indeed any superscript equal to around 70% of normal font.
     Top of verse number should be even with top of capital T.
   */
  if (style->superscript) {
    // Rise n pixels.
    {
      gint rise = 6 * PANGO_SCALE;
      GValue gvalue = { 0, };
      g_value_init(&gvalue, G_TYPE_INT);
      g_value_set_int(&gvalue, rise);
      g_object_set_property(G_OBJECT(tag), "rise", &gvalue);
      g_value_unset(&gvalue);
    }
    // Smaller size.
    {
      double percentage = 0.7;
      GValue gvalue = { 0, };
      g_value_init(&gvalue, G_TYPE_DOUBLE);
      g_value_set_double(&gvalue, percentage);
      g_object_set_property(G_OBJECT(tag), "scale", &gvalue);
      g_value_unset(&gvalue);
    }
  }
  // Styles that occur in paragraphs only, not in character styles.  
  if (paragraph) {

    GtkJustification gtkjustification;
    if (style->justification == CENTER) {
      gtkjustification = GTK_JUSTIFY_CENTER;
    } else if (style->justification == RIGHT) {
      gtkjustification = GTK_JUSTIFY_RIGHT;
    } else if (style->justification == JUSTIFIED) {
      // Gtk+ supports this from version 2.12.
      gtkjustification = GTK_JUSTIFY_LEFT;
      if (GTK_MAJOR_VERSION >= 2)
        if (GTK_MINOR_VERSION >= 12)
          gtkjustification = GTK_JUSTIFY_FILL;
    } else {                    // Default is LEFT.
      gtkjustification = GTK_JUSTIFY_LEFT;
    }
    {
      GValue gvalue = { 0, };
      g_value_init(&gvalue, GTK_TYPE_JUSTIFICATION);
      g_value_set_enum(&gvalue, gtkjustification);
      g_object_set_property(G_OBJECT(tag), "justification", &gvalue);
      g_value_unset(&gvalue);
    }

    // For property "pixels-above/below-...", only values >= 0 are valid.
    if (style->spacebefore > 0) {
      gint spacebefore = (gint) (4 * style->spacebefore);
      GValue gvalue = { 0, };
      g_value_init(&gvalue, G_TYPE_INT);
      g_value_set_int(&gvalue, spacebefore);
      g_object_set_property(G_OBJECT(tag), "pixels-above-lines", &gvalue);
      g_value_unset(&gvalue);
    }

    if (style->spaceafter > 0) {
      gint spaceafter = (gint) (4 * style->spaceafter);
      GValue gvalue = { 0, };
      g_value_init(&gvalue, G_TYPE_INT);
      g_value_set_int(&gvalue, spaceafter);
      g_object_set_property(G_OBJECT(tag), "pixels-below-lines", &gvalue);
      g_value_unset(&gvalue);
    }

    {
      gint leftmargin = (gint) (4 * style->leftmargin);
      // A little left margin is desired to make selecting words easier.
      leftmargin += 5;
      GValue gvalue = { 0, };
      g_value_init(&gvalue, G_TYPE_INT);
      g_value_set_int(&gvalue, leftmargin);
      g_object_set_property(G_OBJECT(tag), "left-margin", &gvalue);
      g_value_unset(&gvalue);
    }

    if (style->rightmargin > 0) {
      gint rightmargin = (gint) (4 * style->rightmargin);
      GValue gvalue = { 0, };
      g_value_init(&gvalue, G_TYPE_INT);
      g_value_set_int(&gvalue, rightmargin);
      g_object_set_property(G_OBJECT(tag), "right-margin", &gvalue);
      g_value_unset(&gvalue);
    }

    {
      gint firstlineindent = (gint) (4 * style->firstlineindent);
      GValue gvalue = { 0, };
      g_value_init(&gvalue, G_TYPE_INT);
      g_value_set_int(&gvalue, firstlineindent);
      g_object_set_property(G_OBJECT(tag), "indent", &gvalue);
      g_value_unset(&gvalue);
    }

  }

  {
    GdkColor color;
    color_decimal_to_gdk(style->color, &color);
    GValue gvalue = { 0, };
    g_value_init(&gvalue, GDK_TYPE_COLOR);
    g_value_set_boxed(&gvalue, &color);
    g_object_set_property(G_OBJECT(tag), "foreground-gdk", &gvalue);
    g_value_unset(&gvalue);
  }
}


bool Editor2::load_text_table_starting_row(ustring & line, EditorTable & editortable, GtkTextBuffer * &textbuffer, bool & row_zero_initialized, gint & row, gint & column, ustring & paragraph_mark, const ustring & marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
/*
 This sets the parameters in case a marker is encountered that starts a new
 row in a table.
 */
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype(project, marker, type, subtype);
        if (style_get_starts_table_row(type, subtype)) {
          // For robustness the first row is always opened. In order that no
          // extra row is opened, the "..initiallized" flag is used.
          if (row_zero_initialized) {
            row++;
            column = 0;
            textbuffer = table_cell_get_buffer(editortable, row, column);
          } else {
            row_zero_initialized = true;
          }
          // Initialize the paragraph mark.
          paragraph_mark = unknown_style();
          if (style_get_displays_marker(type, subtype)) {
            editor_text_append(textbuffer, line.substr(0, marker_length), paragraph_mark, "");
          }
          line.erase(0, marker_length);
          return true;
        }
      }
    }
  }
  return false;
}

bool Editor2::load_text_table_starting_cell(ustring & line, EditorTable & editortable, GtkTextBuffer * &textbuffer, bool & row_zero_initialized, gint & row, gint & column, ustring & paragraph_mark, const ustring & marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
// This function does the administration for the markers that start a cell.
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype(project, marker, type, subtype);
        if (style_get_starts_table_cell(type, subtype)) {
          // Get the column number of this marker.
          ustring stylesheet = stylesheet_get_actual ();
          extern Styles *styles;
          Usfm *usfm = styles->usfm(stylesheet);
          for (unsigned int i = 0; i < usfm->styles.size(); i++) {
            if (marker == usfm->styles[i].marker) {
              column = usfm->styles[i].userint1;
              // The column number is given such that column 1 is the first column,
              // but internally we work with column 0 as the first.
              column--;
            }
          }
          textbuffer = table_cell_get_buffer(editortable, row, column);
          paragraph_mark = marker;
          if (style_get_displays_marker(type, subtype)) {
            editor_text_append(textbuffer, line.substr(0, marker_length), paragraph_mark, "");
          }
          line.erase(0, marker_length);
          return true;
        }
      }
    }
  }
  return false;
}

bool Editor2::load_text_starting_footnote_content(GtkTextBuffer * textbuffer, ustring & line, ustring & paragraph_mark, ustring & character_mark, const ustring & marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
/*
 This function deals with a marker that starts footnote content.
 It does the administration appropriate for that.
 */
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype(project, marker, type, subtype);
        if (style_get_starts_note_content(type, subtype)) {
          // Set the new character markup (the paragraph markup remains the same).
          character_mark = marker;
          // Some styles insert their marker: Do that here if appropriate.
          if (style_get_displays_marker(type, subtype)) {
            editor_text_append(textbuffer, line.substr(0, marker_length), paragraph_mark, character_mark);
          }
          // Remove the markup from the line.
          line.erase(0, marker_length);
          // The information was processed: return true.
          return true;
        }
      }
    }
  }
  return false;
}

bool Editor2::load_text_ending_footnote_content(GtkTextBuffer * textbuffer, ustring & line, ustring & paragraph_mark, ustring & character_mark, const ustring & marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
/*
 This function deals with a marker that ends footnote content.
 It does the administration appropriate for that.
 */
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (!is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype(project, marker, type, subtype);
        if (style_get_starts_note_content(type, subtype)) {
          // Some styles insert their marker: Do that here if appropriate.
          if (style_get_displays_marker(type, subtype)) {
            editor_text_append(textbuffer, line.substr(0, marker_length), paragraph_mark, character_mark);
          }
          // Clear the existing character markup (the paragraph markup remains the same).
          character_mark.clear();
          // Remove the markup from the line.
          line.erase(0, marker_length);
          // The information was processed: return true.
          return true;
        }
      }
    }
  }
  return false;
}


void Editor2::erase_related_note_bits()
{
  /*
  // Collect data for note bits and the editor objects to be removed.
   vector < GtkTextChildAnchor * >related_anchors;
  set < int >objects_to_remove;
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    if (text_child_anchors_being_deleted.find(editornotes[i].childanchor_caller_text) != text_child_anchors_being_deleted.end()) {
      related_anchors.push_back(editornotes[i].childanchor_caller_note);
      related_anchors.push_back(editornotes[i].childanchor_textview);
      objects_to_remove.insert(i);
    }
    if (text_child_anchors_being_deleted.find(editornotes[i].childanchor_caller_note) != text_child_anchors_being_deleted.end()) {
      related_anchors.push_back(editornotes[i].childanchor_caller_text);
      related_anchors.push_back(editornotes[i].childanchor_textview);
      objects_to_remove.insert(i);
    }
    if (text_child_anchors_being_deleted.find(editornotes[i].childanchor_textview) != text_child_anchors_being_deleted.end()) {
      related_anchors.push_back(editornotes[i].childanchor_caller_text);
      related_anchors.push_back(editornotes[i].childanchor_caller_note);
      objects_to_remove.insert(i);
    }
  }
  {
    // Remove double anchors and anchors already deleted.
    set < GtkTextChildAnchor * >anchors;
    for (unsigned int i = 0; i < related_anchors.size(); i++) {
      if (text_child_anchors_being_deleted.find(related_anchors[i]) == text_child_anchors_being_deleted.end())
        anchors.insert(related_anchors[i]);
    }
    related_anchors.clear();
    related_anchors.assign(anchors.begin(), anchors.end());
  }

  // Remove the widgets.
  for (unsigned int i = 0; i < related_anchors.size(); i++) {
    do_not_process_child_anchors_being_deleted = true;
    textbuffer_erase_child_anchor(textbuffer, related_anchors[i]);
    do_not_process_child_anchors_being_deleted = false;
  }

  // If nothing was deleted, bail out.
  if (objects_to_remove.empty())
    return;

  // Remove the objects.
  vector < EditorNote >::iterator iter(editornotes.end());
  for (int i = editornotes.size() - 1; i >= 0; i--) {
    iter--;
    if (objects_to_remove.find(i) != objects_to_remove.end()) {
      editornotes.erase(iter);
    }
  }

  // If notes were deleted, renumber possible remaining notes.
  renumber_and_clean_notes_callers();
  */
}


void Editor2::display_notes_remainder(bool focus_rendered_textview)
/*
 Once the text has been loaded in the editor, any notes have been partially 
 loaded. The note caller in the text has been placed already, but the note caller
 in the note, below the body of text, and the actual text of the note, have
 not yet been placed. This function places these.

 The above is true in case all the notes are new.

 This routine also works if a note has been inserted. 
 It will then place and format only this new note, 
 leaving the other ones untouched.
 */
{
  /*
  // Bail out if there are no notes to be displayed.
  if (editornotes.empty())
    return;

  // No recording of undoable actions while this object is alive.
  PreventEditorUndo preventundo(&record_undo_level);

  // Clean unwanted space out.
  textbuffer_erase_white_space_at_end(textbuffer);
  */
  /*
     Insert a little space between the body of the text and the first note. 
     This could be done using a tag that has the "editable" property set to false,
     like so:
     GtkTextTag * tag = gtk_text_buffer_create_tag (textbuffer, NULL, "editable", false, NULL);
     gtk_text_buffer_insert_with_tags (textbuffer, &iter1, "\n\n", -1, tag, NULL);
     But this appears to cause an undefined crash under some circumstances.
     This little space only needs to be inserted in the case that no notes have
     been rendered yet.
   */
  /*
  bool no_notes_rendered_yet = true;
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    if (editornotes[i].textview)
      no_notes_rendered_yet = false;
  }
  if (no_notes_rendered_yet) {
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(textbuffer, &iter);
    gtk_text_buffer_insert(textbuffer, &iter, "\n\n", -1);
  }

  for (unsigned int i = 0; i < editornotes.size(); i++) {

    // Skip this note if it has been rendered already.
    if (editornotes[i].textview)
      continue;

    // Insert a new line after all other text.
    GtkTextIter iter = editor_get_iter_for_note(textbuffer, editornotes, i, 0);
    gtk_text_buffer_insert(textbuffer, &iter, "\n", -1);

    // Create the anchor for the note caller in the note.
    iter = editor_get_iter_for_note(textbuffer, editornotes, i, 1);
    editornotes[i].childanchor_caller_note = gtk_text_buffer_create_child_anchor(textbuffer, &iter);

    // Create the label that shows the note caller in the note.
    // The actual character(s) of the caller are written later on.
    // This seems easier, because later on, while editing, these callers may have
    // to be updated regularly anyway.
    editornotes[i].label_caller_note = gtk_label_new("");
    gtk_text_view_add_child_at_anchor(GTK_TEXT_VIEW(textview), editornotes[i].label_caller_note, editornotes[i].childanchor_caller_note);
    gtk_widget_show_all(editornotes[i].label_caller_note);

    // The caller in the note, and the note text itself, may have different heights, depending
    // on the amount of text to be displayed. The textview would display the note caller 
    // at the bottom of the note text, whereas the user's expectation
    // is to have it at the top, at the start of the first line of text.
    // The next line puts it at the top, but to make the story complete, 
    // there is also a signal fired on allocation of the size of the widgets.
    // This signal calls routines to set the height of the note caller.
    gtk_misc_set_alignment(GTK_MISC(editornotes[i].label_caller_note), 0, 0);

    // Create the anchor for the note text.
    iter = editor_get_iter_for_note(textbuffer, editornotes, i, 2);
    editornotes[i].childanchor_textview = gtk_text_buffer_create_child_anchor(textbuffer, &iter);

    // A textbuffer to store the note's text.
    editornotes[i].textbuffer = gtk_text_buffer_new(texttagtable);

    // Signal handlers for the textbuffer.
    //g_signal_connect(G_OBJECT(editornotes[i].textbuffer), "insert-text", G_CALLBACK(on_buffer_insert_text_before), gpointer(this));
    //g_signal_connect_after(G_OBJECT(editornotes[i].textbuffer), "insert-text", G_CALLBACK(on_buffer_insert_text_after), gpointer(this));
    //g_signal_connect(G_OBJECT(editornotes[i].textbuffer), "delete-range", G_CALLBACK(on_buffer_delete_range_before), gpointer(this));
    //g_signal_connect_after(G_OBJECT(editornotes[i].textbuffer), "delete-range", G_CALLBACK(on_buffer_delete_range_after), gpointer(this));
    //g_signal_connect(G_OBJECT(editornotes[i].textbuffer), "apply-tag", G_CALLBACK(on_buffer_apply_tag), gpointer(this));
    //g_signal_connect(G_OBJECT(editornotes[i].textbuffer), "remove-tag", G_CALLBACK(on_buffer_remove_tag), gpointer(this));
    //g_signal_connect(G_OBJECT(editornotes[i].textbuffer), "changed", G_CALLBACK(on_textbuffer_footnotes_changed), gpointer(this));
    //g_signal_connect(G_OBJECT(editornotes[i].textbuffer), "changed", G_CALLBACK(on_textbuffer_changed), gpointer(this));

    // A text view to display the buffer.
    editornotes[i].textview = gtk_text_view_new_with_buffer(editornotes[i].textbuffer);
    gtk_widget_show(editornotes[i].textview);
    gtk_text_view_add_child_at_anchor(GTK_TEXT_VIEW(textview), editornotes[i].textview, editornotes[i].childanchor_textview);
    gtk_widget_show_all(editornotes[i].textview);
    gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(editornotes[i].textview), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(editornotes[i].textview), GTK_WRAP_WORD);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(editornotes[i].textview), editable);

    // Signal handlers for this textview.
    spellingchecker->attach(editornotes[i].textview);
    g_signal_connect_after((gpointer) editornotes[i].textview, "move_cursor", G_CALLBACK(on_textview_move_cursor), gpointer(this));
    g_signal_connect_after((gpointer) editornotes[i].textview, "grab_focus", G_CALLBACK(on_textview_grab_focus), gpointer(this));
    g_signal_connect((gpointer) editornotes[i].textview, "key-press-event", G_CALLBACK(text_key_press_event_before), gpointer(this));
    g_signal_connect_after((gpointer) editornotes[i].textview, "key-press-event", G_CALLBACK(text_key_press_event_after), gpointer(this));
    g_signal_connect((gpointer) editornotes[i].textview, "button_press_event", G_CALLBACK(on_textview_button_press_event), gpointer(this));

    // Extract the note caller.
    if (!editornotes[i].rawtext.empty()) {
      editornotes[i].caller = editornotes[i].rawtext.substr(0, 1);
      editornotes[i].rawtext.erase(0, 1);
      editornotes[i].rawtext = trim(editornotes[i].rawtext);
    }
    // Format the text.
    ustring paragraph_mark(style_get_default_note_style(project, note_type_get(project, editornotes[i].marker)));
    ustring character_mark;
    ustring marker;
    size_t marker_pos;
    size_t marker_length;
    bool is_opener;
    bool marker_found;
    while (!editornotes[i].rawtext.empty()) {
      marker_found = usfm_search_marker(editornotes[i].rawtext, marker, marker_pos, marker_length, is_opener);
      else if (load_text_starting_new_paragraph(editornotes[i].textbuffer, editornotes[i].rawtext, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found)) ;
      else if (load_text_starting_character_style(editornotes[i].textbuffer, editornotes[i].rawtext, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found)) ;
      else if (load_text_ending_character_style(editornotes[i].textbuffer, editornotes[i].rawtext, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found)) ;
      else if (load_text_starting_footnote_content(editornotes[i].textbuffer, editornotes[i].rawtext, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found)) ;
      else if (load_text_ending_footnote_content(editornotes[i].textbuffer, editornotes[i].rawtext, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found)) ;
      else fallback;
    }

    // Remove any superfluous whitespace at the end of the note.
    textbuffer_erase_character_before_text_insertion_point_if_space(editornotes[i].textbuffer);

    // Signal related to the sizes, that is, the height and the width, of the widgets.
    g_signal_connect(GTK_OBJECT(editornotes[i].label_caller_note), "size-allocate", G_CALLBACK(on_related_widget_size_allocated), gpointer(this));
    g_signal_connect(GTK_OBJECT(editornotes[i].textview), "size-allocate", G_CALLBACK(on_related_widget_size_allocated), gpointer(this));

    // Optionally focus the rendered textview and scroll to it.
    if (focus_rendered_textview) {
      while (gtk_events_pending())
        gtk_main_iteration();
      programmatically_grab_focus(editornotes[i].textview);
      GtkTextIter iter;
      gtk_text_buffer_get_iter_at_child_anchor(textbuffer, &iter, editornotes[i].childanchor_textview);
      scroll_cursor_on_screen ();
    }
  }

  // Set fonts.
  set_font();
  */
}

void Editor2::renumber_and_clean_notes_callers()
// Renumbers the note callers.
// At the same time clear up unwanted stuff.
{
  /*
  // Get the different markers we have.
  vector < ustring > note_openers;
  {
    set < ustring > note_openers_set;
    for (unsigned int i = 0; i < editornotes.size(); i++) {
      note_openers_set.insert(editornotes[i].marker);
    }
    note_openers.assign(note_openers_set.begin(), note_openers_set.end());
  }

  // Create the caller objects, one for each note opening marker.
  map < ustring, NoteCaller * >notecallers;
  for (unsigned int i = 0; i < note_openers.size(); i++) {
    NoteCaller *caller = new NoteCaller(note_numbering_type_get(project, note_openers[i]), note_numbering_user_sequence_get(project, note_openers[i]));
    notecallers[note_openers[i]] = caller;
  }

  // Set the text of the callers.
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    ustring caller;
    if (editornotes[i].caller == "+") {
      caller = notecallers[editornotes[i].marker]->get_caller();
    } else if (editornotes[i].caller == "-") {
      caller = " ";
    } else {
      caller = editornotes[i].caller;
    }
    // The background of the callers is going to be grey, so that the user 
    // understands that these callers are fields and are going to be updated
    // automatically by the program itself. 
    // Also grey is good for making a distinction between a verse number and a 
    // note caller. 
    // Without this distinction they look too much alike, and that would cause 
    // confusion.
    // Courier font is chosen to make the spacing of the callers equal 
    // in case there are more of them.
    char *markup = g_markup_printf_escaped("<span background=\"grey\" size=\"x-small\"> </span><span background=\"grey\" face=\"Courier\">%s</span><span background=\"grey\" size=\"x-small\"> </span>", caller.c_str());
    gtk_label_set_markup(GTK_LABEL(editornotes[i].label_caller_text), markup);
    gtk_label_set_markup(GTK_LABEL(editornotes[i].label_caller_note), markup);
    g_free(markup);
  }

  // Destroy all the note caller objects.
  for (unsigned int i = 0; i < note_openers.size(); i++) {
    NoteCaller *caller = notecallers[note_openers[i]];
    delete caller;
  }

  // Just to be sure, erase any whitespace at the end of the buffer.
  for (unsigned int i = 0; i < 20; i++) {
    textbuffer_erase_white_space_at_end(textbuffer);
  }

  // Clean up bits between the notes.
  // While the user edits the text, he may add extra spaces or new lines
  // between the notes. These need to be cleared out.
  for (int i = 0; i < (int)editornotes.size() - 1; i++) {
    GtkTextIter startiter, enditer;
    gtk_text_buffer_get_iter_at_child_anchor(textbuffer, &startiter, editornotes[i].childanchor_textview);
    gtk_text_iter_forward_char(&startiter);
    gtk_text_buffer_insert(textbuffer, &startiter, "\n", -1);
    gtk_text_buffer_get_iter_at_child_anchor(textbuffer, &enditer, editornotes[i + 1].childanchor_textview);
    gtk_text_iter_backward_char(&enditer);
    gtk_text_buffer_delete(textbuffer, &startiter, &enditer);
  }

  // Get the offset of the last essential character in the buffer.
  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(textbuffer, &iter);
  bool essential_character_found = false;
  do {
    gunichar character = gtk_text_iter_get_char(&iter);
    GtkTextChildAnchor *childanchor = gtk_text_iter_get_child_anchor(&iter);
    if (!childanchor) {
      if (character)
        if (!g_unichar_isspace(character))
          essential_character_found = true;
    }
    // The order of the following two logical parameters is important.
  } while ((!essential_character_found) && gtk_text_iter_backward_char(&iter));
  unsigned int offset_last_essential_character = gtk_text_iter_get_offset(&iter);
  // If a note is inserted, there is code that if any character is added to the
  // end of the textbuffer, the last known paragraph style continues. Hence,
  // if a note is inserted, then two new lines are inserted to separate the 
  // text body from the notes. These usually get the \p style applied.
  // This \p style is unwanted, and therefore needs to be cleaned away.
  // Clear all styles after the last essential character.
  gtk_text_buffer_get_iter_at_offset(textbuffer, &iter, offset_last_essential_character + 1);
  GtkTextIter iter2;
  gtk_text_buffer_get_end_iter(textbuffer, &iter2);
  gtk_text_buffer_remove_all_tags(textbuffer, &iter, &iter2);
  */
}


void Editor2::display_table(ustring line, GtkTextIter iter)
// Displays a table, inserting it at "iter". The raw USFM code is in "line".
{
  /*
  // Find out the number of columns and rows in this table.
  vector < ustring > markers = usfm_get_all_markers(line);
  int rowcount = 0;
  int columncount = 0;
  ustring stylesheet = stylesheet_get_actual ();
  extern Styles *styles;
  Usfm *usfm = styles->usfm(stylesheet);
  for (unsigned int i = 0; i < markers.size(); i++) {
    StyleType type;
    int subtype;
    marker_get_type_and_subtype(project, markers[i], type, subtype);
    if (style_get_starts_table_cell(type, subtype)) {
      if (rowcount == 0)
        rowcount = 1;
      for (unsigned int i2 = 0; i2 < usfm->styles.size(); i2++) {
        if (usfm->styles[i2].marker == markers[i]) {
          if (usfm->styles[i2].userint1 > columncount)
            columncount = usfm->styles[i2].userint1;
        }
      }
    }
    if (style_get_starts_table_row(type, subtype)) {
      rowcount++;
    }
  }

  // Bail out if there aren't enough rows or columns.
  if (rowcount == 0)
    return;
  if (columncount == 0)
    return;

  // Create the table object.  
  EditorTable editortable(0);

  // Insert the table and the textviews in the text.
  editortable.childanchor = gtk_text_buffer_create_child_anchor(textbuffer, &iter);

  editortable.table = gtk_table_new(0, 0, false);
  gtk_widget_show(editortable.table);
  gtk_text_view_add_child_at_anchor(GTK_TEXT_VIEW(textview), editortable.table, editortable.childanchor);
  table_resize(editortable, texttagtable, rowcount, columncount, editable);

  // Connect signal handlers and insert the insertion point mark.
  for (unsigned int row = 0; row < editortable.textviews.size(); row++) {
    for (unsigned int column = 0; column < editortable.textviews[row].size(); column++) {
      GtkWidget *textview = table_cell_get_view(editortable, row, column);
      spellingchecker->attach(textview);
      g_signal_connect_after((gpointer) textview, "move_cursor", G_CALLBACK(on_textview_move_cursor), gpointer(this));
      g_signal_connect_after((gpointer) textview, "grab_focus", G_CALLBACK(on_textview_grab_focus), gpointer(this));
      g_signal_connect((gpointer) textview, "key-press-event", G_CALLBACK(text_key_press_event_before), gpointer(this));
      g_signal_connect_after((gpointer) textview, "key-press-event", G_CALLBACK(text_key_press_event_after), gpointer(this));
      g_signal_connect((gpointer) textview, "button_press_event", G_CALLBACK(on_textview_button_press_event), gpointer(this));
      //GtkTextBuffer *textbuffer = table_cell_get_buffer(editortable, row, column);
      //g_signal_connect(G_OBJECT(textbuffer), "insert-text", G_CALLBACK(on_buffer_insert_text_before), gpointer(this));
      //g_signal_connect_after(G_OBJECT(textbuffer), "insert-text", G_CALLBACK(on_buffer_insert_text_after), gpointer(this));
      //g_signal_connect(G_OBJECT(textbuffer), "delete-range", G_CALLBACK(on_buffer_delete_range_before), gpointer(this));
      //g_signal_connect_after(G_OBJECT(textbuffer), "delete-range", G_CALLBACK(on_buffer_delete_range_after), gpointer(this));
      //g_signal_connect(G_OBJECT(textbuffer), "apply-tag", G_CALLBACK(on_buffer_apply_tag), gpointer(this));
      //g_signal_connect(G_OBJECT(textbuffer), "remove-tag", G_CALLBACK(on_buffer_remove_tag), gpointer(this));
      //g_signal_connect(G_OBJECT(textbuffer), "changed", G_CALLBACK(on_textbuffer_changed), gpointer(this));
    }
  }

  // Store the object.
  editortables.push_back(editortable);

  // Put the text in the table.
  ustring paragraph_mark(unknown_style());
  ustring character_mark;
  ustring marker;
  size_t marker_pos;
  size_t marker_length;
  bool is_opener;
  bool marker_found;
  gint row = 0;
  bool row_zero_initialized = false;
  gint column = 0;
  GtkTextBuffer *textbuffer = table_cell_get_buffer(editortable, row, column);
  while (!line.empty()) {
    marker_found = usfm_search_marker(line, marker, marker_pos, marker_length, is_opener);
    if (load_text_table_starting_row(line, editortable, textbuffer, row_zero_initialized, row, column, paragraph_mark, marker, marker_pos, marker_length, is_opener, marker_found)) ;
    else if (load_text_table_starting_cell(line, editortable, textbuffer, row_zero_initialized, row, column, paragraph_mark, marker, marker_pos, marker_length, is_opener, marker_found)) ;
    else fallback;
  }

  // Erase any superfluous whitespace at the end of the cells.
  for (unsigned int row = 0; row < editortable.textbuffers.size(); row++) {
    for (unsigned int column = 0; column < editortable.textbuffers[row].size(); column++) {
      GtkTextBuffer *buffer = table_cell_get_buffer(editortable, row, column);
      textbuffer_erase_character_before_text_insertion_point_if_space(buffer);
    }
  }

  // Set the font.
  set_font();

  // Set the table's width.
  set_embedded_table_textviews_width(editortables.size() - 1);
  */
}

void Editor2::erase_tables()
/*
 This function is called when a GtkTextChildAnchor is in the process of 
 being deleted. If this anchor or these anchors happens to contain
 a table, this function does the administration of getting rid of this table.
 */
{
  // Remove the objects.
  vector < EditorTable >::iterator iter(editortables.end());
  for (int i = editortables.size() - 1; i >= 0; i--) {
    iter--;
    if (text_child_anchors_being_deleted.find(editortables[i].childanchor) != text_child_anchors_being_deleted.end()) {
      editortables.erase(iter);
    }
  }
}


void Editor2::on_buffer_insert_text_before(GtkTextBuffer * textbuffer, GtkTextIter * pos_iter, gchar * text, gint length, gpointer user_data)
{
  ((Editor2 *) user_data)->buffer_insert_text_before(textbuffer, pos_iter, text, length);
}


void Editor2::buffer_insert_text_before(GtkTextBuffer * textbuffer, GtkTextIter * pos_iter, gchar * text, gint length)
{
  //cout << "buffer_insert_text_before (GtkTextBuffer * textbuffer " << textbuffer << ", GtkTextIter * pos_iter offset " << gtk_text_iter_get_offset (pos_iter) << ", gchar * text " << text << ", gint length " << length << ")" << endl;
}


void Editor2::on_buffer_insert_text_after(GtkTextBuffer * textbuffer, GtkTextIter * pos_iter, gchar * text, gint length, gpointer user_data)
{
  ((Editor2 *) user_data)->buffer_insert_text_after(textbuffer, pos_iter, text, length);
}


void Editor2::buffer_insert_text_after(GtkTextBuffer * textbuffer, GtkTextIter * pos_iter, gchar * text, gint length)
{
  
  return;
  //cout << "buffer_insert_text_after (GtkTextBuffer * textbuffer " << textbuffer << ", GtkTextIter * pos_iter offset " << gtk_text_iter_get_offset (pos_iter) << ", gchar * text " << text << ", gint length " << length << ")" << endl;
  
  // Bail out if no undoes are to be recorded.
  if (!recording_undo_actions())
    return;

  // Signal that the editor changed.
  signal_editor_changed();

  /*
     The code below is going to solve a problem that occurs when adding text to
     the end of a textbuffer or at a place where no style tags have been applied.
     When the user types text at the end of a textbuffer, he or she expects that
     this text is going to have the same paragraph and/or character style as the
     preceding text. However, without special measures this is not the case.
     The text is inserted plain, without any tags applied.
     This signal handler solves that. It looks whether no style is applied on
     the inserted text. If no style is there, it iterates back in the buffer
     till it finds styls(s). It then applies the same style(s) on the inserted
     text.
   */
  ustring paragraph_style, character_style;
  GtkTextIter iter = *pos_iter;
  get_styles_at_iterator(iter, paragraph_style, character_style);
  if (paragraph_style.empty()) {
    while ((paragraph_style.empty()) && gtk_text_iter_backward_char(&iter)) {
      get_styles_at_iterator(iter, paragraph_style, character_style);
    }
    if (!paragraph_style.empty()) {
      iter = *pos_iter;
      gtk_text_iter_backward_char(&iter);
      GtkTextIter iter2 = *pos_iter;
      gtk_text_iter_forward_chars(&iter2, length);
      textbuffer_apply_named_tag(textbuffer, paragraph_style, &iter, &iter2);
      if (!character_style.empty()) {
        textbuffer_apply_named_tag(textbuffer, character_style, &iter, &iter2);
      }
    }
  }

  /*
     The routine below is going to solve another problem that occurs at times.
     When a character style has been applied, and then the user starts typing,
     he expects that this style is going to be applied to the text he types.
     This does not happen though, except special code is there.
     The code below looks whether a character style has been applied
     and is still valid. And if no character style is applied at the cursor,
     it then applies this style.
   */
  if (character_style.empty()) {
    if (!character_style_on_start_typing.empty()) {
      iter = *pos_iter;
      gtk_text_iter_backward_char(&iter);
      GtkTextIter iter2 = *pos_iter;
      gtk_text_iter_forward_chars(&iter2, length - 1);
      textbuffer_apply_named_tag(textbuffer, character_style_on_start_typing, &iter, &iter2);
    }
  }

  /*
     The routine below is going to solve a problem that occurs under certain 
     circumstances.
     When a certain paragraph style has been applied to a line, 
     and the user starts typing text right at the start of that paragraph,
     the editor does not apply the right paragraph style on the new text,
     but instead applies no style at all.
   */
  if (!paragraph_style.empty()) {
    GtkTextIter iter = *pos_iter;
    gtk_text_iter_backward_char(&iter);
    ustring paragraph, character;
    get_styles_at_iterator(iter, paragraph, character);
    if (paragraph.empty()) {
      iter = *pos_iter;
      GtkTextIter iter0 = *pos_iter;
      gtk_text_iter_backward_chars(&iter0, length);
      textbuffer_apply_named_tag(textbuffer, paragraph_style, &iter0, &iter);
    }
  }

  /*
     The routine below solves a problem that occurs when inserting a new table.
     After the table has been inserted, no style will be applied to any of the
     table cells.
     If the user starts typing in a cell, no style is applied. 
     This routine will apply the style appropriate for that cell.
   */
  if (paragraph_style.empty()) {
    for (unsigned int i = 0; i < editortables.size(); i++) {
      EditorTextViewType editortype = last_focused_type();
      if (editortype == etvtTable) {
        unsigned int column = last_focused_column();
        ustring marker = style_get_table_cell_marker(project, column);
        apply_style(marker);
      }
    }
  }

  /*
     The code below implements intelligent verse handling.
   */
  if (length == 1) {
    gunichar character = g_utf8_get_char(text);
    // When the cursor is after a verse, and the user types a space,
    // he wishes to stop the verse and start normal text.
    if (g_unichar_isspace(character)) {
      // Handle spaces inserted: Switch verse style off if it was on.
      ustring paragraph_style, character_style;
      GtkTextIter iter = *pos_iter;
      get_styles_at_iterator(iter, paragraph_style, character_style);
      ustring versemarker = style_get_verse_marker(project);
      if (character_style_on_start_typing == versemarker) {
        character_style_on_start_typing.clear();
      }
    }
    // When the cursor is after a verse, and the user types a numeral,
    // a hyphen or a comma, it means he wishes to extend the verse style.
    if (g_unichar_isdigit(character) || !strcmp(text, "-") || !strcmp(text, ",")) {
      ustring paragraph_style, character_style;
      GtkTextIter iter = *pos_iter;
      gtk_text_iter_backward_chars(&iter, 2);
      get_styles_at_iterator(iter, paragraph_style, character_style);
      ustring versemarker = style_get_verse_marker(project);
      if (character_style == versemarker) {
        iter = *pos_iter;
        GtkTextIter iter2 = iter;
        gtk_text_iter_backward_char(&iter2);
        gtk_text_buffer_apply_tag_by_name(textbuffer, character_style.c_str(), &iter2, &iter);
      }
    }
  }
}


void Editor2::on_buffer_delete_range_before(GtkTextBuffer * textbuffer, GtkTextIter * start, GtkTextIter * end, gpointer user_data)
{
  ((Editor2 *) user_data)->buffer_delete_range_before(textbuffer, start, end);

}


void Editor2::buffer_delete_range_before(GtkTextBuffer * textbuffer, GtkTextIter * start, GtkTextIter * end)
{
  //cout << "buffer_delete_range_before (GtkTextBuffer * textbuffer " << textbuffer << ", GtkTextIter * start offset " << gtk_text_iter_get_offset (start) << ", GtkTextIter * end offset " << gtk_text_iter_get_offset (end) << ")" << endl;

  // Handle deleting footnotes.
  //collect_text_child_anchors_being_deleted(start, end);
}


void Editor2::on_buffer_delete_range_after(GtkTextBuffer * textbuffer, GtkTextIter * start, GtkTextIter * end, gpointer user_data)
{
  ((Editor2 *) user_data)->buffer_delete_range_after(textbuffer, start, end);
}


void Editor2::buffer_delete_range_after(GtkTextBuffer * textbuffer, GtkTextIter * start, GtkTextIter * end)
{
  //cout << "buffer_delete_range_after (GtkTextBuffer * textbuffer " << textbuffer << ", GtkTextIter * start offset " << gtk_text_iter_get_offset (start) << ", GtkTextIter * end offset " << gtk_text_iter_get_offset (end) << ")" << endl;
  //signal_editor_changed();
  //process_text_child_anchors_deleted();
}


void Editor2::on_buffer_apply_tag(GtkTextBuffer * textbuffer, GtkTextTag * tag, GtkTextIter * startiter, GtkTextIter * enditer, gpointer user_data)
{
  ((Editor2 *) user_data)->buffer_apply_tag(textbuffer, tag, startiter, enditer);
}


void Editor2::buffer_apply_tag(GtkTextBuffer * textbuffer, GtkTextTag * tag, GtkTextIter * startiter, GtkTextIter * enditer)
{
  //cout << "buffer_apply_tag (GtkTextBuffer * textbuffer " << textbuffer << ", GtkTextIter * start offset " << gtk_text_iter_get_offset (startiter) << ", GtkTextIter * end offset " << gtk_text_iter_get_offset (enditer) << ")" << endl;

  /*
  // Only proceed if undo-able actions are to be recorded.
  if (recording_undo_actions()) {
    signal_editor_changed();
    gtk_text_buffer_set_modified(textbuffer, true);
    trigger_undo_redo_recording ();
  }
  */
}


void Editor2::on_buffer_remove_tag(GtkTextBuffer * textbuffer, GtkTextTag * tag, GtkTextIter * startiter, GtkTextIter * enditer, gpointer user_data)
{
  ((Editor2 *) user_data)->buffer_remove_tag(textbuffer, tag, startiter, enditer);
}


void Editor2::buffer_remove_tag(GtkTextBuffer * textbuffer, GtkTextTag * tag, GtkTextIter * startiter, GtkTextIter * enditer)
{
  // cout << "buffer_remove_tag (GtkTextBuffer * textbuffer " << textbuffer << ", GtkTextIter * start offset " << gtk_text_iter_get_offset (startiter) << ", GtkTextIter * end offset " << gtk_text_iter_get_offset (enditer) << ")" << endl;
  /*
  // Only proceed if undo-able actions are to be recorded.
  if (recording_undo_actions()) {
    signal_editor_changed();
    gtk_text_buffer_set_modified(textbuffer, true);
    trigger_undo_redo_recording ();
  }
  */
}


void Editor2::on_buffer_insert_pixbuf(GtkTextBuffer * textbuffer, GtkTextIter * pos_iter, GdkPixbuf * pixbuf, gpointer user_data)
{
  ((Editor2 *) user_data)->buffer_insert_pixbuf(textbuffer, pos_iter, pixbuf);
}


void Editor2::buffer_insert_pixbuf(GtkTextBuffer * textbuffer, GtkTextIter * pos_iter, GdkPixbuf * pixbuf)
{
  /*
  // Signal that the editor changed.
  signal_editor_changed();
  trigger_undo_redo_recording ();
  */
}


void Editor2::collect_text_child_anchors_being_deleted(GtkTextIter * startiter, GtkTextIter * enditer)
// This function stores the GtkTextChildAnchors that are being deleted.
{
  /*
  if (do_not_process_child_anchors_being_deleted)
    return;
  GtkTextIter iter = *startiter;
  do {
    GtkTextChildAnchor *anchor = gtk_text_iter_get_child_anchor(&iter);
    if (anchor)
      text_child_anchors_being_deleted.insert(anchor);
    gtk_text_iter_forward_char(&iter);
  } while (gtk_text_iter_in_range(&iter, startiter, enditer));
  */
}


void Editor2::process_text_child_anchors_deleted()
// This function processes the GtkTextChildAnchors that have been deleted.
{
  /*
  if (do_not_process_child_anchors_being_deleted)
    return;
  if (!text_child_anchors_being_deleted.empty()) {
    erase_related_note_bits();
    erase_tables();
  }
  text_child_anchors_being_deleted.clear();
  */
}


void Editor2::signal_if_styles_changed()
{
  set < ustring > styles = get_styles_at_cursor();
  if (styles != styles_at_cursor) {
    styles_at_cursor = styles;
    if (new_styles_signal) {
      gtk_button_clicked(GTK_BUTTON(new_styles_signal));
    }
  }
}


set < ustring > Editor2::get_styles_at_cursor()
// Gets all the styles that apply to the cursor, or to the selection.
{
  // Hold the styles.
  set < ustring > styles;

  /*
  // Get the iterators at the selection bounds of the main textview or note or table.
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_selection_bounds(textbuffer, &startiter, &enditer);
  if (gtk_widget_is_focus(textview)) {
    gtk_text_buffer_get_selection_bounds(textbuffer, &startiter, &enditer);
  } else {
    for (unsigned int i = 0; i < editornotes.size(); i++) {
      if (gtk_widget_is_focus(editornotes[i].textview)) {
        GtkTextBuffer *buffer = editornotes[i].textbuffer;
        gtk_text_buffer_get_selection_bounds(buffer, &startiter, &enditer);
      }
    }
    for (unsigned int i = 0; i < editortables.size(); i++) {
      for (unsigned int row = 0; row < editortables[i].textviews.size(); row++) {
        for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
          if (gtk_widget_is_focus(table_cell_get_view(editortables[i], row, column))) {
            GtkTextBuffer *buffer = table_cell_get_buffer(editortables[i], row, column);
            gtk_text_buffer_get_selection_bounds(buffer, &startiter, &enditer);
          }
        }
      }
    }
  }

  // Get the applicable styles.
  // This is done by getting the names of the styles between these iterators.
  GtkTextIter iter = startiter;
  do {
    ustring paragraphstyle, characterstyle;
    get_styles_at_iterator(iter, paragraphstyle, characterstyle);
    if (!paragraphstyle.empty())
      styles.insert(paragraphstyle);
    if (!characterstyle.empty())
      styles.insert(characterstyle);
    gtk_text_iter_forward_char(&iter);
  } while (gtk_text_iter_in_range(&iter, &startiter, &enditer));
  */
  
  // Return the list.
  return styles;
}


set < ustring > Editor2::styles_at_iterator(GtkTextIter iter)
// Get all the styles that apply at the iterator.
{
  set < ustring > styles;
  ustring paragraph_style, character_style;
  get_styles_at_iterator(iter, paragraph_style, character_style);
  if (!paragraph_style.empty())
    styles.insert(paragraph_style);
  if (!character_style.empty())
    styles.insert(character_style);
  return styles;
}

GtkTextBuffer *Editor2::last_focused_textbuffer()
// Returns the focused textbuffer, or NULL if none.
{
  return gtk_text_view_get_buffer(GTK_TEXT_VIEW(last_focused_widget));
}

GtkWidget *Editor2::last_focused_textview()
// Returns the textview that was focused last.
{
  return last_focused_widget;
}


EditorTextViewType Editor2::last_focused_type()
// Returns the type of the textview that was focused most recently.
// This could be the main body of text, or a note, or a table.
{ 
  /*
  if (last_focused_widget == textview) {
    return etvtBody;
  }
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    if (last_focused_widget == editornotes[i].textview) {
      return etvtNote;
    }
  }
  for (unsigned int i = 0; i < editortables.size(); i++) {
    for (unsigned int row = 0; row < editortables[i].textviews.size(); row++) {
      for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
        if (last_focused_widget == table_cell_get_view(editortables[i], row, column)) {
          return etvtTable;
        }
      }
    }
  }
  */
  return etvtBody;
}


unsigned int Editor2::last_focused_column()
// In case the last focused widget is a table cell,
// it returns the column number of that cell.
// The first column is number 1.
// Outside of the table it just returns 0.
{
  for (unsigned int i = 0; i < editortables.size(); i++) {
    for (unsigned int row = 0; row < editortables[i].textviews.size(); row++) {
      for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
        if (last_focused_widget == table_cell_get_view(editortables[i], row, column)) {
          return ++column;
        }
      }
    }
  }
  return 0;
}


void Editor2::apply_style(const ustring & marker)
/*
 It applies the style of "marker" to the text.
 If it is a paragraph style, it applies the style throughout the paragraph.
 If it is a character style, it only applies it on the selected text,
 or on the cursor position, or on the word that the cursor is in.
 Applying character styles works as a toggle: if the style is not there,
 it will be put there. If it was there, it will be removed.
 */
{
  // Get the type and subtype of the marker.
  StyleType type;
  int subtype;
  marker_get_type_and_subtype(project, marker, type, subtype);

  /*
     Get the type of textview that was focused last, 
     and find out whether the style that is now going to be inserted
     is applicable in this particular textview.
     For example to a table only the relevant table styles can be applied. 
     Give a message if there is a mismatch.
   */
  EditorTextViewType textviewtype = last_focused_type();
  bool style_application_ok = true;
  ustring style_application_fault_reason;
  switch (textviewtype) {
  case etvtBody:
    {
      if ((type == stFootEndNote) || (type == stCrossreference) || (type == stTableElement)) {
        style_application_ok = false;
      }
      style_application_fault_reason = "Trying to apply a ";
      if (type == stFootEndNote)
        style_application_fault_reason.append("foot- or endnote");
      if (type == stCrossreference)
        style_application_fault_reason.append("crossreference");
      if (type == stTableElement)
        style_application_fault_reason.append("table");
      style_application_fault_reason.append(" style");
      break;
    }
  case etvtNote:
    {
      if ((type != stFootEndNote) && (type != stCrossreference)) {
        style_application_ok = false;
      }
      style_application_fault_reason = "Only note related styles apply";
      break;
    }
  case etvtTable:
    {
      // Check that only a table element is going to be inserted.
      if (type != stTableElement) {
        style_application_ok = false;
        style_application_fault_reason = "Only table styles apply";
        break;
      }
      // Check that only a style with the right column number is going to be applied.
      ustring stylesheet = stylesheet_get_actual();
      extern Styles *styles;
      Usfm *usfm = styles->usfm(stylesheet);
      for (unsigned int i = 0; i < usfm->styles.size(); i++) {
        if (marker == usfm->styles[i].marker) {
          unsigned int column = usfm->styles[i].userint1;
          if (column != last_focused_column()) {
            style_application_ok = false;
            style_application_fault_reason = "Table column number mismatch";
            break;
          }
        }
      }
      break;
    }
  }
  if (!style_application_ok) {
    ustring message("This style cannot be applied here.");
    message.append("\n\n");
    message.append(style_application_fault_reason);
    message.append(".");
    gtkw_dialog_error(NULL, message.c_str());
    return;
  }
  // Get the textbuffer that was focused last.
  GtkTextBuffer *textbuffer = last_focused_textbuffer();

  // Get the textview that was focused last.
  GtkWidget *textview = last_focused_textview();

  if (style_get_starts_new_line_in_editor(type, subtype)) {
    // Handle a paragraph style.

    // Find the iterator at the start and at the end of the line that has the insertion point.
    GtkTextIter iter, startiter, enditer;
    gtk_text_buffer_get_iter_at_mark(textbuffer, &iter, gtk_text_buffer_get_insert(textbuffer));
    gint line_number = gtk_text_iter_get_line(&iter);
    gtk_text_buffer_get_iter_at_line(textbuffer, &startiter, line_number);
    line_number++;
    if (line_number == gtk_text_buffer_get_line_count(textbuffer)) {
      gtk_text_buffer_get_end_iter(textbuffer, &enditer);
    } else {
      gtk_text_buffer_get_iter_at_line(textbuffer, &enditer, line_number);
    }

    // Get the currently applied character styles.
    vector < ustring > character_styles;
    iter = startiter;
    do {
      ustring paragraph_style, character_style;
      get_styles_at_iterator(iter, paragraph_style, character_style);
      character_styles.push_back(character_style);
      gtk_text_iter_forward_char(&iter);
    } while (gtk_text_iter_in_range(&iter, &startiter, &enditer));

    // Clear all styles from the range.
    gtk_text_buffer_remove_all_tags(textbuffer, &startiter, &enditer);

    // Apply the new paragraph style to the range.
    textbuffer_apply_named_tag(textbuffer, marker, &startiter, &enditer);

    // Apply the previously found character style(s).
    // (The reason that the character styles are applied again, after having
    //  been removed, is, that throughout the Editor2 object it is assumed
    //  that the first named style is the paragraph style, and the second
    //  named style is the character style. So the order in which the styles
    //  are being applied is important.)
    iter = startiter;
    for (unsigned int i = 0; i < character_styles.size(); i++) {
      GtkTextIter iter2 = iter;
      gtk_text_iter_forward_char(&iter2);
      if (!character_styles[i].empty()) {
        textbuffer_apply_named_tag(textbuffer, character_styles[i], &iter, &iter2);
      }
      gtk_text_iter_forward_char(&iter);
    }

  } else {
    // Handle a character style.

    // Find the iterator at the start and at the end of the text to be put in this style.
    GtkTextIter iter, startiter, enditer;
    // If some text has been selected, take that.
    if (gtk_text_buffer_get_selection_bounds(textbuffer, &startiter, &enditer)) {
    } else {
      // No selection:
      // If the insertion point is inside a word, take that word.  
      // Else just take the insertion point.
      gtk_text_buffer_get_iter_at_mark(textbuffer, &iter, gtk_text_buffer_get_insert(textbuffer));
      startiter = iter;
      enditer = iter;
      if (gtk_text_iter_inside_word(&iter) && !gtk_text_iter_starts_word(&iter)) {
        gtk_text_iter_backward_word_start(&startiter);
        gtk_text_iter_forward_word_end(&enditer);
      }
    }

    // Remove any currently applied character style(s).
    // At the same time check whether the character style that we are going to 
    // apply has been applied already throughout the range.
    iter = startiter;
    bool character_style_already_applied = true;
    do {
      ustring paragraph_style, character_style;
      get_styles_at_iterator(iter, paragraph_style, character_style);
      if (character_style != marker)
        character_style_already_applied = false;
      GtkTextIter iter2 = iter;
      gtk_text_iter_forward_char(&iter2);
      if (!character_style.empty()) {
        gtk_text_buffer_remove_tag_by_name(textbuffer, character_style.c_str(), &iter, &iter2);
      }
      gtk_text_iter_forward_char(&iter);
    } while (gtk_text_iter_in_range(&iter, &startiter, &enditer));

    // Apply the new character style. If the character style was applied already,
    // it works like a toggle: it will be left removed.
    if (!character_style_already_applied) {
      textbuffer_apply_named_tag(textbuffer, marker, &startiter, &enditer);
    }
    // Store this character style so it can be re-used when the user starts 
    // typing text. But when the style was toggled off, clear it.
    if (!character_style_already_applied) {
      character_style_on_start_typing = marker;
    } else {
      character_style_on_start_typing.clear();
    }

  }

  // Update gui so it shows the right styles.
  signal_if_styles_changed();

  // Focus editor.
  programmatically_grab_focus(textview);

}

bool Editor2::on_apply_style_at_cursor(gpointer user_data)
{
  ((Editor2 *) user_data)->apply_style_at_cursor_handler();
  return false;
}

void Editor2::apply_style_at_cursor_handler()
{
  apply_style(style_to_be_applied_at_cursor);
}

void Editor2::insert_note(const ustring & marker, const ustring & rawtext, bool render)
/*
 Inserts a note in the editor.
 marker:    The marker that starts the note, e.g. "fe" for an endnote.
 rawtext:   The raw text of the note, e.g. "+ Mat 1.1.". Note that this excludes
 the note opener and note closer. It has only the caller and the
 USFM code of the note body.
 render:    Whether to render the notes straightaway.
 */
{
  /*
  // Create the object for holding the data related to this note.
  EditorNote editornote(0);

  // Save the marker.
  editornote.marker = marker;

  // Save the raw note text.
  editornote.rawtext = rawtext;

  // Get the iterator where to insert the note caller.
  GtkTextIter insertiter;
  gtk_text_buffer_get_iter_at_mark(textbuffer, &insertiter, gtk_text_buffer_get_insert(textbuffer));

  // Create the anchor for the note caller in the text.
  editornote.childanchor_caller_text = gtk_text_buffer_create_child_anchor(textbuffer, &insertiter);

  // Create the label that shows the note caller in the text.
  // The actual character(s) of the caller are determined later on.
  // This seems easier, because later on, while editing, these callers may have
  // to be updated regularly.
  editornote.label_caller_text = gtk_label_new("");
  gtk_text_view_add_child_at_anchor(GTK_TEXT_VIEW(textview), editornote.label_caller_text, editornote.childanchor_caller_text);
  gtk_widget_show_all(editornote.label_caller_text);

  // Store object. This is not just a matter of pushing it at the back of the container.
  // We need to find out at which position in the container this note should be stored.
  // The position where to store depends on the position of this note in the text
  // relative to the other notes in the text.
  if (editornotes.empty()) {
    editornotes.push_back(editornote);
  } else {
    unsigned int offset_new_note = gtk_text_iter_get_offset(&insertiter);
    unsigned int insert_offset = 0;
    bool insert_offset_found = false;
    for (unsigned int i = 0; i < editornotes.size(); i++) {
      GtkTextIter iter;
      gtk_text_buffer_get_iter_at_child_anchor(textbuffer, &iter, editornotes[i].childanchor_caller_text);
      unsigned int offset_existing_note = gtk_text_iter_get_offset(&iter);
      if (!insert_offset_found) {
        if (offset_new_note <= offset_existing_note) {
          insert_offset = i;
          insert_offset_found = true;
        }
      }
    }
    if (insert_offset_found) {
      vector < EditorNote >::iterator iter(editornotes.begin());
      for (unsigned int i = 0; i < insert_offset; i++) {
        iter++;
      }
      editornotes.insert(iter, editornote);
    } else {
      editornotes.push_back(editornote);
    }
  }

  if (render) {
    // Render this note, if requested, placing the cursor in the new textview.
    display_notes_remainder(true);
    renumber_and_clean_notes_callers();
  }
  */
}


void Editor2::insert_table(const ustring & rawtext, GtkTextIter * iter)
/*
 Inserts a table in the editor.
 rawtext: The raw text of the table, e.g. "\tr \tc1 \tc2 \tc3 \tc4 ".
 iter:    Where to insert the note. If NULL, it inserts the note at the cursor.
 */
{
  /*
  // Get the location where to insert the note caller.
  GtkTextIter insertiter;
  if (iter)
    insertiter = *iter;
  else
    gtk_text_buffer_get_iter_at_mark(textbuffer, &insertiter, gtk_text_buffer_get_insert(textbuffer));

  // Insert the table at that location.  
  display_table(rawtext, insertiter);
  */
}


void Editor2::restore_snapshot(int pointer, bool undo)
// Restores a snapshot into the text buffer.
// undo: whether this is called from an undo routine.
{
  /*
  // No recording of undoable actions while this object is alive.
  PreventEditorUndo preventundo(&record_undo_level);

  // Restore the previous snapshot.
  EditorSnapshot snapshot = snapshots[pointer];
  text_load (snapshot.text);

  // Restoring the position of the cursor and the scrolled window.
  // This depends on whether the routine is called for undoing or redoing.
  // For undoing we need to restore the position of the next snapshot after this.
  // For redoing we take the snapshot as it is.
  if (undo) {
    pointer++;
    if ((unsigned int) pointer < snapshots.size()) {
      snapshot = snapshots[pointer];
    }
  }
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_offset (textbuffer, &iter, snapshot.insert);
  gtk_text_buffer_place_cursor (textbuffer, &iter);
  GtkAdjustment * adjustment = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolledwindow));
  gtk_adjustment_set_value (adjustment, snapshot.scroll);
  scroll_cursor_on_screen ();
  */
}


bool Editor2::recording_undo_actions()
// Returns whether to record undo-able actions.
{
  return (record_undo_level == 0);
}


void Editor2::on_textbuffer_changed(GtkTextBuffer * textbuffer, gpointer user_data)
{
  ((Editor2 *) user_data)->textbuffer_changed(textbuffer);
}


void Editor2::textbuffer_changed(GtkTextBuffer * textbuffer)
{
  spelling_trigger();
  trigger_undo_redo_recording ();
}


void Editor2::trigger_undo_redo_recording()
{
  if (recording_undo_actions()) {
    gw_destroy_source(textbuffer_changed_event_id);
    textbuffer_changed_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 100, GSourceFunc(on_textbuffer_changed_timeout), gpointer(this), NULL);
  }
}


bool Editor2::on_textbuffer_changed_timeout (gpointer user_data)
{
  ((Editor2 *) user_data)->textbuffer_changed_timeout();
  return false;
}


void Editor2::textbuffer_changed_timeout()
{
  // Clear the event id.
  textbuffer_changed_event_id = 0;

  /*
  // Create a new Snapshot object.
  EditorSnapshot snapshot (0);

  // Store chapter text.
  snapshot.text = get_chapter();

  // Store insert offset.
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_mark (textbuffer, &iter, gtk_text_buffer_get_insert (textbuffer));
  snapshot.insert = gtk_text_iter_get_offset (&iter);

  // Store scrollled window's position.
  GtkAdjustment * adjustment = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolledwindow));
  snapshot.scroll = gtk_adjustment_get_value (adjustment);

  // Store the snapshot, but only when it differs from the most recent one.
  bool store = true;
  if (!snapshots.empty()) {
    if (snapshot.text == snapshots[snapshots.size() -1].text) {
      store = false;
    }
  }
  if (store) {
    snapshots.push_back (snapshot);
  }

  // If there are too many snapshots, throw away the oldest.
  if (snapshots.size() > 100) {
    snapshots.pop_front();
  }  
  */ 
  // Reset the redo counter.
  redo_counter = 0;
}


void Editor2::highlight_searchwords()
// Highlights all the search words.
{
  // Destroy optional previous object.
  if (highlight)
    delete highlight;
  // Create a new object.
  //highlight = new Highlight(textbuffer, textview, project, editornotes, editortables, reference_tag, current_verse_number);
  //g_thread_create(GThreadFunc(highlight_thread_start), gpointer(this), false, NULL);
}

bool Editor2::on_highlight_timeout(gpointer data)
{
  return ((Editor2 *) data)->highlight_timeout();
}

bool Editor2::highlight_timeout()
{
  // Proceed if a highlighting object is there.
  if (highlight) {
    // Proceed if the locations for highlighting are ready.
    if (highlight->locations_ready) {
      // Possible insertion of highlighting tags may trigger storage of an unwanted
      // undo-able action. 
      // Therefore: No recording of undoable actions while this object is alive.
      PreventEditorUndo preventundo(&record_undo_level);
      highlight->highlight();
      // Delete and NULL the object making it ready for next use.
      delete highlight;
      highlight = NULL;
    }
  }
  return true;
}

void Editor2::highlight_thread_start(gpointer data)
{
  ((Editor2 *) data)->highlight_thread_main();
}

void Editor2::highlight_thread_main()
{
  // The time-consuming part of highlighting is to determine what bits of text
  // to highlight. Because it takes time, and the program should continue
  // to respond, it is done in a thread.
  if (highlight) {
    highlight->determine_locations();
  }
}

void Editor2::signal_editor_changed()
{
  gtk_button_clicked(GTK_BUTTON(changed_signal));
}

ustring Editor2::get_chapter()
{
  /*
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_start_iter(textbuffer, &startiter);
  gtk_text_buffer_get_end_iter(textbuffer, &enditer);
  */
  ustring chaptertext;
  /*
  usfm_get_text(textbuffer, startiter, enditer, &editornotes, &editortables, project, chaptertext, verse_restarts_paragraph);
  replace_text(chaptertext, "  ", " ");
  */
  return chaptertext;
}


void Editor2::spelling_trigger()
{
  if (project.empty())
    return;
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  if (!projectconfig)
    return;
  if (!projectconfig->spelling_check_get())
    return;
  gw_destroy_source(spelling_timeout_event_id);
  spelling_timeout_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 1000, GSourceFunc(on_spelling_timeout), gpointer(this), NULL);
}


bool Editor2::on_spelling_timeout(gpointer data)
{
  ((Editor2 *) data)->spelling_timeout();
  return false;
}


void Editor2::spelling_timeout()
{

  // Clear event id.
  spelling_timeout_event_id = 0;

  /*    
  // No recording of undoable actions while this object is alive.
  // It means that the textbuffer won't be modified if markers for spelling
  // mistakes are added or removed.
  PreventEditorUndo preventundo(&record_undo_level);
  // Check spelling of main textbuffer, ...
  spellingchecker->check(textbuffer);
  // ... embedded tables, ...
  for (unsigned int i = 0; i < editortables.size(); i++) {
    for (unsigned int row = 0; row < editortables[i].textbuffers.size(); row++) {
      for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
        spellingchecker->check(table_cell_get_buffer(editortables[i], row, column));
      }
    }
  }
  // ... and notes.
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    spellingchecker->check(editornotes[i].textbuffer);
  }
  // Signal spelling checked.
  gtk_button_clicked (GTK_BUTTON (spelling_checked_signal));
  */
}


void Editor2::on_button_spelling_recheck_clicked(GtkButton * button, gpointer user_data)
{
  ((Editor2 *) user_data)->spelling_timeout();
}


void Editor2::load_dictionaries()
{
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  if (projectconfig->spelling_check_get()) {
    spellingchecker->set_dictionaries(projectconfig->spelling_dictionaries_get());
  }
}


vector <ustring> Editor2::spelling_get_misspelled ()
{
  // Collect the misspelled words.
  vector <ustring> words;
  /*
  words = spellingchecker->get_misspellings(textbuffer);
  for (unsigned int i = 0; i < editortables.size(); i++) {
    for (unsigned int row = 0; row < editortables[i].textbuffers.size(); row++) {
      for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
        vector <ustring> words2 = spellingchecker->get_misspellings(table_cell_get_buffer(editortables[i], row, column));
        for (unsigned int i2 = 0; i2 < words2.size(); i2++) {
          words.push_back (words2[i2]);
        }
      }
    }
  }
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    vector <ustring> words2 = spellingchecker->get_misspellings(editornotes[i].textbuffer);
    for (unsigned int i2 = 0; i2 < words2.size(); i2++) {
      words.push_back (words2[i2]);
    }
  }
  */
  // Remove double ones.
  set <ustring> wordset (words.begin (), words.end());
  words.clear();
  words.assign (wordset.begin (), wordset.end());
  // Give words.
  return words;  
}


void Editor2::spelling_approve (const vector <ustring>& words)
{
  // Approve all the words in the list.
  // Since this may take time, a windows will show the progress.
  ProgressWindow progresswindow ("Adding words to dictionary", false);
  progresswindow.set_iterate (0, 1, words.size());
  for (unsigned int i = 0; i < words.size(); i++)  {
    progresswindow.iterate ();
    spellingchecker->add_to_dictionary (words[i].c_str());
  }
  // Trigger a new spelling check.
  spelling_trigger();
}


void Editor2::check_move_textview_to_textview()
{
  /*
  // Bail out if there was no change of textview in which the cursor was moved.
  bool changed = texview_to_textview_new != texview_to_textview_old;
  texview_to_textview_old = texview_to_textview_new;
  if (changed) {
    textview_to_textview_offset = -1;
    return;
  }
  // Find out whether the cursor moved in the textview. If so, bail out.
  GtkTextBuffer *textbuffer = gtk_text_view_get_buffer(texview_to_textview_new);
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_mark(textbuffer, &iter, gtk_text_buffer_get_insert(textbuffer));
  gint newoffset = gtk_text_iter_get_offset(&iter);
  changed = newoffset != textview_to_textview_offset;
  textview_to_textview_offset = newoffset;
  if (changed)
    return;

  // At this stage the cursor didn't move.
  // This means that the cursor is at some border of a textview.

  // Bail out if the textview is the main one. No need to move to another one.
  GtkWidget *currenttextview = GTK_WIDGET(texview_to_textview_new);
  if (currenttextview == textview)
    return;

  // Find out what movement to make for going to another textview.

  EditorMovementType movementtype = emtForward;
  switch (textview_to_textview_steptype) {
  case GTK_MOVEMENT_LOGICAL_POSITIONS: // move by forw/back graphemes
  case GTK_MOVEMENT_VISUAL_POSITIONS:  // move by left/right graphemes
  case GTK_MOVEMENT_WORDS:     // move by forward/back words
  case GTK_MOVEMENT_HORIZONTAL_PAGES:  // move horizontally by pages
    {
      if (textview_to_textview_stepcount > 0)
        movementtype = emtForward;
      else
        movementtype = emtBack;
      break;
    }
  case GTK_MOVEMENT_DISPLAY_LINES:     // move up/down lines (wrapped lines)
  case GTK_MOVEMENT_DISPLAY_LINE_ENDS: // move up/down lines (wrapped lines)
  case GTK_MOVEMENT_PARAGRAPHS:        // move up/down paragraphs (newline-ended lines)
  case GTK_MOVEMENT_PARAGRAPH_ENDS:    // move to either end of a paragraph
  case GTK_MOVEMENT_PAGES:     // move by pages
  case GTK_MOVEMENT_BUFFER_ENDS:       // move to ends of the buffer
    {
      if (textview_to_textview_stepcount > 0)
        movementtype = emtDown;
      else
        movementtype = emtUp;
      break;
    }
  }

  // Try whether the next textview is a note.
  GtkWidget *nextview = textview_note_get_another(textbuffer, currenttextview, editornotes, movementtype);
  if (nextview) {
    gtk_widget_grab_focus(nextview);
    return;
  }
  */
}

void Editor2::position_cursor_at_verse(const ustring & cursorposition, bool focus)
// This function starts the procedure to move the cursor of the editor to the 
// verse given.
{
  /*
  // Save the current verse.
  current_verse_number = cursorposition;

  // Find out whether we need to reposition the cursor. We will not move the 
  // cursor or scroll to it when the cursor is already on the right verse.
  bool reposition = current_verse_number != verse_number_get();

  // If the verse tracker is still off, postpone the repositioning.
  if (!verse_tracker_on)
    reposition = false;

  // Do the repositioning if needed.
  if (reposition) {

    // If requested, grab focus to get the scrolling done properly, and the user can type in the editor.
    if (focus) {
      programmatically_grab_focus(textview);
    }
    if ((current_verse_number == "0") || (current_verse_number.empty())) {
      // Verse 0 or empty: beginning of file.
      GtkTextIter iter;
      gtk_text_buffer_get_start_iter(textbuffer, &iter);
      gtk_text_buffer_place_cursor(textbuffer, &iter);
      scroll_cursor_on_screen ();
    } else {
      // The verse marker.
      ustring verse_marker = style_get_verse_marker(project);
      // Go through the buffer and find out about the verse.
      GtkTextIter iter;
      gtk_text_buffer_get_start_iter(textbuffer, &iter);
      do {
        set < ustring > styles = styles_at_iterator(iter);
        if (styles.find(verse_marker) != styles.end()) {
          GtkTextIter enditer = iter;
          gtk_text_iter_forward_chars(&enditer, 10);
          ustring verse = gtk_text_iter_get_slice(&iter, &enditer);
          size_t position = verse.find(" ");
          position = CLAMP(position, 0, verse.length());
          verse = verse.substr(0, position);
          // Position the cursor at the requested verse, if the verse is there.
          // Also look whether the verse is in a sequence or range of verses.
          bool position_here = (verse == current_verse_number);
          unsigned int verse_int = convert_to_int(current_verse_number);
          vector < unsigned int >combined_verses = verse_range_sequence(verse);
          for (unsigned int i2 = 0; i2 < combined_verses.size(); i2++) {
            if (verse_int == combined_verses[i2]) {
              position_here = true;
              current_verse_number = verse;
            }
          }
          if (position_here) {
            // Move the cursor to it.
            // Move it to the beginning of the text, if there is any.
            gtk_text_iter_forward_chars(&iter, verse.length() + 1);
            gtk_text_buffer_place_cursor(textbuffer, &iter);
            // Scroll also to it. It will scroll to the beginning of the text after the verse marker.
            // Exact scrolling is needed to put the line being edited near the top of the window.
            scroll_cursor_on_screen ();
            // Bail out.
            break;
          }
        }
      } while (gtk_text_iter_forward_char(&iter));
    }

    // Highlight search words.
    highlight_searchwords();
  }
  */
}

void Editor2::restart_verse_tracker()
// Restarts the verse tracker with a delay.
{
  verse_tracker_on = false;
  gw_destroy_source(verse_tracker_event_id);
  gw_destroy_source(start_verse_tracker_event_id);
  start_verse_tracker_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 100, GSourceFunc(on_restart_verse_tracker_timeout), gpointer(this), NULL);
}

bool Editor2::on_restart_verse_tracker_timeout(gpointer data)
// Timeout callback.
{
  return ((Editor2 *) data)->on_restart_verse_tracker();
}

bool Editor2::on_restart_verse_tracker()
// Usually called once after a chapter was loaded.
{
  // If gtk events are pending, don't start the verse tracker right now, but try again later.
  if (gtk_events_pending()) {
    return true;
  }
  // Start the regular cursor tracker.
  gw_destroy_source(verse_tracker_event_id);
  verse_tracker_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 300, GSourceFunc(on_verse_tracker_timeout), gpointer(this), NULL);
  return false;
}

bool Editor2::on_verse_tracker_timeout(gpointer data)
// Timeout callback.
{
  return ((Editor2 *) data)->verse_tracker_timeout();
}

bool Editor2::verse_tracker_timeout()
// Regular verse tracker.
{
  /*
  if (!verse_tracker_on) {
    verse_tracker_on = true;
    position_cursor_at_verse(current_verse_number, true);
  }

  // Speed up: Only proceed if the cursor changed.
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_mark(textbuffer, &iter, gtk_text_buffer_get_insert(textbuffer));
  if (!gtk_text_iter_equal  (&iter, &previous_insert_iter)) {
    previous_insert_iter = iter;
    // Only proceed if no text was selected.
    if (!gtk_text_buffer_get_has_selection (textbuffer)) {
      // Emit a signal if the verse number at the cursor changed.
      ustring versenumber = verse_number_get();
      if (versenumber != current_verse_number) {
        current_verse_number = versenumber;
        if (new_verse_signal) {
          gtk_button_clicked(GTK_BUTTON(new_verse_signal));
        }
      }
    }    
  }
  */
  // Next iteration.
  return true;
}


bool Editor2::move_cursor_to_spelling_error (bool next, bool extremity)
// Move the cursor to the next (or previous) spelling error.
// Returns true if it was found, else false.
{
  /*
  bool moved = spellingchecker->move_cursor_to_spelling_error (textbuffer, next, extremity);
  if (moved) {
    scroll_cursor_on_screen ();
  }
  return moved;
  */
  return false;
}


void Editor2::scroll_cursor_on_screen ()
{
  scroll_cursor_on_screen_timeout ();
  // At times scrolling once does not suffice.
  // It happens when the project notes window is open, and the user pressed F6 so as to go to a reference,
  // and this references is down in a chapter. In such cases the first scrolling is not enough,
  // and it needs a second one.
  g_timeout_add(300, GSourceFunc(on_scroll_cursor_on_screen_timeout), gpointer(this));
}


bool Editor2::on_scroll_cursor_on_screen_timeout(gpointer data)
{
  ((Editor2 *) data)->scroll_cursor_on_screen_timeout();
  return false;
}


void Editor2::scroll_cursor_on_screen_timeout()
{
  //GtkTextMark * mark = gtk_text_buffer_get_insert(textbuffer);
  //textview_scroll_to_mark (GTK_TEXT_VIEW (textview), mark, true);
}


void Editor2::apply_editor_action (EditorAction * action)
{
  GtkWidget * widget_that_should_grab_focus = NULL;
  switch (action->type) {

    case eatCreateParagraph:
    {
      // A new textbuffer that uses the text tag table.
      GtkTextBuffer * textbuffer;
      textbuffer = gtk_text_buffer_new(texttagtable);

      // The buffer's signal handlers.
      g_signal_connect(G_OBJECT(textbuffer), "insert-text", G_CALLBACK(on_buffer_insert_text_before), gpointer(this));
      g_signal_connect_after(G_OBJECT(textbuffer), "insert-text", G_CALLBACK(on_buffer_insert_text_after), gpointer(this));
      g_signal_connect(G_OBJECT(textbuffer), "delete-range", G_CALLBACK(on_buffer_delete_range_before), gpointer(this));
      g_signal_connect_after(G_OBJECT(textbuffer), "delete-range", G_CALLBACK(on_buffer_delete_range_after), gpointer(this));
      g_signal_connect(G_OBJECT(textbuffer), "apply-tag", G_CALLBACK(on_buffer_apply_tag), gpointer(this));
      g_signal_connect(G_OBJECT(textbuffer), "remove-tag", G_CALLBACK(on_buffer_remove_tag), gpointer(this));
      g_signal_connect(G_OBJECT(textbuffer), "insert-pixbuf", G_CALLBACK(on_buffer_insert_pixbuf), gpointer(this));

      // Add a new text view to the GUI to view the text buffer.
      GtkWidget *textview;
      textview = gtk_text_view_new_with_buffer(textbuffer);
      gtk_widget_show(textview);
      gtk_box_pack_start(GTK_BOX(vbox_v2), textview, false, false, 0);

      // Set some parameters of the view.
      gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(textview), FALSE);
      gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD);
      gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), editable);
      gtk_text_view_set_left_margin(GTK_TEXT_VIEW(textview), 5);
      gtk_text_view_set_right_margin(GTK_TEXT_VIEW(textview), 5);

      // Connect to the signals.
      spellingchecker->attach(textview);
      g_signal_connect_after((gpointer) textview, "move_cursor", G_CALLBACK(on_textview_move_cursor), gpointer(this));
      g_signal_connect((gpointer) textview, "motion-notify-event", G_CALLBACK(on_text_motion_notify_event), gpointer(this));
      g_signal_connect_after((gpointer) textview, "grab_focus", G_CALLBACK(on_textview_grab_focus), gpointer(this));
      //g_signal_connect((gpointer) textview, "event-after", G_CALLBACK(on_text_event_after), gpointer(this));
      //g_signal_connect((gpointer) textview, "key-press-event", G_CALLBACK(text_key_press_event_before), gpointer(this));
      //g_signal_connect_after((gpointer) textview, "key-press-event", G_CALLBACK(text_key_press_event_after), gpointer(this));
      //g_signal_connect((gpointer) textview, "visibility-notify-event", G_CALLBACK(screen_visibility_notify_event), gpointer(this));
      //g_signal_connect((gpointer) textview, "button_press_event", G_CALLBACK(on_textview_button_press_event), gpointer(this));
      //g_signal_connect((gpointer) textview, "size-allocate", G_CALLBACK(on_related_widget_size_allocated), gpointer(this));

      // Store a pointer to the new textview.
      EditorActionCreateParagraph * paragraphaction = static_cast <EditorActionCreateParagraph *> (action);
      paragraphaction->widget = textview;
      
      // Let the newly created textview be earmarked to grab focus
      // so that the user can type in it,
      // and the internal Editor logic knows about it.
      widget_that_should_grab_focus = textview;

      break;
    }

    case eatSetParagraphStyle:
    {
      // Cast the action to the right object to work with.
      EditorActionSetParagraphStyle * style_action = static_cast <EditorActionSetParagraphStyle *> (action);
      // Look for the parent paragraph.
      EditorActionCreateParagraph * parent_action = identifier2paragraphcreationaction (style_action->parent_identifier);
      if (parent_action) {
        // Store the old paragraph style, and set the new.
        style_action->previous_style = parent_action->style;
        parent_action->style = style_action->current_style;
        // Apply it to the widget.
        textview_apply_paragraph_style (parent_action->widget, parent_action->style);
      } else {
        gw_critical ("Could not apply style to paragraph with identifier " + convert_to_string (style_action->parent_identifier));
      }

      break;
    }

    case eatInsertText:
    {
      // Cast the action to the right object to work with.
      EditorActionInsertText * insert_action = static_cast <EditorActionInsertText *> (action);
      // Get the parent paragraph.
      EditorActionCreateParagraph * parent_action = identifier2paragraphcreationaction (insert_action->parent_identifier);
      // Insert text.
      GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (parent_action->widget));
      if (textbuffer) {
        GtkTextIter iter;
        gtk_text_buffer_get_iter_at_offset (textbuffer, &iter, insert_action->offset);
        gtk_text_buffer_insert (textbuffer, &iter, insert_action->text.c_str(), -1);
        textview_apply_paragraph_style (parent_action->widget, parent_action->style);
      } else {
        gw_critical ("Could not find the paragraph where to insert text " + insert_action->text);
      }
      break;
    }

    case eatDeleteText:
    {
      // Cast the action to the right object to work with.
      EditorActionDeleteText * delete_action = static_cast <EditorActionDeleteText *> (action);
      // Get the parent paragraph.
      EditorActionCreateParagraph * parent_action = identifier2paragraphcreationaction (delete_action->parent_identifier);
      // Delete text.
      GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (parent_action->widget));
      if (textbuffer) {
        GtkTextIter startiter;
        gtk_text_buffer_get_iter_at_offset (textbuffer, &startiter, delete_action->offset);
        GtkTextIter enditer;
        gtk_text_buffer_get_iter_at_offset (textbuffer, &enditer, delete_action->offset + delete_action->length);
        gtk_text_buffer_delete (textbuffer, &startiter, &enditer);
      } else {
        gw_critical ("Could not find the paragraph from where to delete text");
      }
      break;
    }

  }

  // Store this action in the list of ones done.
  actions_done.push_back (action);

  // If there's any widget that was earmarked to be focused, grab its focus.
  // This can only be done at the end when the whole object has been set up,
  // because the callback for grabbing the focus uses this object.
  if (widget_that_should_grab_focus) {
    gtk_widget_grab_focus (widget_that_should_grab_focus);
  }
}


bool Editor2::create_editor_objects_starting_new_paragraph(vector <EditorAction *>& editoractions, ustring & line, ustring & character_mark, const ustring & marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
// This function deals with a marker that starts a paragraph.
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype(project, marker, type, subtype);
        if (style_get_starts_new_line_in_editor(type, subtype)) {

          // Get the currently focused paragraph action.
          EditorActionCreateParagraph * paragraph_action = identifier2paragraphcreationaction (focused_textview_identifier);
          
          // Because the ends of lines of USFM are changed to spaces, 
          // and these spaces get inserted in the editor, 
          // if a new line starts, we need to trim these away.
          EditorActionDeleteText * trim_action = paragraph_delete_character_before_text_insertion_point_if_space(paragraph_action);
          if (trim_action) {
            editoractions.push_back (trim_action);
          }

          // Normally a new paragraph should create a new textview.
          // This is because each textview contains only one paragraph.
          // But if the currently focused textview does not have any content, 
          // that existing one will be used.
          bool textview_has_content = true;
          if (paragraph_action) {
            GtkWidget * textview = paragraph_action->widget;
            GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
            textview_has_content = !textbuffer_empty(textbuffer);
          }
          if (textview_has_content) {
            paragraph_action = new EditorActionCreateParagraph (0);
            editoractions.push_back (paragraph_action);
            focused_textview_identifier = paragraph_action->identifier;
          }

          // The new paragraph and character markup.
          EditorActionSetParagraphStyle * style_action = new EditorActionSetParagraphStyle (marker, paragraph_action);
          editoractions.push_back (style_action);
          character_mark.clear();

          // Some styles insert their marker: Do that here if appropriate.
          if (style_get_displays_marker(type, subtype)) {
            // Todo work here editor_text_append(textbuffer, line.substr(0, marker_length), paragraph_mark, "");
          }

          // Remove the markup from the line.
          line.erase(0, marker_length);

          // The information was processed: return true.
          return true;
        }
      }
    }
  }
  return false;
}


void Editor2::create_editor_objects_to_ensure_normal_paragraph(vector <EditorAction *>& editoractions, const ustring& project, GtkWidget * textview, ustring & line, ustring & paragraph_mark, ustring & character_mark)
/*
 This function ensures that a normal paragraph starts.
 line: The raw USFM.
 paragraph_mark: The paragarph style to investigate whether it is a normal paragraph.
 character_mark: The character style.
 */
{
  // Bail out if the current paragraph mark points to a normal paragraph, not a title or heading.
  StyleType type;
  int subtype;
  marker_get_type_and_subtype(project, paragraph_mark, type, subtype);
  if (type == stStartsParagraph)
    if (subtype == ptNormalParagraph)
      return;

  // Review marker \nb, bail out if it does not exist.
  ustring marker = "nb";
  marker_get_type_and_subtype(project, marker, type, subtype);
  if (type != stStartsParagraph)
    return;
  if (subtype != ptNormalParagraph)
    return;

  // Insert the marker in the line, and apply it.
  line.insert(0, usfm_get_full_opening_marker(marker));
  create_editor_objects_starting_new_paragraph(editoractions, line, character_mark, marker, 0, usfm_get_full_opening_marker(marker).length(), true, true);
}


bool Editor2::create_editor_objects_for_text_verse_number(vector <EditorAction *>& editoractions, const ustring& project, GtkWidget * textview, ustring & line, ustring & paragraph_mark, ustring & character_mark, const ustring & marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
/*
 This function returns true if a verse number was loaded in the formatted view.
 Else it returns false.
 Verses are special in that their style only applies to the verse number,
 not to the text that follows.
 */
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype(project, marker, type, subtype);
        if (style_get_starts_verse_number(type, subtype)) {
          // If a verse number starts, ensure that it happens in a normal paragraph.
          create_editor_objects_to_ensure_normal_paragraph(editoractions, project, textview, line, paragraph_mark, character_mark);
          // Clear the character markup (the paragraph markup remains as it is).
          character_mark.clear();
          // Get the textbuffer.
          GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
          // Some styles insert their marker: Do that here if appropriate.
          if (style_get_displays_marker(type, subtype)) {
            editor_text_append(textbuffer, line.substr(0, marker_length), paragraph_mark, "");
          }
          // Remove the markup from the line.
          line.erase(0, marker_length);
          // Get verse number. Handle combined verses too, e.g. 10-12b, etc.
          size_t position = line.find(" ");
          if (position == string::npos)
            position = line.length();
          ustring versenumber = line.substr(0, position);
          // Display the verse and erase it from the input buffer.
          editor_text_append(textbuffer, versenumber, paragraph_mark, marker);
          line.erase(0, position);
          // The information was processed: return true.
          return true;
        }
      }
    }
  }
  return false;
}


unsigned int Editor2::textview2identifier (GtkWidget * textview)
// Given a pointer to a GtkTextView, it returns its identifier.
{
  unsigned int identifier = 0;
  // Look through the actions done to see if the GtkTextView has been created.
  for (unsigned int i = 0; i < actions_done.size(); i++) {
    EditorAction * action = actions_done[i];
    if (action->type == eatCreateParagraph) {
      EditorActionCreateParagraph * paragraphaction = static_cast <EditorActionCreateParagraph *> (action);
      if (paragraphaction->widget == textview) {
        identifier = paragraphaction->identifier;
      }
    }
  }
  // We also need to check whether this one was not destroyed. But this is still to be implemented.
  return identifier;
}


EditorActionCreateParagraph * Editor2::identifier2paragraphcreationaction (unsigned int identifier)
// Given an identifier it returns a pointer to its EditorAction.
{
  EditorActionCreateParagraph * action = NULL;
  for (unsigned int i = 0; i < actions_done.size(); i++) {
    if (actions_done[i]->type == eatCreateParagraph) {
      EditorActionCreateParagraph * action2 = static_cast <EditorActionCreateParagraph *> (actions_done[i]);
      if (action2->identifier == identifier) {
        action = action2;
      }
    }
  }
  return action;
}


void Editor2::create_editor_objects_fallback (vector <EditorAction *>& editoractions, ustring& line, ustring& character_mark, size_t marker_pos, bool marker_found)
// This is a fallback function to load the text.
{
  // Storage for the string to insert.
  ustring insertion;  

  if ((marker_found) && (marker_pos == 0)) {
    // It should not occur that a marker is right at the start and was not handled.
    // It gets handled here to prevent an infinite loop.
    insertion = line.substr(0, 1);
    line.erase(0, 1);
  }
  else if ((marker_found) && (marker_pos != string::npos) && (marker_pos > 0)) {
    // Load text till the next marker.
    insertion = line.substr(0, marker_pos);
    line.erase(0, marker_pos);
  } 
  else {
    // No markup found: The whole line is loaded at once.
    insertion = line;
    line.clear();
  }
  
  // Create editor object for inserting the text.
  EditorActionCreateParagraph * paragraph_action = identifier2paragraphcreationaction (focused_textview_identifier);
  if (paragraph_action) {
    gint insertion_offset = editor_paragraph_insertion_point_get_offset (paragraph_action);
    EditorActionInsertText * insert_action = new EditorActionInsertText (paragraph_action, insertion_offset, insertion);
    editoractions.push_back (insert_action);
  } else {
    gw_critical ("Could not insert text since no paragraph was found with identifier " + convert_to_string (focused_textview_identifier));
  }
}


