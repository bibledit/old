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


#include "libraries.h"
#include <glib.h>
#include "windoweditor.h"
#include "help.h"
#include "floatingwindow.h"
#include "keyterms.h"
#include "tiny_utilities.h"
#include "settings.h"
#include <glib/gi18n.h>

WindowEditor::WindowEditor(const ustring& project_name, GtkWidget * parent_layout, GtkAccelGroup *accelerator_group, bool startup):
FloatingWindow(parent_layout, widEditor, project_name, startup)
// Text editor.
{
  // Initialize variables.
  editor2 = NULL;
  usfmview = NULL;
  
  // Signalling buttons.
  new_verse_signal = gtk_button_new();
  new_styles_signal = gtk_button_new();
  quick_references_button = gtk_button_new();
  word_double_clicked_signal = gtk_button_new();
  reload_signal = gtk_button_new();
  changed_signal = gtk_button_new();
  spelling_checked_signal = gtk_button_new ();

  // Gui.  
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox);
  gtk_container_add(GTK_CONTAINER(vbox_client), vbox);

  // Switch to default view.
  switch_to_view (false, project_name);
}


WindowEditor::~WindowEditor()
{
  gtk_widget_destroy (new_verse_signal);
  gtk_widget_destroy (new_styles_signal);
  gtk_widget_destroy (quick_references_button);
  gtk_widget_destroy (word_double_clicked_signal);
  gtk_widget_destroy (reload_signal);
  gtk_widget_destroy (changed_signal);
  gtk_widget_destroy (spelling_checked_signal);
  if (editor2)
    delete editor2;
  if (usfmview)
    delete usfmview;
}


void WindowEditor::go_to(const Reference & reference)
// Let the editor go to a reference.
{
  if (editor2 || usfmview) {

    // Find out what needs to be changed: book, chapter and/or verse.
    bool new_book = false;
    bool new_chapter = false;
    bool new_verse = false;
    if (editor2) {
      new_book = (reference.book != editor2->current_reference.book);
      editor2->current_reference.book = reference.book;
      new_chapter = (reference.chapter != editor2->current_reference.chapter);
      editor2->current_reference.chapter = reference.chapter;
      new_verse = (reference.verse != editor2->current_reference.verse);
      editor2->current_reference.verse = reference.verse;
    }
    if (usfmview) {
      new_book = (reference.book != usfmview->current_reference.book);
      usfmview->current_reference.book = reference.book;
      new_chapter = (reference.chapter != usfmview->current_reference.chapter);
      usfmview->current_reference.chapter = reference.chapter;
      new_verse = (reference.verse != usfmview->current_reference.verse);
      usfmview->current_reference.verse = reference.verse;
    }

    // Save the editor if need be.
    if (new_book || new_chapter) {
      if (editor2) editor2->chapter_save();
      if (usfmview) usfmview->chapter_save();
    }
    
    // With a new book, also load a new chapter.
    if (new_book) {
      new_chapter = true;
      if (editor2) editor2->book_set(reference.book);
      if (usfmview) usfmview->book_set(reference.book);
    }

    // Deal with a new chapter.
    if (new_chapter) {
      // Load chapter, if need be.
      if (editor2) editor2->chapter_load(reference.chapter);
      if (usfmview) usfmview->chapter_load(reference.chapter);
      // When loading a new chapter, there is also a new verse.
      new_verse = true;
    }

    // New reference handling.  
    if (new_book || new_chapter || new_verse) {
      if (editor2) editor2->go_to_verse(reference.verse, false);
      if (usfmview) usfmview->position_cursor_at_verse(reference.verse);
    }

    // Highlighting of searchwords.
    if (editor2) {
      if (editor2->go_to_new_reference_highlight) {
        editor2->highlight_searchwords();
        editor2->go_to_new_reference_highlight = false;
      }
    }
  }
}


void WindowEditor::load_dictionaries()
{
  if (editor2) {
    editor2->load_dictionaries();
  }
  if (usfmview) {
    usfmview->load_dictionaries();
  }
}


bool WindowEditor::move_cursor_to_spelling_error (bool next, bool extremity)
{
  if (editor2) {
    return editor2->move_cursor_to_spelling_error (next, extremity);
  }
  if (usfmview) {
    return usfmview->move_cursor_to_spelling_error (next, extremity);
  }
  return true;
}


void WindowEditor::undo()
{
  if (editor2) {
    editor2->undo(); 
  }
  if (usfmview) {
    usfmview->undo();  
  }
}


void WindowEditor::redo()
{
  if (editor2) {
    editor2->redo(); 
  }
  if (usfmview) {
    usfmview->redo();  
  }
}


bool WindowEditor::can_undo()
{
  if (editor2) {
    return editor2->can_undo();
  }
  if (usfmview) {
    return usfmview->can_undo();
  }
  return false;
}


bool WindowEditor::can_redo()
{
  if (editor2) {
    return editor2->can_redo();
  }
  if (usfmview) {
    return usfmview->can_redo();
  }
  return false;
}


EditorTextViewType WindowEditor::last_focused_type()
{
  if (editor2) {
    return editor2->last_focused_type();
  }
  if (usfmview) {
    return etvtBody;
  }
  return etvtBody;
}


vector <Reference> WindowEditor::quick_references()
{
  if (usfmview) {
    vector <Reference> dummy;
    return dummy;
  }
  if (editor2) {
    return editor2->quick_references;
  }
  vector <Reference> dummy;
  return dummy;
}


Reference WindowEditor::current_reference()
{
  if (usfmview) { 
    return usfmview->current_reference;
  }
  if (editor2) {
    return editor2->current_reference;
  }
  Reference reference (0);
  return reference;
}


ustring WindowEditor::current_verse_number()
{
  if (usfmview) {
    return usfmview->current_verse_number;
  }
  if (editor2) {
    return editor2->current_verse_number;
  }
  return "0";
}


ustring WindowEditor::project()
{
  if (usfmview) {
    return usfmview->project;
  }
  if (editor2) {
    return editor2->project;
  }
  return "";
}


ustring WindowEditor::text_get_selection()
{
  if (usfmview) {
    return usfmview->text_get_selection();
  }
  if (editor2) {
    return editor2->text_get_selection();
  }
  return "";
}


void WindowEditor::text_insert(ustring text)
{
  if (usfmview) {
    usfmview->text_insert(text);
  }
  if (editor2) {
    editor2->text_insert(text);
  }
}


void WindowEditor::go_to_new_reference_highlight_set() 
{
  if (usfmview) {
  }
  if (editor2) {
    editor2->go_to_new_reference_highlight = true;
  }
}


ustring WindowEditor::word_double_clicked_text()
{
  if (usfmview) {
    return usfmview->word_double_clicked_text;
  }
  if (editor2) {
    return editor2->word_double_clicked_text;
  }
  return "";
}


bool WindowEditor::editable()
{
  if (usfmview) { 
    return usfmview->editable;
  }
  if (editor2) {
    return editor2->editable;
  }
  return false;
}


void WindowEditor::insert_note(const ustring& marker, const ustring& rawtext)
{
  if (usfmview) {
    usfmview->insert_note (marker, rawtext);
  }
  if (editor2) {
    editor2->insert_note (marker, rawtext);
  }
}


ustring WindowEditor::get_chapter()
{
  if (usfmview) {
    return usfmview->get_chapter();
  }
  if (editor2) {
    return editor2->get_chapter();
  }
  return "";
}


void WindowEditor::insert_table(const ustring& rawtext)
{
  if (usfmview) {
    usfmview->text_insert (rawtext);
  }
  if (editor2) {
    editor2->insert_table (rawtext);
  }
}


void WindowEditor::chapter_load(unsigned int chapter_in)
{
  if (usfmview) {
    usfmview->chapter_load (chapter_in);
  }
  if (editor2) {
    editor2->chapter_load (chapter_in);
  }
}


void WindowEditor::chapter_save()
{
  if (usfmview) {
    usfmview->chapter_save();
  }
  if (editor2) {
    editor2->chapter_save();
  }
}


unsigned int WindowEditor::reload_chapter_number()
{
  if (usfmview) { 
    return usfmview->reload_chapter_number;
  }
  if (editor2) {
    return editor2->reload_chapter_number;
  }
  return 0;
}


void WindowEditor::apply_style(const ustring& marker)
{
  if (usfmview) {
  }
  if (editor2) {
    editor2->apply_style (marker);
  }
}


set <ustring> WindowEditor::get_styles_at_cursor()
{
  if (usfmview) {
    set <ustring> dummy;
    return dummy;
  }
  if (editor2) {
    return editor2->get_styles_at_cursor();
  }
  set <ustring> dummy;
  return dummy;
}


void WindowEditor::create_or_update_formatting_data()
{
  if (usfmview) { 
  }
  if (editor2) {
    editor2->create_or_update_formatting_data();
  }
}


void WindowEditor::set_font()
{
  if (usfmview) { 
    usfmview->set_font();
  }
  if (editor2) {
    editor2->set_font();
  }
}


Editor2 * WindowEditor::editor_get()
{
  if (usfmview) {
    return NULL;
  }
  if (editor2) {
    return editor2;
  }
  return NULL;
}


unsigned int WindowEditor::book()
{
  if (usfmview) {
    return usfmview->book;
  }
  if (editor2) {
    return editor2->book;
  }
  return 1;
}


unsigned int WindowEditor::chapter()
{
  if (usfmview) {
    return usfmview->chapter;
  }
  if (editor2) {
    return editor2->chapter;
  }
  return 1;
}


void WindowEditor::on_new_verse_signalled(GtkButton *button, gpointer user_data)
{
  ((WindowEditor *) user_data)->on_new_verse();
}


void WindowEditor::on_new_verse()
{
  gtk_button_clicked (GTK_BUTTON (new_verse_signal));
}


void WindowEditor::on_new_styles_signalled(GtkButton *button, gpointer user_data)
{
  ((WindowEditor *) user_data)->on_new_styles();
}


void WindowEditor::on_new_styles()
{
  // Set the styles in the status bar.
  set <ustring> styles = get_styles_at_cursor();
  vector <ustring> styles2 (styles.begin(), styles.end());
  ustring text = _("Style ");
  for (unsigned int i = 0; i < styles2.size(); i++) {
    if (i)
      text.append(", ");
    text.append(styles2[i]);
  }
  status1 (text);
  // Give a signal that there are new styles.
  gtk_button_clicked (GTK_BUTTON (new_styles_signal));
}


void WindowEditor::on_quick_references_signalled(GtkButton *button, gpointer user_data)
{
  ((WindowEditor *) user_data)->on_quick_references();
}


void WindowEditor::on_quick_references()
{
  gtk_button_clicked (GTK_BUTTON (quick_references_button));
}


void WindowEditor::on_word_double_click_signalled(GtkButton *button, gpointer user_data)
{
  ((WindowEditor *) user_data)->on_word_double_click();
}


void WindowEditor::on_word_double_click()
{
  gtk_button_clicked (GTK_BUTTON (word_double_clicked_signal));
}


void WindowEditor::on_reload_signalled(GtkButton *button, gpointer user_data)
{
  ((WindowEditor *) user_data)->on_reload();
}


void WindowEditor::on_reload()
{
  gtk_button_clicked (GTK_BUTTON (reload_signal));
}


void WindowEditor::on_changed_signalled(GtkButton *button, gpointer user_data)
{
  ((WindowEditor *) user_data)->on_changed();
}


void WindowEditor::on_changed()
{
  gtk_button_clicked (GTK_BUTTON (changed_signal));
}


bool WindowEditor::editing_usfm_code_get()
{
  return usfmview;
}


void WindowEditor::editing_usfm_code_set (bool setting)
{
  // Bail out if the setting does not change.
  if (setting == (usfmview != NULL))
    return;
    
  // Take action.
  switch_to_view (setting, "");
}


void WindowEditor::switch_to_view (bool viewusfm, ustring project) // Todo
// Switch between normal editor, which is the default,
// or to the USFM editor if argument "usfmview" is true.
{
  // If no project was given, then we have switched.
  bool switched = project.empty();
  
  // Get state of and destroy any previous view.
  Reference reference (0);
  if (editor2) {
    project = editor2->project;
    reference = editor2->current_reference;
    delete editor2;
    editor2 = NULL;
  }
  if (usfmview) {
    project = usfmview->project;
    reference = usfmview->current_reference;
    delete usfmview;
    usfmview = NULL;
  }

  // Create new view.
  if (viewusfm) {
    usfmview = new USFMView (vbox, project);
    connect_focus_signals (usfmview->sourceview);
    g_signal_connect ((gpointer) usfmview->reload_signal, "clicked", G_CALLBACK(on_reload_signalled), gpointer(this));
    g_signal_connect ((gpointer) usfmview->changed_signal, "clicked", G_CALLBACK(on_changed_signalled), gpointer(this));
    g_signal_connect ((gpointer) usfmview->new_verse_signal, "clicked", G_CALLBACK(on_new_verse_signalled), gpointer(this));
    g_signal_connect ((gpointer) usfmview->word_double_clicked_signal, "clicked", G_CALLBACK(on_word_double_click_signalled), gpointer(this));
    last_focused_widget = usfmview->sourceview;
  } else {
    editor2 = new Editor2 (vbox, project);
    g_signal_connect ((gpointer) editor2->new_verse_signal, "clicked", G_CALLBACK(on_new_verse_signalled), gpointer(this));
    g_signal_connect ((gpointer) editor2->new_styles_signal, "clicked", G_CALLBACK(on_new_styles_signalled), gpointer(this));
    g_signal_connect ((gpointer) editor2->quick_references_button, "clicked", G_CALLBACK(on_quick_references_signalled), gpointer(this));
    g_signal_connect ((gpointer) editor2->word_double_clicked_signal, "clicked", G_CALLBACK(on_word_double_click_signalled), gpointer(this));
    g_signal_connect ((gpointer) editor2->reload_signal, "clicked", G_CALLBACK(on_reload_signalled), gpointer(this));
    g_signal_connect ((gpointer) editor2->changed_signal, "clicked", G_CALLBACK(on_changed_signalled), gpointer(this));
    g_signal_connect ((gpointer) editor2->spelling_checked_signal, "clicked", G_CALLBACK(on_spelling_checked_signalled), gpointer(this));
    g_signal_connect ((gpointer) editor2->new_widget_signal, "clicked", G_CALLBACK(on_new_widget_signal_clicked), gpointer(this));
    last_focused_widget = editor2->last_focused_widget;
  }  
  // Main widget grabs focus.
  gtk_widget_grab_focus (last_focused_widget);

  // If we switched, set the editor to the right place.
  if (switched) {
    go_to (reference);
  }
}


GtkTextBuffer * WindowEditor::edit_usfm_textbuffer ()
{
  GtkTextBuffer * textbuffer = NULL;
  if (usfmview) {
    textbuffer = GTK_TEXT_BUFFER (usfmview->sourcebuffer);
  }
  return textbuffer;
}


void WindowEditor::on_spelling_checked_signalled(GtkButton *button, gpointer user_data)
{
  ((WindowEditor *) user_data)->on_spelling_checked();
}


void WindowEditor::on_spelling_checked()
{
  gtk_button_clicked (GTK_BUTTON (spelling_checked_signal));
}


void WindowEditor::spelling_trigger ()
{
  if (editor2) {
    editor2->spelling_trigger ();
  }
  if (usfmview) {
    usfmview->spelling_trigger ();
  }
}


void WindowEditor::on_new_widget_signal_clicked(GtkButton *button, gpointer user_data)
{
  ((WindowEditor *) user_data)->on_new_widget_signal();
}


void WindowEditor::on_new_widget_signal ()
{
  if (editor2) {
    connect_focus_signals (editor2->new_widget_pointer);
  }
}


void WindowEditor::cut ()
{
  if (usfmview) {
    usfmview->cut ();
  }
  if (editor2) {
    editor2->cut();
  }
}


void WindowEditor::copy ()
{
  if (usfmview) {
    usfmview->copy ();
  }
  if (editor2) {
    editor2->copy();
  }
}


void WindowEditor::paste ()
{
  if (usfmview) {
    usfmview->paste();
  }
  if (editor2) {
    editor2->paste();
  }
}


vector <ustring> WindowEditor::spelling_get_misspelled ()
{
  vector <ustring> misspelled_words;
  if (usfmview)
    misspelled_words = usfmview->spelling_get_misspelled ();
  if (editor2)
    misspelled_words = editor2->spelling_get_misspelled ();
  return misspelled_words;
}


void WindowEditor::spelling_approve (const vector <ustring>& words)
{
  if (usfmview)
    usfmview->spelling_approve (words);
  if (editor2)
    editor2->spelling_approve (words);
}

