/* Copyright (©) 2003-2013 Teus Benschop.
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
#include "git.h"
#include "progresswindow.h"
#include "merge_utils.h"
#include <glib/gi18n.h>

/*


Text editor with a formatted view and undo and redo.
Care was taken not to embed child widgets in the GtkTextView,
because this could give crashes in Gtk under certain circumstances.


The following elements of text have their own textview and textbuffer:
- every paragraph
- every cell of a table.
- every note.

While the user edits text, signal handlers are connected so it can be tracked what is being edited.
The signal handlers call routines that make EditorActions out of this, and optionally correct the text being typed.
The EditorActions, when played back, will have the same effect as if the user typed.
If Undo is done, then the last EditorAction is undone. It can also be Redone.


The following things need to be tested after a change was made to the Editor object:
* USFM \id GEN needs to be displayed with the full \id line.
* USFM \p starts a new paragraph.
* USFM \v works fine for verse markup.
* USFM \add does the character style.
* If USFM text is pasted through the clipboard, this should load properly.
* If a new line is entered, even in the middle of existing text, the editor should display this.
* If a new line is entered in a character style, the existing character style should go to the next paragraph.
* If plain text with new lines is pasted through the clipboard, the editor should display the new lines
* When typing text where a character style starts, it should take this character style.
* When typing text right after where a character style ends, it should take this character style.
* 
*/


Editor2::Editor2(GtkWidget * vbox_in, const ustring & project_in):
current_reference(0, 1000, "")
{
  // Save and initialize variables.
  project = project_in;
  do_not_process_child_anchors_being_deleted = false;
  texttagtable = NULL;
  previous_hand_cursor = false;
  highlight = NULL;
  editable = false;
  event_id_show_quick_references = 0;
  book = 0;
  chapter = 0;
  signal_if_verse_changed_event_id = 0;
  verse_restarts_paragraph = false;
  focused_paragraph = NULL;
  disregard_text_buffer_signals = 0;
  textbuffer_delete_range_was_fired = false;
  verse_tracking_on = false;
  editor_actions_size_at_no_save = false;
  font_size_multiplier = 1;
  highlight_timeout_event_id = 0;
  scroll_insertion_point_on_screen_id = 0;
    
  // Create data that is needed for any of the possible formatted views.
  create_or_update_formatting_data();

  // Spelling checker.
  spellingchecker = new SpellingChecker(texttagtable);
  g_signal_connect((gpointer) spellingchecker->check_signal, "clicked", G_CALLBACK(on_button_spelling_recheck_clicked), gpointer(this));
  load_dictionaries();

  // The basic GUI, which actually is empty until text will be loaded in it.
  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow);
  gtk_box_pack_start(GTK_BOX(vbox_in), scrolledwindow, true, true, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  viewport = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);
  
  vbox_viewport = gtk_vbox_new (false, 0);
  gtk_widget_show(vbox_viewport);
  gtk_container_add (GTK_CONTAINER (viewport), vbox_viewport);

  // Box to store the standard paragraphs.
  vbox_paragraphs = gtk_vbox_new (false, 0);
  gtk_widget_show(vbox_paragraphs);
  gtk_box_pack_start(GTK_BOX(vbox_viewport), vbox_paragraphs, false, false, 0);

  last_focused_widget = vbox_paragraphs;

  // The separator between text and notes.
  hseparator = gtk_hseparator_new ();
  gtk_widget_show(hseparator);
  gtk_box_pack_start(GTK_BOX(vbox_viewport), hseparator, false, false, 0);

  // Box to store the notes.
  vbox_notes = gtk_vbox_new (false, 0);
  gtk_widget_show(vbox_notes);
  gtk_box_pack_start(GTK_BOX(vbox_viewport), vbox_notes, false, false, 0);
  
  // Create the invisible parking lot where GtkTextViews get parked while not in use.
  vbox_parking_lot = gtk_vbox_new (false, 0);
  gtk_box_pack_start(GTK_BOX(vbox_in), vbox_parking_lot, false, false, 0);

  // Buttons to give signals.
  new_verse_signal = gtk_button_new();
  new_styles_signal = gtk_button_new();
  word_double_clicked_signal = gtk_button_new();
  reload_signal = gtk_button_new();
  changed_signal = gtk_button_new();
  quick_references_button = gtk_button_new();
  spelling_checked_signal = gtk_button_new ();
  new_widget_signal = gtk_button_new ();

  // Initialize a couple of event ids.
  textview_move_cursor_id = 0;
  textview_grab_focus_event_id = 0;
  spelling_timeout_event_id = 0;
  textview_button_press_event_id = 0;

  go_to_new_reference_highlight = false; // 3/22/2016 MAP

  // Tag for highlighting search words and current verse.
  // For convenience the GtkTextBuffer function is called. 
  // This adds the tag to the GtkTextTagTable, making it available
  // to any other text buffer that uses the same text tag table.
  {
    GtkTextBuffer * textbuffer = gtk_text_buffer_new (texttagtable);
    reference_tag = gtk_text_buffer_create_tag(textbuffer, NULL, "background", "khaki", NULL);
    verse_highlight_tag = gtk_text_buffer_create_tag(textbuffer, NULL, "background", "yellow", NULL);
    g_object_unref (textbuffer);
  }

  // Automatic saving of the file, periodically.
  save_timeout_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 60000, GSourceFunc(on_save_timeout), gpointer(this), NULL);
}


Editor2::~Editor2()
{
  // Verse tracking off.
  switch_verse_tracking_off ();

  // Save the chapter.
  chapter_save();

  // Destroy a couple of timeout sources.
  gw_destroy_source(textview_move_cursor_id);
  gw_destroy_source(textview_grab_focus_event_id);
  gw_destroy_source(save_timeout_event_id);
  gw_destroy_source(highlight_timeout_event_id);
  gw_destroy_source(spelling_timeout_event_id);
  gw_destroy_source(event_id_show_quick_references);
  gw_destroy_source(signal_if_verse_changed_event_id);
  gw_destroy_source(textview_button_press_event_id);
  gw_destroy_source(scroll_insertion_point_on_screen_id);

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
  gtk_widget_destroy (new_widget_signal);

  // Destroy the texttag tables.
  g_object_unref(texttagtable);

  // Destroy possible highlight object.
  if (highlight)
    delete highlight;
    
  // Destroy the editor actions.
  // This will also destroy any GtkTextViews these actions created.
  clear_and_destroy_editor_actions (actions_done);
  clear_and_destroy_editor_actions (actions_undone);

  // Destroy remainder of text area.
  gtk_widget_destroy(scrolledwindow);
}


void Editor2::book_set(unsigned int book_in)
{
  book = book_in;
}


void Editor2::chapter_load(unsigned int chapter_in) 
// Loads a chapter with the number "chapter_in".
{
  // Clear the stacks of actions done and redoable.
  clear_and_destroy_editor_actions (actions_done);
  clear_and_destroy_editor_actions (actions_undone);

  // Switch verse tracking off.
  switch_verse_tracking_off ();

  // Save chapter number.
  chapter = chapter_in;

  // Settings.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);

  // Reset note style generator.
  {
    ustring dummy;
    get_next_note_caller_and_style (entFootnote, dummy, dummy, true);
  }
  
  // Load text in memory and cache it for later use.
  loaded_chapter_lines = project_retrieve_chapter(project, book, chapter);

  // Whether chapter is editable.
  editable = true;
  if (loaded_chapter_lines.empty())
    editable = false;
  if (!projectconfig->editable_get())
    editable = false;

  // Get rid of possible previous widgets with their data.
  gtk_container_foreach(GTK_CONTAINER(vbox_paragraphs), on_container_tree_callback_destroy, gpointer(this));
  focused_paragraph = NULL;

  // Make one long line containing the whole chapter.
  // This is done so as to exclude any possibility that the editor does not
  // properly load a possible chapter that has line-breaks at unexpected places.
  // Add one space to the end so as to accomodate situation such as that the 
  // last marker is "\toc". Without that extra space it won't see this marker,
  // because the formatter looks for "\toc ". The space will solve the issue.
  ustring line;
  for (unsigned int i = 0; i < loaded_chapter_lines.size(); i++) {
    if (!line.empty())
      line.append(" ");
    line.append(loaded_chapter_lines[i]);
  }
  line.append(" ");

  // Load in editor.
  text_load (line, "", false);

  // Clean up extra spaces before the insertion points in all the newly created textbuffers.
  for (unsigned int i = 0; i < actions_done.size(); i++) {
    EditorAction * action = actions_done[i];
    if ((action->type == eatCreateParagraph) || (action->type == eatCreateNoteParagraph)) {
      EditorActionCreateParagraph * paragraph = static_cast <EditorActionCreateParagraph *> (action);
      EditorActionDeleteText * trim_action = paragraph_delete_last_character_if_space (paragraph);
      if (trim_action) {
        apply_editor_action (trim_action);
      }
    }
  }

  // Insert the chapter load boundary.
  apply_editor_action (new EditorAction (eatLoadChapterBoundary));

  // Place cursor at the start and scroll it onto the screen.
  vector <GtkWidget *> textviews = editor_get_widgets (vbox_paragraphs);
  if (textviews.empty()) {
    give_focus (textviews[0]);
    GtkTextIter iter;
    gtk_text_buffer_get_start_iter(focused_paragraph->textbuffer, &iter);
    gtk_text_buffer_place_cursor(focused_paragraph->textbuffer, &iter);
    //scroll_insertion_point_on_screen ();
    // Remove all the complicated timeout stuff and go right to work
    scroll_insertion_point_on_screen_timeout ();
  }
  
  // Store size of actions buffer so we know whether the chapter changed.
  editor_actions_size_at_no_save = actions_done.size();
}


void Editor2::text_load (ustring text, ustring character_style, bool note_mode)
// This loads text into the formatted editor.
// text: text to load.
// character_style: character style to start with.
// note_mode: If true, it is limited to formatting notes only.
{
  // Clean away possible new lines.
  replace_text (text, "\n", " ");

  // Load the text into the editor by creating and applying editor actions.
  ustring marker_text;
  size_t marker_pos;
  size_t marker_length;
  bool is_opener;
  bool marker_found;
  while (!text.empty()) {
    marker_found = usfm_search_marker(text, marker_text, marker_pos, marker_length, is_opener);
    bool handled = false;
    if (!handled && !note_mode) {
      if (text_starts_paragraph (project, text, marker_text, marker_pos, marker_length, is_opener, marker_found)) {
        character_style.clear();
        editor_start_new_standard_paragraph (marker_text);
        handled = true;
      }
    }
    if (!handled && !note_mode) {
      if (text_starts_verse (project, text, marker_text, marker_pos, marker_length, is_opener, marker_found)) {
        editor_start_verse (text, marker_text, character_style);
        handled = true;
      }
    }
    if (!handled) {
      if (editor_starts_character_style (text, character_style, marker_text, marker_pos, marker_length, is_opener, marker_found)) {
        handled = true;
      }
    }
    if (!handled) {
      if (editor_ends_character_style (text, character_style, marker_text, marker_pos, marker_length, is_opener, marker_found)) {
        handled = true;
      }
    }
    if (!handled && !note_mode) {
      ustring raw_note;
      if (text_starts_note_raw (text, character_style, marker_text, marker_pos, marker_length, is_opener, marker_found, raw_note)) {
        editor_start_note_raw (raw_note, marker_text);
        handled = true;
      }
    }
    if (!handled) {
      editor_text_fallback (text, character_style, marker_pos, marker_found);
    }
  }

  // Update gui.
  if (!note_mode) {
    signal_if_styles_changed();
    signal_if_verse_changed();
  }

  // Trigger a spelling check.
  if (!note_mode) {
    spelling_trigger();
  }
}


void Editor2::chapter_save()
// Handles saving the chapter.
{
  // Set variables.
  reload_chapter_number = chapter;

  // If the text is not editable, bail out.
  if (!editable)
    return;

  // If the text was not changed, bail out.
  if (editor_actions_size_at_no_save == actions_done.size())
    return;
    
  // If the project is empty, bail out.
  if (project.empty())
    return;

  // Get the USFM text.
  ustring chaptertext = get_chapter();

  // Flags for use below.
  bool reload = false;
  bool save_action_is_over = false;
  bool check_chapter_cache = true;

  // If the chapter text is completely empty, 
  // that means that the user has deleted everything.
  // This is interpreted as to mean that the user wishes to delete this chapter.
  // After asking for confirmation, delete the chapter.
  if (chaptertext.empty()) {
    if (gtkw_dialog_question(NULL, _("The chapter is empty.\nDo you wish to delete this chapter?"), GTK_RESPONSE_YES) == GTK_RESPONSE_YES) {
      project_remove_chapter(project, book, chapter);
      save_action_is_over = true;
      reload = true;
      if (chapter > 0) reload_chapter_number = chapter - 1;
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
      message = _("Chapter ") + convert_to_string(chapter) + _(" was loaded, but it appears that the chapter number has been changed to ") + convert_to_string(chapter_in_text) + _(".\nDo you wish to save the text as a different chapter, that is, as chapter ") + convert_to_string(chapter_in_text) + "?";
      if (gtkw_dialog_question(NULL, message.c_str()) == GTK_RESPONSE_YES) {
        confirmed_chapter_number = chapter_in_text;
        reload = true;
        reload_chapter_number = chapter_in_text;
        check_chapter_cache = false;
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
          message = _("The new chapter number already exists\nDo you wish to overwrite it?");
          if (gtkw_dialog_question(NULL, message.c_str()) == GTK_RESPONSE_NO) {
            gtkw_dialog_info(NULL, _("The changes have been discarded"));
            save_action_is_over = true;
            reload = true;
          }
        }
      }
    }

    // Check the chapter cache with the version on disk. 
    // The chapter cache contains the chapter data when it was loaded in the editor.
    // Normally cache and disk are the same.
    // In case of collaboration, the text on disk may differ from the text in the chapter cache.
    if ((!save_action_is_over) && check_chapter_cache) {
      // The code is disabled, because it at times reverts changes entered in the editor.
      //vector <ustring> file_data = project_retrieve_chapter (project, book, chapter);
      //if (loaded_chapter_lines != file_data) {
        //merge_editor_and_file (loaded_chapter_lines, parseline.lines, project, book, chapter);
        //save_action_is_over = true;
        //reload = true;
      //}
    }

    // Store chapter.
    if (!save_action_is_over) {
      project_store_chapter(project, book, ccv);
      save_action_is_over = true;
    }
  }

  // Store size of editor actions. Based on this it knows next time whether to save the chapter.
  editor_actions_size_at_no_save = actions_done.size();

  // Possible reload signal.
  if (reload) {
    gtk_button_clicked(GTK_BUTTON(reload_signal));
  }
}


ustring Editor2::text_get_selection()
// Retrieves the selected text from the textview that has the focus, 
// and returns it as USFM code.
{
  ustring text;
  if (focused_paragraph) {
    GtkTextIter startiter, enditer;
    gtk_text_buffer_get_selection_bounds(focused_paragraph->textbuffer, &startiter, &enditer);
    text = usfm_get_text(focused_paragraph->textbuffer, startiter, enditer);
  }
  return text;
}


void Editor2::text_insert(ustring text)
// This inserts plain or USFM text at the cursor location of the focused textview.
// If text is selected, this is erased first.
{
  // If the text is not editable, bail out.
  if (!editable)
    return;
  // If no paragraph is in focus, bail out.
  if (!focused_paragraph)
    return;
  // Buffer.    
  GtkTextBuffer * textbuffer = focused_paragraph->textbuffer;
  // Erase selected text.
  gtk_text_buffer_delete_selection(textbuffer, true, editable);
  // Insert the text at the cursor.
  gtk_text_buffer_insert_at_cursor (textbuffer, text.c_str(), -1);
}


void Editor2::show_quick_references()
// Starts the process to show the quick references.
// A delaying routine is used to make the program more responsive.
// That is, the quick references are not shown at each change,
// but only shortly after. 
// Without this pasting a long text in the footnote takes a lot of time.
{
  gw_destroy_source(event_id_show_quick_references);
  event_id_show_quick_references = g_timeout_add_full(G_PRIORITY_DEFAULT, 200, GSourceFunc(show_quick_references_timeout), gpointer(this), NULL);
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

  // If we're not in a paragraph, bail out.
  if (!focused_paragraph)
    return;

  // Get the text of the focused note.
  GtkTextBuffer *note_buffer = focused_paragraph->textbuffer;
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

  // Make the references available and fire a signal.
  quick_references = refscanner.references;
  gtk_button_clicked(GTK_BUTTON(quick_references_button));
}


void Editor2::on_textview_move_cursor(GtkTextView * textview, GtkMovementStep step, gint count, gboolean extend_selection, gpointer user_data)
{
  ((Editor2 *) user_data)->textview_move_cursor(textview, step, count);
}


void Editor2::textview_move_cursor(GtkTextView * textview, GtkMovementStep step, gint count)
{
  // Clear the character style that was going to be applied when the user starts typing.
  character_style_on_start_typing.clear();
  // Keep postponing the actual handler if a new cursor movement was detected before the previous one was processed.
  gw_destroy_source(textview_move_cursor_id);
  textview_move_cursor_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 100, GSourceFunc(on_textview_move_cursor_delayed), gpointer(this), NULL);
  // Act on paragraph crossing.
  paragraph_crossing_act (step, count);
}


bool Editor2::on_textview_move_cursor_delayed(gpointer user_data)
{
  ((Editor2 *) user_data)->textview_move_cursor_delayed();
  return false;
}


void Editor2::textview_move_cursor_delayed()
// Handle cursor movement.
{
  textview_move_cursor_id = 0;
  signal_if_styles_changed();
  signal_if_verse_changed();
}


ustring Editor2::verse_number_get()
// Returns the verse number of the insertion point.
{
  // Default verse number.
  ustring number = "0";
  // Proceed if there's a focused paragraph.
  if (focused_paragraph) {
    // Get an iterator at the cursor location of the focused textview.
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(focused_paragraph->textbuffer, &iter, gtk_text_buffer_get_insert(focused_paragraph->textbuffer));
    // Get verse number.
    number = get_verse_number_at_iterator(iter, style_get_verse_marker(project), project, vbox_paragraphs);
  }
  return number;
}


void Editor2::on_textview_grab_focus(GtkWidget * widget, gpointer user_data)
{
  ((Editor2 *) user_data)->textview_grab_focus(widget);
}


void Editor2::textview_grab_focus(GtkWidget * widget)
{
  // Store the paragraph action that created the widget
  focused_paragraph = widget2paragraph_action (widget);
  // Clear the character style that was going to be applied when the user starts typing.
  character_style_on_start_typing.clear();
  // Further processing of the focus grab is done with a delay.
  gw_destroy_source(textview_grab_focus_event_id);
  textview_grab_focus_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 10, GSourceFunc(on_textview_grab_focus_delayed), gpointer(this), NULL);
}


bool Editor2::on_textview_grab_focus_delayed(gpointer data)
{
  ((Editor2 *) data)->textview_grab_focus_delayed();
  return false;
}


void Editor2::textview_grab_focus_delayed() // Todo
/*
 If the user clicks in the editor window, 
 and straight after that the position of the cursor is requested, 
 we get the position where the cursor was prior to clicking. 
 This delayed handler solves that.
 */
{
  textview_grab_focus_event_id = 0;
  signal_if_styles_changed();
  signal_if_verse_changed();
  show_quick_references();
}


void Editor2::undo()
{
  // If edits were made, the last action on the stack is the OneActionBoundary.
  // Undo the actions on the stack, and stop at the second OneActionBoundary.
  unsigned int one_action_boundaries_encountered = 0;
  while (can_undo() && (one_action_boundaries_encountered <= 1)) {
    EditorAction * action = actions_done[actions_done.size() - 1];
    if (action->type == eatOneActionBoundary) {
      one_action_boundaries_encountered++;
    }
    if (one_action_boundaries_encountered <= 1) {
      apply_editor_action (action, eaaUndo);
    }
  }
}


void Editor2::redo()
{
  // Redo the actions on the stack till we encounter the OneActionBoundary.
  unsigned int one_action_boundaries_encountered = 0;
  while (can_redo() && (one_action_boundaries_encountered == 0)) {
    EditorAction * action = actions_undone [actions_undone.size() - 1];
    if (action->type == eatOneActionBoundary) {
      one_action_boundaries_encountered++;
    }
    apply_editor_action (action, eaaRedo);
  }
}


bool Editor2::can_undo()
{
  bool undoable_actions_available = false;
  if (!actions_done.empty()) {
    EditorAction * last_action = actions_done[actions_done.size() - 1];
    undoable_actions_available = (last_action->type != eatLoadChapterBoundary);
  }
  return undoable_actions_available;
}


bool Editor2::can_redo()
{
  return !actions_undone.empty();
}


void Editor2::set_font()
{
  vector <GtkWidget *> textviews = editor_get_widgets (vbox_paragraphs);
  for (unsigned int i = 0; i < textviews.size(); i++) {
    set_font_textview (textviews[i]);
  }
}


void Editor2::set_font_textview (GtkWidget * textview)
{
  // Set font.
  PangoFontDescription *font_desc = NULL;
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  if (!projectconfig->editor_font_default_get()) {
    font_desc = pango_font_description_from_string(projectconfig->editor_font_name_get().c_str());
  }
  gtk_widget_modify_font(textview, font_desc);
  if (font_desc)
    pango_font_description_free(font_desc);

  // Set the colours.
  if (projectconfig->editor_default_color_get()) {
    color_widget_default(textview);
  } else {
    color_widget_set(textview, projectconfig->editor_normal_text_color_get(), projectconfig->editor_background_color_get(), projectconfig->editor_selected_text_color_get(), projectconfig->editor_selection_color_get());
  }

  // Set predominant text direction.
  if (projectconfig->right_to_left_get()) {
    gtk_widget_set_direction(textview, GTK_TEXT_DIR_RTL);
  } else {
    gtk_widget_set_direction(textview, GTK_TEXT_DIR_LTR);
  }
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


gboolean Editor2::on_motion_notify_event(GtkWidget * textview, GdkEventMotion * event, gpointer user_data)
{
  return ((Editor2 *) user_data)->motion_notify_event(textview, event);
}


gboolean Editor2::motion_notify_event(GtkWidget * textview, GdkEventMotion * event)
// Update the cursor image if the pointer moved. 
{
  gint x, y;
  gtk_text_view_window_to_buffer_coords(GTK_TEXT_VIEW(textview), GTK_TEXT_WINDOW_WIDGET, gint(event->x), gint(event->y), &x, &y);
  GtkTextIter iter;
  gtk_text_view_get_iter_at_location(GTK_TEXT_VIEW(textview), &iter, x, y);
  ustring paragraph_style, character_style;
  get_styles_at_iterator(iter, paragraph_style, character_style);
  bool hand_cursor = character_style.find (note_starting_style ()) != string::npos;
  if (hand_cursor != previous_hand_cursor) {
    GdkCursor *cursor;
    if (hand_cursor) {
      cursor = gdk_cursor_new(GDK_HAND2);
    } else {
      cursor = gdk_cursor_new(GDK_XTERM);
    }
    gdk_window_set_cursor(gtk_text_view_get_window(GTK_TEXT_VIEW(textview), GTK_TEXT_WINDOW_TEXT), cursor);
    gdk_cursor_unref (cursor);
  }
  previous_hand_cursor = hand_cursor;
  gdk_window_get_pointer (gtk_widget_get_window (textview), NULL, NULL, NULL);
  return false;
}


gboolean Editor2::on_textview_button_press_event(GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
  return ((Editor2 *) user_data)->textview_button_press_event(widget, event);
}


gboolean Editor2::textview_button_press_event(GtkWidget * widget, GdkEventButton * event)
{
  // See whether the user clicked on a note caller.
  if (event->type == GDK_BUTTON_PRESS) {
    // Get iterator at clicking location.
    GtkTextIter iter;
    gint x, y;
    gtk_text_view_window_to_buffer_coords(GTK_TEXT_VIEW(widget), GTK_TEXT_WINDOW_WIDGET, gint(event->x), gint(event->y), &x, &y);
    gtk_text_view_get_iter_at_location(GTK_TEXT_VIEW(widget), &iter, x, y);
    // Check whether this is a note caller.
    ustring paragraph_style, character_style;
    get_styles_at_iterator(iter, paragraph_style, character_style);
    if (character_style.find (note_starting_style ()) != string::npos) {
      // Focus the note paragraph that has this identifier.
      EditorActionCreateNoteParagraph * note_paragraph = note2paragraph_action (character_style);
      if (note_paragraph) {
        give_focus (note_paragraph->textview);
      }
      // Do not propagate the button press event.
      return true;
    }
  }

  // Process this event with a delay.
  gw_destroy_source(textview_button_press_event_id);
  textview_button_press_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 100, GSourceFunc(on_textview_button_press_delayed), gpointer(this), NULL);

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
    // Do not include note markers.
    GtkTextIter moved_enditer;
    if (move_end_iterator_before_note_caller_and_validate (startiter, enditer, moved_enditer)) {
      word_double_clicked_text = gtk_text_buffer_get_text(textbuffer, &startiter, &moved_enditer, false);
  
      // Signal to have it sent to Toolbox.
      gtk_button_clicked(GTK_BUTTON(word_double_clicked_signal));
    }
  }

  // Propagate the event.
  return false;
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


void Editor2::on_buffer_insert_text_after(GtkTextBuffer * textbuffer, GtkTextIter * pos_iter, gchar * text, gint length, gpointer user_data)
{
  // The "length" parameter does not give the length as the number of characters 
  // but the byte length of the "text" parameter. It is not passed on.
  ((Editor2 *) user_data)->buffer_insert_text_after(textbuffer, pos_iter, text);
}


void Editor2::buffer_insert_text_after(GtkTextBuffer * textbuffer, GtkTextIter * pos_iter, gchar * text)
// This function is called after the default handler has inserted the text.
// At this stage "pos_iter" points to the end of the inserted text.
{
  // If text buffers signals are to be disregarded, bail out.
  if (disregard_text_buffer_signals) {
    return;
  }
  
  // Text to work with.
  ustring utext (text);
  
  // New lines in notes are not supported.
  if (focused_paragraph->type == eatCreateNoteParagraph) {
    replace_text (utext, "\n", " ");
  }

  // Get offset of text insertion point.
  gint text_insertion_offset = gtk_text_iter_get_offset (pos_iter) - utext.length();

  // Variable for the character style that the routines below indicate should be applied to the inserted text.
  ustring character_style_to_be_applied;

  /*
  If text is inserted right before where a character style was in effect,
  the GtkTextBuffer does not apply any style to that text.
  The user expects that the character style that applies to the insertion point
  will be applied to the new text as well. 
  Do not extend a note style.
  */
  if (character_style_to_be_applied.empty()) {
    ustring paragraph_style;
    GtkTextIter iter = *pos_iter;
    get_styles_at_iterator(iter, paragraph_style, character_style_to_be_applied);
    if (character_style_to_be_applied.find (note_starting_style ()) == 0) {
      character_style_to_be_applied.clear();
    }
  }

  /*
  If text is inserted right after where a character style is in effect,
  the user expects this character style to be used for the inserted text as well.
  This does not happen automatically in the GtkTextBuffer.
  The code below cares for it.
  Do not extend a note style.
  */
  if (character_style_to_be_applied.empty()) {
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_offset (textbuffer, &iter, text_insertion_offset);
    if (gtk_text_iter_backward_char (&iter)) {
      ustring paragraph_style;
      get_styles_at_iterator(iter, paragraph_style, character_style_to_be_applied);
    }
    if (character_style_to_be_applied.find (note_starting_style ()) == 0) {
      character_style_to_be_applied.clear();
    }
  }

  /*
  When a character style has been applied, and then the user starts typing,
  he expects that this style is going to be applied to the text he types.
  The code below cares for that.
  */
  if (character_style_to_be_applied.empty()) {
    character_style_to_be_applied = character_style_on_start_typing;
    character_style_on_start_typing.clear();
  }

  // Intelligent verse handling.
  if (!character_style_to_be_applied.empty()) {
    ustring verse_style = style_get_verse_marker(project);
    if (character_style_to_be_applied == verse_style) {
      gunichar character = g_utf8_get_char(text);
      // When the cursor is at a verse, and the user types a space,
      // he wishes to stop the verse and start normal text.
      if (g_unichar_isspace(character)) {
        character_style_to_be_applied.clear();
      }
      // When the cursor is after a verse, and the user types anything
      // but a numeral, a hyphen, or a comma, it means he wishes to stop the verse.
      if (!g_unichar_isdigit(character) && (character != '-') && (character != ',')) {
        character_style_to_be_applied.clear();
      }
    }
  }

  // Remove the text that was inserted into the textbuffer.
  // Then, it needs to be inserted through Editor Actions.
  // This is for the Undo and Redo system.
  disregard_text_buffer_signals++;
  GtkTextIter startiter = *pos_iter;
  gtk_text_iter_backward_chars (&startiter, utext.length());
  gtk_text_buffer_delete (textbuffer, &startiter, pos_iter);
  disregard_text_buffer_signals--;

  // If there are one or more backslashes in the text, then USFM code is being entered.
  // Else treat it as if the user is typing text.
  if (utext.find ("\\") != string::npos) {
    // Load USFM code.
    text_load (text, character_style_to_be_applied, false);
  } else {
    // Load plain text. Handle new lines as well.
    size_t newlineposition = utext.find("\n");
    while (newlineposition != string::npos) {
      ustring line = utext.substr(0, newlineposition);
      if (!line.empty()) {
        text_load (line, character_style_to_be_applied, false);
        character_style_to_be_applied.clear();
      }
      // Get markup after insertion point. New paragraph.
      ustring paragraph_style = unknown_style ();
      vector <ustring> text;
      vector <ustring> styles;        
      if (focused_paragraph) {
        paragraph_style = focused_paragraph->style;
        EditorActionDeleteText * delete_action = paragraph_get_text_and_styles_after_insertion_point(focused_paragraph, text, styles);
        if (delete_action) {
          apply_editor_action (delete_action);
        }
      }      
      editor_start_new_standard_paragraph (paragraph_style);
      // Transfer anything from the previous paragraph to the new one.
      gint initial_offset = editor_paragraph_insertion_point_get_offset (focused_paragraph);
      gint accumulated_offset = initial_offset;
      for (unsigned int i = 0; i < text.size(); i++) {
        EditorActionInsertText * insert_action = new EditorActionInsertText (focused_paragraph, accumulated_offset, text[i]);
        apply_editor_action (insert_action);
        if (!styles[i].empty()) {
          EditorActionChangeCharacterStyle * style_action = new EditorActionChangeCharacterStyle(focused_paragraph, styles[i], accumulated_offset, text[i].length());
          apply_editor_action (style_action);
        }
        accumulated_offset += text[i].length();
      }
      // Move insertion points to the proper position.
      editor_paragraph_insertion_point_set_offset (focused_paragraph, initial_offset);
      // Remove the part of the input text that has been handled.
      utext.erase(0, newlineposition + 1);
      newlineposition = utext.find("\n");
    }
    if (!utext.empty()) {
      text_load (utext, character_style_to_be_applied, false);
      character_style_to_be_applied.clear();
    }
  }
  
  // Insert the One Action boundary.
  apply_editor_action (new EditorAction (eatOneActionBoundary));

  // The pos_iter variable that was passed to this function was invalidated because text was removed and added.
  // Here it is validated again. This prevents critical errors within Gtk.
  gtk_text_buffer_get_iter_at_offset (textbuffer, pos_iter, text_insertion_offset);
}


void Editor2::on_buffer_delete_range_before(GtkTextBuffer * textbuffer, GtkTextIter * start, GtkTextIter * end, gpointer user_data)
{
  ((Editor2 *) user_data)->buffer_delete_range_before(textbuffer, start, end);
}


void Editor2::buffer_delete_range_before(GtkTextBuffer * textbuffer, GtkTextIter * start, GtkTextIter * end)
{
  // Bail out if we don't care about textbuffer signals.
  if (disregard_text_buffer_signals) {
    return;
  }
  disregard_text_buffer_signals++;

  textbuffer_delete_range_was_fired = true;

  // Record the content that is about to be deleted.
  get_text_and_styles_between_iterators(start, end, text_to_be_deleted, styles_to_be_deleted);

  // Make the end iterator the same as the start iterator, so that nothing gets deleted.
  // It will get deleted through EditorActions, so that Undo and Redo work.
  * end = * start;

  // Care about textbuffer signals again.
  disregard_text_buffer_signals--;
}


void Editor2::on_buffer_delete_range_after(GtkTextBuffer * textbuffer, GtkTextIter * start, GtkTextIter * end, gpointer user_data)
{
  ((Editor2 *) user_data)->buffer_delete_range_after(textbuffer, start, end);
}


void Editor2::buffer_delete_range_after(GtkTextBuffer * textbuffer, GtkTextIter * start, GtkTextIter * end)
{
  // Bail out if we don't care about textbuffer signals.
  if (disregard_text_buffer_signals) {
    return;
  }
  disregard_text_buffer_signals++;

  // Delete the text.  
  if (focused_paragraph) {
    ustring text;
    for (unsigned int i = 0; i < text_to_be_deleted.size(); i++) {
      text.append (text_to_be_deleted[i]);
    }
    gint offset = gtk_text_iter_get_offset (start);
    EditorActionDeleteText * delete_action = new EditorActionDeleteText(focused_paragraph, offset, text.length());
    apply_editor_action (delete_action);
  }
  
  // If there are any notes among the deleted text, delete these notes as well.
  for (unsigned int i = 0; i < styles_to_be_deleted.size(); i++) {
    if (styles_to_be_deleted[i].find (note_starting_style ()) == 0) {
      EditorActionCreateNoteParagraph * paragraph_action = note2paragraph_action (styles_to_be_deleted[i]);
      if (paragraph_action) {
        GtkTextIter iter;
        gtk_text_buffer_get_end_iter (paragraph_action->textbuffer, &iter);
        gint length = gtk_text_iter_get_offset (&iter);
        apply_editor_action (new EditorActionDeleteText (paragraph_action, 0, length));
        apply_editor_action (new EditorActionDeleteParagraph(paragraph_action));
      }
    }
  }
  
  // Clear data that was used to find out what to delete.
  text_to_be_deleted.clear();
  styles_to_be_deleted.clear();

  // Insert the One Action boundary.
  apply_editor_action (new EditorAction (eatOneActionBoundary));

  // Care about textbuffer signals again.
  disregard_text_buffer_signals--;
}


void Editor2::signal_if_styles_changed() // Todo
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
  // The styles.
  set <ustring> styles;

  // Carry on if there's a focused paragraph.
  if (focused_paragraph) {
    
    GtkTextBuffer * textbuffer = focused_paragraph->textbuffer;

    // Get the iterators at the selection bounds of the focused textview.
    GtkTextIter startiter, enditer;
    gtk_text_buffer_get_selection_bounds(textbuffer, &startiter, &enditer);

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

  }

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
  if (focused_paragraph) {
    return focused_paragraph->textbuffer;
  }
  return NULL;
}


EditorTextViewType Editor2::last_focused_type()
// Returns the type of the textview that was focused most recently.
// This could be the main body of text, or a note, or a table.
{ 
  if (focused_paragraph) {
    if (focused_paragraph->type == eatCreateNoteParagraph) {
      return etvtNote;
    }
  }
  // return etvtTable;
  return etvtBody;
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
      style_application_fault_reason = _("Trying to apply a ");
      if (type == stFootEndNote)
        style_application_fault_reason.append(_("foot- or endnote"));
      if (type == stCrossreference)
        style_application_fault_reason.append(_("crossreference"));
      if (type == stTableElement)
        style_application_fault_reason.append(_("table"));
      style_application_fault_reason.append(_(" style"));
      break;
    }
    case etvtNote:
    {
      if ((type != stFootEndNote) && (type != stCrossreference)) {
        style_application_ok = false;
      }
      style_application_fault_reason = _("Only note related styles apply");
      break;
    }
    case etvtTable:
    {
      // Check that only a table element is going to be inserted.
      if (type != stTableElement) {
        style_application_ok = false;
        style_application_fault_reason = _("Only table styles apply");
        break;
      }
      // Check that only a style with the right column number is going to be applied.
      ustring stylesheet = stylesheet_get_actual();
      extern Styles *styles;
      Usfm *usfm = styles->usfm(stylesheet);
      for (unsigned int i = 0; i < usfm->styles.size(); i++) {
        if (marker == usfm->styles[i].marker) {
          /*
          unsigned int column = usfm->styles[i].userint1;
          if (column > 5) {
            style_application_ok = false;
            style_application_fault_reason = _("Table column number mismatch");
            break;
          }
          */
        }
      }
      break;
    }
  }
  // Whether there's a paragraph to apply the style to.
  if (!focused_paragraph) {
    style_application_ok = false;
    style_application_fault_reason = _("Cannot find paragraph");
  }
  // If necessary give error message.
  if (!style_application_ok) {
    ustring message(_("This style cannot be applied here."));
    message.append("\n\n");
    message.append(style_application_fault_reason);
    message.append(".");
    gtkw_dialog_error(NULL, message.c_str());
    return;
  }

  // Get the active textbuffer and textview.
  GtkTextBuffer *textbuffer = focused_paragraph->textbuffer;
  GtkWidget *textview = focused_paragraph->textview;

  if (style_get_starts_new_line_in_editor(type, subtype)) {
    // Handle a paragraph style.
    apply_editor_action (new EditorActionChangeParagraphStyle (marker, focused_paragraph));
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
    // Check whether the character style that we are going to 
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
      gtk_text_iter_forward_char(&iter);
    } while (gtk_text_iter_in_range(&iter, &startiter, &enditer));
    // If the character style was applied already, toggle it.
    ustring style (marker);
    if (character_style_already_applied) {
      style.clear();
    }
    // Apply the new character style
    gint startoffset = gtk_text_iter_get_offset (&startiter);
    gint endoffset = gtk_text_iter_get_offset (&enditer);
    EditorActionChangeCharacterStyle * style_action = new EditorActionChangeCharacterStyle(focused_paragraph, style, startoffset, endoffset - startoffset);
    apply_editor_action (style_action);
    // Store this character style so it can be re-used when the user starts typing text.
    character_style_on_start_typing = style;
  }
  
  // One Action boundary.
  apply_editor_action (new EditorAction (eatOneActionBoundary));

  // Update gui.
  signal_if_styles_changed();

  // Focus editor.
  give_focus(textview);
}


void Editor2::insert_note(const ustring & marker, const ustring & rawtext)
/*
 Inserts a note in the editor.
 marker:    The marker that starts the note, e.g. "fe" for an endnote.
 rawtext:   The raw text of the note, e.g. "+ Mat 1.1.". Note that this excludes
 the note opener and note closer. It has only the caller and the
 USFM code of the note body.
 */
{
  ustring usfmcode;
  usfmcode.append (usfm_get_full_opening_marker (marker));
  usfmcode.append (rawtext);
  usfmcode.append (usfm_get_full_closing_marker (marker));
  if (focused_paragraph) {
    gtk_text_buffer_insert_at_cursor (focused_paragraph->textbuffer, usfmcode.c_str(), -1);
  }
}


void Editor2::insert_table(const ustring & rawtext)
// Inserts a table in the editor.
// rawtext: The raw text of the table, e.g. "\tr \tc1 \tc2 \tc3 \tc4 ".
{
  if (focused_paragraph) {
    gtk_text_buffer_insert_at_cursor (focused_paragraph->textbuffer, rawtext.c_str(), -1);
  }
}


void Editor2::on_textbuffer_changed(GtkTextBuffer * textbuffer, gpointer user_data)
{
  ((Editor2 *) user_data)->textbuffer_changed(textbuffer);
}


void Editor2::textbuffer_changed(GtkTextBuffer * textbuffer)
{
  spelling_trigger();
}


void Editor2::highlight_searchwords()
// Highlights all the search words.
{
  // Destroy optional previous object.
  if (highlight)
    delete highlight;

  // Bail out if there's no focused paragraph.
  if (!focused_paragraph) {
    return;
  }

  // This code is why the highlighting is slow. It does all the
  // operations essentially backward, waiting on a new thread to get
  // started and to its work before doing anything else. Thread
  // creation is a fairly very high overhead operation. As a result,
  // this method is guaranteed to introduce more delay in the process
  // of figuring out and marking the highlighted text than it would to
  // just do it. See below.
/*
  // Highlighting timeout.
  if (highlight_timeout_event_id) {
    gw_destroy_source (highlight_timeout_event_id);
  }
  highlight_timeout_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 500, GSourceFunc(on_highlight_timeout), gpointer(this), NULL);

  // Create a new highlighting object.
  highlight = new Highlight(focused_paragraph->textbuffer, focused_paragraph->textview, project, reference_tag, current_verse_number);
  // New g_thread_new ("highlight", GThreadFunc (highlight_thread_start), gpointer(this));
  g_thread_create(GThreadFunc(highlight_thread_start), gpointer(this), false, NULL);
*/
  
  // MAP: Here's how I think it should be done, more synchronously instead of threaded.
  highlight = new Highlight(focused_paragraph->textbuffer, focused_paragraph->textview, project, reference_tag, current_verse_number);
  // The time-consuming part of highlighting is to determine what bits
  // of text to highlight. Because it takes time, and the program
  // should continue to respond, it is [was] done in a thread. MAP:
  // But there is a problem when you type text, delete, then start
  // typing again. The threaded-ness of this means that sometimes,
  // bibledit starts overwriting text from the beginning of the verse
  // instead of typing in the location where the cursor is. TODO: I am
  // planning eventually to FIX the TIME CONSUMING PART so that the
  // whole thing will be more efficient.
  highlight->determine_locations();
  assert(highlight->locations_ready);
  highlight->highlight();
  // Delete and NULL the object making it ready for next use.
  delete highlight;
  highlight = NULL;
}


bool Editor2::on_highlight_timeout(gpointer data)
{
  return ((Editor2 *) data)->highlight_timeout();
}


bool Editor2::highlight_timeout()
{
  // If the highlighting object is not there, destroy timer and bail out.
  if (!highlight) {
    return false;
  }
  // Proceed if the locations for highlighting are ready.
  if (highlight->locations_ready) {
    highlight->highlight();
    // Delete and NULL the object making it ready for next use.
    delete highlight;
    highlight = NULL;
  }
  // Timer keeps going.
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
  // to respond, it is done in a thread. MAP: But there is a problem when you type
  // text, delete, then start typing again, the threaded-ness of this means that
  // sometimes, be starts overwriting text from the beginning of the verse instead
  // of typing in the location where the cursor is.
  if (highlight) {
    highlight->determine_locations();
  }
}


ustring Editor2::get_chapter()
{
  ustring chaptertext;
  vector <GtkWidget *> textviews = editor_get_widgets (vbox_paragraphs);
  for (unsigned int i = 0; i < textviews.size(); i++) {
    GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textviews[i]));
    GtkTextIter startiter, enditer;
    gtk_text_buffer_get_start_iter(textbuffer, &startiter);
    gtk_text_buffer_get_end_iter(textbuffer, &enditer);
    ustring paragraph_text = usfm_get_text(textbuffer, startiter, enditer);
    replace_text(paragraph_text, "  ", " ");
    if (i) {
      chaptertext.append ("\n");
    }
    chaptertext.append (paragraph_text);
  }
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

  // Check spelling of all active textviews.
  vector <GtkWidget *> textviews = editor_get_widgets (vbox_paragraphs);
  for (unsigned int i = 0; i < textviews.size(); i++) {
    GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textviews[i]));
    spellingchecker->check(textbuffer);
  }

  // Signal spelling checked.
  gtk_button_clicked (GTK_BUTTON (spelling_checked_signal));
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
  vector <GtkWidget *> textviews = editor_get_widgets (vbox_paragraphs);
  for (unsigned int i = 0; i < textviews.size(); i++) {
    GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textviews[i]));
    vector <ustring> words2 = spellingchecker->get_misspellings(textbuffer);
    for (unsigned int i2 = 0; i2 < words2.size(); i2++) {
      words.push_back (words2[i2]);
    }
  }
  // Remove double ones.
  set <ustring> wordset (words.begin (), words.end());
  words.clear();
  words.assign (wordset.begin (), wordset.end());
  // Give result.
  return words;  
}


void Editor2::spelling_approve (const vector <ustring>& words)
{
  // Approve all the words in the list.
  // Since this may take time, a windows will show the progress.
  ProgressWindow progresswindow (_("Adding words to dictionary"), false);
  progresswindow.set_iterate (0, 1, words.size());
  for (unsigned int i = 0; i < words.size(); i++)  {
    progresswindow.iterate ();
    spellingchecker->add_to_dictionary (words[i].c_str());
  }
  // Trigger a new spelling check.
  spelling_trigger();
}


bool Editor2::move_cursor_to_spelling_error (bool next, bool extremity)
// Move the cursor to the next (or previous) spelling error.
// Returns true if it was found, else false.
{
  bool moved = false;
  if (focused_paragraph) {
    GtkTextBuffer * textbuffer = focused_paragraph->textbuffer;
    do {
      moved = spellingchecker->move_cursor_to_spelling_error (textbuffer, next, extremity);
      if (!moved) {
        GtkWidget * textview = focused_paragraph->textview;
        textbuffer = NULL;
        if (next) {
          textview = editor_get_next_textview (vbox_paragraphs, textview);
        } else {
          textview = editor_get_previous_textview (vbox_paragraphs, textview);
        }
        if (textview) {
          give_focus (textview);
          textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
          GtkTextIter iter;
          if (next) 
            gtk_text_buffer_get_start_iter (textbuffer, &iter);
          else
            gtk_text_buffer_get_end_iter (textbuffer, &iter);
          gtk_text_buffer_place_cursor (textbuffer, &iter);
        }
      }
    } while (!moved && textbuffer);
  }
  if (moved) {
    //scroll_insertion_point_on_screen ();
    // Remove all the complicated timeout stuff and go right to work
    scroll_insertion_point_on_screen_timeout ();
  }
  return moved;
}


void Editor2::scroll_insertion_point_on_screen ()
{
  scroll_insertion_point_on_screen_id = g_timeout_add(100, GSourceFunc(on_scroll_insertion_point_on_screen_timeout), gpointer(this));
}


bool Editor2::on_scroll_insertion_point_on_screen_timeout(gpointer data)
{
  ((Editor2 *) data)->scroll_insertion_point_on_screen_timeout();
  return false;
}


void Editor2::scroll_insertion_point_on_screen_timeout() // Todo crashes here.
{
  if (!focused_paragraph->textbuffer) return;
  
  if (focused_paragraph) {

    // Ensure that the screen has been fully displayed.
    while (gtk_events_pending()) { gtk_main_iteration(); }

    // Adjustment.
    GtkAdjustment * adjustment = gtk_viewport_get_vadjustment (GTK_VIEWPORT (viewport));

    // Visible window height.
    gdouble visible_window_height = gtk_adjustment_get_page_size (adjustment);

    // Total window height.
    gdouble total_window_height = gtk_adjustment_get_upper (adjustment);

    // Get all the textviews.
    vector <GtkWidget *> textviews = editor_get_widgets (vbox_paragraphs);

   
    // Offset of insertion point starting from top.
    gint insertion_point_offset = 0;
    {
      for (unsigned int i = 0; i < textviews.size(); i++) {
        if (focused_paragraph->textview == textviews[i]) {
          break;
        }
        GtkAllocation allocation;
        gtk_widget_get_allocation (textviews[i], &allocation);
        insertion_point_offset += allocation.height;
      }
      //GtkTextMark * gtktextmark = gtk_text_buffer_get_insert (focused_paragraph->textbuffer);
      GtkTextMark * gtktextmark = gtk_text_buffer_get_mark (focused_paragraph->textbuffer, "insert");
      GtkTextIter iter;
      gtk_text_buffer_get_iter_at_mark (focused_paragraph->textbuffer, &iter, gtktextmark);
      GdkRectangle rectangle;
      gtk_text_view_get_iter_location (GTK_TEXT_VIEW (focused_paragraph->textview), &iter, &rectangle);
      gint window_x, window_y;
      gtk_text_view_buffer_to_window_coords (GTK_TEXT_VIEW (focused_paragraph->textview), GTK_TEXT_WINDOW_WIDGET, rectangle.x, rectangle.y, &window_x, &window_y);
      insertion_point_offset += rectangle.y;
    }

    // Set the adjustment to move the insertion point into 1/3th of
    // the visible part of the window. TODO: This should be an option
    // that the user can set. Sometimes it is distracting to have the
    // text move automatically. In Emacs, for instance, the user can
    // hit Ctrl-L to do that manually. We could have a preference that
    // says "auto-scroll text window to center 1/3 of window" or
    // something like that. This code slows the perceived user
    // experience because they have to reorient their eyes to where
    // the text moves to. Certainly when the cursor moves out of the
    // window then we need to auto-scroll by some amount, but it is
    // debatable whether we should auto-center the text or just move
    // the window by a line or two. I've attempted to do the latter, but
    // it is not perfect at the moment, so there is still some more TODO.

    /*
    If the insertion point is at 800, and the height of the visible window is 500,
    and the total window height is 1000, then the calculation of the offset is as follows:
    Half the height of the visible window is 250.
    Insertion point is at 800, so the start of the visible window should be at 800 - 250 = 550.
    Therefore the adjustment should move to 550.
    The adjustment value should stay within its limits. If it exceeds these, the viewport draws double lines.
    */
    
    //gdouble adjustment_value = insertion_point_offset - (visible_window_height * 0.33);
    // While working within a viewport, we will not scroll
    gdouble adjustment_value = gtk_adjustment_get_value(adjustment);
    if (insertion_point_offset < (gtk_adjustment_get_value(adjustment)+20)) {
      adjustment_value = insertion_point_offset - 60;
    }
    else if (insertion_point_offset > (gtk_adjustment_get_value(adjustment) + visible_window_height - 20)) {
      adjustment_value = insertion_point_offset - visible_window_height + 60;
    }
    if (adjustment_value < 0) {
      adjustment_value = 0;
    }
    else if (adjustment_value > (total_window_height - visible_window_height)) {
      adjustment_value = total_window_height - visible_window_height;
    }
    gtk_adjustment_set_value (adjustment, adjustment_value);

    // Remove any previous verse number highlight.
    {
      GtkTextIter startiter, enditer;
      for (unsigned int i = 0; i < textviews.size(); i++) {
        GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textviews[i]));
        gtk_text_buffer_get_start_iter (textbuffer, &startiter);
        gtk_text_buffer_get_end_iter (textbuffer, &enditer);
        gtk_text_buffer_remove_tag (textbuffer, verse_highlight_tag, &startiter, &enditer);
      }
    }
    
    // Highlight the verse if it is non-zero.
    if (current_verse_number != "0") {
      GtkWidget * textview;
      GtkTextIter startiter, enditer;
      if (get_iterator_at_verse_number (current_verse_number, style_get_verse_marker(project), vbox_paragraphs, startiter, textview)) {
        GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
        enditer = startiter;
        gtk_text_iter_forward_chars (&enditer, current_verse_number.length());
        gtk_text_buffer_apply_tag (textbuffer, verse_highlight_tag, &startiter, &enditer);
      }
    }
  }
}


void Editor2::apply_editor_action (EditorAction * action, EditorActionApplication application)
{
  // An editor action is being applied.
  disregard_text_buffer_signals++;
  
  // Pointer to any widget that should grab focus.
  GtkWidget * widget_that_should_grab_focus = NULL;
  
  // Whether contents was been changed.
  bool contents_was_changed = false;
  
  // Deal with the action depending on its type.
  switch (action->type) {

    case eatCreateParagraph:
    {
      EditorActionCreateParagraph * paragraph_action = static_cast <EditorActionCreateParagraph *> (action);
      switch (application) {
        case eaaInitial:
        {
          paragraph_action->apply(texttagtable, editable, focused_paragraph, widget_that_should_grab_focus);
          paragraph_create_actions (paragraph_action);
          break;
        }
        case eaaUndo: paragraph_action->undo (vbox_parking_lot, widget_that_should_grab_focus); break;
        case eaaRedo: paragraph_action->redo (widget_that_should_grab_focus); break;
      }
      break;
    }

    case eatChangeParagraphStyle:
    {
      EditorActionChangeParagraphStyle * style_action = static_cast <EditorActionChangeParagraphStyle *> (action);
      switch (application) {
        case eaaInitial: style_action->apply(widget_that_should_grab_focus); break;
        case eaaUndo:    style_action->undo (widget_that_should_grab_focus); break;
        case eaaRedo:    style_action->redo (widget_that_should_grab_focus); break;
      }
      break;
    }

    case eatInsertText:
    {
      EditorActionInsertText * insert_action = static_cast <EditorActionInsertText *> (action);
      switch (application) {
        case eaaInitial: insert_action->apply(widget_that_should_grab_focus); break;
        case eaaUndo:    insert_action->undo (widget_that_should_grab_focus); break;
        case eaaRedo:    insert_action->redo (widget_that_should_grab_focus); break;
      }
      contents_was_changed = true;
      break;
    }

    case eatDeleteText:
    {
      EditorActionDeleteText * delete_action = static_cast <EditorActionDeleteText *> (action);
      switch (application) {
        case eaaInitial: delete_action->apply(widget_that_should_grab_focus); break;
        case eaaUndo:    delete_action->undo (widget_that_should_grab_focus); break;
        case eaaRedo:    delete_action->redo (widget_that_should_grab_focus); break;
      }
      contents_was_changed = true;
      break;
    }

    case eatChangeCharacterStyle:
    {
      EditorActionChangeCharacterStyle * style_action = static_cast <EditorActionChangeCharacterStyle *> (action);
      switch (application) {
        case eaaInitial: style_action->apply(widget_that_should_grab_focus); break;
        case eaaUndo:    style_action->undo (widget_that_should_grab_focus); break;
        case eaaRedo:    style_action->redo (widget_that_should_grab_focus); break;
      }
      break;
    }
    
    case eatLoadChapterBoundary:
    case eatOneActionBoundary:
    {
      break;
    }

    case eatDeleteParagraph:
    {
      EditorActionDeleteParagraph * delete_action = static_cast <EditorActionDeleteParagraph *> (action);
      switch (application) {
        case eaaInitial: delete_action->apply(vbox_parking_lot, widget_that_should_grab_focus); break;
        case eaaUndo:    delete_action->undo (widget_that_should_grab_focus); break;
        case eaaRedo:    delete_action->redo (vbox_parking_lot, widget_that_should_grab_focus); break;
      }
      break;
    }

    case eatCreateNoteParagraph:
    {
      EditorActionCreateNoteParagraph * paragraph_action = static_cast <EditorActionCreateNoteParagraph *> (action);
      switch (application) {
        case eaaInitial:
        {
          paragraph_action->apply(texttagtable, editable, focused_paragraph, widget_that_should_grab_focus);
          paragraph_create_actions (paragraph_action);
          break;
        }
        case eaaUndo: paragraph_action->undo (vbox_parking_lot, widget_that_should_grab_focus); break;
        case eaaRedo: paragraph_action->redo (widget_that_should_grab_focus); break;
      }
      break;
    }

  }

  // Put this action in the right list.
  switch (application) {
    case eaaInitial: action->apply(actions_done                ); break;
    case eaaUndo:    action->undo (actions_done, actions_undone); break;
    case eaaRedo:    action->redo (actions_done, actions_undone); break;
  }

  // If there's any widget that was earmarked to be focused, grab its focus.
  // This can only be done at the end when the whole object has been set up,
  // because the callback for grabbing the focus uses this object.
  if (widget_that_should_grab_focus) {
    give_focus (widget_that_should_grab_focus);
  }

  // Handle situation where the contents of the editor has been changed.
  if (contents_was_changed) {
    show_quick_references ();
    gtk_button_clicked(GTK_BUTTON(changed_signal));
  }
  
  // Applying the editor action is over.
  disregard_text_buffer_signals--;
}


void Editor2::paragraph_create_actions (EditorActionCreateParagraph * paragraph_action)
{
  // Connect text buffer signals.
  g_signal_connect_after(G_OBJECT(paragraph_action->textbuffer), "insert-text", G_CALLBACK(on_buffer_insert_text_after), gpointer(this));
  g_signal_connect(G_OBJECT(paragraph_action->textbuffer), "delete-range", G_CALLBACK(on_buffer_delete_range_before), gpointer(this));
  g_signal_connect_after(G_OBJECT(paragraph_action->textbuffer), "delete-range", G_CALLBACK(on_buffer_delete_range_after), gpointer(this));
  g_signal_connect(G_OBJECT(paragraph_action->textbuffer), "changed", G_CALLBACK(on_textbuffer_changed), gpointer(this));
  // Connect spelling checker.
  spellingchecker->attach(paragraph_action->textview);
  // Connect text view signals.
  g_signal_connect_after((gpointer) paragraph_action->textview, "move_cursor", G_CALLBACK(on_textview_move_cursor), gpointer(this));
  g_signal_connect((gpointer) paragraph_action->textview, "motion-notify-event", G_CALLBACK(on_motion_notify_event), gpointer(this));
  g_signal_connect_after((gpointer) paragraph_action->textview, "grab-focus", G_CALLBACK(on_textview_grab_focus), gpointer(this));
  g_signal_connect((gpointer) paragraph_action->textview, "key-press-event", G_CALLBACK(on_textview_key_press_event), gpointer(this));
  g_signal_connect((gpointer) paragraph_action->textview, "key-release-event", G_CALLBACK(on_textview_key_release_event), gpointer(this));
  g_signal_connect((gpointer) paragraph_action->textview, "button_press_event", G_CALLBACK(on_textview_button_press_event), gpointer(this));
  // Set font
  set_font_textview (paragraph_action->textview);
  // Signal the parent window to connect to the signals of the text view.
  new_widget_pointer = paragraph_action->textview;
  gtk_button_clicked (GTK_BUTTON (new_widget_signal));
  // Extra bits to be done for a note.
  if (paragraph_action->type == eatCreateNoteParagraph) {
    // Cast the object to the right type.
    EditorActionCreateNoteParagraph * note_action = static_cast <EditorActionCreateNoteParagraph *> (paragraph_action);
    // Connect signal for note caller in note.
    g_signal_connect ((gpointer) note_action->eventbox, "button_press_event", G_CALLBACK (on_caller_button_press_event), gpointer (this));
  }
}


void Editor2::editor_start_new_standard_paragraph (const ustring& marker_text)
// This function deals with a marker that starts a standard paragraph.
{
  // Create a new paragraph.
  EditorActionCreateParagraph * paragraph = new EditorActionCreateParagraph (vbox_paragraphs);
  apply_editor_action (paragraph); 

  // The new paragraph markup.
  EditorActionChangeParagraphStyle * style_action = new EditorActionChangeParagraphStyle (marker_text, paragraph);
  apply_editor_action (style_action);

  // Some styles insert their marker: Do that here if appropriate.
  StyleType type;
  int subtype;
  marker_get_type_and_subtype(project, marker_text, type, subtype);
  if (style_get_displays_marker(type, subtype)) {
    gint insertion_offset = editor_paragraph_insertion_point_get_offset (paragraph);
    EditorActionInsertText * insert_action = new EditorActionInsertText (paragraph, insertion_offset, usfm_get_full_opening_marker (marker_text));
    apply_editor_action (insert_action);
  }
}


void Editor2::editor_start_verse(ustring& line, ustring& marker_text, ustring& character_style)
{
  // Clear any character style.
  character_style.clear();

  // Get the currently focused paragraph. In rare cases there may be none.
  EditorActionCreateParagraph * paragraph = focused_paragraph;
  
  // A verse number should start in a normal paragraph, not a title or heading. Check for that.
  bool in_normal_paragraph = false;
  if (paragraph) {
    StyleType type;
    int subtype;
    marker_get_type_and_subtype(project, paragraph->style, type, subtype);
    if (type == stStartsParagraph) {
      if (subtype == ptNormalParagraph) {
        in_normal_paragraph = true;
      }
    }
  }
  if (in_normal_paragraph) {
    // Review marker \nb, proceed if it exists.
    ustring marker_text = "nb";
    StyleType type;
    int subtype;
    marker_get_type_and_subtype(project, marker_text, type, subtype);
    if (type == stStartsParagraph) {
      if (subtype != ptNormalParagraph) {
        // Create new paragraph.
        editor_start_new_standard_paragraph(marker_text);
      }
    }
  }  

  // Get the currently focused paragraph. In rare cases there may be none. If there's none, create one.
  paragraph = focused_paragraph;
  if (paragraph == NULL) {
    editor_start_new_standard_paragraph (unknown_style());
  }
  // From here on we are sure there's an open paragraph.
  
  // Get verse number. Handle combined verses too, e.g. 10-12b, etc.
  size_t position = line.find(" ");
  if (position == string::npos)
    position = line.length();
  ustring versenumber = line.substr(0, position);
  line.erase(0, position);
  
  // Insert the verse number.
  paragraph = focused_paragraph;
  gint insertion_offset = editor_paragraph_insertion_point_get_offset (paragraph);
  EditorActionInsertText * insert_action = new EditorActionInsertText (paragraph, insertion_offset, versenumber);
  apply_editor_action (insert_action);
  EditorActionChangeCharacterStyle * style_action = new EditorActionChangeCharacterStyle (paragraph, marker_text, insertion_offset, versenumber.length());
  apply_editor_action (style_action);
}


EditorActionCreateParagraph * Editor2::widget2paragraph_action (GtkWidget * widget)
// Given a pointer to a GtkTextView or HBox, it returns its (note) paragraph create action.
{
  for (unsigned int i = 0; i < actions_done.size(); i++) {
    EditorAction * action = actions_done[i];
    if ((action->type == eatCreateParagraph) || (action->type == eatCreateNoteParagraph)) {
      EditorActionCreateParagraph * paragraph_action = static_cast <EditorActionCreateParagraph *> (action);
      if (paragraph_action->textview == widget) {
        return paragraph_action;
      }
      if ((action->type == eatCreateNoteParagraph)) {
        EditorActionCreateNoteParagraph * note_paragraph_action = static_cast <EditorActionCreateNoteParagraph *> (action);
        if (note_paragraph_action->hbox == widget) {
          return paragraph_action;
        }
        if (note_paragraph_action->eventbox == widget) {
          return paragraph_action;
        }
      }
    }
  }
  return NULL;
}


EditorActionCreateNoteParagraph * Editor2::note2paragraph_action (const ustring& note)
// Given a note identifier, it returns its note paragraph create action.
{
  for (unsigned int i = 0; i < actions_done.size(); i++) {
    EditorAction * action = actions_done[i];
    if ((action->type == eatCreateNoteParagraph)) {
      EditorActionCreateNoteParagraph * note_paragraph_action = static_cast <EditorActionCreateNoteParagraph *> (action);
      if (note_paragraph_action->identifier == note) {
        return note_paragraph_action;
      }
    }
  }
  return NULL;
}


ustring Editor2::usfm_get_text(GtkTextBuffer * textbuffer, GtkTextIter startiter, GtkTextIter enditer)
{
  // To hold the text it is going to retrieve.
  ustring text;
  
  // Initialize the iterator.
  GtkTextIter iter = startiter;

  // Paragraph and character styles.
  ustring previous_paragraph_style;
  ustring previous_character_style;

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

    // Is it a note caller or normal text?
    if (new_character_style.find (note_starting_style ()) == 0) {
      
      // Note caller found. Retrieve its text.
      EditorActionCreateNoteParagraph * note_paragraph = note2paragraph_action (new_character_style);
      if (note_paragraph) {
        ustring note_text;
        // Add the note opener.
        note_text.append (usfm_get_full_opening_marker(note_paragraph->opening_closing_marker));
        // Add the usfm caller.
        note_text.append (note_paragraph->caller_usfm);
        note_text.append (" ");
        // Get the main note body.
        GtkTextBuffer * textbuffer = note_paragraph->textbuffer;
        GtkTextIter startiter, enditer;
        gtk_text_buffer_get_start_iter(textbuffer, &startiter);
        gtk_text_buffer_get_end_iter(textbuffer, &enditer);
        note_text.append (usfm_get_note_text(startiter, enditer, project));
        // Add the note closer.
        note_text.append (usfm_get_full_closing_marker(note_paragraph->opening_closing_marker));
        // Add the note to the main text.
        text.append (note_text);
      }
      
    } else {
      
      // Normal text found.

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
  
  // Return the text it got.
  return text;
}


void Editor2::editor_text_fallback (ustring& line, ustring& character_style, size_t marker_pos, bool marker_found)
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

  // Get the currently focused paragraph. If there's none, create one.
  EditorActionCreateParagraph * paragraph = focused_paragraph;
  if (paragraph == NULL) {
    editor_start_new_standard_paragraph (unknown_style());
  }
  
  // Insert the text.
  paragraph = focused_paragraph;
  gint insertion_offset = editor_paragraph_insertion_point_get_offset (paragraph);
  EditorActionInsertText * insert_action = new EditorActionInsertText (paragraph, insertion_offset, insertion);
  apply_editor_action (insert_action);
  if (!character_style.empty()) {
    EditorActionChangeCharacterStyle * style_action = new EditorActionChangeCharacterStyle (paragraph, character_style, insertion_offset, insertion.length());
    apply_editor_action (style_action);
  }
}


bool Editor2::editor_starts_character_style(ustring & line, ustring & character_style, const ustring & marker_text, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype(project, marker_text, type, subtype);
        if (style_get_starts_character_style(type, subtype)) {
          character_style = marker_text;
          line.erase(0, marker_length);
          return true;
        }
      }
    }
  }
  return false;
}


bool Editor2::editor_ends_character_style(ustring & line, ustring & character_style, const ustring & marker_text, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found)
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (!is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype(project, marker_text, type, subtype);
        if (style_get_starts_character_style(type, subtype)) {
          character_style.clear();
          line.erase(0, marker_length);
          return true;
        }
      }
    }
  }
  return false;
}


bool Editor2::text_starts_note_raw(ustring & line, ustring & character_style, const ustring & marker_text, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found, ustring& raw_note)
// This function determines whether the text starts a footnote, an endnote, or a crossreference.
{
  if (marker_found) {
    if (marker_pos == 0) {
      if (is_opener) {
        StyleType type;
        int subtype;
        marker_get_type_and_subtype(project, marker_text, type, subtype);
        if (style_get_starts_footnote(type, subtype) || style_get_starts_endnote(type, subtype) || style_get_starts_crossreference(type, subtype)) {
          // Proceed if the endmarker is in the text too.
          ustring endmarker = usfm_get_full_closing_marker(marker_text);
          size_t endmarkerpos = line.find(endmarker);
          if (endmarkerpos != string::npos) {
            // Get raw note text and erase it from the input buffer.
            raw_note = line.substr(marker_length, endmarkerpos - endmarker.length());
            line.erase(0, endmarkerpos + endmarker.length());
            // The information was processed: return true
            return true;
          }
        }
      }
    }
  }
  return false;
}


void Editor2::editor_start_note_raw (ustring raw_note, const ustring & marker_text)
// Starts a note in the editor.
{
  // Type of the note.
  EditorNoteType note_type = note_type_get (project, marker_text);

  // Add a note caller in superscript.
  ustring caller_in_text, caller_style;
  {
    get_next_note_caller_and_style (note_type, caller_in_text, caller_style, false);
    Style style ("", caller_style, false);
    style.superscript = true;
    create_or_update_text_style(&style, false, false, font_size_multiplier);
    gint insertion_offset = editor_paragraph_insertion_point_get_offset (focused_paragraph);
    EditorActionInsertText * insert_action = new EditorActionInsertText (focused_paragraph, insertion_offset, caller_in_text);
    apply_editor_action (insert_action);
    EditorActionChangeCharacterStyle * style_action = new EditorActionChangeCharacterStyle (focused_paragraph, caller_style, insertion_offset, caller_in_text.length());
    apply_editor_action (style_action);
  }

  // Store currently focused standard paragraph so it can be restored after the note has been created.
  EditorActionCreateParagraph * focused_standard_paragraph = focused_paragraph;

  // Extract the USFM caller.
  ustring caller_in_usfm;
  if (!raw_note.empty()) {
    caller_in_usfm = raw_note.substr(0, 1);
    raw_note.erase(0, 1);
    raw_note = trim(raw_note);
  }

  // New note paragraph.
  EditorActionCreateNoteParagraph * note_paragraph = new EditorActionCreateNoteParagraph (vbox_notes, marker_text, caller_in_usfm, caller_in_text, caller_style);
  apply_editor_action (note_paragraph); 

  // Note paragraph style.
  ustring paragraph_style (style_get_default_note_style(project, note_type));
  EditorActionChangeParagraphStyle * style_action = new EditorActionChangeParagraphStyle (paragraph_style, note_paragraph);
  apply_editor_action (style_action);

  // Load remaining text of the note.
  text_load (raw_note, "", true);

  // Restore the focus to the standard paragraph that had focus before the note was created.
  give_focus (focused_standard_paragraph->textview);
}


void Editor2::copy_clipboard_intelligently ()
// Copies the plain text to the clipboard, and copies both plain and usfm text to internal storage.
{
  GtkTextIter startiter, enditer;
  if (gtk_text_buffer_get_selection_bounds (focused_paragraph->textbuffer, &startiter, &enditer)) {
    clipboard_text_plain.clear();
    vector <ustring> text;
    vector <ustring> styles;
    get_text_and_styles_between_iterators(&startiter, &enditer, text, styles);
    for (unsigned int i = 0; i < text.size(); i++) {
      if (styles[i].find (note_starting_style()) == string::npos) {
        clipboard_text_plain.append (text[i]);
      }
    }
    clipboard_text_usfm = text_get_selection ();
    // If no plain text is put on the clipboard, but usfm text, then put something on the clipboard.
    // This facilitates copying of notes.
    if (clipboard_text_plain.empty()) {
      if (!clipboard_text_usfm.empty()) {
        clipboard_text_plain = usfm_clipboard_code ();
      }
    }
    // Put the plain text on the clipboard.
    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_clipboard_set_text (clipboard, clipboard_text_plain.c_str(), -1);
  }
}


void Editor2::cut ()
// Cut to clipboard.
{
  if (editable) {
    if (focused_paragraph) {
      // Copy the text to the clipboard in an intelligent manner.
      copy_clipboard_intelligently ();
      // Remove the text from the text buffer.
      gtk_text_buffer_delete_selection (focused_paragraph->textbuffer, true, editable);
    }
  }
}


void Editor2::copy ()
// Copy to clipboard.
{
  if (focused_paragraph) {
    // Copy the text to the clipboard in an intelligent manner.
    copy_clipboard_intelligently ();
  }
}


void Editor2::paste ()
// Paste from clipboard.
{
  // Proceed if the Editor is editable and there's a focused paragraph where to put the text into.
  if (editable) {
    if (focused_paragraph) {
      // Get the text that would be pasted.
      GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
      gchar * text = gtk_clipboard_wait_for_text (clipboard);
      if (text) {
        ustring utext (text);
        if ((utext == clipboard_text_plain) || (utext == usfm_clipboard_code ())) {
          // Since the text that would be pasted is the same as the plain text 
          // that results from the previous copy or cut operation, 
          // it inserts the equivalent usfm text instead.
          // Or if USFM code only was copied, nothing else, then take that too.
          gtk_text_buffer_delete_selection (focused_paragraph->textbuffer, true, editable);
          gtk_text_buffer_insert_at_cursor (focused_paragraph->textbuffer, clipboard_text_usfm.c_str(), -1);
        } else {
          // The text that would be pasted differs from the plain text
          // that resulted from the previous copy or cut operation,
          // so insert the text that would be pasted as it is.
          gtk_text_buffer_paste_clipboard(focused_paragraph->textbuffer, clipboard, NULL, true);
        }
        // Free memory.
        g_free (text);
      }
    }
  }
}


gboolean Editor2::on_textview_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
  return ((Editor2 *) user_data)->textview_key_press_event(widget, event);
}


gboolean Editor2::textview_key_press_event(GtkWidget *widget, GdkEventKey *event)
{
  // Clear flag for monitoring deletions from textbuffers.
  textbuffer_delete_range_was_fired = false;

  // Store data for paragraph crossing control.
  paragraph_crossing_textview_at_key_press = widget;
  GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
  gtk_text_buffer_get_iter_at_mark(textbuffer, &paragraph_crossing_insertion_point_iterator_at_key_press, gtk_text_buffer_get_insert(textbuffer));

  // A GtkTextView has standard keybindings for clipboard operations.
  // It has been found that if the user presses, e.g. Ctrl-c, that
  // text is copied to the clipboard twice, or e.g. Ctrl-v, that it is
  // pasted twice. This is probably a bug in Gtk2.
  // The relevant key bindings for clipboard operations are blocked here.
  // The default bindings for copying to clipboard are Ctrl-c and Ctrl-Insert.
  // The default bindings for cutting to clipboard are Ctrl-x and Shift-Delete.
  // The default bindings for pasting from clipboard are Ctrl-v and Shift-Insert.
  if (keyboard_control_state(event)) {
    if (event->keyval == GDK_KEY_c) return true;
    if (event->keyval == GDK_KEY_x) return true;
    if (event->keyval == GDK_KEY_v) return true;
    if (keyboard_insert_pressed(event)) return true;
  }
  if (keyboard_shift_state(event)) {
    if (keyboard_delete_pressed(event)) return true;
    if (keyboard_insert_pressed(event)) return true;
  }
  
  // Pressing Page Up while the cursor is in the note brings the user
  // to the note caller in the text.
  if (keyboard_page_up_pressed(event)) {
    if (focused_paragraph) {
      if (focused_paragraph->type == eatCreateNoteParagraph) {
        on_caller_button_press (focused_paragraph->textview);
      }
    }
  }

  // Propagate event further.
  return FALSE;
}


gboolean Editor2::on_textview_key_release_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
  ((Editor2 *) user_data)->textview_key_release_event(widget, event);
  return FALSE;
}


void Editor2::textview_key_release_event(GtkWidget *widget, GdkEventKey *event)
{
  // Handle pressing the Backspace key.
  if (keyboard_backspace_pressed (event)) {
    // Handle the case that the backspace key didn't delete text.
    // Do this only when the Editor is editable.
    if (!textbuffer_delete_range_was_fired && editable) {
      // Get the current and preceding paragraphs.
      // The preceding one may not be there.
      EditorActionCreateParagraph * current_paragraph = widget2paragraph_action (widget);
      EditorActionCreateParagraph * preceding_paragraph = widget2paragraph_action (editor_get_previous_textview (vbox_paragraphs, widget));
      if (current_paragraph && preceding_paragraph) {
        // Get the text and styles of the current paragraph.
        vector <ustring> text;
        vector <ustring> styles;
        EditorActionDeleteText * delete_action = paragraph_get_text_and_styles_after_insertion_point(current_paragraph, text, styles);
        // Delete the text from the current paragraph.
        if (delete_action) {
          apply_editor_action (delete_action);
        }
        // Insert the text into the preceding paragraph.
        GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (preceding_paragraph->textview));
        GtkTextIter enditer;
        gtk_text_buffer_get_end_iter (textbuffer, &enditer);
        gint initial_offset = gtk_text_iter_get_offset (&enditer);
        for (unsigned int i = 0; i < text.size(); i++) {
          gtk_text_buffer_get_end_iter (textbuffer, &enditer);
          gint offset = gtk_text_iter_get_offset (&enditer);
          EditorActionInsertText * insert_action = new EditorActionInsertText (preceding_paragraph, offset, text[i]);
          apply_editor_action (insert_action);
          if (!styles[i].empty()) {
            EditorActionChangeCharacterStyle * style_action = new EditorActionChangeCharacterStyle(preceding_paragraph, styles[i], offset, text[i].length());
            apply_editor_action (style_action);
          }
        }
        // Move the insertion point to the position just before the joined text.
        editor_paragraph_insertion_point_set_offset (preceding_paragraph, initial_offset);
        // Remove the current paragraph.
        apply_editor_action (new EditorActionDeleteParagraph(current_paragraph));
        // Focus the preceding paragraph.
        give_focus (preceding_paragraph->textview);
        // Insert the One Action boundary.
        apply_editor_action (new EditorAction (eatOneActionBoundary));
      }      
    }
  }

  // Handle pressing the Delete keys.
  if (keyboard_delete_pressed (event)) {
    // Handle the case that the delete keys didn't delete text.
    // Do this only when the Editor is editable.
    if (!textbuffer_delete_range_was_fired && editable) {
      // Get the current and following paragraphs.
      // The following one may not be there.
      EditorActionCreateParagraph * current_paragraph = widget2paragraph_action (widget);
      EditorActionCreateParagraph * following_paragraph = widget2paragraph_action (editor_get_next_textview (vbox_paragraphs, widget));
      if (current_paragraph && following_paragraph) {
        // Get the text and styles of the whole following paragraph.
        editor_paragraph_insertion_point_set_offset (following_paragraph, 0);
        vector <ustring> text;
        vector <ustring> styles;
        EditorActionDeleteText * delete_action = paragraph_get_text_and_styles_after_insertion_point(following_paragraph, text, styles);
        // Delete the text from the following paragraph.
        if (delete_action) {
          apply_editor_action (delete_action);
        }
        // Insert the text into the current paragraph.
        GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (current_paragraph->textview));
        GtkTextIter enditer;
        gtk_text_buffer_get_end_iter (textbuffer, &enditer);
        gint initial_offset = gtk_text_iter_get_offset (&enditer);
        for (unsigned int i = 0; i < text.size(); i++) {
          gtk_text_buffer_get_end_iter (textbuffer, &enditer);
          gint offset = gtk_text_iter_get_offset (&enditer);
          EditorActionInsertText * insert_action = new EditorActionInsertText (current_paragraph, offset, text[i]);
          apply_editor_action (insert_action);
          if (!styles[i].empty()) {
            EditorActionChangeCharacterStyle * style_action = new EditorActionChangeCharacterStyle(current_paragraph, styles[i], offset, text[i].length());
            apply_editor_action (style_action);
          }
        }
        // Move the insertion point to the position just before the joined text.
        editor_paragraph_insertion_point_set_offset (current_paragraph, initial_offset);
        // Remove the following paragraph.
        apply_editor_action (new EditorActionDeleteParagraph(following_paragraph));
        // Insert the One Action boundary.
        apply_editor_action (new EditorAction (eatOneActionBoundary));
      }      
    }
  }

  // Clear flag for monitoring deletions from textbuffers.
  textbuffer_delete_range_was_fired = false;  
}


bool Editor2::on_textview_button_press_delayed (gpointer user_data)
{
  ((Editor2 *) user_data)->textview_button_press_delayed();
  return false;
}


void Editor2::textview_button_press_delayed ()
{
  textview_button_press_event_id = 0;
  signal_if_styles_changed();
  signal_if_verse_changed();
}


void Editor2::switch_verse_tracking_off ()
{
  if (!verse_tracking_on)
    return;
  verse_tracking_on = false;
}


void Editor2::switch_verse_tracking_on ()
{
  if (verse_tracking_on)
    return;
  verse_tracking_on = true;
}


void Editor2::go_to_verse(const ustring& number, bool focus)
// Moves the insertion point of the editor to the verse number.
{
  // Ensure verse tracking is on.
  switch_verse_tracking_on ();
  
  // Save the current verse. This prevents a race-condition.
  current_verse_number = number;

  // Only move the insertion point if it goes to another verse.
  if (number != verse_number_get()) {

    // Get the iterator and textview that contain the verse number.
    GtkTextIter iter;
    GtkWidget * textview;
    if (get_iterator_at_verse_number (number, style_get_verse_marker(project), vbox_paragraphs, iter, textview)) {
      if (focus) {
      }
      give_focus (textview);
      GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
      gtk_text_buffer_place_cursor(textbuffer, &iter);
    }
  
  }

  // Scroll the insertion point onto the screen.
  //scroll_insertion_point_on_screen ();
  // Remove all the complicated timeout stuff and go right to work
  scroll_insertion_point_on_screen_timeout ();

  // Highlight search words.
  highlight_searchwords();
}


void Editor2::signal_if_verse_changed ()
{
  gw_destroy_source(signal_if_verse_changed_event_id);
  signal_if_verse_changed_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 100, GSourceFunc(on_signal_if_verse_changed_timeout), gpointer(this), NULL);
}


bool Editor2::on_signal_if_verse_changed_timeout(gpointer data)
{
  ((Editor2 *) data)->signal_if_verse_changed_timeout();
  return false;
}


void Editor2::signal_if_verse_changed_timeout()
{
  // Proceed if verse tracking is on.
  if (verse_tracking_on) {
    // Proceed if there's a focused paragraph.
    if (focused_paragraph) {
      // Proceed if there's no selection.
      if (!gtk_text_buffer_get_has_selection (focused_paragraph->textbuffer)) {
        // Emit a signal if the verse number at the insertion point changed.
        ustring verse_number = verse_number_get();
        if (verse_number != current_verse_number) {
          current_verse_number = verse_number;
          if (new_verse_signal) {
            gtk_button_clicked(GTK_BUTTON(new_verse_signal));
          }
        }
      }
    }
  }
}


void Editor2::paragraph_crossing_act(GtkMovementStep step, gint count)
{
  // Bail out if there's no paragraph.
  if (focused_paragraph == NULL) {
    return;
  }
  
  // Get the iterator at the insert position of the currently focused paragraph.
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_mark (focused_paragraph->textbuffer, &iter, gtk_text_buffer_get_insert (focused_paragraph->textbuffer));

  // Bail out if there was real movement.
  if (!gtk_text_iter_equal (&paragraph_crossing_insertion_point_iterator_at_key_press, &iter)) {
    return;
  }

  // Focus the crossed widget and place its cursor.  
  GtkWidget * crossed_widget;
  if (count > 0) {
    crossed_widget = editor_get_next_textview (vbox_paragraphs, focused_paragraph->textview);
  } else {
    crossed_widget = editor_get_previous_textview (vbox_paragraphs, focused_paragraph->textview);
  }  
  if (crossed_widget) {
    GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (crossed_widget));
    if (count > 0) {
      gtk_text_buffer_get_start_iter (textbuffer, &iter);
    } else {
      gtk_text_buffer_get_end_iter (textbuffer, &iter);
    }
    gtk_text_buffer_place_cursor (textbuffer, &iter);
    give_focus (crossed_widget);
  }
}


gboolean Editor2::on_caller_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  return ((Editor2 *) user_data)->on_caller_button_press(widget);
}


gboolean Editor2::on_caller_button_press (GtkWidget *widget)
// Called when the user clicks on a note caller at the bottom of the screen.
// It will focus the note caller in the text.
{
  // Look for the note paragraph.
  EditorActionCreateParagraph * paragraph = widget2paragraph_action (widget);
  if (paragraph) {
    EditorActionCreateNoteParagraph * note_paragraph = static_cast <EditorActionCreateNoteParagraph *> (paragraph);
    // Get the note style.
    ustring note_style = note_paragraph->identifier;
    // Get the iterator and the textview of the note caller in the text.
    vector <GtkWidget *> textviews = editor_get_widgets (vbox_paragraphs);
    for (unsigned int i = 0; i < textviews.size(); i++) {
      GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textviews[i]));
      GtkTextIter iter;
      gtk_text_buffer_get_start_iter (textbuffer, &iter);
      do {
        ustring paragraph_style, character_style, verse_at_iter;
        get_styles_at_iterator(iter, paragraph_style, character_style);
        if (character_style == note_style) {
          gtk_text_buffer_place_cursor (textbuffer, &iter);
          give_focus (textviews[i]);
        }
      } while (gtk_text_iter_forward_char(&iter));
    }
  }
  // Propagate the button press event.
  return false;
}


bool Editor2::has_focus ()
// Returns whether the editor has focus.
{
  vector <GtkWidget *> widgets = editor_get_widgets (vbox_paragraphs);
  for (unsigned int i = 0; i < widgets.size(); i++) {
    if (gtk_widget_has_focus (widgets[i]))
      return true;
  }
  widgets = editor_get_widgets (vbox_notes);
  for (unsigned int i = 0; i < widgets.size(); i++) {
    if (gtk_widget_has_focus (widgets[i]))
      return true;
  }
  return false;
}


void Editor2::give_focus (GtkWidget * widget)
// Gives focus to a widget.
{
  if (has_focus ()) {
    // If the editor has focus, then the widget is actually given focus.
    gtk_widget_grab_focus (widget);
  } else {
    // If the editor does not have focus, only the internal focus system is called, without actually having the widget grab focus.
    textview_grab_focus(widget);
  }
}

