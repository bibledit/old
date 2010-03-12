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
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourcestyleschememanager.h>
#include "progresswindow.h"


USFMView::USFMView(GtkWidget * vbox, const ustring & project_in):
current_reference(0, 1000, "")
{
  // Save and initialize variables.
  project = project_in;
  book = 0;
  chapter = 0;
  verse_tracker_event_id = 0;
  editable = false;
  spelling_timeout_event_id = 0;

  // The scrolled window that contains the sourceview.
  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow);
  gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, true, true, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  // A sourcebuffer to store all text.
  sourcebuffer = gtk_source_buffer_new (NULL);
  
  // Tag for highlighting markup.
  markup_tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER (sourcebuffer), NULL, "foreground", "red", NULL);

  // A sourceview to display the buffer.
  sourceview = gtk_source_view_new_with_buffer(sourcebuffer);
  gtk_widget_show(sourceview);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), sourceview);
  gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(sourceview), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(sourceview), GTK_WRAP_WORD);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(sourceview), FALSE);
  gtk_text_view_set_left_margin(GTK_TEXT_VIEW(sourceview), 5);
  gtk_source_view_set_highlight_current_line (GTK_SOURCE_VIEW (sourceview), true);

  // The buffer's signal handlers.
  g_signal_connect(G_OBJECT(sourcebuffer), "changed", G_CALLBACK(on_textbuffer_changed), gpointer(this));
  
  // The view's signal handlers.
  g_signal_connect_after((gpointer) sourceview, "move_cursor", G_CALLBACK(on_textview_move_cursor), gpointer(this));
  g_signal_connect_after((gpointer) sourceview, "grab_focus", G_CALLBACK(on_textview_grab_focus), gpointer(this));
  g_signal_connect((gpointer) sourceview, "button_press_event", G_CALLBACK(on_textview_button_press_event), gpointer(this));
  g_signal_connect((gpointer) sourceview, "key-press-event", G_CALLBACK(on_textview_key_press_event), gpointer(this));

  // Buttons to give signals.
  reload_signal = gtk_button_new();
  changed_signal = gtk_button_new();
  new_verse_signal = gtk_button_new();
  word_double_clicked_signal = gtk_button_new();
  spelling_checked_signal = gtk_button_new ();

  // Automatic saving of the file, periodically.
  save_timeout_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 60000, GSourceFunc(on_save_timeout), gpointer(this), NULL);

  // Fonts.
  set_font();

  // Spelling checker.
  GtkTextTagTable * texttagtable = gtk_text_buffer_get_tag_table (GTK_TEXT_BUFFER (sourcebuffer));
  spellingchecker = new SpellingChecker(texttagtable);
  g_signal_connect((gpointer) spellingchecker->check_signal, "clicked", G_CALLBACK(on_button_spelling_recheck_clicked), gpointer(this));
  load_dictionaries();
  spellingchecker->attach(sourceview);
}


USFMView::~USFMView()
{
  // Save the chapter.
  chapter_save();

  // Destroy a couple of timeout sources.
  gw_destroy_source(save_timeout_event_id);
  gw_destroy_source(verse_tracker_event_id);
  gw_destroy_source(spelling_timeout_event_id);

  // Destroy the signalling buttons.
  gtk_widget_destroy(reload_signal);
  gtk_widget_destroy(changed_signal);
  gtk_widget_destroy(new_verse_signal);
  new_verse_signal = NULL;
  gtk_widget_destroy(word_double_clicked_signal);
  gtk_widget_destroy (spelling_checked_signal);
  
  // Delete speller.
  delete spellingchecker;

  // Destroy the sourceview.
  gtk_widget_destroy(scrolledwindow);
}


void USFMView::book_set(unsigned int book_in)
{
  book = book_in;
}


void USFMView::chapter_load(unsigned int chapter_in)
// Loads a chapter with the number "chapter_in".
{
  // Clear the undo buffer.
  gtk_source_buffer_set_max_undo_levels (sourcebuffer, 0);
  gtk_source_buffer_set_max_undo_levels (sourcebuffer, -1);

  // No recording of undoable actions.
  gtk_source_buffer_begin_not_undoable_action (sourcebuffer);

  // Get rid of possible previous text.
  gtk_text_buffer_set_text(GTK_TEXT_BUFFER (sourcebuffer), "", -1);

  // Save chapter number.
  chapter = chapter_in;

  // Load text in memory.
  vector <ustring> lines = project_retrieve_chapter(project, book, chapter);

  // Settings.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);

  // Deal with (non-)editable.
  editable = true;
  if (lines.empty())
    editable = false;
  if (!projectconfig->editable_get())
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
  if (!editable)
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
       But if chapter 0 is detected, do nothing, no questions asked.
     */
    unsigned int chapter_in_text = chapter;
    for (unsigned int i = 0; i < ccv.chapter.size(); i++) {
      if (ccv.chapter[i] != chapter) {
        chapter_in_text = ccv.chapter[i];
      }
    }
    if (chapter_in_text == 0) {
      chapter_in_text = chapter;
      for (unsigned int i = 0; i < ccv.chapter.size(); i++) {
        ccv.chapter[i] = chapter;
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
    gtk_button_clicked(GTK_BUTTON(reload_signal));
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


bool USFMView::can_undo ()
{
  return gtk_source_buffer_can_undo (sourcebuffer);
}


void USFMView::undo()
{
  gtk_source_buffer_undo (sourcebuffer);
}


bool USFMView::can_redo()
{
  return gtk_source_buffer_can_redo (sourcebuffer);
}


void USFMView::redo()
{
  gtk_source_buffer_redo (sourcebuffer);
}


void USFMView::set_font()
{
  // Set font.
  PangoFontDescription *font_desc = NULL;
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  if (!projectconfig->editor_font_default_get()) {
    font_desc = pango_font_description_from_string(projectconfig->editor_font_name_get().c_str());
  }
  gtk_widget_modify_font(sourceview, font_desc);
  if (font_desc)
    pango_font_description_free(font_desc);

  // Set the colours.
  if (projectconfig->editor_default_color_get()) {
    color_widget_default(sourceview);
  } else {
    color_widget_set(sourceview, projectconfig->editor_normal_text_color_get(), projectconfig->editor_background_color_get(), projectconfig->editor_selected_text_color_get(), projectconfig->editor_selection_color_get());
  }

  // Set predominant text direction.
  if (projectconfig->right_to_left_get()) {
    gtk_widget_set_direction(sourceview, GTK_TEXT_DIR_RTL);
  } else {
    gtk_widget_set_direction(sourceview, GTK_TEXT_DIR_LTR);
  }
}


bool USFMView::on_save_timeout(gpointer data)
{
  return ((USFMView *) data)->save_timeout();
}


bool USFMView::save_timeout()
{
  chapter_save();
  return true;
}


void USFMView::on_textbuffer_changed(GtkTextBuffer * textbuffer, gpointer user_data)
{
  ((USFMView *) user_data)->textbuffer_changed();
}


void USFMView::textbuffer_changed()
{
  gtk_button_clicked(GTK_BUTTON(changed_signal));
  
  // Colour the usfm code.
  GtkTextBuffer * textbuffer = GTK_TEXT_BUFFER (sourcebuffer);
  GtkTextIter iter;
  gtk_text_buffer_get_start_iter(textbuffer, &iter);
  GtkTextIter iter2;
  gtk_text_buffer_get_end_iter(textbuffer, &iter2);
  gtk_text_buffer_remove_tag(textbuffer, markup_tag, &iter, &iter2);
  bool within_usfm = false;
  do {
    gunichar character = gtk_text_iter_get_char(&iter);
    if (character) {
      bool digit = g_unichar_isdigit (character);
      if (character == '\\')
        within_usfm = true;
      if (g_unichar_isspace (character))
        within_usfm = false;
      if (character == '*')
        within_usfm = false;
      if (digit || within_usfm) {
        GtkTextIter enditer = iter;
        gtk_text_iter_forward_char (&enditer);
        gtk_text_buffer_apply_tag(textbuffer, markup_tag, &iter, &enditer);
      }
    }
  } while (gtk_text_iter_forward_char (&iter));
  
  // Initiate spelling check.
  spelling_trigger ();
}


void USFMView::on_textview_move_cursor(GtkTextView * textview, GtkMovementStep step, gint count, gboolean extend_selection, gpointer user_data)
{
  ((USFMView *) user_data)->on_textview_cursor_moved();
}


void USFMView::on_textview_cursor_moved()
{
  restart_verse_tracker();
}


void USFMView::on_textview_grab_focus(GtkWidget * widget, gpointer user_data)
{
  ((USFMView *) user_data)->textview_grab_focus();
}


void USFMView::textview_grab_focus()
{
  restart_verse_tracker();
}


void USFMView::restart_verse_tracker()
{
  gw_destroy_source(verse_tracker_event_id);
  verse_tracker_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 100, GSourceFunc(on_verse_tracker_timeout), gpointer(this), NULL);
}


bool USFMView::on_verse_tracker_timeout(gpointer data)
{
  return ((USFMView *) data)->on_verse_tracker();
}


bool USFMView::on_verse_tracker()
{
  GtkTextIter startiter;
  gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER (sourcebuffer), &startiter, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER (sourcebuffer)));
  ustring verse = "0";
  GtkTextIter enditer = startiter;
  gtk_text_iter_forward_chars (&enditer, 3);
  gtk_text_iter_backward_chars (&startiter, 3);
  ustring text_around_cursor = gtk_text_iter_get_text (&startiter, &enditer);
  size_t pos = text_around_cursor.find ("\\v ");
  if (pos != string::npos) {
    gtk_text_iter_forward_chars (&startiter, 3);
  }
  gtk_text_iter_forward_chars (&startiter, 3);
  if (gtk_text_iter_backward_search (&startiter, "\\v ", GtkTextSearchFlags (0), &startiter, NULL, NULL)) {
    GtkTextIter enditer = startiter;
    gtk_text_iter_forward_chars (&enditer, 3);
    enditer = startiter;
    gtk_text_iter_forward_chars (&enditer, 10);
    ustring text_at_verse = gtk_text_iter_get_text (&startiter, &enditer);
    text_at_verse.erase (0, 2);
    text_at_verse = trim (text_at_verse);
    if (!number_in_string (text_at_verse.substr (0, 1)).empty()) {
      size_t pos = text_at_verse.find_first_of (" \n");
      if (pos != string::npos) {
        text_at_verse.erase (pos, 1000);
        verse = text_at_verse;
      }
    }
  }
  bool new_verse = (current_verse_number != verse);
  current_verse_number = verse;
  if (new_verse) {
    gtk_button_clicked (GTK_BUTTON (new_verse_signal));
  }
  return false;
}


void USFMView::position_cursor_at_verse(const ustring & verse)
// Moves the cursor to the verse
{
  // Bail out of the cursor is already on the verse.
  if (verse == current_verse_number)
    return;

  // Save the current verse.
  current_verse_number = verse;
  
  // Place the cursor at the verse. Verse 0 is at the start of the buffer.
  GtkTextIter startiter;
  gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (sourcebuffer), &startiter);
  ustring usfmcode = "\\v " + verse;
  gtk_text_iter_forward_search (&startiter, usfmcode.c_str(), GtkTextSearchFlags (0), &startiter, NULL, NULL);
  gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER (sourcebuffer), &startiter);
  while (gtk_events_pending()) gtk_main_iteration();
  textview_scroll_to_mark (GTK_TEXT_VIEW(sourceview), gtk_text_buffer_get_insert (GTK_TEXT_BUFFER (sourcebuffer)), true);
}


ustring USFMView::text_get_selection()
// Retrieves the selected text from the sourceview.
{
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER (sourcebuffer), &startiter, &enditer);
  gchar *text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER (sourcebuffer), &startiter, &enditer, true);
  ustring selectedtext (text);
  g_free(text);
  return selectedtext;
}


void USFMView::text_insert(ustring text)
// Inserts text at the cursor location.
// If text is selected, this is erased first.
{
  // If the text is not editable, bail out.
  if (!editable)
    return;

  // Erase selected text.
  gtk_text_buffer_delete_selection(GTK_TEXT_BUFFER (sourcebuffer), true, editable);
  
  // Insert the new text.
  gtk_text_buffer_insert_at_cursor (GTK_TEXT_BUFFER (sourcebuffer), text.c_str(), -1);
}


gboolean USFMView::on_textview_button_press_event(GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
  ((USFMView *) user_data)->on_texteditor_click(widget, event);
  return false;
}


void USFMView::on_texteditor_click(GtkWidget * widget, GdkEventButton * event)
{
  // Double-clicking sends the word to Toolbox.
  if (event->type == GDK_2BUTTON_PRESS) {

    // Get the word.
    GtkTextIter startiter;
    GtkTextIter enditer;
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER (sourcebuffer), &enditer, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER (sourcebuffer)));
    if (!gtk_text_iter_ends_word(&enditer))
      gtk_text_iter_forward_word_end(&enditer);
    startiter = enditer;
    gtk_text_iter_backward_word_start(&startiter);
    word_double_clicked_text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER (sourcebuffer), &startiter, &enditer, false);

    // Signal to have it sent to Toolbox.
    gtk_button_clicked(GTK_BUTTON(word_double_clicked_signal));
  }
}


void USFMView::insert_note(const ustring & marker, const ustring & rawtext)
/*
 Inserts a note in the editor.
 marker:    The marker that starts the note, e.g. "fe" for an endnote.
 rawtext:   The raw text of the note, e.g. "+ Mat 1.1.". This excludes the note opener and note closer.
 */
{
  ustring text = usfm_get_full_opening_marker (marker);
  text.append (rawtext);
  text.append (usfm_get_full_closing_marker (marker));
  gtk_text_buffer_insert_at_cursor (GTK_TEXT_BUFFER (sourcebuffer), text.c_str(), -1);  
}


void USFMView::cut ()
{
  GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
  gtk_text_buffer_cut_clipboard  (GTK_TEXT_BUFFER (sourcebuffer), clipboard, editable);
}


void USFMView::copy ()
{
  GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
  gtk_text_buffer_copy_clipboard(GTK_TEXT_BUFFER (sourcebuffer), clipboard);
 }


void USFMView::paste ()
{
  GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
  gtk_text_buffer_paste_clipboard (GTK_TEXT_BUFFER (sourcebuffer), clipboard, NULL, editable);
}


gboolean USFMView::on_textview_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
  return ((USFMView *) user_data)->textview_key_press_event(widget, event);
}


gboolean USFMView::textview_key_press_event(GtkWidget *widget, GdkEventKey *event)
{
  // A GtkTextView has standard keybindings for clipboard operations.
  // It has been found that if the user presses, e.g. Ctrl-c, that
  // text is copied to the clipboard twice, or e.g. Ctrl-v, that it is
  // pasted twice. This is probably a bug in Gtk2.
  // The relevant clipboard keys are blocked here.
  // The default bindings for copying to clipboard are Ctrl-c and Ctrl-Insert.
  // The default bindings for cutting to clipboard are Ctrl-x and Shift-Delete.
  // The default bindings for pasting from clipboard are Ctrl-v and Shift-Insert.
  if (keyboard_control_state(event)) {
    if (event->keyval == GDK_c) return true;
    if (event->keyval == GDK_x) return true;
    if (event->keyval == GDK_v) return true;
    if (keyboard_insert_pressed(event)) return true;
  }
  if (keyboard_shift_state(event)) {
    if (keyboard_delete_pressed(event)) return true;
    if (keyboard_insert_pressed(event)) return true;
  }
  // Propagate event further.
  return FALSE;
}


void USFMView::load_dictionaries()
{
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  if (projectconfig->spelling_check_get()) {
    spellingchecker->set_dictionaries(projectconfig->spelling_dictionaries_get());
  }
}


bool USFMView::move_cursor_to_spelling_error (bool next, bool extremity)
// Move the cursor to the next (or previous) spelling error.
// Returns true if it was found, else false.
{
  bool moved = spellingchecker->move_cursor_to_spelling_error (GTK_TEXT_BUFFER (sourcebuffer), next, extremity);
  if (moved) {
    textview_scroll_to_mark (GTK_TEXT_VIEW (sourceview), gtk_text_buffer_get_insert (GTK_TEXT_BUFFER (sourcebuffer)), true);
  }
  return moved;
}


void USFMView::spelling_trigger()
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


bool USFMView::on_spelling_timeout(gpointer data)
{
  ((USFMView *) data)->spelling_timeout();
  return false;
}


void USFMView::spelling_timeout()
{
  // Clear event id.
  spelling_timeout_event_id = 0;

  // Check spelling of textview.
  spellingchecker->check(GTK_TEXT_BUFFER (sourcebuffer));

  // Signal spelling checked.
  gtk_button_clicked (GTK_BUTTON (spelling_checked_signal));
}


void USFMView::on_button_spelling_recheck_clicked(GtkButton * button, gpointer user_data)
{
  ((USFMView *) user_data)->spelling_timeout();
}


vector <ustring> USFMView::spelling_get_misspelled ()
{
  // Collect the misspelled words.
  GtkTextBuffer * textbuffer = GTK_TEXT_BUFFER (sourcebuffer);
  vector <ustring> words = spellingchecker->get_misspellings(textbuffer);
  // Remove double ones.
  set <ustring> wordset (words.begin (), words.end());
  words.clear();
  words.assign (wordset.begin (), wordset.end());
  // Give result.
  return words;  
}


void USFMView::spelling_approve (const vector <ustring>& words)
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


