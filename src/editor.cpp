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


#include "utilities.h"
#include <glib.h>
#include "editor.h"
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


Editor::Editor (GtkWidget * vbox, GtkWidget * notebook_page, GtkWidget * tab_label, const ustring& project_in):
current_reference (0, 1000, "")
{
  // Save and initialize variables.
  parent_vbox = vbox;
  parent_notebook_page = notebook_page;
  my_tab_label = tab_label;
  project = project_in;
  focus_set = false;
  do_not_process_child_anchors_being_deleted = false;
  textview_allocated_width = 0;
  texttagtable = NULL;
  record_undo_level = 0;
  previous_hand_cursor = false;
  highlight = NULL;
  testbool = false;
  editable = false;
  event_id_show_quick_references = 0;

  // Create data that is needed for any of the possible formatted views.
  create_or_update_formatting_data ();

  // The title button with progressbar for focus and close button.
  hbox_title = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_title);
  gtk_box_pack_start (GTK_BOX (parent_vbox), hbox_title, FALSE, FALSE, 0);

  titlebutton = gtk_button_new ();
  gtk_widget_show (titlebutton);
  gtk_box_pack_start (GTK_BOX (hbox_title), titlebutton, true, true, 0);
  gtk_button_set_relief (GTK_BUTTON (titlebutton), GTK_RELIEF_NONE);
  gtk_button_set_focus_on_click (GTK_BUTTON (titlebutton), FALSE);
  GTK_WIDGET_UNSET_FLAGS (titlebutton, GTK_CAN_FOCUS);

  progressbar = gtk_progress_bar_new ();
  gtk_widget_show (progressbar);
  gtk_container_add (GTK_CONTAINER (titlebutton), progressbar);
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), 0);
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progressbar), "");
  gtk_progress_bar_set_ellipsize (GTK_PROGRESS_BAR (progressbar), PANGO_ELLIPSIZE_MIDDLE);

  close_button = gtk_button_new ();
  gtk_widget_show (close_button);
  gtk_box_pack_start (GTK_BOX (hbox_title), close_button, FALSE, FALSE, 0);
  gtk_button_set_focus_on_click (GTK_BUTTON (close_button), FALSE);
  GTK_WIDGET_UNSET_FLAGS (close_button, GTK_CAN_FOCUS);

  GtkWidget *image_close;
  image_close = gtk_image_new_from_stock ("gtk-close", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_close);
  gtk_container_add (GTK_CONTAINER (close_button), image_close);

  g_signal_connect ((gpointer) titlebutton, "clicked", G_CALLBACK (on_button_titlebar_clicked), gpointer (this));

  // Set the height of title bar and close button.
  GtkRequisition sizerequisition;
  gtk_widget_size_request (progressbar, &sizerequisition);
  gint default_progressbar_height = sizerequisition.height;  
  gtk_widget_set_size_request (image_close, default_progressbar_height / 2, default_progressbar_height / 2);
  gtk_widget_set_size_request (titlebutton, -1, default_progressbar_height * 10 / 8);

  // The scrolled window that contains the main formatted view.
  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_box_pack_start (GTK_BOX (parent_vbox), scrolledwindow, true, true, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  // Set pointer to the vertical box to NULL if the widget get destroyed.
  g_signal_connect (G_OBJECT (parent_vbox), "destroy", GTK_SIGNAL_FUNC (gtk_widget_destroyed), &parent_vbox);
  
  // A textbuffer to store all text.
  textbuffer = gtk_text_buffer_new (texttagtable);
  
  // The buffer's signal handlers.
  g_signal_connect (G_OBJECT (textbuffer), "insert-text", G_CALLBACK (on_buffer_insert_text_before), gpointer (this));
  g_signal_connect_after (G_OBJECT (textbuffer), "insert-text", G_CALLBACK (on_buffer_insert_text_after), gpointer (this));
  g_signal_connect (G_OBJECT (textbuffer), "delete-range", G_CALLBACK (on_buffer_delete_range_before), gpointer (this));
  g_signal_connect_after (G_OBJECT (textbuffer), "delete-range", G_CALLBACK (on_buffer_delete_range_after), gpointer (this));
  g_signal_connect (G_OBJECT (textbuffer), "apply-tag", G_CALLBACK (on_buffer_apply_tag), gpointer (this));
  g_signal_connect (G_OBJECT (textbuffer), "remove-tag", G_CALLBACK (on_buffer_remove_tag), gpointer (this));
  g_signal_connect (G_OBJECT (textbuffer), "insert-child-anchor", G_CALLBACK (on_buffer_insert_child_anchor), gpointer (this));
  g_signal_connect (G_OBJECT (textbuffer), "insert-pixbuf", G_CALLBACK (on_buffer_insert_pixbuf), gpointer (this));
  g_signal_connect (G_OBJECT (textbuffer), "changed", G_CALLBACK (on_textbuffer_changed), gpointer (this));
  g_signal_connect (G_OBJECT (textbuffer), "modified-changed", G_CALLBACK (on_textbuffer_modified_changed), gpointer (this));

  // A text view to display the buffer.
  textview = gtk_text_view_new_with_buffer (textbuffer);
  gtk_widget_show (textview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), textview);
  gtk_text_view_set_accepts_tab (GTK_TEXT_VIEW (textview), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview), GTK_WRAP_WORD);
  gtk_text_view_set_editable  (GTK_TEXT_VIEW (textview), FALSE);
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (textview), 5);

  // The view's signal handlers.
  g_signal_connect_after ((gpointer) textview, "move_cursor", G_CALLBACK (on_textview_move_cursor), gpointer(this));
  g_signal_connect_after ((gpointer) textview, "grab_focus", G_CALLBACK (on_textview_grab_focus), gpointer(this));
  g_signal_connect ((gpointer) textview, "motion-notify-event", G_CALLBACK (on_text_motion_notify_event), gpointer(this));
  g_signal_connect ((gpointer) textview, "event-after", G_CALLBACK (on_text_event_after), gpointer(this));
  g_signal_connect ((gpointer) textview, "key-press-event", G_CALLBACK (text_key_press_event_before), gpointer(this));
  g_signal_connect_after ((gpointer) textview, "key-press-event", G_CALLBACK (text_key_press_event_after), gpointer(this));
  g_signal_connect ((gpointer) textview, "visibility-notify-event", G_CALLBACK (screen_visibility_notify_event), gpointer(this));
  g_signal_connect ((gpointer) textview, "button_press_event", G_CALLBACK (on_textview_button_press_event), gpointer(this));
  g_signal_connect ((gpointer) textview, "size-allocate", G_CALLBACK (on_related_widget_size_allocated), gpointer (this));

  // Initialize the last focused textview to the main textview.
  last_focused_widget = textview;

  // Buttons to give signals.
  new_verse_signal = gtk_button_new ();
  new_styles_signal = gtk_button_new ();
  word_double_clicked_signal = gtk_button_new ();
  reload_signal = gtk_button_new ();
  focus_signal = gtk_button_new ();
  changed_signal = gtk_button_new ();
  
  // Initialize a couple of event ids.
  textview_cursor_moved_delayer_event_id = 0;
  grab_focus_event_id = 0;
  position_cursor_at_verse_event_id = 0;
  undo_redo_event_id = 0;
  save_timeout_event_id = 0;
  highlight_timeout_event_id = 0;

  // Tag for highlighting search words.
  // Note that for convenience the GtkTextBuffer function is called. 
  // But this adds the reference to the GtkTextTagTable, so making it available
  // to any other buffer that uses the same text tag table.
  reference_tag = gtk_text_buffer_create_tag (textbuffer, NULL, "background", "khaki", NULL);

  // Highlighting searchwords timeout.
  highlight_timeout_event_id = g_timeout_add_full (G_PRIORITY_DEFAULT, 500, GSourceFunc (on_highlight_timeout), gpointer(this), NULL);
  
  // Automatic saving of the file, periodically.
  save_timeout_event_id = g_timeout_add_full (G_PRIORITY_DEFAULT, 60000, GSourceFunc (on_save_timeout), gpointer(this), NULL);
  
  // The display will not properly show the cursor always, because it has not
  // yet been created fully on widget creation.
  g_timeout_add (1000, GSourceFunc (on_widget_creation_timeout), gpointer(this));

  // Fonts.
  set_font ();
  
  // Grab focus.
  focus_programmatically_being_grabbed = false;
  gtk_widget_grab_focus (textview);  
}


Editor::~Editor ()
{
  // Save the chapter.
  chapter_save ();
  
  // Destroy the signalling buttons.
  gtk_widget_destroy (new_verse_signal);
  gtk_widget_destroy (new_styles_signal);
  gtk_widget_destroy (word_double_clicked_signal);
  gtk_widget_destroy (reload_signal);
  gtk_widget_destroy (focus_signal);
  gtk_widget_destroy (changed_signal);
  
  // Destroy the texttag tables.
  g_object_unref (texttagtable);
  
  // Destroy a couple of timeout sources.
  gw_destroy_source (textview_cursor_moved_delayer_event_id);
  gw_destroy_source (grab_focus_event_id);
  gw_destroy_source (position_cursor_at_verse_event_id);
  gw_destroy_source (undo_redo_event_id);
  gw_destroy_source (save_timeout_event_id);
  gw_destroy_source (highlight_timeout_event_id);
  
  // Destroy possible highlight object.
  if (highlight) delete highlight;
    
  // Destroy topmost widget. 
  // Normally, when shutting down Bibledit, Gtk will destroy it. 
  // But if we don't shut down Bibledit, but only remove the editor, 
  // then the widget need to be destroyed manually, 
  // which is what is done here.
  if (parent_vbox) gtk_widget_destroy (parent_vbox);
}


void Editor::book_set (unsigned int book_in)
{
  book = book_in;
}


void Editor::chapter_load (unsigned int chapter_in, vector <ustring> * lines_in)
// Loads a chapter with the number "chapter_in".
// If "lines_in" exists, it load these instead of getting the chapter.
{
  // No recording of undoable actions while this object is alive.
  PreventEditorUndo preventundo (&record_undo_level);
  
  // Get rid of possible previous text.
  gtk_text_buffer_set_text (textbuffer, "", -1);

  // Save chapter number.
  chapter = chapter_in;
  
  // Settings.
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig (project);
  
  // Load text in memory.
  vector <ustring> lines;
  if (lines_in) lines = * lines_in;
  else lines = project_retrieve_chapter (project, book, chapter);
  
  // Deal with editable / non-editable.
  editable = true;
  if (lines.empty()) editable = false;
  if (!projectconfig->editable_get()) editable = false;
  if (lines_in) editable = false;
  gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), editable);
  
  // Make one long line containing the whole chapter.
  // This is done so as to exclude any possibility that the editor does not
  // properly load a possible chapter that has line-breaks at unexpected places.
  // Add one space to the end so as to accomodate situation such as that the 
  // last marker is "\toc". Without that extra space it won't see this marker,
  // because the formatter looks for "\toc ". The space will solve the issue.
  ustring line;
  for (unsigned int i = 0; i < lines.size(); i++) {
    if (!line.empty ())
      line.append (" ");
    line.append (lines[i]);
  }
  line.append (" ");

  // Load text in the view.
  // Load it at the position of the text insertion point marker.
  // Because the buffer has just been cleared, this marker will always be at the end.
  editornotes.clear ();
  editortables.clear ();
  ustring paragraph_mark (unknown_style ());
  ustring character_mark;
  ustring marker;
  size_t marker_pos;
  size_t marker_length;
  bool is_opener;
  bool marker_found;
  while (!line.empty ()) {
    marker_found = usfm_search_marker (line, marker, marker_pos, marker_length, is_opener);
    if (load_text_not_starting_with_marker (textbuffer, line, paragraph_mark, character_mark, marker_pos, marker_length, marker_found));
    else if (load_text_table_raw (line, paragraph_mark ,marker, marker_pos, marker_length, is_opener, marker_found));
    else if (load_text_starting_new_paragraph (textbuffer, line, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found));
    else if (load_text_starting_character_style (textbuffer, line, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found));
    else if (load_text_ending_character_style (textbuffer, line, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found));
    else if (load_text_verse_number (line, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found));
    else if (load_text_note_raw (line, marker, marker_pos, marker_length, is_opener, marker_found));
    else load_text_with_unknown_markup (textbuffer, line, paragraph_mark, character_mark);
  }

  // Finalize displaying any notes.
  display_notes_remainder (false);
  renumber_and_clean_notes_callers ();

  // Clear undo buffer.
  editorundoes.clear ();
  
  // Set the buffer(s) non-modified.
  textbuffers_set_unmodified (textbuffer, editornotes, editortables);
 
  // Update gui for styles.
  signal_if_styles_changed ();
}


void Editor::chapter_save ()
{
  // If the text is not editable, bail out.
  if (!gtk_text_view_get_editable (GTK_TEXT_VIEW (textview))) return;
    
  // If the text was not changed, bail out.
  if (!textbuffers_get_modified (textbuffer, editornotes, editortables)) return;
    
  // Get the USFM text.
  ustring chaptertext = get_chapter ();

  // If the chapter text is completely empty, 
  // that means that the user has deleted everything.
  // This is interpreted as to mean that the user wishes to delete this chapter.
  // After asking for confirmation, delete the chapter.
  bool reload = false;
  reload_chapter_number = chapter;
  bool save_action_is_over = false;
  if (chaptertext.empty ()) {
    if (gtkw_dialog_question (NULL, "The chapter is empty.\n"
                                    "Do you wish to delete this chapter?", GTK_RESPONSE_YES) == GTK_RESPONSE_YES) {
      project_remove_chapter (project, book, chapter);
      save_action_is_over = true;
      reload = true;
      if (chapter > 0) reload_chapter_number = chapter - 1;
    }
  }    
  
  // Temporal code to compare editor's input and output.
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
  
  // If the text has not yet been dealt with, save it.  
  if (!save_action_is_over) {
    // Clean it up a bit and divide it into lines.
    ParseLine parseline (trim (chaptertext));
    // Add verse information.
    CategorizeChapterVerse ccv (parseline.lines);
    /*
    We have noticed people editing Bibledit's data directly. 
    If this was done with OpenOffice, and then saving it as text again, 
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
      message = "Chapter " + convert_to_string (chapter) + " was loaded, "
                "but it appears that the chapter number has been changed to " + convert_to_string (chapter_in_text) + ".\n"
                "Do you wish to save the text as a different chapter, that is, as chapter " + convert_to_string (chapter_in_text) + "?";
      if (gtkw_dialog_question (NULL, message.c_str ()) == GTK_RESPONSE_YES) {
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
        vector <unsigned int> chapters = project_get_chapters (project, book);
        set <unsigned int> chapter_set (chapters.begin (), chapters.end ());
        if (chapter_set.find (confirmed_chapter_number) != chapter_set.end ()) {
          message = "The new chapter number already exists\n"
                    "Do you wish to overwrite it?";
          if (gtkw_dialog_question (NULL, message.c_str ()) == GTK_RESPONSE_NO) {
            gtkw_dialog_info (NULL, "The changes have been discarded");
            save_action_is_over = true;
            reload = true;
          }
        }
      }
    }
    
    // Store chapter in database.
    if (!save_action_is_over) project_store_chapter (project, book, ccv);
    save_action_is_over = true;
  }

  // Set the buffer(s) non-modified.
  textbuffers_set_unmodified (textbuffer, editornotes, editortables);
  
  // Possible reload signal.
  if (reload) {
    gtk_button_clicked (GTK_BUTTON (reload_signal));
  }
}


ustring Editor::text_get (GtkTextIter * startiter, GtkTextIter * enditer)
// Iterate through the buffer and collect and assemble usfm text.
{
  return "";
}


ustring Editor::text_get_selection ()
// Retrieves the selected text from the textview that has the focus, 
// and returns it as USFM code.
{
  // Variable to hold the USFM text that is going to be produced.
  ustring text;
  
  // Get the iterators at the selection bounds of the main textview or note or 
  // table, whichever has the focus. Get the text too.
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_selection_bounds (textbuffer, &startiter, &enditer);
  if (GTK_WIDGET_HAS_FOCUS (textview)) {
    gtk_text_buffer_get_selection_bounds (textbuffer, &startiter, &enditer);
    usfm_get_text (textbuffer, startiter, enditer, &editornotes, &editortables, project, text);
  }
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    if (GTK_WIDGET_HAS_FOCUS (editornotes[i].textview)) {
      GtkTextBuffer * buffer = editornotes[i].textbuffer;
      gtk_text_buffer_get_selection_bounds (buffer, &startiter, &enditer);
      usfm_get_note_text (editornotes[i], startiter, enditer, project, text);
    }
  }
  for (unsigned int i = 0; i < editortables.size (); i++) {
    for (unsigned int row = 0; row < editortables[i].textviews.size (); row++) {
      for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
        if (GTK_WIDGET_HAS_FOCUS (table_cell_get_view (editortables[i], row, column))) {
          GtkTextBuffer * buffer = table_cell_get_buffer (editortables[i], row, column);
          gtk_text_buffer_get_selection_bounds (buffer, &startiter, &enditer);
          usfm_get_text (buffer, startiter, enditer, NULL, NULL, project, text);
        }
      }
    }
  }

  // Return the text.
  return text;
}


void Editor::text_erase_selection ()
// Erases the selected text from the textview that has the focus.
{
  // Get the iterators at the selection bounds of the main textview or note or 
  // table, whichever has the focus. Erase that bit.
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_selection_bounds (textbuffer, &startiter, &enditer);
  if (GTK_WIDGET_HAS_FOCUS (textview)) {
    gtk_text_buffer_get_selection_bounds (textbuffer, &startiter, &enditer);
    gtk_text_buffer_delete (textbuffer, &startiter, &enditer);
  }
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    if (GTK_WIDGET_HAS_FOCUS (editornotes[i].textview)) {
      GtkTextBuffer * buffer = editornotes[i].textbuffer;
      gtk_text_buffer_get_selection_bounds (buffer, &startiter, &enditer);
      gtk_text_buffer_delete (buffer, &startiter, &enditer);
    }
  }
  for (unsigned int i = 0; i < editortables.size (); i++) {
    for (unsigned int row = 0; row < editortables[i].textviews.size (); row++) {
      for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
        if (GTK_WIDGET_HAS_FOCUS (table_cell_get_view (editortables[i], row, column))) {
          GtkTextBuffer * buffer = table_cell_get_buffer (editortables[i], row, column);
          gtk_text_buffer_get_selection_bounds (buffer, &startiter, &enditer);
          gtk_text_buffer_delete (buffer, &startiter, &enditer);
        }
      }
    }
  }
}


void Editor::text_insert (ustring text)
/*
This inserts plain or USFM text at the cursor location 
of the focused textview.

If plain text is pasted, the style(s) now applied at the cursor continue in the 
pasted text.
If usfm text is pasted, normal formatting rules apply.

If text is selected, this is erased first.
*/
{
  // If the text is not editable, bail out.
  if (!editable) return;

  // Get the textbuffer that is focused.
  GtkTextBuffer * buffer = textbuffer;
  if (GTK_WIDGET_HAS_FOCUS (textview)) {
    buffer = textbuffer;
  }
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    if (GTK_WIDGET_HAS_FOCUS (editornotes[i].textview)) {
      buffer = editornotes[i].textbuffer;
    }
  }
  for (unsigned int i = 0; i < editortables.size (); i++) {
    for (unsigned int row = 0; row < editortables[i].textviews.size (); row++) {
      for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
        if (GTK_WIDGET_HAS_FOCUS (table_cell_get_view (editortables[i], row, column))) {
          buffer = table_cell_get_buffer (editortables[i], row, column);
        }
      }
    }
  }

  // Erase selected text.
  gtk_text_buffer_delete_selection (buffer, true, editable);

  // Move the cursor to the insert position.
  GtkTextIter insert_iter;
  gtk_text_buffer_get_iter_at_mark (buffer, &insert_iter, gtk_text_buffer_get_insert (buffer));
  gtk_text_buffer_place_cursor (buffer, &insert_iter);

  // If not pasting in the main text body, but in a table or note, 
  // remove all markers to prevent trouble.
  if (buffer != textbuffer) {
    ustring marker;
    size_t marker_pos;
    size_t marker_length;
    bool is_opener;
    while (usfm_search_marker (text, marker, marker_pos, marker_length, is_opener)) {
      text.erase (marker_pos, marker_length);
    }
  }
  
  // Initialize paragraph and character styles to the ones prevalent at the insert location.
  ustring paragraph_mark;
  ustring character_mark;
  // It seems to be more natural to initialize the markers according to the 
  // character that the cursor is after: move the iterator backward.
  // This addresses the following bug:
  //   When text is pasted just before a verse number, it gets it wrong.
  //   It puts the text in the verse style.
  //   It does not happen with just typing, but only with pasting text.
  //   The same thing happens when pasting before any character style.
  gtk_text_iter_backward_char (&insert_iter);
  get_styles_at_iterator (insert_iter, paragraph_mark, character_mark);
  // When inserting text at the end of the buffer, no paragraph mark will be found.
  // In that case iterate back till there is one.
  while ((paragraph_mark.empty ()) && gtk_text_iter_backward_char (&insert_iter)) {
    get_styles_at_iterator (insert_iter, paragraph_mark, character_mark);
  }
  // In the unlikely case that the paragraph style still was not found, use a default.
  if (paragraph_mark.empty ()) paragraph_mark = "p";
  
  // Insert the text in the view at the position of the text insertion point marker.
  ustring marker;
  size_t marker_pos;
  size_t marker_length;
  bool is_opener;
  bool marker_found;
  while (!text.empty ()) {
    marker_found = usfm_search_marker (text, marker, marker_pos, marker_length, is_opener);
    if (load_text_not_starting_with_marker (buffer, text, paragraph_mark, character_mark, marker_pos, marker_length, marker_found));
    else if (load_text_table_raw (text, paragraph_mark, marker, marker_pos, marker_length, is_opener, marker_found));
    else if (load_text_starting_new_paragraph (buffer, text, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found));
    else if (load_text_starting_character_style (buffer, text, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found));
    else if (load_text_ending_character_style (buffer, text, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found));
    else if (load_text_verse_number (text, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found));
    else if (load_text_note_raw (text, marker, marker_pos, marker_length, is_opener, marker_found));
    else load_text_with_unknown_markup (buffer, text, paragraph_mark, character_mark);
  }

  // Finalize displaying any notes.
  display_notes_remainder (false);
  renumber_and_clean_notes_callers ();
}


void Editor::show_quick_references ()
// Starts the process to show the quick references.
// A delaying routine is used to make the program more responsive.
// That is, the quick references are now shown at each change,
// but only shortly after. 
// Without this pasting a long text in the footnote takes a lot of time.
{
  gw_destroy_source (event_id_show_quick_references);
  event_id_show_quick_references = g_timeout_add_full (G_PRIORITY_DEFAULT, 100, GSourceFunc (show_quick_references_timeout), gpointer(this), NULL);
}


bool Editor::show_quick_references_timeout (gpointer user_data)
{
  ((Editor *) user_data)->show_quick_references_execute ();
  return false;
}


void Editor::show_quick_references_execute ()
// Takes the text of the references in the note that has the cursor,
// and shows that text in the quick reference area.
{
  // If we're not in a note, bail out.
  if (last_focused_type () != etvtNote) return;
  
  // Get the text of the focused note.
  GtkTextBuffer * note_buffer = last_focused_textbuffer ();
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_start_iter (note_buffer, &startiter);
  gtk_text_buffer_get_end_iter (note_buffer, &enditer);
  ustring note_text = gtk_text_buffer_get_text (note_buffer, &startiter, &enditer, true);

  // Get the language of the project.
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig (project);
  ustring language = projectconfig->language_get();
  
  // Extract references.
  ReferencesScanner refscanner (language, book, note_text);
  
  // Produce the text for the quick reference area.
  ustring quickreferences;
  for (unsigned int i = 0; i < refscanner.references.size(); i++) {
    quickreferences.append (refscanner.references[i].human_readable (language));
    quickreferences.append (" ");
    ustring text = project_retrieve_verse (project, refscanner.references[i].book, refscanner.references[i].chapter, refscanner.references[i].verse);
    if (text.empty()) {
      quickreferences.append ("<empty>");
    } else {
      CategorizeLine cl (text);
      quickreferences.append (cl.verse);
    }
    quickreferences.append ("\n");
  }

  // Set the text in the buffer.
  GtkTextBuffer * quick_ref_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (quick_references_textview));
  gtk_text_buffer_set_text (quick_ref_buffer, quickreferences.c_str(), -1);
}




void Editor::on_textview_move_cursor (GtkTextView * textview, GtkMovementStep step, gint count,
                                           gboolean extend_selection, gpointer user_data)
{
  ((Editor *) user_data)->on_textview_cursor_moved_delayer ();
}


void Editor::on_textview_cursor_moved_delayer ()
{
  // Clear the character style that was going to be applied when the user starts typing.
  character_style_on_start_typing.clear ();
  // Keep postponing the actual handler if a new cursor movement was detected
  // before the previous one was processed: destroy the GSource.
  gw_destroy_source (textview_cursor_moved_delayer_event_id);
  textview_cursor_moved_delayer_event_id = g_timeout_add_full (G_PRIORITY_DEFAULT, 100, GSourceFunc (on_textview_cursor_moved_delayer_handler), gpointer(this), NULL);
}


bool Editor::on_textview_cursor_moved_delayer_handler (gpointer user_data)
{
  ((Editor *) user_data)->on_textview_cursor_moved ();
  return false;
}


void Editor::on_textview_cursor_moved ()
// Handle the administration if the cursor moved.
{
  signal_if_verse_changed ();
  signal_if_styles_changed ();
}


ustring Editor::verse_number_get ()
// Returns the verse number the cursor is in.
{
  // Get an iterator at the cursor location of the main textview.
  // But if the cursor is in a footnote, or in a table, then the iterator
  // should be retrieved from another location in the main textview.
  // This location should correspond to the verse that the footnote refers to
  // or that the table is in.
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_mark (textbuffer, &iter, gtk_text_buffer_get_insert (textbuffer));
  if (GTK_WIDGET_HAS_FOCUS (textview)) {
    gtk_text_buffer_get_iter_at_mark (textbuffer, &iter, gtk_text_buffer_get_insert (textbuffer));
  } else {
    for (unsigned int i = 0; i < editornotes.size(); i++) {
      if (GTK_WIDGET_HAS_FOCUS (editornotes[i].textview)) {
        gtk_text_buffer_get_iter_at_child_anchor (textbuffer, &iter, editornotes[i].childanchor_caller_text);
      }
    }
    for (unsigned int i = 0; i < editortables.size (); i++) {
      for (unsigned int row = 0; row < editortables[i].textviews.size (); row++) {
        for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
          if (GTK_WIDGET_HAS_FOCUS (table_cell_get_view (editortables[i], row, column))) {
            gtk_text_buffer_get_iter_at_child_anchor (textbuffer, &iter, editortables[i].childanchor);
          }
        }
      }
    }
  }

  // Get and return the verse.
  ustring verse_marker = style_get_verse_marker (project);
  return get_verse_number_at_iterator (iter, verse_marker);
}



void Editor::on_textview_grab_focus (GtkWidget * widget, gpointer user_data)
{
  ((Editor *) user_data)->textview_grab_focus (widget);
}


void Editor::textview_grab_focus (GtkWidget * widget)
{
  // Focus handling.
  last_focused_widget = widget;
  // Bail out if the focus is grabbed by the program itself.
  if (focus_programmatically_being_grabbed)
    return;
  // Focus handling (continued).
  focus ();
  // Clear the character style that was going to be applied when the user starts typing.
  character_style_on_start_typing.clear ();
  // Further processing of the focus grab is done with a delay.
  gw_destroy_source (grab_focus_event_id);
  grab_focus_event_id = g_timeout_add_full (G_PRIORITY_DEFAULT, 10, GSourceFunc (on_grab_focus_delayer_timeout), gpointer(this), NULL);
}


bool Editor::on_grab_focus_delayer_timeout (gpointer data)
{
  ((Editor *) data)->on_grab_focus_delayed_handler ();
  return false;
}


void Editor::on_grab_focus_delayed_handler ()
/*
If the user clicks in the editor window, 
and straight after that the position of the cursor is requested, 
we get the position where the cursor was prior to clicking. 
This delayed handler solves that.
*/
{
  signal_if_verse_changed ();
  signal_if_styles_changed ();
  if (record_undo_actions ()) {
    show_quick_references ();
  }

  // If the user clicks on a note caller, then the focus will move to or from 
  // that note. This functionality is processed below.
  if (child_anchor_clicked) {
    // Go through the available notes and if the anchor agrees, 
    // focus the appropriate textview.
    for (unsigned int i = 0; i < editornotes.size (); i++) {
      if (child_anchor_clicked == editornotes[i].childanchor_caller_text) {
        gtk_widget_grab_focus (editornotes[i].textview);
        GtkTextIter iter;
        gtk_text_buffer_get_start_iter (editornotes[i].textbuffer, &iter);
        gtk_text_buffer_place_cursor (editornotes[i].textbuffer, &iter);
        gtk_text_buffer_get_end_iter (textbuffer, &iter);
        screen_scroll_to_iterator (GTK_TEXT_VIEW (textview), &iter);
      }
      if (child_anchor_clicked == editornotes[i].childanchor_caller_note) {
        gtk_widget_grab_focus (textview);
        GtkTextIter iter;
        gtk_text_buffer_get_iter_at_child_anchor (textbuffer, &iter, editornotes[i].childanchor_caller_text);
        gtk_text_buffer_place_cursor (textbuffer, &iter);
        screen_scroll_to_iterator (GTK_TEXT_VIEW (textview), &iter);
      }
    }
    child_anchor_clicked = NULL;
  }
}


void Editor::programmatically_grab_focus (GtkWidget * widget)
{
  focus_programmatically_being_grabbed = true;
  gtk_widget_grab_focus (widget);
  focus_programmatically_being_grabbed = false;
}


void Editor::signal_if_verse_changed ()
// If the verse number of the cursor changed it emits a signal.
{
  ustring versenumber = verse_number_get ();
  if (versenumber != previous_versenumber) {
    gtk_button_clicked (GTK_BUTTON (new_verse_signal));
    previous_versenumber = versenumber;
  }
}


void Editor::undo ()
{
  // Bail out if nothing to undo.
  if (editorundoes.empty ()) return;

  // No recording of undoable actions while this object is alive.
  PreventEditorUndo preventundo (&record_undo_level);
  
  // For debugging purposes.  
  list_undo_buffer ();
  
  // Get the Undo object and remove it from the container..
  EditorUndo editorundo = editorundoes[editorundoes.size () - 1];
  editorundoes.pop_back ();

  switch (editorundo.type) {
    case eudInsertText:
    {
      // The change was to insert text. To undo it, remove this text again.
      GtkTextIter startiter, enditer;
      gtk_text_buffer_get_iter_at_offset (textbuffer, &startiter, editorundo.startoffset);
      gtk_text_buffer_get_iter_at_offset (textbuffer, &enditer, editorundo.endoffset);
      gtk_text_buffer_delete (textbuffer, &startiter, &enditer);
      // Place the cursor.
      gtk_text_buffer_place_cursor (textbuffer, &startiter);
      screen_scroll_to_iterator (GTK_TEXT_VIEW (textview), &startiter);
      break;
    }
    case eudDeleteText:
    {
      // The change was to delete text. To undo it insert this text again.

      // Replace newlines with spaces in the text to be inserted.
      // This is done because the usfm formatting routines have been built 
      // to accept one such line, the original idea being to exclude any 
      // possibility that the editor does not properly load text that has 
      // line-breaks at unexpected places.
      replace_text (editorundo.text, "\n", " ");
      
      // Place the cursor at the insert position.
      GtkTextIter insert_iter;
      gtk_text_buffer_get_iter_at_offset (textbuffer, &insert_iter, editorundo.startoffset);
      gtk_text_buffer_place_cursor (textbuffer, &insert_iter);
      
      // Initialize paragraph and character styles to the ones prevalent at the insert location.
      ustring paragraph_mark;
      ustring character_mark;
      get_styles_at_iterator (insert_iter, paragraph_mark, character_mark);

      // If the text at the time it was deleted did not start a new line,
      // and the code that is now inserted again would start a new line,
      // remove that first usfm marker that causes the new line.
      if (!editorundo.flag) {
        ustring marker;
        size_t marker_pos;
        size_t marker_length;
        bool is_opener;
        bool marker_found;
        marker_found = usfm_search_marker (editorundo.text, marker, marker_pos, marker_length, is_opener);
        if (marker_found) {
          if (marker_pos == 0) {
            if (is_opener) {
              StyleType type;
              int subtype;
              marker_get_type_and_subtype (project, marker, type, subtype);
              if (style_get_starts_new_line_in_editor (type, subtype)) {
                editorundo.text.erase (0, marker_length);
              }
            }
          }
        }
      }

      // Insert the text in the view at the position of the text insertion point marker.
      ustring marker;
      size_t marker_pos;
      size_t marker_length;
      bool is_opener;
      bool marker_found;
      while (!editorundo.text.empty ()) {
        marker_found = usfm_search_marker (editorundo.text, marker, marker_pos, marker_length, is_opener);
        if (load_text_not_starting_with_marker (textbuffer, editorundo.text, paragraph_mark, character_mark, marker_pos, marker_length, marker_found));
        else if (load_text_table_raw (editorundo.text, paragraph_mark ,marker, marker_pos, marker_length, is_opener, marker_found));
        else if (load_text_starting_new_paragraph (textbuffer, editorundo.text, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found));
        else if (load_text_starting_character_style (textbuffer, editorundo.text, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found));
        else if (load_text_ending_character_style (textbuffer, editorundo.text, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found));
        else if (load_text_verse_number (editorundo.text, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found));
        else if (load_text_note_raw (editorundo.text, marker, marker_pos, marker_length, is_opener, marker_found));
        else load_text_with_unknown_markup (textbuffer, editorundo.text, paragraph_mark, character_mark);
      }
      
      // Finalize displaying any notes.
      display_notes_remainder (false);
      renumber_and_clean_notes_callers ();
      
      // Place the cursor after the restored text.
      GtkTextIter cursor_iter;
      gtk_text_buffer_get_iter_at_offset (textbuffer, &cursor_iter, editorundo.endoffset);
      
      // Done.
      break;
    }
    case eudApplyTag:
    {
      // The change was to apply a tag. To undo it, remove it again.
      GtkTextIter startiter, enditer;
      gtk_text_buffer_get_iter_at_offset (textbuffer, &startiter, editorundo.startoffset);
      gtk_text_buffer_get_iter_at_offset (textbuffer, &enditer, editorundo.endoffset);
      gtk_text_buffer_remove_tag_by_name (textbuffer, editorundo.text.c_str (), &startiter, &enditer);
      // Place the cursor.
      gtk_text_buffer_place_cursor (textbuffer, &startiter);
      screen_scroll_to_iterator (GTK_TEXT_VIEW (textview), &startiter);
      break;
    }
    case eudRemoveTag:
    {
      // The change was to remove a tag. To undo it, apply it again.
      GtkTextIter startiter, enditer;
      gtk_text_buffer_get_iter_at_offset (textbuffer, &startiter, editorundo.startoffset);
      gtk_text_buffer_get_iter_at_offset (textbuffer, &enditer, editorundo.endoffset);
      textbuffer_apply_named_tag (textbuffer, editorundo.text, &startiter, &enditer);
      // Place the cursor.
      gtk_text_buffer_place_cursor (textbuffer, &startiter);
      screen_scroll_to_iterator (GTK_TEXT_VIEW (textview), &startiter);
      break;
    }
  }
}


void Editor::redo ()
{
}


bool Editor::can_undo ()
{
  return !editorundoes.empty ();
}


bool Editor::can_redo ()
{
  return false;
}


void Editor::set_font ()
{
  // Get a list of all textviews that make up the editor.
  vector <GtkWidget *> textviews;
  textviews.push_back (textview);
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    if (editornotes[i].textview) {
      textviews.push_back (editornotes[i].textview);
    }
  }
  for (unsigned int i = 0; i < editortables.size (); i++) {
    for (unsigned int row = 0; row < editortables[i].textviews.size (); row++) {
      for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
        textviews.push_back (table_cell_get_view (editortables[i], row, column));
      }
    }
  }


  // Set font.
  PangoFontDescription *font_desc = NULL;
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig (project);
  if (!projectconfig->editor_font_default_get ()) {
    font_desc = pango_font_description_from_string (projectconfig->editor_font_name_get().c_str ());
  }
  for (unsigned int i = 0; i < textviews.size (); i++) {
    gtk_widget_modify_font (textviews[i], font_desc);
  }
  if (font_desc) pango_font_description_free (font_desc);
    
  // Set the colours.
  for (unsigned int i = 0; i < textviews.size (); i++) {
    if (projectconfig->editor_default_color_get ()) {
      color_widget_default (textviews[i]);
    } else {
      color_widget_set (textviews[i],
                        projectconfig->editor_normal_text_color_get (),
                        projectconfig->editor_background_color_get (),
                        projectconfig->editor_selected_text_color_get (),
                        projectconfig->editor_selection_color_get ());
    }
  }
  
  // Set predominant text direction.
  for (unsigned int i = 0; i < textviews.size (); i++) {
    if (projectconfig->right_to_left_get ()) {
      gtk_widget_set_direction (textviews[i], GTK_TEXT_DIR_RTL);
    } else {
      gtk_widget_set_direction (textviews[i], GTK_TEXT_DIR_LTR);
    }  
  }
}


void Editor::position_cursor_at_verse (const ustring& cursorposition, bool focus)
// This function starts the procedure to move the cursor of the editor to the 
// verse given.
{
  position_cursor_at_verse_cursorposition = cursorposition;
  position_cursor_at_verse_focus = focus;
  // Keep postponing the actual handler if a new positioning request was 
  // received before the previous one was processed: destroy the GSource.
  gw_destroy_source (position_cursor_at_verse_event_id);
  position_cursor_at_verse_event_id = g_timeout_add_full (G_PRIORITY_DEFAULT, 100, GSourceFunc (position_cursor_at_verse_postponer_handler), gpointer(this), NULL);
}


bool Editor::position_cursor_at_verse_postponer_handler (gpointer user_data)
{
  ((Editor *) user_data)->position_cursor_at_verse_executer ();
  return false;
}


void Editor::position_cursor_at_verse_executer ()
{
  // Find out whether we need to reposition the cursor. We will not move the 
  // cursor or scroll to it when the cursor is already on the right verse.
  bool reposition = position_cursor_at_verse_cursorposition != verse_number_get ();

  // When the cursor is at the end of the buffer, it usually means that the buffer has just been filled.
  // So the cursor does need to be repositioned in this case.
  GtkTextIter cursor_iterator;
  gtk_text_buffer_get_iter_at_mark (textbuffer, &cursor_iterator, gtk_text_buffer_get_insert (textbuffer));
  GtkTextIter enditer;
  gtk_text_buffer_get_end_iter (textbuffer, &enditer);
  if (gtk_text_iter_compare (&cursor_iterator, &enditer) == 0) {
    reposition = true;
  }

  // Do the repositioning if needed.
  if (reposition) {
    // Grab focus here to get the scrolling done properly, and the user can type 
    // in the editor. But this is only done if requested.
    if (position_cursor_at_verse_focus) {
      programmatically_grab_focus (textview);
    }
    if ((position_cursor_at_verse_cursorposition == "0") || (position_cursor_at_verse_cursorposition.empty())) {
      // Verse 0 or empty: beginning of file.
      GtkTextIter iter;
      gtk_text_buffer_get_start_iter (textbuffer, &iter);
      gtk_text_buffer_place_cursor (textbuffer, &iter);
      screen_scroll_to_iterator (GTK_TEXT_VIEW (textview), &iter);
      while (gtk_events_pending ()) gtk_main_iteration ();
    } else {
      // The verse marker.
      ustring verse_marker = style_get_verse_marker (project);
      // Go through the buffer and find out about the verse.
      GtkTextIter iter;
      gtk_text_buffer_get_start_iter (textbuffer, &iter);
      do {
        set <ustring> styles = styles_at_iterator (iter);
        if (styles.find (verse_marker) != styles.end ()) {
          GtkTextIter enditer = iter;
          gtk_text_iter_forward_chars (&enditer, 10);
          ustring verse = gtk_text_iter_get_slice (&iter, &enditer);        
          size_t position = verse.find (" ");
          position = CLAMP (position, 0, verse.length());
          verse = verse.substr (0, position);
          // Position the cursor at the requested verse, if the verse is there.
          // Also look whether the verse is in a sequence or range of verses.
          bool position_here = (verse == position_cursor_at_verse_cursorposition);
          unsigned int verse_int = convert_to_int (position_cursor_at_verse_cursorposition);
          vector <unsigned int> combined_verses = verse_range_sequence (verse);
          for (unsigned int i2 = 0; i2 < combined_verses.size(); i2++) {
            if (verse_int == combined_verses[i2]) {
              position_here = true;
              position_cursor_at_verse_cursorposition = verse;
            }        
          }
          if (position_here) {
            // Move the cursor to it.
            // Move it to the beginning of the text, if there is any.
            gtk_text_iter_forward_chars (&iter, verse.length () + 1);
            gtk_text_buffer_place_cursor (textbuffer, &iter);
            while (gtk_events_pending ()) gtk_main_iteration ();
            // Scroll also to it. It will scroll to the beginning of the text after the verse marker.
            // Alignment is needed to put the line being edited near the top of the window.
            screen_scroll_to_iterator (GTK_TEXT_VIEW (textview), &iter);
            while (gtk_events_pending ()) gtk_main_iteration ();
            // Bail out.
            break;
          }
        }       
      } while (gtk_text_iter_forward_char (&iter));
    }

    // Highlight search words.
    highlight_searchwords ();
  }
}


bool Editor::on_save_timeout (gpointer data)
{
  return ((Editor *) data)->save_timeout ();
}


bool Editor::save_timeout ()
{
  chapter_save ();
  return true;
}


gboolean Editor::on_text_motion_notify_event (GtkWidget *textview, GdkEventMotion *event, gpointer user_data)
{
  return ((Editor *) user_data)->text_motion_notify_event (textview, event);
}

  
gboolean Editor::text_motion_notify_event (GtkWidget *textview, GdkEventMotion *event)
// Update the cursor image if the pointer moved. 
{
  gint x, y;
  gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (textview), GTK_TEXT_WINDOW_WIDGET,
                                         gint(event->x), gint(event->y), &x, &y);
  GtkTextIter iter;
  gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (textview), &iter, x, y);
  GtkTextChildAnchor * anchor = gtk_text_iter_get_child_anchor (&iter);
  bool hand_cursor = false;
  if (anchor) {
    for (unsigned int i = 0; i < editornotes.size (); i++) {
      if (anchor == editornotes[i].childanchor_caller_text)
        hand_cursor = true;
      if (anchor == editornotes[i].childanchor_caller_note)
        hand_cursor = true;
    }
  }
  if (hand_cursor != previous_hand_cursor) {
    GdkCursor *cursor;
    if (hand_cursor) {
      cursor = gdk_cursor_new (GDK_HAND2);
    } else {
      cursor = gdk_cursor_new (GDK_XTERM);
    }
    gdk_window_set_cursor (gtk_text_view_get_window (GTK_TEXT_VIEW (textview), GTK_TEXT_WINDOW_TEXT), cursor);
  }
  previous_hand_cursor = hand_cursor;
  gdk_window_get_pointer (textview->window, NULL, NULL, NULL);
  return false;
}


void Editor::on_text_event_after (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
  ((Editor *) user_data)->text_event_after (widget, event);  
}


void Editor::text_event_after (GtkWidget *textview, GdkEvent *ev)
/* Links can also be activated by clicking.
 */
{
  GtkTextIter start, end, iter;
  GtkTextBuffer *buffer;
  GdkEventButton *event;
  gint x, y;
  if (ev->type != GDK_BUTTON_RELEASE)
    return;
  event = (GdkEventButton *)ev;
  if (event->button != 1)
    return;
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
  // We shouldn't follow a link if the user has selected something.
  gtk_text_buffer_get_selection_bounds (buffer, &start, &end);
  if (gtk_text_iter_get_offset (&start) != gtk_text_iter_get_offset (&end))
    return;
  gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (textview), GTK_TEXT_WINDOW_WIDGET,
                                         gint(event->x), gint(event->y), &x, &y);
  gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (textview), &iter, x, y);
  text_edit_if_link (textview, &iter);
}


void Editor::text_edit_if_link (GtkWidget *textview, GtkTextIter *iter)
/* Looks at all tags covering the position of iter in the text view, 
 * and if one of them is a link, follow it by showing the page identified
 * by the data attached to it.
 */
{
  GSList *tags = NULL, *tagp = NULL;
  tags = gtk_text_iter_get_tags (iter);
  for (tagp = tags;  tagp != NULL;  tagp = tagp->next) {
    GtkTextTag *tag = (GtkTextTag *) tagp->data;
    gint id = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (tag), "id"));
    if (id != 0) {
    }
  }
  if (tags) 
    g_slist_free (tags);
}


gboolean Editor::text_key_press_event_before (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
  return ((Editor *) user_data)->on_text_key_press_event_before (widget, event);
}


gboolean Editor::on_text_key_press_event_before (GtkWidget *widget, GdkEventKey *event)
// Preprocessing of the keyboard events in the text editors.
{
  if (keyboard_enter_pressed (event)) {
    // No <Enter> in a crossreference.
    if (last_focused_type () == etvtNote) {
      GtkWidget * textview = last_focused_textview ();
      for (unsigned int i = 0; i < editornotes.size (); i++) {
        if (textview == editornotes[i].textview) {
          EditorNoteType notetype = note_type_get (project, editornotes[i].marker);
          if (notetype == entCrossreference) {
            return true;
          } else {
            // If <Enter> has been given in a foot/end note, 
            // apply the paragraph continuation style.
            // This is done with a delay to allow the keypress to be fully processed.
            // It was thought at first to use the keypress handler after the
            // event, but it appears that the event is handled and no after event
            // is called anymore.
            style_to_be_applied_at_cursor = style_get_paragraph_note_style (project);
            g_timeout_add (1, GSourceFunc (on_apply_style_at_cursor), gpointer (this));
          }
        }
      }
    }
    // No <Enter> in a table cell.
    if (last_focused_type () == etvtTable) {
      return true;
    }
  }

  // Pressing Page Up while the cursor is in the footnote brings the user
  // to the note caller in the text.
  if (keyboard_page_up_pressed (event)) {
    if (last_focused_type () == etvtNote) {
      GtkTextBuffer * buffer = last_focused_textbuffer ();
      for (unsigned int i = 0; i < editornotes.size (); i++) {
        if (buffer == editornotes[i].textbuffer) {
          programmatically_grab_focus (textview);
          GtkTextIter iter;
          gtk_text_buffer_get_iter_at_child_anchor (textbuffer, &iter, editornotes[i].childanchor_caller_text);
          gtk_text_buffer_place_cursor (textbuffer, &iter);
          screen_scroll_to_iterator (GTK_TEXT_VIEW (textview), &iter);
          break;
        }
      }
    }    
  }
  
  return false;
}


gboolean Editor::text_key_press_event_after (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
  return ((Editor *) user_data)->on_text_key_press_event_after (widget, event);
}


gboolean Editor::on_text_key_press_event_after (GtkWidget *widget, GdkEventKey *event)
// Postprocessing of the keyboard events in the text editors.
{
  return false;
}


void Editor::on_textbuffer_footnotes_changed (GtkEditable * editable, gpointer user_data)
{
  ((Editor *) user_data)->on_textbuffer_footnotes ();
}


void Editor::on_textbuffer_footnotes ()
{
  if (record_undo_actions ()) {
    show_quick_references ();
  }
}


void Editor::quick_references_textview_set (GtkWidget * widget)
{
  quick_references_textview = widget;
}


gboolean Editor::on_textview_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  ((Editor *) user_data)->on_texteditor_click (widget, event);
  return false;
}


void Editor::on_texteditor_click (GtkWidget * widget, GdkEventButton *event)
{
  // Double-clicking sends the word to Toolbox.
  if (event->type == GDK_2BUTTON_PRESS) {
    
    // Get the textbuffer.
    GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));

    // Get the word.
    GtkTextIter enditer;
    GtkTextIter startiter;
    gtk_text_buffer_get_iter_at_mark (textbuffer, &enditer, gtk_text_buffer_get_insert (textbuffer));
    if (!gtk_text_iter_ends_word (&enditer))
      gtk_text_iter_forward_word_end (&enditer);
    startiter = enditer;
    gtk_text_iter_backward_word_start (&startiter);
    word_double_clicked_text = gtk_text_buffer_get_text (textbuffer, &startiter, &enditer, false);
    
    // Signal to have it sent to Toolbox.
    gtk_button_clicked (GTK_BUTTON (word_double_clicked_signal));
  }
  
  // See whether the user clicked on a note caller.
  child_anchor_clicked = NULL;
  if (widget == textview) {
    if (event->type == GDK_BUTTON_PRESS) {
      // Get iterator at clicking location.
      GtkTextIter iter;
      gint x, y;
      gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (widget), GTK_TEXT_WINDOW_WIDGET, gint(event->x), gint(event->y), &x, &y);
      gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (widget), &iter, x, y);
      // Get the child anchor at the iterator, if there is one.
      child_anchor_clicked = gtk_text_iter_get_child_anchor (&iter);
      // Further processing of this child anchor is done in the focus grabbed handler.
    }
  }
}


bool Editor::on_widget_creation_timeout (gpointer data)
{
  ((Editor *) data)->on_widget_creation ();
  return false;
}


void Editor::on_widget_creation ()
{
  // Scroll to the locaton of the cursor.
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_mark (textbuffer, &iter, gtk_text_buffer_get_insert (textbuffer));
  screen_scroll_to_iterator (GTK_TEXT_VIEW (textview), &iter);
}


void Editor::create_or_update_formatting_data ()
// Create and fill the text tag table for all the formatted views.
// If already there, update it.
{
  // If there is no text tag table, create a new one.
  if (!texttagtable) texttagtable = gtk_text_tag_table_new ();
  
  // Get the stylesheet.
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig (project);
  ustring stylesheet = projectconfig->stylesheet_get ();

  // Get the font size multiplication factor.
  double font_size_multiplier = 1;
  PangoFontDescription *font_desc = NULL;
  if (!projectconfig->editor_font_default_get ()) {
    font_desc = pango_font_description_from_string (projectconfig->editor_font_name_get().c_str ());
    gint fontsize = pango_font_description_get_size (font_desc) / PANGO_SCALE;
    font_size_multiplier = (double) fontsize / 12;
    pango_font_description_free (font_desc);
  }
  
  // Create the unknown style.
  {
    Style style ("", unknown_style (), false);
    create_or_update_text_style (&style, false, true, font_size_multiplier);
  }
  
  // Get all the Styles.
  extern Styles * styles;
  Usfm * usfm = styles->usfm (stylesheet);
  
  // Handle the known styles.
  // The priorities of the character styles should be higher than those of the 
  // paragraph styles. 
  // Therefore paragraph styles are created first, then the other ones.
  for (unsigned int i = 0; i < usfm->styles.size(); i++) {
    // Get the properties of this style.
    bool paragraphstyle = style_get_paragraph (usfm->styles[i].type, usfm->styles[i].subtype);
    bool plaintext = style_get_plaintext (usfm->styles[i].type, usfm->styles[i].subtype);
    // Create a text style, only paragraph styles.
    if (paragraphstyle)
      create_or_update_text_style (&(usfm->styles[i]), true, plaintext, font_size_multiplier);
  }
  for (unsigned int i = 0; i < usfm->styles.size(); i++) {
    // Get the properties of this style.
    bool paragraphstyle = style_get_paragraph (usfm->styles[i].type, usfm->styles[i].subtype);
    bool plaintext = style_get_plaintext (usfm->styles[i].type, usfm->styles[i].subtype);
    // Create a text style, the non-paragraph styles.
    if (!paragraphstyle)
      create_or_update_text_style (&(usfm->styles[i]), false, plaintext, font_size_multiplier);
  }
  
  // Create the styles for insertions and deletions.
  {
    Style style_ins ("", INSERTION_MARKER, false);
    create_or_update_text_style (&style_ins, false, false, font_size_multiplier);
    Style style_del ("", DELETION_MARKER, false);
    create_or_update_text_style (&style_del, false, false, font_size_multiplier);
  }
}


void Editor::create_or_update_text_style (Style * style, bool paragraph, bool plaintext, double font_multiplier)
/*
This creates or updates a GtkTextTag with the data stored in "style".
The fontsize of thi style is calculated by the value as stored in "style", and 
multiplied by "font_multiplier".
*/
{
  // Take the existing tag, or create a new one and add it to the tagtable.
  GtkTextTag *tag = gtk_text_tag_table_lookup (texttagtable, style->marker.c_str());
  if (!tag) {
    tag = gtk_text_tag_new (style->marker.c_str());
    gtk_text_tag_table_add (texttagtable, tag);
    g_object_unref (tag);
  }

  // Optionally handle plain-text style and return.
  if (plaintext) {
    GValue gvalue = {0, };
    g_value_init (&gvalue, G_TYPE_STRING);
    int fontsize = (int) (12 * font_multiplier);
    ustring font = "Courier " + convert_to_string (fontsize);
    g_value_set_string (&gvalue, font.c_str());
    g_object_set_property (G_OBJECT (tag), "font", &gvalue);
    g_value_unset (&gvalue);    
    return;
  }

  // Fontsize or fontpercentage.
  if (paragraph)
  {
    GValue gvalue = {0, };
    g_value_init (&gvalue, G_TYPE_DOUBLE);
    double fontsize = style->fontsize * font_multiplier;
    g_value_set_double (&gvalue, fontsize);
    g_object_set_property (G_OBJECT (tag), "size-points", &gvalue);
    g_value_unset (&gvalue);    
  } else {
    double percentage = (double) style->fontpercentage / 100 * font_multiplier;
    GValue gvalue = {0, };
    g_value_init (&gvalue, G_TYPE_DOUBLE);
    g_value_set_double (&gvalue, percentage);
    g_object_set_property (G_OBJECT (tag), "scale", &gvalue);
    g_value_unset (&gvalue);
  }

  // Italic, bold, underline, smallcaps can be ON, OFF, INHERIT, or TOGGLE.
  // Right now, INHERIT is taken as OFF, and TOGGLE is interpreted as ON.
  // Improvements might be needed.
  {
    PangoStyle pangostyle = PANGO_STYLE_NORMAL;
    if ((style->italic == ON) || (style->italic == TOGGLE))
      pangostyle = PANGO_STYLE_ITALIC;
    GValue gvalue = {0, };
    g_value_init (&gvalue, PANGO_TYPE_STYLE);
    g_value_set_enum (&gvalue, pangostyle);
    g_object_set_property (G_OBJECT (tag), "style", &gvalue);
    g_value_unset (&gvalue);    
  }
  {
    PangoWeight pangoweight = PANGO_WEIGHT_NORMAL;
    if ((style->bold == ON) || (style->bold == TOGGLE)) 
      pangoweight = PANGO_WEIGHT_BOLD;  
    GValue gvalue = {0, };
    g_value_init (&gvalue, PANGO_TYPE_WEIGHT);
    g_value_set_enum (&gvalue, pangoweight);
    g_object_set_property (G_OBJECT (tag), "weight", &gvalue);
    g_value_unset (&gvalue);    
  }
  {
    PangoUnderline pangounderline = PANGO_UNDERLINE_NONE;
    if ((style->underline == ON) || (style->underline == TOGGLE)) 
      pangounderline = PANGO_UNDERLINE_SINGLE;
    GValue gvalue = {0, };
    g_value_init (&gvalue, PANGO_TYPE_UNDERLINE);
    g_value_set_enum (&gvalue, pangounderline);
    g_object_set_property (G_OBJECT (tag), "underline", &gvalue);
    g_value_unset (&gvalue);    
  }
  {
    PangoVariant pangovariant = PANGO_VARIANT_NORMAL;
    if ((style->smallcaps == ON) || (style->smallcaps == TOGGLE))
      pangovariant = PANGO_VARIANT_NORMAL;
    GValue gvalue = {0, };
    g_value_init (&gvalue, PANGO_TYPE_VARIANT);
    g_value_set_enum (&gvalue, pangovariant);
    g_object_set_property (G_OBJECT (tag), "variant", &gvalue);
    g_value_unset (&gvalue);    
  }

  // Superscript.
  if (style->superscript) {
    // Rise n pixels.
    gint rise = 6 * PANGO_SCALE;
    GValue gvalue = {0, };
    g_value_init (&gvalue, G_TYPE_INT);
    g_value_set_int (&gvalue, rise);
    g_object_set_property (G_OBJECT (tag), "rise", &gvalue);
    g_value_unset (&gvalue);
  }
  
  // Styles that only occur in paragraphs, not in character styles.  
  if (paragraph) {
     
    GtkJustification gtkjustification;  
    if (style->justification == CENTER) {
      gtkjustification = GTK_JUSTIFY_CENTER;
    } else if (style->justification == RIGHT) {
      gtkjustification = GTK_JUSTIFY_RIGHT;
    } else if (style->justification == JUSTIFIED) {
      // Gtk+ does not yet support this way of justification. 
      // To avoid a lot of lines in our system log, saying this is not yet supported,
      // it has been disabled here till such time that this is supported.
      // Works as from version Gtk+ 2.12.
      // todo implement when generally this Gtk version has been deployed.
      // gtkjustification = GTK_JUSTIFY_FILL;
      gtkjustification = GTK_JUSTIFY_LEFT;
    } else { // Default is LEFT.
      gtkjustification = GTK_JUSTIFY_LEFT;
    }
    {
      GValue gvalue = {0, };
      g_value_init (&gvalue, GTK_TYPE_JUSTIFICATION);
      g_value_set_enum (&gvalue, gtkjustification);
      g_object_set_property (G_OBJECT (tag), "justification", &gvalue);
      g_value_unset (&gvalue);    
    }
    
    // For property "pixels-above/below-...", only values >= 0 are valid.
    if (style->spacebefore > 0) {
      gint spacebefore = (gint) (4 * style->spacebefore);
      GValue gvalue = {0, };
      g_value_init (&gvalue, G_TYPE_INT);
      g_value_set_int (&gvalue, spacebefore);
      g_object_set_property (G_OBJECT (tag), "pixels-above-lines", &gvalue);
      g_value_unset (&gvalue);    
    }
    
    if (style->spaceafter > 0) {
      gint spaceafter = (gint) (4 * style->spaceafter);
      GValue gvalue = {0, };
      g_value_init (&gvalue, G_TYPE_INT);
      g_value_set_int (&gvalue, spaceafter);
      g_object_set_property (G_OBJECT (tag), "pixels-below-lines", &gvalue);
      g_value_unset (&gvalue);    
    }

    {
      gint leftmargin = (gint) (4 * style->leftmargin);
      // A little left margin is desired to make selecting words easier.
      leftmargin += 5; 
      GValue gvalue = {0, };
      g_value_init (&gvalue, G_TYPE_INT);
      g_value_set_int (&gvalue, leftmargin);
      g_object_set_property (G_OBJECT (tag), "left-margin", &gvalue);
      g_value_unset (&gvalue);    
    }

    if (style->rightmargin > 0) {
      gint rightmargin = (gint) (4 * style->rightmargin);
      GValue gvalue = {0, };
      g_value_init (&gvalue, G_TYPE_INT);
      g_value_set_int (&gvalue, rightmargin);
      g_object_set_property (G_OBJECT (tag), "right-margin", &gvalue);
      g_value_unset (&gvalue);    
    }

    {
      gint firstlineindent = (gint) (4 * style->firstlineindent);
      GValue gvalue = {0, };
      g_value_init (&gvalue, G_TYPE_INT);
      g_value_set_int (&gvalue, firstlineindent);
      g_object_set_property (G_OBJECT (tag), "indent", &gvalue);
      g_value_unset (&gvalue);    
    }

  }

  //if (style->color) {
  {
    GdkColor color;
    color_decimal_to_gdk (style->color, &color);
    GValue gvalue = {0, };
    g_value_init (&gvalue, GDK_TYPE_COLOR);
    g_value_set_boxed (&gvalue, &color);
    g_object_set_property (G_OBJECT (tag), "foreground-gdk", &gvalue);
    g_value_unset (&gvalue);    
  }
}


bool Editor::load_text_not_starting_with_marker (GtkTextBuffer * textbuffer, ustring& line, ustring& paragraph_mark, ustring& character_mark, size_t marker_pos, size_t marker_length, bool marker_found)
/*
This function loads text that does not start with a marker.
It then erases that bit of text from the input.
In order to get things right, it loads the text with the prevailing
paragraph and character markup.
*/
{
  // Proceed if a marker was found not at the start of a line.
  if (marker_found) {
    if (marker_pos != string::npos) {
      if (marker_pos > 0) {
        ustring text (line.substr (0, marker_pos));
        line.erase (0, marker_pos);
        text_append (textbuffer, text, paragraph_mark, character_mark);
        return true;
      }
    }
  }
  return false;
}


bool Editor::load_text_starting_new_paragraph (GtkTextBuffer * textbuffer, ustring& line, ustring& paragraph_mark, ustring& character_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
/*
This function deals with a marker that starts a paragraph.
It does the administration that starting the paragraph requires.
*/
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype (project, marker, type, subtype);
        if (style_get_starts_new_line_in_editor (type, subtype)) {
          // Because the ends of lines are changed to spaces, and these spaces
          // get inserted in the editor, if a new line starts, we need to trim
          // away the extra space at the end. This is done here.
          textbuffer_erase_character_before_text_insertion_point_if_space (textbuffer);
          // Insert a new line with the still prevailing markup, if the buffer is empty.
          if (!textbuffer_empty (textbuffer))
            text_append (textbuffer, "\n", paragraph_mark, character_mark);
          // Set the new paragraph and character markup.
          paragraph_mark = marker;
          character_mark.clear ();
          // Some styles insert their marker: Do that here if appropriate.
          if (style_get_displays_marker (type, subtype)) {
            text_append (textbuffer, line.substr (0, marker_length), paragraph_mark, "");
          }
          // Remove the markup from the line.
          line.erase (0, marker_length);
          // The information was processed: return true.
          return true;
        }
      }
    }
  }
  return false;
}


bool Editor::load_text_starting_character_style (GtkTextBuffer * textbuffer, ustring& line, ustring& paragraph_mark, ustring& character_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
/*
This function deals with a marker that starts a character style.
It does the administration that starting this style requires.
*/
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype (project, marker, type, subtype);
        if (style_get_starts_character_style (type, subtype)) {
          // Set the new character markup (the paragraph markup remains the same).
          character_mark = marker;
          // Some styles insert their marker: Do that here if appropriate.
          if (style_get_displays_marker (type, subtype)) {
            text_append (textbuffer, line.substr (0, marker_length), paragraph_mark, character_mark);
          }
          // Remove the markup from the line.
          line.erase (0, marker_length);
          // The information was processed: return true.
          return true;
        }
      }
    }
  }
  return false;
}


bool Editor::load_text_ending_character_style (GtkTextBuffer * textbuffer, ustring& line, ustring& paragraph_mark, ustring& character_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
/*
This function deals with a marker that ends a character style.
It does the administration that ending this style requires.
*/
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (!is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype (project, marker, type, subtype);
        if (style_get_starts_character_style (type, subtype)) {
          // Clear the character markup (the paragraph markup remains as it is).
          character_mark.clear ();
          // Some styles insert their marker: Do that here if appropriate.
          if (style_get_displays_marker (type, subtype)) {
            text_append (textbuffer, line.substr (0, marker_length), paragraph_mark, character_mark);
          }
          // Remove the markup from the line.
          line.erase (0, marker_length);
          // The information was processed: return true.
          return true;
        }
      }
    }
  }
  return false;
}


bool Editor::load_text_verse_number (ustring& line, ustring& paragraph_mark, ustring& character_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
/*
This function returns true if a verse number was loaded in the formatted view.
Else it returns false.
Verses are special in the sense that their style only applies to the verse
number, and not to the verse text that follows.
*/
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype (project, marker, type, subtype);
        if (style_get_starts_verse_number (type, subtype)) {
          // Clear the character markup (the paragraph markup remains as it is).
          character_mark.clear ();
          // Some styles insert their marker: Do that here if appropriate.
          if (style_get_displays_marker (type, subtype)) {
            text_append (textbuffer, line.substr (0, marker_length), paragraph_mark, "");
          }
          // Remove the markup from the line.
          line.erase (0, marker_length);
          // Get verse number. Handle combined verses too, e.g. 10-12b, etc.
          size_t position = line.find (" ");
          if (position == string::npos)
            position = line.length ();
          ustring versenumber = line.substr (0, position);
          // Display the verse and erase it from the input buffer.
          text_append (textbuffer, versenumber, paragraph_mark, marker);
          line.erase (0, position);
          // The information was processed: return true.
          return true;
        }
      }
    }
  }
  return false;
}


bool Editor::load_text_note_raw (ustring& line, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
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
        marker_get_type_and_subtype (project, marker, type, subtype);
        if (style_get_starts_footnote (type, subtype) || style_get_starts_endnote (type, subtype) || style_get_starts_crossreference (type, subtype)) {
          // Proceed if the endmarker is in the text too.
          ustring endmarker = usfm_get_full_closing_marker (marker);
          size_t endmarkerpos = line.find (endmarker);
          if (endmarkerpos != string::npos) {
            // Get raw note text and erase it from the input buffer.
            ustring rawnote (line.substr (marker_length, endmarkerpos - endmarker.length ()));
            line.erase (0, endmarkerpos + endmarker.length ());
            // Get the position where to insert the note caller in the text.
            GtkTextIter iter;
            gtk_text_buffer_get_iter_at_mark (textbuffer, &iter, gtk_text_buffer_get_insert (textbuffer));
            // Insert the note.
            insert_note (marker, rawnote, &iter, false);
            // The information was processed: return true.
            return true;
          }
        }
      }
    }
  }
  return false;
}


bool Editor::load_text_table_raw (ustring& line, const ustring& paragraph_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
// This function loads the raw text of a table.
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype (project, marker, type, subtype);
        // Proceed if this bit starts with any table-related marker.
        if (style_get_starts_table_row (type, subtype) || style_get_starts_table_cell (type, subtype)) {
          // Get the marker that ends the table bits.
          ustring endmarker;
          vector <ustring> markers = usfm_get_all_markers (line);
          for (unsigned int i = 0; i < markers.size(); i++) {
            StyleType type2;
            int subtype2;
            marker_get_type_and_subtype (project, markers[i], type2, subtype2);
            if (type2 != stTableElement) {
              endmarker = markers[i];
              break;
            }
          }
          // Proceed with the text till the next non-table marker or the end of the line.
          ustring rawtable;
          if (endmarker.empty ()) {
            rawtable = line;
            line.clear ();
          } else {
            size_t pos1, pos2;
            pos1 = line.find (usfm_get_full_opening_marker (endmarker));
            pos2 = line.find (usfm_get_full_closing_marker (endmarker));
            size_t pos = MIN (pos1, pos2);
            rawtable = line.substr (0, pos);
            line.erase (0, pos);
          }
          // Display the table
          GtkTextIter iter;
          gtk_text_buffer_get_iter_at_mark (textbuffer, &iter, gtk_text_buffer_get_insert (textbuffer));
          display_table (rawtable, iter);
          // The information was processed: return true.
          return true;
        }
      }
    }
  }
  return false;
}


bool Editor::load_text_table_starting_row (ustring& line, EditorTable& editortable, GtkTextBuffer *& textbuffer, bool& row_zero_initialized, gint& row, gint& column, ustring& paragraph_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
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
        marker_get_type_and_subtype (project, marker, type, subtype);
        if (style_get_starts_table_row (type, subtype)) {
          // For robustness the first row is always opened. In order that no
          // extra row is opened, the "..initiallized" flag is used.
          if (row_zero_initialized) {
            row++;
            column = 0;
            textbuffer = table_cell_get_buffer (editortable, row, column);
          } else {
            row_zero_initialized = true;
          }
          // Initialize the paragraph mark.
          paragraph_mark = unknown_style ();
          if (style_get_displays_marker (type, subtype)) {
            text_append (textbuffer, line.substr (0, marker_length), paragraph_mark, "");
          }
          line.erase (0, marker_length);
          return true;
        }
      }
    }
  }
  return false;
}


bool Editor::load_text_table_starting_cell (ustring& line, EditorTable& editortable, GtkTextBuffer *& textbuffer, bool& row_zero_initialized, gint& row, gint& column, ustring& paragraph_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
// This function does the administration for the markers that start a cell.
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype (project, marker, type, subtype);
        if (style_get_starts_table_cell (type, subtype)) {
          // Get the column number of this marker.
          extern Settings * settings;
          ustring stylesheet = settings->projectconfig (project)->stylesheet_get ();
          extern Styles * styles;
          Usfm * usfm = styles->usfm (stylesheet);
          for (unsigned int i = 0; i < usfm->styles.size (); i++) {
            if (marker == usfm->styles[i].marker) {
              column = usfm->styles[i].userint1;
              // The column number is given such that column 1 is the first column,
              // but internally we work with column 0 as the first.
              column--;
            }
          }
          textbuffer = table_cell_get_buffer (editortable, row, column);
          paragraph_mark = marker;
          if (style_get_displays_marker (type, subtype)) {
            text_append (textbuffer, line.substr (0, marker_length), paragraph_mark, "");
          }
          line.erase (0, marker_length);
          return true;
        }
      }
    }
  }
  return false;
}


bool Editor::load_text_starting_footnote_content (GtkTextBuffer * textbuffer, ustring& line, ustring& paragraph_mark, ustring& character_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
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
        marker_get_type_and_subtype (project, marker, type, subtype);
        if (style_get_starts_note_content (type, subtype)) {
          // Set the new character markup (the paragraph markup remains the same).
          character_mark = marker;
          // Some styles insert their marker: Do that here if appropriate.
          if (style_get_displays_marker (type, subtype)) {
            text_append (textbuffer, line.substr (0, marker_length), paragraph_mark, character_mark);
          }
          // Remove the markup from the line.
          line.erase (0, marker_length);
          // The information was processed: return true.
          return true;
        }
      }
    }
  }
  return false;
}


bool Editor::load_text_ending_footnote_content (GtkTextBuffer * textbuffer, ustring& line, ustring& paragraph_mark, ustring& character_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
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
        marker_get_type_and_subtype (project, marker, type, subtype);
        if (style_get_starts_note_content (type, subtype)) {
          // Some styles insert their marker: Do that here if appropriate.
          if (style_get_displays_marker (type, subtype)) {
            text_append (textbuffer, line.substr (0, marker_length), paragraph_mark, character_mark);
          }
          // Clear the existing character markup (the paragraph markup remains the same).
          character_mark.clear ();
          // Remove the markup from the line.
          line.erase (0, marker_length);
          // The information was processed: return true.
          return true;
        }
      }
    }
  }
  return false;
}


void Editor::load_text_with_unknown_markup (GtkTextBuffer * textbuffer, ustring& line, ustring& paragraph_mark, ustring& character_mark)
/*
This function loads text with unknown markup in the formatted view.
It is a fallback function that gets called when everything else has failed.
Basically it just takes one character from the available text, 
and loads it into the formatted view with the prevailing markup.
By taking only one character at a time it is hoped that one of the next times,
the lines gives enough markup information again to be handled properly.
*/
{
  ustring one_character (line.substr (0, 1));  
  line.erase (0, 1);
  text_append (textbuffer, one_character, paragraph_mark, character_mark);
}


void Editor::erase_related_note_bits ()
{
  // Collect data for note bits and the editor objects to be removed.
  vector <GtkTextChildAnchor *> related_anchors;
  set <int> objects_to_remove;
  for (unsigned int i = 0; i < editornotes.size (); i++) {
    if (text_child_anchors_being_deleted.find (editornotes[i].childanchor_caller_text) != text_child_anchors_being_deleted.end ()) {
      related_anchors.push_back (editornotes[i].childanchor_caller_note);
      related_anchors.push_back (editornotes[i].childanchor_textview);
      objects_to_remove.insert (i);
    }
    if (text_child_anchors_being_deleted.find (editornotes[i].childanchor_caller_note) != text_child_anchors_being_deleted.end ()) {
      related_anchors.push_back (editornotes[i].childanchor_caller_text);
      related_anchors.push_back (editornotes[i].childanchor_textview);
      objects_to_remove.insert (i);
    }
    if (text_child_anchors_being_deleted.find (editornotes[i].childanchor_textview) != text_child_anchors_being_deleted.end ()) {
      related_anchors.push_back (editornotes[i].childanchor_caller_text);
      related_anchors.push_back (editornotes[i].childanchor_caller_note);
      objects_to_remove.insert (i);
    }
  }
  {
    // Remove double anchors and anchors already deleted.
    set <GtkTextChildAnchor *> anchors;
    for (unsigned int i = 0; i < related_anchors.size (); i++) {
      if (text_child_anchors_being_deleted.find (related_anchors[i]) == text_child_anchors_being_deleted.end ())
        anchors.insert (related_anchors[i]);
    }
    related_anchors.clear ();
    related_anchors.assign (anchors.begin (), anchors.end ());
  }
  
  // Remove the widgets.
  for (unsigned int i = 0; i < related_anchors.size (); i++) {
    do_not_process_child_anchors_being_deleted = true;
    textbuffer_erase_child_anchor (textbuffer, related_anchors[i]);
    do_not_process_child_anchors_being_deleted = false;    
  }

  // If nothing was deleted, bail out.
  if (objects_to_remove.empty ()) return;

  // Remove the objects.
  vector <EditorNote>::iterator iter (editornotes.end ());
  for (int i = editornotes.size () - 1; i >= 0; i--) {
    iter--;
    if (objects_to_remove.find (i) != objects_to_remove.end ()) {
      editornotes.erase (iter);
    }
  }

  // If notes were deleted, renumber possible remaining notes.
  renumber_and_clean_notes_callers ();
}


void Editor::display_notes_remainder (bool focus_rendered_textview)
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
  // Bail out if there are no notes to be displayed.
  if (editornotes.empty ()) return;
    
  // No recording of undoable actions while this object is alive.
  PreventEditorUndo preventundo (&record_undo_level);
  
  // Clean unwanted space out.
  textbuffer_erase_white_space_at_end (textbuffer);

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
  bool no_notes_rendered_yet = true;
  for (unsigned int i = 0; i < editornotes.size (); i++) {
    if (editornotes[i].textview)
      no_notes_rendered_yet = false;
  }
  if (no_notes_rendered_yet) {
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter (textbuffer, &iter);
    gtk_text_buffer_insert (textbuffer, &iter, "\n\n", -1);
  }
  
  for (unsigned int i = 0; i < editornotes.size(); i++) {

    // Skip this note if it has been rendered already.
    if (editornotes[i].textview) continue;
    
    // Insert a new line after all other text.
    GtkTextIter iter = editor_get_iter_for_note (textbuffer, editornotes, i, 0);
    gtk_text_buffer_insert (textbuffer, &iter, "\n", -1);

    // Create the anchor for the note caller in the note.
    iter = editor_get_iter_for_note (textbuffer, editornotes, i, 1);
    editornotes[i].childanchor_caller_note = gtk_text_buffer_create_child_anchor (textbuffer, &iter);

    // Create the label that shows the note caller in the note.
    // The actual character(s) of the caller are written later on.
    // This seems easier, because later on, while editing, these callers may have
    // to be updated regularly anyway.
    editornotes[i].label_caller_note = gtk_label_new ("");
    gtk_text_view_add_child_at_anchor (GTK_TEXT_VIEW (textview), editornotes[i].label_caller_note, editornotes[i].childanchor_caller_note);
    gtk_widget_show_all (editornotes[i].label_caller_note);

    // The caller in the note, and the note text itself, may have different heights, depending
    // on the amount of text to be displayed. The textview would display the note caller 
    // at the bottom of the note text, whereas the user's expectation
    // is to have it at the top, at the start of the first line of text.
    // The next line puts it at the top, but to make the story complete, 
    // there is also a signal fired on allocation of the size of the widgets.
    // This signal calls routines to set the height of the note caller.
    gtk_misc_set_alignment (GTK_MISC (editornotes[i].label_caller_note), 0, 0);
    
    // Create the anchor for the note text.
    iter = editor_get_iter_for_note (textbuffer, editornotes, i, 2);
    editornotes[i].childanchor_textview = gtk_text_buffer_create_child_anchor (textbuffer, &iter);
    
    // A textbuffer to store the note's text.
    editornotes[i].textbuffer = gtk_text_buffer_new (texttagtable);

    // Signal handlers for the textbuffer.
    g_signal_connect (G_OBJECT (editornotes[i].textbuffer), "insert-text", G_CALLBACK (on_buffer_insert_text_before), gpointer (this));
    g_signal_connect_after (G_OBJECT (editornotes[i].textbuffer), "insert-text", G_CALLBACK (on_buffer_insert_text_after), gpointer (this));
    g_signal_connect (G_OBJECT (editornotes[i].textbuffer), "delete-range", G_CALLBACK (on_buffer_delete_range_before), gpointer (this));
    g_signal_connect_after (G_OBJECT (editornotes[i].textbuffer), "delete-range", G_CALLBACK (on_buffer_delete_range_after), gpointer (this));
    g_signal_connect (G_OBJECT (editornotes[i].textbuffer), "apply-tag", G_CALLBACK (on_buffer_apply_tag), gpointer (this));
    g_signal_connect (G_OBJECT (editornotes[i].textbuffer), "remove-tag", G_CALLBACK (on_buffer_remove_tag), gpointer (this));
    g_signal_connect (G_OBJECT (editornotes[i].textbuffer), "changed", G_CALLBACK (on_textbuffer_footnotes_changed), gpointer(this));

    // A text view to display the buffer.
    editornotes[i].textview = gtk_text_view_new_with_buffer (editornotes[i].textbuffer);
    gtk_widget_show (editornotes[i].textview);
    gtk_text_view_add_child_at_anchor (GTK_TEXT_VIEW (textview), editornotes[i].textview, editornotes[i].childanchor_textview);
    gtk_widget_show_all (editornotes[i].textview);
    gtk_text_view_set_accepts_tab (GTK_TEXT_VIEW (editornotes[i].textview), FALSE);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (editornotes[i].textview), GTK_WRAP_WORD);
    gtk_text_view_set_editable (GTK_TEXT_VIEW (editornotes[i].textview), editable);

    // Signal handlers for this textview.
    g_signal_connect_after ((gpointer) editornotes[i].textview, "move_cursor", G_CALLBACK (on_textview_move_cursor), gpointer(this));
    g_signal_connect_after ((gpointer) editornotes[i].textview, "grab_focus", G_CALLBACK (on_textview_grab_focus), gpointer(this));
    g_signal_connect ((gpointer) editornotes[i].textview, "key-press-event", G_CALLBACK (text_key_press_event_before), gpointer(this));
    g_signal_connect_after ((gpointer) editornotes[i].textview, "key-press-event", G_CALLBACK (text_key_press_event_after), gpointer(this));
    g_signal_connect ((gpointer) editornotes[i].textview, "button_press_event", G_CALLBACK (on_textview_button_press_event), gpointer(this));

    // Extract the note caller.
    if (!editornotes[i].rawtext.empty ()) {
      editornotes[i].caller = editornotes[i].rawtext.substr (0, 1);
      editornotes[i].rawtext.erase (0, 1);
      editornotes[i].rawtext = trim (editornotes[i].rawtext);
    }

    // Format the text.
    ustring paragraph_mark (style_get_default_note_style (project, note_type_get (project, editornotes[i].marker)));
    ustring character_mark;
    ustring marker;
    size_t marker_pos;
    size_t marker_length;
    bool is_opener;
    bool marker_found;
    while (!editornotes[i].rawtext.empty ()) {
      marker_found = usfm_search_marker (editornotes[i].rawtext, marker, marker_pos, marker_length, is_opener);
      if (load_text_not_starting_with_marker (editornotes[i].textbuffer, editornotes[i].rawtext, paragraph_mark, character_mark, marker_pos, marker_length, marker_found));
      else if (load_text_starting_new_paragraph (editornotes[i].textbuffer, editornotes[i].rawtext, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found));
      else if (load_text_starting_character_style (editornotes[i].textbuffer, editornotes[i].rawtext, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found));
      else if (load_text_ending_character_style (editornotes[i].textbuffer, editornotes[i].rawtext, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found));
      else if (load_text_starting_footnote_content (editornotes[i].textbuffer, editornotes[i].rawtext, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found));
      else if (load_text_ending_footnote_content (editornotes[i].textbuffer, editornotes[i].rawtext, paragraph_mark, character_mark, marker, marker_pos, marker_length, is_opener, marker_found));
      else load_text_with_unknown_markup (editornotes[i].textbuffer, editornotes[i].rawtext, paragraph_mark, character_mark);
    }

    // Remove any superfluous whitespace at the end of the note.
    textbuffer_erase_character_before_text_insertion_point_if_space (editornotes[i].textbuffer);
    
    // Signal related to the sizes, that is, the height and the width, of the widgets.
    g_signal_connect (GTK_OBJECT (editornotes[i].label_caller_note), "size-allocate", G_CALLBACK (on_related_widget_size_allocated), gpointer (this));
    g_signal_connect (GTK_OBJECT (editornotes[i].textview), "size-allocate", G_CALLBACK (on_related_widget_size_allocated), gpointer (this));
    
    // Optionally focus the rendered textview and scroll to it.
    if (focus_rendered_textview) {
      while (gtk_events_pending ()) gtk_main_iteration ();
      programmatically_grab_focus (editornotes[i].textview);
      GtkTextIter iter;
      gtk_text_buffer_get_iter_at_child_anchor (textbuffer, &iter, editornotes[i].childanchor_textview);
      screen_scroll_to_iterator (GTK_TEXT_VIEW (textview), &iter);
    }
  }
  
  // Set fonts.
  set_font ();
}


void Editor::renumber_and_clean_notes_callers ()
// Renumbers the note callers.
// At the same time clear up unwanted stuff.
{
  // Get the different markers we have.
  vector <ustring> note_openers;
  {
    set <ustring> note_openers_set;
    for (unsigned int i = 0; i < editornotes.size (); i++) {
      note_openers_set.insert (editornotes[i].marker);
    }
    note_openers.assign (note_openers_set.begin (), note_openers_set.end ());
  }
 
  // Create the caller objects, one for each note opening marker.
  map <ustring, NoteCaller *> notecallers;
  for (unsigned int i = 0; i < note_openers.size (); i++) {
    NoteCaller * caller = new NoteCaller (note_numbering_type_get (project, note_openers[i]), note_numbering_user_sequence_get (project, note_openers[i]), false);
    notecallers[note_openers[i]] = caller;
  }
  
  // Set the text of the callers.
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    ustring caller;
    if (editornotes[i].caller == "+") {
      caller = notecallers[editornotes[i].marker]->get_caller ();
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
    char *markup = g_markup_printf_escaped ("<span background=\"grey\" size=\"x-small\"> </span><span background=\"grey\" face=\"Courier\">%s</span><span background=\"grey\" size=\"x-small\"> </span>", caller.c_str ()); 
    gtk_label_set_markup (GTK_LABEL (editornotes[i].label_caller_text), markup);
    gtk_label_set_markup (GTK_LABEL (editornotes[i].label_caller_note), markup);
    g_free (markup);
  }

  // Destroy all the note caller objects.
  for (unsigned int i = 0; i < note_openers.size (); i++) {
    NoteCaller * caller = notecallers[note_openers[i]];
    delete caller;
  }

  // Just to be sure, erase any whitespace at the end of the buffer.
  for (unsigned int i = 0; i < 20; i++) {
    textbuffer_erase_white_space_at_end (textbuffer);
  }

  // Clean up bits between the notes.
  // While the user edits the text, he may add extra spaces or new lines
  // between the notes. These need to be cleared out.
  for (int i = 0; i < (int) editornotes.size () - 1; i++) {
    GtkTextIter startiter, enditer;
    gtk_text_buffer_get_iter_at_child_anchor (textbuffer, &startiter, editornotes[i].childanchor_textview);
    gtk_text_iter_forward_char (&startiter);
    gtk_text_buffer_insert (textbuffer, &startiter, "\n", -1);
    gtk_text_buffer_get_iter_at_child_anchor (textbuffer, &enditer, editornotes[i+1].childanchor_textview);
    gtk_text_iter_backward_char (&enditer);
    gtk_text_buffer_delete (textbuffer, &startiter, &enditer);
  }

  // Get the offset of the last essential character in the buffer.
  GtkTextIter iter;
  gtk_text_buffer_get_end_iter (textbuffer, &iter);
  bool essential_character_found = false;
  do {
    gunichar character = gtk_text_iter_get_char (&iter);
    GtkTextChildAnchor * childanchor = gtk_text_iter_get_child_anchor (&iter);
    if (!childanchor) {
      if (character)
        if (!g_unichar_isspace (character))
          essential_character_found = true;
    }
    // The order of the following two logical parameters is important.
  } while ((!essential_character_found) && gtk_text_iter_backward_char (&iter));
  unsigned int offset_last_essential_character = gtk_text_iter_get_offset (&iter);
  // If a note is inserted, there is code that if any character is added to the
  // end of the textbuffer, the last known paragraph style continues. Hence,
  // if a note is inserted, then two new lines are inserted to separate the 
  // text body from the notes. These usually get the \p style applied.
  // This \p style is unwanted, and therefore needs to be cleaned away.
  // Clear all styles after the last essential character.
  gtk_text_buffer_get_iter_at_offset (textbuffer, &iter, offset_last_essential_character + 1);
  GtkTextIter iter2;
  gtk_text_buffer_get_end_iter (textbuffer, &iter2);
  gtk_text_buffer_remove_all_tags (textbuffer, &iter, &iter2);
}


void Editor::display_table (ustring line, GtkTextIter iter)
// Displays a table, inserting it at "iter". The raw USFM code is in "line".
{
  // Find out the number of columns and rows in this table.
  vector <ustring> markers = usfm_get_all_markers (line);
  int rowcount = 0;
  int columncount = 0;
  extern Settings * settings;
  ustring stylesheet = settings->projectconfig (project)->stylesheet_get ();
  extern Styles * styles;
  Usfm * usfm = styles->usfm (stylesheet);
  for (unsigned int i = 0; i < markers.size (); i++) {
    StyleType type;
    int subtype;
    marker_get_type_and_subtype (project, markers[i], type, subtype);
    if (style_get_starts_table_cell (type, subtype)) {
      if (rowcount == 0) rowcount = 1;
      for (unsigned int i2 = 0; i2 < usfm->styles.size (); i2++) {
        if (usfm->styles[i2].marker == markers[i]) {
          if (usfm->styles[i2].userint1 > columncount)
            columncount = usfm->styles[i2].userint1;
        }
      }
    }
    if (style_get_starts_table_row (type, subtype)) {
      rowcount++;
    }
  }
  
  // Bail out if there aren't enough rows or columns.
  if (rowcount == 0) return;
  if (columncount == 0) return;
  
  // Create the table object.  
  EditorTable editortable (0);
  
  // Insert the table and the textviews in the text.
  editortable.childanchor = gtk_text_buffer_create_child_anchor (textbuffer, &iter);

  editortable.table = gtk_table_new (0, 0, false);
  gtk_widget_show (editortable.table);
  gtk_text_view_add_child_at_anchor (GTK_TEXT_VIEW (textview), editortable.table, editortable.childanchor);
  table_resize (editortable, texttagtable, rowcount, columncount, editable);

  // Connect signal handlers and insert the insertion point mark.
  for (unsigned int row = 0; row < editortable.textviews.size (); row++) {
    for (unsigned int column = 0; column < editortable.textviews[row].size(); column++) {
      GtkWidget * textview = table_cell_get_view (editortable, row, column);
      g_signal_connect_after ((gpointer) textview, "move_cursor", G_CALLBACK (on_textview_move_cursor), gpointer(this));
      g_signal_connect_after ((gpointer) textview, "grab_focus", G_CALLBACK (on_textview_grab_focus), gpointer(this));
      g_signal_connect ((gpointer) textview, "key-press-event", G_CALLBACK (text_key_press_event_before), gpointer(this));
      g_signal_connect_after ((gpointer) textview, "key-press-event", G_CALLBACK (text_key_press_event_after), gpointer(this));
      g_signal_connect ((gpointer) textview, "button_press_event", G_CALLBACK (on_textview_button_press_event), gpointer(this));
      GtkTextBuffer * textbuffer = table_cell_get_buffer (editortable, row, column);
      g_signal_connect (G_OBJECT (textbuffer), "insert-text", G_CALLBACK (on_buffer_insert_text_before), gpointer (this));
      g_signal_connect_after (G_OBJECT (textbuffer), "insert-text", G_CALLBACK (on_buffer_insert_text_after), gpointer (this));
      g_signal_connect (G_OBJECT (textbuffer), "delete-range", G_CALLBACK (on_buffer_delete_range_before), gpointer (this));
      g_signal_connect_after (G_OBJECT (textbuffer), "delete-range", G_CALLBACK (on_buffer_delete_range_after), gpointer (this));
      g_signal_connect (G_OBJECT (textbuffer), "apply-tag", G_CALLBACK (on_buffer_apply_tag), gpointer (this));
      g_signal_connect (G_OBJECT (textbuffer), "remove-tag", G_CALLBACK (on_buffer_remove_tag), gpointer (this));
    }
  }

  // Store the object.
  editortables.push_back (editortable);
  
  // Put the text in the table.
  ustring paragraph_mark (unknown_style ());
  ustring character_mark;
  ustring marker;
  size_t marker_pos;
  size_t marker_length;
  bool is_opener;
  bool marker_found;
  gint row = 0;
  bool row_zero_initialized = false;
  gint column = 0;
  GtkTextBuffer * textbuffer = table_cell_get_buffer (editortable, row, column);
  while (!line.empty ()) {
    marker_found = usfm_search_marker (line, marker, marker_pos, marker_length, is_opener);
    if (load_text_not_starting_with_marker (textbuffer, line, paragraph_mark, character_mark, marker_pos, marker_length, marker_found));
    else if (load_text_table_starting_row (line, editortable, textbuffer, row_zero_initialized, row, column, paragraph_mark, marker, marker_pos, marker_length, is_opener, marker_found));
    else if (load_text_table_starting_cell (line, editortable, textbuffer, row_zero_initialized, row, column, paragraph_mark, marker, marker_pos, marker_length, is_opener, marker_found));
    else load_text_with_unknown_markup (textbuffer, line, paragraph_mark, character_mark);
  }
  
  // Erase any superfluous whitespace at the end of the cells.
  for (unsigned int row = 0; row < editortable.textbuffers.size(); row++) {
    for (unsigned int column = 0; column < editortable.textbuffers[row].size(); column++) {
      GtkTextBuffer * buffer = table_cell_get_buffer (editortable, row, column);
      textbuffer_erase_character_before_text_insertion_point_if_space (buffer);
    }
  }
  
  // Set the font.
  set_font ();
  
  // Set the table's width.
  set_embedded_table_textviews_width (editortables.size () - 1);
}


void Editor::erase_tables ()
/*
This function is called when a GtkTextChildAnchor is in the process of 
being deleted. If this anchor or these anchors happens to contain
a table, this function does the administration of getting rid of this table.
*/
{
  // Remove the objects.
  vector <EditorTable>::iterator iter (editortables.end ());
  for (int i = editortables .size () - 1; i >= 0; i--) {
    iter--;
    if (text_child_anchors_being_deleted.find (editortables[i].childanchor) != text_child_anchors_being_deleted.end ()) {
      editortables.erase (iter);
    }
  }
}


void Editor::on_related_widget_size_allocated (GtkWidget *widget, GtkAllocation *allocation, gpointer user_data)
{
  ((Editor *) user_data)->related_widget_size_allocated (widget, allocation);
}


void Editor::related_widget_size_allocated (GtkWidget *widget, GtkAllocation *allocation)
/*
There are a couple of widget whose size depends upon other widgets, and which
do not naturally take their right size, so need some human intervention to get
them to display properly. This function deals with these various widgets.
*/
{
  if (widget == textview) {
    if (allocation->width != textview_allocated_width) {
      textview_allocated_width = allocation->width;
      if (textview_allocated_width > 1) {
        for (unsigned int i = 0; i < editornotes.size (); i++) {
          set_embedded_note_textview_width (i);
        }
        for (unsigned int i = 0; i < editortables.size (); i++) {
          set_embedded_table_textviews_width (i);
        }
      }
    }
  }

  for (unsigned int i = 0; i < editornotes.size (); i++) {
    if (widget == editornotes[i].label_caller_note) {
      if (allocation->width != editornotes[i].label_caller_note_allocated_width) {
        editornotes[i].label_caller_note_allocated_width = allocation->width;
        if (editornotes[i].label_caller_note_allocated_width > 1) {
          set_embedded_note_textview_width (i);
        }
      }
    }

    if (widget == editornotes[i].textview) {
      if (allocation->height != editornotes[i].textview_allocated_height) {
        editornotes[i].textview_allocated_height = allocation->height;
        if (editornotes[i].textview_allocated_height) {
          set_embedded_note_caller_height (i);
        }
      }
    }
  }
}


void Editor::set_embedded_note_textview_width (unsigned int notenumber)
// This sets the width of the textview that contains the text of a note.
{
  gint width = textview_allocated_width - editornotes[notenumber].label_caller_note_allocated_width - 100;
  if (width < 1) return;
  extern Settings * settings;
  if (width > settings->genconfig.window_width_get () - 200) {
    return;
  }
  gtk_widget_set_size_request (editornotes[notenumber].textview, width, -1);
}


void Editor::set_embedded_note_caller_height (unsigned int notenumber)
// Sets the height of the note caller.
{
  gint height = editornotes[notenumber].textview_allocated_height;
  if (height < 1) return;
  gtk_widget_set_size_request (editornotes[notenumber].label_caller_note, -1, height);
}


void Editor::set_embedded_table_textviews_width (unsigned int tablenumber)
// This sets the width of the textview that contains the text of a note.
{
  // Bail out if the width is too small to be properly handled.  
  gint width = textview_allocated_width - 50;
  if (width < 1) return;

  // Get the maximum column count.
  unsigned int max_column_count = 0;
  for (unsigned int row = 0; row < editortables[tablenumber].textviews.size (); row++) {
    if (editortables[tablenumber].textviews[row].size () > max_column_count) {
      max_column_count = editortables[tablenumber].textviews[row].size ();
    }
  }
  
  // Calculate the width of the textview of one cell.
  width /= max_column_count;
  
  // Set the width of all the cells.
  for (unsigned int row = 0; row < editortables[tablenumber].textviews.size (); row++) {
    for (unsigned int column = 0; column < editortables[tablenumber].textviews[row].size (); column++) {
      GtkWidget * textview = table_cell_get_view (editortables[tablenumber], row, column);
      gtk_widget_set_size_request (textview, width, -1);    
    }
  }
}


void Editor::on_buffer_insert_text_before (GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, gchar *text, gint length, gpointer user_data)
{
  ((Editor *) user_data)->buffer_insert_text_before (textbuffer, pos_iter, text, length);
}


void Editor::buffer_insert_text_before (GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, gchar *text, gint length)
{
  // Only proceed if undo-able actions are to be recorded.
  if (record_undo_actions ()) {

    // At present "undo" only works on the main textbuffer.
    if (textbuffer == this->textbuffer) {

      // Store change in the undo buffer.
      EditorUndo editorundo (0);
      editorundo.type = eudInsertText;
      editorundo.startoffset = gtk_text_iter_get_offset (pos_iter);
      editorundo.endoffset = editorundo.startoffset + length;
      store_undo_action (editorundo);
    }
  }    
}


void Editor::on_buffer_insert_text_after (GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, gchar *text, gint length, gpointer user_data)
{
  ((Editor *) user_data)->buffer_insert_text_after (textbuffer, pos_iter, text, length);
}


void Editor::buffer_insert_text_after (GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, gchar *text, gint length)
{
  // Bail out if no undoes are to be recorded.
  if (!record_undo_actions ()) return;
  
  // Signal that the editor changed.
  signal_editor_changed ();
  
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
  get_styles_at_iterator (iter, paragraph_style, character_style);
  if (paragraph_style.empty ()) {
    while ((paragraph_style.empty ()) && gtk_text_iter_backward_char (&iter)) {
      get_styles_at_iterator (iter, paragraph_style, character_style);
    }
    if (!paragraph_style.empty ()) {
      iter = *pos_iter;
      gtk_text_iter_backward_char (&iter);
      GtkTextIter iter2 = *pos_iter;
      gtk_text_iter_forward_chars (&iter2, length);
      textbuffer_apply_named_tag (textbuffer, paragraph_style, &iter, &iter2);
      if (!character_style.empty ()) {
        textbuffer_apply_named_tag (textbuffer, character_style, &iter, &iter2);
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
  if (character_style.empty ()) {
    if (!character_style_on_start_typing.empty ()) {
      iter = *pos_iter;
      gtk_text_iter_backward_char (&iter);
      GtkTextIter iter2 = *pos_iter;
      gtk_text_iter_forward_chars (&iter2, length - 1);
      textbuffer_apply_named_tag (textbuffer, character_style_on_start_typing, &iter, &iter2);
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
  if (!paragraph_style.empty ()) {
    GtkTextIter iter = * pos_iter;
    gtk_text_iter_backward_char (&iter);  
    ustring paragraph, character;
    get_styles_at_iterator (iter, paragraph, character);
    if (paragraph.empty ()) {
      iter = * pos_iter;
      GtkTextIter iter0 = * pos_iter;
      gtk_text_iter_backward_chars (&iter0, length);
      textbuffer_apply_named_tag (textbuffer, paragraph_style, &iter0, &iter);
    }
  }
  
  /*
  The routine below solves a problem that occurs when inserting a new table.
  After the table has been inserted, no style will be applied to any of the
  table cells.
  If the user starts typing in a cell, no style is applied. 
  This routine will apply the style appropriate for that cell.
  */
  if (paragraph_style.empty ()) {
    for (unsigned int i = 0; i < editortables.size (); i++) {
      EditorTextViewType editortype = last_focused_type ();
      if (editortype == etvtTable) {
        unsigned int column = last_focused_column ();
        ustring marker = style_get_table_cell_marker (project, column);
        apply_style (marker);
      }
    }
  }
  
  /*
  The code below implements intelligent verse handling.
  */
  if (length == 1) {
    gunichar character = g_utf8_get_char (text);
    // When the cursor is after a verse, and the user types a space,
    // he wishes to stop the verse and start normal text.
    if (g_unichar_isspace (character)) {
      // Handle spaces inserted: Switch verse style off if it was on.
      ustring paragraph_style, character_style;
      GtkTextIter iter = *pos_iter;
      get_styles_at_iterator (iter, paragraph_style, character_style);
      ustring versemarker = style_get_verse_marker (project);
      if (character_style_on_start_typing == versemarker) {
        character_style_on_start_typing.clear ();
      }
    }
    // When the cursor is after a verse, and the user types a numeral,
    // a hyphen or a comma, it means he wishes to extend the verse style.
    if (g_unichar_isdigit (character) || !strcmp (text, "-") || !strcmp (text, ",")) {
      ustring paragraph_style, character_style;
      GtkTextIter iter = *pos_iter;
      gtk_text_iter_backward_chars (&iter, 2);
      get_styles_at_iterator (iter, paragraph_style, character_style);
      ustring versemarker = style_get_verse_marker (project);
      if (character_style == versemarker) {
        iter = *pos_iter;
        GtkTextIter iter2 = iter;
        gtk_text_iter_backward_char (&iter2);
        gtk_text_buffer_apply_tag_by_name (textbuffer, character_style.c_str (), &iter2, &iter);        
      }
    }
  }
}


void Editor::on_buffer_delete_range_before (GtkTextBuffer *textbuffer, GtkTextIter *start, GtkTextIter *end, gpointer user_data)
{
  ((Editor *) user_data)->buffer_delete_range_before (textbuffer, start, end);
 
}


void Editor::buffer_delete_range_before (GtkTextBuffer *textbuffer, GtkTextIter *start, GtkTextIter *end)
{
  // Handle deleting footnotes.
  collect_text_child_anchors_being_deleted (start, end);

  // Only proceed if undo-able actions are to be recorded.
  if (record_undo_actions ()) {

    // At present "undo" only works on the main textbuffer.
    if (textbuffer == this->textbuffer) {

      // Store change in the undo buffer, if there was a real change.
      EditorUndo editorundo (0);
      editorundo.type = eudDeleteText;
      usfm_get_text (textbuffer, * start, * end, &editornotes, &editortables, project, editorundo.text);
      if (!editorundo.text.empty ()) {
        editorundo.startoffset = gtk_text_iter_get_offset (start);
        editorundo.endoffset = gtk_text_iter_get_offset (end);
        // Also store whether this text started a new line. Needed for undo.
        editorundo.flag = gtk_text_iter_starts_line (start);
        store_undo_action (editorundo);
      }
    }
  }
}


void Editor::on_buffer_delete_range_after (GtkTextBuffer *textbuffer, GtkTextIter *start, GtkTextIter *end, gpointer user_data)
{
  ((Editor *) user_data)->buffer_delete_range_after (textbuffer, start, end);
}


void Editor::buffer_delete_range_after (GtkTextBuffer *textbuffer, GtkTextIter *start, GtkTextIter *end)
{
  signal_editor_changed ();
  process_text_child_anchors_deleted ();
}


void Editor::on_buffer_apply_tag (GtkTextBuffer *textbuffer, GtkTextTag *tag, GtkTextIter *startiter, GtkTextIter *enditer, gpointer user_data)
{
  ((Editor *) user_data)->buffer_apply_tag (textbuffer, tag, startiter, enditer);
}


void Editor::buffer_apply_tag (GtkTextBuffer *textbuffer, GtkTextTag *tag, GtkTextIter *startiter, GtkTextIter *enditer)
{
  // Only proceed if undo-able actions are to be recorded.
  if (record_undo_actions ()) {

    // Signal that the editor changed.
    signal_editor_changed ();
  
    // Set the textbuffer modified.
    gtk_text_buffer_set_modified (textbuffer, true);

    // At present "undo" only works on the main textbuffer.
    if (textbuffer == this->textbuffer) {

      // Get the name of the tag.
      ustring tagname;
      gchar * strval;
      g_object_get (G_OBJECT (tag), "name", &strval, NULL);
      if (strval) {
        if (strlen (strval)) {
          tagname = strval;
        }
        g_free (strval);
      }
      
      // If there was a named tag, store the change in the undo buffer.
      if (!tagname.empty ()) {
        EditorUndo editorundo (0);
        editorundo.type = eudApplyTag;
        editorundo.text = tagname;
        editorundo.startoffset = gtk_text_iter_get_offset (startiter);
        editorundo.endoffset = gtk_text_iter_get_offset (enditer);
        store_undo_action (editorundo);
      }
    }
  }
}


void Editor::on_buffer_remove_tag (GtkTextBuffer *textbuffer, GtkTextTag *tag, GtkTextIter *startiter, GtkTextIter *enditer, gpointer user_data)
{
  ((Editor *) user_data)->buffer_remove_tag (textbuffer, tag, startiter, enditer);
}


void Editor::buffer_remove_tag (GtkTextBuffer *textbuffer, GtkTextTag *tag, GtkTextIter *startiter, GtkTextIter *enditer)
{
  // Only proceed if undo-able actions are to be recorded.
  if (record_undo_actions ()) {

    // Signal that the editor changed.
    signal_editor_changed ();
  
    // Set the textbuffer modified.
    gtk_text_buffer_set_modified (textbuffer, true);

    // At present "undo" only works on the main textbuffer.
    if (textbuffer == this->textbuffer) {

      // Get the name of the tag.
      ustring tagname;
      gchar * strval;
      g_object_get (G_OBJECT (tag), "name", &strval, NULL);
      if (strval) {
        if (strlen (strval)) {
          tagname = strval;
        }
        g_free (strval);
      }
      
      // If there was a named tag, store the change in the undo buffer.
      if (!tagname.empty ()) {
        EditorUndo editorundo (0);
        editorundo.type = eudRemoveTag;
        editorundo.text = tagname;
        editorundo.startoffset = gtk_text_iter_get_offset (startiter);
        editorundo.endoffset = gtk_text_iter_get_offset (enditer);
        store_undo_action (editorundo);
      }
    }
  }
}


void Editor::on_buffer_insert_child_anchor (GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, GtkTextChildAnchor *childanchor, gpointer user_data)
{
  ((Editor *) user_data)->buffer_insert_child_anchor (textbuffer, pos_iter, childanchor);
}


void Editor::buffer_insert_child_anchor (GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, GtkTextChildAnchor *childanchor)
{
  // Only proceed if undo-able actions are to be recorded.
  if (record_undo_actions ()) {
    // Signal that the editor changed.
    signal_editor_changed ();
    // At present "undo" only works on the main textbuffer.
    if (textbuffer == this->textbuffer) {
      // Store change in the undo buffer.
      EditorUndo editorundo (0);
      editorundo.type = eudInsertText;
      editorundo.startoffset = gtk_text_iter_get_offset (pos_iter);
      editorundo.endoffset = editorundo.startoffset + 1;
      store_undo_action (editorundo);
    }
  }
}


void Editor::on_buffer_insert_pixbuf (GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, GdkPixbuf *pixbuf, gpointer user_data)
{
  ((Editor *) user_data)->buffer_insert_pixbuf (textbuffer, pos_iter, pixbuf);
}


void Editor::buffer_insert_pixbuf (GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, GdkPixbuf *pixbuf)
{
  // Signal that the editor changed.
  signal_editor_changed ();
}


void Editor::collect_text_child_anchors_being_deleted (GtkTextIter *startiter, GtkTextIter *enditer)
// This function stores the GtkTextChildAnchors that are being deleted.
{
  if (do_not_process_child_anchors_being_deleted) return;
  GtkTextIter iter = * startiter;
  do {
    GtkTextChildAnchor * anchor = gtk_text_iter_get_child_anchor (&iter);
    if (anchor)
      text_child_anchors_being_deleted.insert (anchor);
    gtk_text_iter_forward_char (&iter);
  } while (gtk_text_iter_in_range (&iter, startiter, enditer));
}


void Editor::process_text_child_anchors_deleted ()
// This function processes the GtkTextChildAnchors that have been deleted.
{
  if (do_not_process_child_anchors_being_deleted) return;
  if (!text_child_anchors_being_deleted.empty ()) {
    erase_related_note_bits ();
    erase_tables ();
  }
  text_child_anchors_being_deleted.clear ();
}


void Editor::text_append (GtkTextBuffer * textbuffer, const ustring& text, const ustring& paragraph_style, const ustring& character_style)
// This function appends text to the textbuffer.
// It inserts the text at the cursor.
{
  // Get the iterator at the text insertion point.
  GtkTextIter insertiter;
  gtk_text_buffer_get_iter_at_mark (textbuffer, &insertiter, gtk_text_buffer_get_insert (textbuffer));
  // Insert text together with the style(s).
  textbuffer_insert_with_named_tags (textbuffer, &insertiter, text, paragraph_style, character_style);
}


void Editor::signal_if_styles_changed ()
{
  set <ustring> styles = get_styles_at_cursor ();
  if (styles != styles_at_cursor) {
    styles_at_cursor = styles;
    gtk_button_clicked (GTK_BUTTON (new_styles_signal));
  }
}


set <ustring> Editor::get_styles_at_cursor ()
// Gets all the styles that apply to the cursor, or to the selection.
{
  // Hold the styles.
  set <ustring> styles;
  
  // Get the iterators at the selection bounds of the main textview or note or table.
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_selection_bounds (textbuffer, &startiter, &enditer);
  if (GTK_WIDGET_HAS_FOCUS (textview)) {
    gtk_text_buffer_get_selection_bounds (textbuffer, &startiter, &enditer);
  } else {
    for (unsigned int i = 0; i < editornotes.size(); i++) {
      if (GTK_WIDGET_HAS_FOCUS (editornotes[i].textview)) {
        GtkTextBuffer * buffer = editornotes[i].textbuffer;
        gtk_text_buffer_get_selection_bounds (buffer, &startiter, &enditer);
      }
    }
    for (unsigned int i = 0; i < editortables.size (); i++) {
      for (unsigned int row = 0; row < editortables[i].textviews.size (); row++) {
        for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
          if (GTK_WIDGET_HAS_FOCUS (table_cell_get_view (editortables[i], row, column))) {
            GtkTextBuffer * buffer = table_cell_get_buffer (editortables[i], row, column);
            gtk_text_buffer_get_selection_bounds (buffer, &startiter, &enditer);
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
    get_styles_at_iterator (iter, paragraphstyle, characterstyle);
    if (!paragraphstyle.empty ()) styles.insert(paragraphstyle);
    if (!characterstyle.empty ()) styles.insert(characterstyle);
    gtk_text_iter_forward_char (&iter);
  } while (gtk_text_iter_in_range (&iter, &startiter, &enditer));
  
  // Return the list.
  return styles;
}


set <ustring> Editor::styles_at_iterator (GtkTextIter iter)
// Get all the styles that apply at the iterator.
{
  set <ustring> styles;
  ustring paragraph_style, character_style;
  get_styles_at_iterator (iter, paragraph_style, character_style);
  if (!paragraph_style.empty ()) styles.insert (paragraph_style);
  if (!character_style.empty ()) styles.insert (character_style);    
  return styles;
}


bool Editor::has_focus ()
// This returns true if the editor is focused now.
// Focused, that means that either the main textview has focus, 
// or any of the embedded notes or tables.
{
  if (GTK_WIDGET_HAS_FOCUS (textview)) {
    return true;
  }
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    if (GTK_WIDGET_HAS_FOCUS (editornotes[i].textview)) {
      return true;
    }
  }
  for (unsigned int i = 0; i < editortables.size (); i++) {
    for (unsigned int row = 0; row < editortables[i].textviews.size (); row++) {
      for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
        if (GTK_WIDGET_HAS_FOCUS (table_cell_get_view (editortables[i], row, column))) {
          return true;
        }
      }
    }
  }
  return false;
}


GtkTextBuffer * Editor::last_focused_textbuffer ()
// Returns the focused textbuffer, or NULL if none.
{
  return gtk_text_view_get_buffer (GTK_TEXT_VIEW (last_focused_widget));
}


GtkWidget * Editor::last_focused_textview ()
// Returns the textview that was focused last.
{
  return last_focused_widget;
}


EditorTextViewType Editor::last_focused_type ()
// Returns the type of the textview that was focused most recently.
// This could be the main body of text, or a note, or a table.
{
  if (last_focused_widget == textview) {
    return etvtBody;
  }
  for (unsigned int i = 0; i < editornotes.size(); i++) {
    if (last_focused_widget== editornotes[i].textview) {
      return etvtNote;
    }
  }
  for (unsigned int i = 0; i < editortables.size (); i++) {
    for (unsigned int row = 0; row < editortables[i].textviews.size (); row++) {
      for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
        if (last_focused_widget == table_cell_get_view (editortables[i], row, column)) {
          return etvtTable;
        }
      }
    }
  }
  return etvtBody;
}


unsigned int Editor::last_focused_column ()
// In case the last focused widget is a table cell,
// it returns the column number of that cell.
// The first column is number 1.
// Outside of the table it just returns 0.
{
  for (unsigned int i = 0; i < editortables.size (); i++) {
    for (unsigned int row = 0; row < editortables[i].textviews.size (); row++) {
      for (unsigned int column = 0; column < editortables[i].textviews[row].size(); column++) {
        if (last_focused_widget == table_cell_get_view (editortables[i], row, column)) {
          return ++column;
        }
      }
    }
  }
  return 0;
}


void Editor::apply_style (const ustring& marker)
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
  marker_get_type_and_subtype (project, marker, type, subtype);

  /*
  Get the type of textview that was focused last, 
  and find out whether the style that is now going to be inserted
  is applicable in this particular textview.
  For example to a table only the relevant table styles can be applied. 
  Give a message if there is a mismatch.
  */
  EditorTextViewType textviewtype = last_focused_type ();
  bool style_application_ok = true;
  ustring style_application_fault_reason;
  switch (textviewtype) {
    case etvtBody:
    {
      if ((type == stFootEndNote) || (type == stCrossreference) || (type == stTableElement)) {
        style_application_ok = false;
      }
      style_application_fault_reason = "Trying to apply a ";
      if (type == stFootEndNote) style_application_fault_reason.append ("foot- or endnote");
      if (type == stCrossreference) style_application_fault_reason.append ("crossreference");
      if (type == stTableElement) style_application_fault_reason.append ("table");
      style_application_fault_reason.append (" style");
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
      extern Settings * settings;
      ustring stylesheet = settings->projectconfig (project)->stylesheet_get ();
      extern Styles * styles;
      Usfm * usfm = styles->usfm (stylesheet);
      for (unsigned int i = 0; i < usfm->styles.size (); i++) {
        if (marker == usfm->styles[i].marker) {
          unsigned int column = usfm->styles[i].userint1;
          if (column != last_focused_column ()) {
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
    ustring message ("This style cannot be applied here.");
    message.append ("\n\n");
    message.append (style_application_fault_reason);
    message.append (".");
    gtkw_dialog_error (NULL, message.c_str ());
    return;
  }
  
  // Get the textbuffer that was focused last.
  GtkTextBuffer * textbuffer = last_focused_textbuffer ();
  
  // Get the textview that was focused last.
  GtkWidget * textview = last_focused_textview ();

  if (style_get_starts_new_line_in_editor (type, subtype)) {
    // Handle a paragraph style.

    // Find the iterator at the start and at the end of the line that has the insertion point.
    GtkTextIter iter, startiter, enditer;
    gtk_text_buffer_get_iter_at_mark (textbuffer, &iter, gtk_text_buffer_get_insert (textbuffer));
    gint line_number = gtk_text_iter_get_line (&iter);
    gtk_text_buffer_get_iter_at_line (textbuffer, &startiter, line_number);
    line_number++;
    if (line_number == gtk_text_buffer_get_line_count (textbuffer)) {
      gtk_text_buffer_get_end_iter (textbuffer, &enditer);
    } else {
      gtk_text_buffer_get_iter_at_line (textbuffer, &enditer, line_number);
    }
    
    // Get the currently applied character styles.
    vector <ustring> character_styles;
    iter = startiter;
    do {
      ustring paragraph_style, character_style;
      get_styles_at_iterator (iter, paragraph_style, character_style);
      character_styles.push_back (character_style);
      gtk_text_iter_forward_char (&iter);
    } while (gtk_text_iter_in_range (&iter, &startiter, &enditer));
    
    // Clear all styles from the range.
    gtk_text_buffer_remove_all_tags (textbuffer, &startiter, &enditer);
    
    // Apply the new paragraph style to the range.
    textbuffer_apply_named_tag (textbuffer, marker, &startiter, &enditer);

    // Apply the previously found character style(s).
    // (The reason that the character styles are applied again, after having
    //  been removed, is, that throughout the Editor object it is assumed
    //  that the first named style is the paragraph style, and the second
    //  named style is the character style. So the order in which the styles
    //  are being applied is important.)
    iter = startiter;
    for (unsigned int i = 0; i < character_styles.size (); i++) {
      GtkTextIter iter2 = iter;
      gtk_text_iter_forward_char (&iter2);
      if (!character_styles[i].empty ()) {
        textbuffer_apply_named_tag (textbuffer, character_styles[i], &iter, &iter2);
      }
      gtk_text_iter_forward_char (&iter);
    }

  } else {
    // Handle a character style.
    
    // Find the iterator at the start and at the end of the text to be put in this style.
    GtkTextIter iter, startiter, enditer;
    // If some text has been selected, take that.
    if (gtk_text_buffer_get_selection_bounds (textbuffer, &startiter, &enditer)) {
    } else {
      // No selection:
      // If the insertion point is inside a word, take that word.  
      // Else just take the insertion point.
      gtk_text_buffer_get_iter_at_mark (textbuffer, &iter, gtk_text_buffer_get_insert (textbuffer));
      startiter = iter;
      enditer = iter;
      if (gtk_text_iter_inside_word (&iter) && !gtk_text_iter_starts_word (&iter)) {
        gtk_text_iter_backward_word_start (&startiter);
        gtk_text_iter_forward_word_end (&enditer);
      }
    }
    
    // Remove any currently applied character style(s).
    // At the same time check whether the character style that we are going to 
    // apply has been applied already throughout the range.
    iter = startiter;
    bool character_style_already_applied = true;
    do {
      ustring paragraph_style, character_style;
      get_styles_at_iterator (iter, paragraph_style, character_style);
      if (character_style != marker)
        character_style_already_applied = false;
      GtkTextIter iter2 = iter;
      gtk_text_iter_forward_char (&iter2);
      if (!character_style.empty ()) {
        gtk_text_buffer_remove_tag_by_name (textbuffer, character_style.c_str (), &iter, &iter2);
      }
      gtk_text_iter_forward_char (&iter);
    } while (gtk_text_iter_in_range (&iter, &startiter, &enditer));

    // Apply the new character style. If the character style was applied already,
    // it works like a toggle: it will be left removed.
    if (!character_style_already_applied) {
      textbuffer_apply_named_tag (textbuffer, marker, &startiter, &enditer);
    }
    
    // Store this character style so it can be re-used when the user starts 
    // typing text. But when the style was toggled off, clear it.
    if (!character_style_already_applied) {
      character_style_on_start_typing = marker;
    } else {
      character_style_on_start_typing.clear ();
    }

  }

  // Update gui so it shows the right styles.
  signal_if_styles_changed ();
  
  // Focus editor.
  programmatically_grab_focus (textview);
  
}



bool Editor::on_apply_style_at_cursor (gpointer user_data) 
{
  ((Editor *) user_data)->apply_style_at_cursor_handler ();
  return false;
}


void Editor::apply_style_at_cursor_handler ()
{
  apply_style (style_to_be_applied_at_cursor);
}


void Editor::insert_note (const ustring& marker, const ustring& rawtext, GtkTextIter * iter, bool render)
/*
Inserts a note in the editor.
marker:    The marker that starts the note, e.g. "fe" for an endnote.
rawtext:   The raw text of the note, e.g. "+ Mat 1.1.". Note that this excludes
           the note opener and note closer. It has only the caller and the
           USFM code of the note body.
iter:      Where to insert the note. If NULL, it inserts the note at the cursor.
render:    Whether to render the notes straightaway.
*/
{
  // Create the object for holding the data related to this note.
  EditorNote editornote (0);
  
  // Save the marker.
  editornote.marker = marker;

  // Save the raw note text.
  editornote.rawtext = rawtext;

  // Get the iterator where to insert the note caller.
  GtkTextIter insertiter;
  if (iter) insertiter = *iter;
  else gtk_text_buffer_get_iter_at_mark (textbuffer, &insertiter, gtk_text_buffer_get_insert (textbuffer));

  // Create the anchor for the note caller in the text.
  editornote.childanchor_caller_text = gtk_text_buffer_create_child_anchor (textbuffer, &insertiter);

  // Create the label that shows the note caller in the text.
  // The actual character(s) of the caller are determined later on.
  // This seems easier, because later on, while editing, these callers may have
  // to be updated regularly.
  editornote.label_caller_text = gtk_label_new ("");
  gtk_text_view_add_child_at_anchor (GTK_TEXT_VIEW (textview), editornote.label_caller_text, editornote.childanchor_caller_text);
  gtk_widget_show_all (editornote.label_caller_text);
  
  // Store object. This is not just a matter of pushing it at the back of the container.
  // We need to find out at which position in the container this note should be stored.
  // The position where to store depends on the position of this note in the text
  // relative to the other notes in the text.
  if (editornotes.empty ()) {
    editornotes.push_back (editornote);
  } else {
    unsigned int offset_new_note = gtk_text_iter_get_offset (&insertiter);
    unsigned int insert_offset = 0;
    bool insert_offset_found = false;
    for (unsigned int i = 0; i < editornotes.size (); i++) {
      GtkTextIter iter;
      gtk_text_buffer_get_iter_at_child_anchor (textbuffer, &iter, editornotes[i].childanchor_caller_text);
      unsigned int offset_existing_note = gtk_text_iter_get_offset (&iter);
      if (!insert_offset_found) {
        if (offset_new_note <= offset_existing_note) {
          insert_offset = i;
          insert_offset_found = true;
        }
      }
    }
    if (insert_offset_found) {
      vector <EditorNote>::iterator iter (editornotes.begin ());
      for (unsigned int i = 0; i < insert_offset; i++) {
        iter++;
      }
      editornotes.insert (iter, editornote);
    } else {
      editornotes.push_back (editornote);
    }
  }

  if (render) {
    // Render this note, if requested, placing the cursor in the new textview.
    display_notes_remainder (true);
    renumber_and_clean_notes_callers ();
  }
}


void Editor::insert_table (const ustring& rawtext, GtkTextIter * iter)
/*
Inserts a table in the editor.
rawtext: The raw text of the table, e.g. "\tr \tc1 \tc2 \tc3 \tc4 ".
iter:    Where to insert the note. If NULL, it inserts the note at the cursor.
*/
{
  // Get the location where to insert the note caller.
  GtkTextIter insertiter;
  if (iter) insertiter = *iter;
  else gtk_text_buffer_get_iter_at_mark (textbuffer, &insertiter, gtk_text_buffer_get_insert (textbuffer));

  // Insert the table at that location.  
  display_table (rawtext, insertiter);

}


void Editor::list_undo_buffer () 
{
  /*
  cout << "Start listing the undo buffer" << endl;
  for (unsigned int i = 0; i < editorundoes.size (); i++) {
    cout << editorundoes[i].type2text () << " " << editorundoes[i].text << " " << editorundoes[i].startoffset << " " << editorundoes[i].endoffset << endl;
  }
  cout << "Ready listing the undo buffer" << endl;
  */
}


void Editor::store_undo_action (const EditorUndo& editorundo)
/*
The purpose of this function is to store an undo action sensibly.
To do that properly it either adds this undo action to the end of the container,
or else if that is more sensible it joins this action to the one already at the
end of the container.
*/
{
  // Variable whether it got stored.
  bool stored = false;

  // Joining only possible when there is already something in the buffer.
  if (!editorundoes.empty ()) {
    
    // Get the most recent undo action.
    EditorUndo recent_undo = editorundoes[editorundoes.size () - 1];
    
    // See whether we can join an action that a tag was removed to the previous one.
    if (!stored) {
      if (editorundo.type == eudRemoveTag) {
        if (editorundo.text == recent_undo.text) {
          if (editorundo.startoffset == recent_undo.endoffset) {
            editorundoes[editorundoes.size()-1].endoffset = editorundo.endoffset;
            stored = true;
          }
        }
      }
    }
    
  }

  // If not stored yet, store it now.
  if (!stored) {
    editorundoes.push_back (editorundo);
  }
}


bool Editor::record_undo_actions ()
// Returns whether to record undo-able actions.
{
  return (record_undo_level == 0);
}


void Editor::on_textbuffer_changed (GtkTextBuffer * textbuffer, gpointer user_data)
{
}


void Editor::on_textbuffer_modified_changed (GtkTextBuffer * textbuffer, gpointer user_data)
{
}


void Editor::test ()
{
  gw_message ("start test");  
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_start_iter (textbuffer, &startiter);
  gtk_text_buffer_get_end_iter (textbuffer, &enditer);
  ustring chaptertext;
  usfm_get_text (textbuffer, startiter, enditer, &editornotes, &editortables, project, chaptertext);
  gw_message (chaptertext);  
  gw_message ("end test");  
}


void Editor::highlight_searchwords ()
// Highlights all the search words.
{
  // Destroy optional previous object.
  if (highlight) delete highlight;
  // Create a new object.
  highlight = new Highlight (textbuffer, textview, project, editornotes, editortables, reference_tag, position_cursor_at_verse_cursorposition);
  g_thread_create (GThreadFunc (highlight_thread_start), gpointer (this), false, NULL);
}


bool Editor::on_highlight_timeout (gpointer data)
{
  return ((Editor *) data)->highlight_timeout ();
}


bool Editor::highlight_timeout ()
{
  // Proceed if a highlighting object is there.
  if (highlight) {
    // Proceed if the locations for highlighting are ready.
    if (highlight->locations_ready) {
      // Possible insertion of highlighting tags may trigger storage of an unwanted
      // undo-able action. 
      // Therefore: No recording of undoable actions while this object is alive.
      PreventEditorUndo preventundo (&record_undo_level);
      highlight->highlight ();
      // Delete and NULL the object making it ready for next use.
      delete highlight;
      highlight = NULL;
    }
  }
  return true;
}


void Editor::highlight_thread_start (gpointer data)
{
  ((Editor*) data)->highlight_thread_main();
}


void Editor::highlight_thread_main ()
{
  // The time-consuming part of highlighting is to determine what bits of text
  // to highlight. Because it takes time, and the program should continue
  // to respond, it is done in a thread.
  if (highlight) {
    highlight->determine_locations ();
  }
}


void Editor::focus ()
{
  focus_set = true; // Set focus flag.
  gtk_button_clicked (GTK_BUTTON (focus_signal));
  focus_set = true; // Set flag again.
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), 1);
  if (my_tab_label) gtk_label_set_text (GTK_LABEL (my_tab_label), project.c_str());
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progressbar), project.c_str());
  // Focus the active textview.  
  focus_programmatically_being_grabbed = true;  
  if (GTK_WIDGET_REALIZED (last_focused_widget)) {
    gtk_widget_grab_focus (last_focused_widget);  
  }
  focus_programmatically_being_grabbed = false;  
}


bool Editor::focused ()
{
  return focus_set;
}


void Editor::defocus ()
{
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), 0);
  focus_set = false;
}


void Editor::on_button_titlebar_clicked (GtkButton *button, gpointer user_data)
{
  ((Editor *) user_data)->focus ();
}


void Editor::title_bar_show (bool show)
// Whether to show the title bar.
{
  if (show) {
    gtk_widget_show (hbox_title);
  } else {
    gtk_widget_hide (hbox_title);
  }
}


void Editor::signal_editor_changed ()
{
  gtk_button_clicked (GTK_BUTTON (changed_signal));
}


ustring Editor::get_chapter ()
{
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_start_iter (textbuffer, &startiter);
  gtk_text_buffer_get_end_iter (textbuffer, &enditer);
  ustring chaptertext;
  usfm_get_text (textbuffer, startiter, enditer, &editornotes, &editortables, project, chaptertext);
  replace_text (chaptertext, "  ", " ");
  return chaptertext;
}
