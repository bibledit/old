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
  current_view = vtFormatted; // formatted view is default
  currView = NULL;
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
  switch_to_view (current_view, project_name);
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
  if (editor2)  { delete editor2;  editor2 = NULL; }
  if (usfmview) { delete usfmview; usfmview = NULL; }
  currView = NULL;
}


void WindowEditor::go_to(const Reference & reference)
// Let the editor go to a reference.
{
  if (editor2 || usfmview) { // we know currView is set in this case

    // Find out what needs to be changed: book, chapter and/or verse.
    bool new_book = false;
    bool new_chapter = false;
    bool new_verse = false;
    if (currView) {
      Reference currRef = currView->current_reference_get();
      new_book = (reference.book_get() != currRef.book_get());
      new_chapter = (reference.chapter_get() != currRef.chapter_get());
      new_verse = (reference.verse_get() != currRef.verse_get());
      currView->current_reference_set(reference);
    }

    // Save the editor if need be.
    if (new_book || new_chapter) {
      currView->chapter_save();
    }
    
    // With a new book, also load a new chapter.
    if (new_book) {
      new_chapter = true;
      currView->book_set(reference.book_get());
    }

    // Deal with a new chapter.
    if (new_chapter) {
      // Load chapter, if need be.
      currView->chapter_load(reference.chapter_get());
      // When loading a new chapter, there is also a new verse.
      new_verse = true;
    }

    // New reference handling.  
    if (new_book || new_chapter || new_verse) {
      currView->go_to_verse(reference.verse_get(), false);
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
  currView->load_dictionaries();
}

bool WindowEditor::move_cursor_to_spelling_error (bool next, bool extremity)
{
  if (currView) {
    return currView->move_cursor_to_spelling_error (next, extremity);
  }
  gw_warning("Returning true from WindowEditor::move_cursor_to_spelling_error");
  return true;
}


void WindowEditor::undo()
{
  currView->undo(); 
}

void WindowEditor::redo()
{
  currView->redo(); 
}

bool WindowEditor::can_undo()
{
  if (currView) {
    return currView->can_undo();
  }
  gw_warning("Returning false from WindowEditor::can_undo");
  return false;
}

bool WindowEditor::can_redo()
{
  if (currView) {
    return currView->can_redo();
  }
  gw_warning("Returning false from WindowEditor::can_redo");
  return false;
}

EditorTextViewType WindowEditor::last_focused_type()
{
  if (editor2) {
    return editor2->last_focused_type();
  }
  return etvtBody;
}


vector <Reference> WindowEditor::quick_references()
{
  if (editor2) {
    return editor2->quick_references_get();
  }
  gw_warning("Returning empty Reference vector from WindowEditor::quick_references");
  vector <Reference> dummy;
  return dummy;
}


Reference WindowEditor::current_reference()
{
  if (currView) { 
    return currView->current_reference_get();
  }
  gw_warning("Returning empty Reference from WindowEditor::current_reference");
  Reference reference (0);
  return reference;
}


ustring WindowEditor::current_verse_number()
{
  if (currView) {
    return currView->current_verse_get();
  }
  gw_warning("Returning blank from WindowEditor::current_verse_number");
  return "0";
}


ustring WindowEditor::project()
{
  if (currView) {
    return currView->project_get();
  }
  gw_warning("Returning blank from WindowEditor::project");
  return "";
}


ustring WindowEditor::text_get_selection()
{
  if (currView) {
    return currView->text_get_selection();
  }
  gw_warning("Returning blank from WindowEditor::text_get_selection");
  return "";
}


void WindowEditor::insert_text(const ustring &text)
{
  currView->insert_text(text);
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
  if (currView) {
    return currView->word_double_clicked_text_get();
  }
  gw_warning("Returning empty string from WindowEditor::word_double_clicked_text");
  return "";
}


bool WindowEditor::editable()
{
  if (currView) { 
    return currView->editable_get();
  }
  gw_warning("Returning false from WindowEditor::editable");
  return false;
}


void WindowEditor::insert_note(const ustring& marker, const ustring& rawtext)
{
  currView->insert_note (marker, rawtext);
}

ustring WindowEditor::get_chapter()
{
  if (currView) {
    return currView->chapter_get_ustring();
  }
  gw_warning("Returning empty string from WindowEditor::get_chapter");
  return "";
}


void WindowEditor::insert_table(const ustring& rawtext)
{
  currView->insert_table (rawtext);
}


void WindowEditor::chapter_load(unsigned int chapter_in)
{
  currView->chapter_load (chapter_in);
}


void WindowEditor::chapter_save()
{
  currView->chapter_save();
}


unsigned int WindowEditor::reload_chapter_number()
{
  if (currView) { 
    return currView->reload_chapter_num_get();
  }
  gw_warning("Returning 0 from WindowEditor::reload_chapter_number");
  return 0;
}


void WindowEditor::apply_style(const ustring& marker)
{
  currView->apply_style (marker);
}


set <ustring> WindowEditor::get_styles_at_cursor()
{
  if (editor2) {
    return editor2->get_styles_at_cursor();
  }
  gw_warning("Returning dummy ustring set from WindowEditor::get_styles_at_cursor");
  set <ustring> dummy;
  return dummy;
}


void WindowEditor::create_or_update_formatting_data()
{
  currView->create_or_update_formatting_data();
}


void WindowEditor::set_font()
{
  currView->font_set();
}


Editor2 * WindowEditor::editor_get()
{
  if (usfmview) {
    return NULL;
  }
  return editor2; // if editor2 is NULL, we want to return NULL anyway
}


unsigned int WindowEditor::book()
{
  if (currView) { return currView->book_get(); }
  gw_warning("Returning 1 from WindowEditor::book");
  return 1;
}


unsigned int WindowEditor::chapter()
{
  if (currView) { return currView->chapter_num_get(); } 
  gw_warning("Returning 1 from WindowEditor::chapter");
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

void WindowEditor::vt_set (viewType vt)
{
  // Bail out if the setting does not change.
  if (current_view == vt) { return; }

  // Take action.
  current_view = vt;
  switch_to_view (vt, "");
}


void WindowEditor::switch_to_view (viewType vt, ustring project)
// Switch to a new view vt; vtFormatted is default. vtUSFM is the 
// "reveal codes" view.
{
  // If no project was given, then we have switched.
  bool switched = project.empty();
  Reference reference (0);
  
  // Get state of and destroy any previous view, if there was one.
  if (currView) {
    project = currView->project_get();
    reference = currView->current_reference_get();
    delete currView; // this will delete the derived class object, then ChapterView base "object"
    currView = NULL;
    editor2 = NULL;
    usfmview = NULL;
  }

  // Create new view.
  switch (vt) {
  case vtNone: break;

  case vtFormatted:
    editor2 = new Editor2 (vbox, project);
    currView = editor2;
    g_signal_connect ((gpointer) editor2->new_verse_signal, "clicked", G_CALLBACK(on_new_verse_signalled), gpointer(this));
    g_signal_connect ((gpointer) editor2->new_styles_signal, "clicked", G_CALLBACK(on_new_styles_signalled), gpointer(this));
    g_signal_connect ((gpointer) editor2->quick_references_button, "clicked", G_CALLBACK(on_quick_references_signalled), gpointer(this));
    g_signal_connect ((gpointer) editor2->word_double_clicked_signal, "clicked", G_CALLBACK(on_word_double_click_signalled), gpointer(this));
    g_signal_connect ((gpointer) editor2->reload_signal, "clicked", G_CALLBACK(on_reload_signalled), gpointer(this));
    g_signal_connect ((gpointer) editor2->changed_signal, "clicked", G_CALLBACK(on_changed_signalled), gpointer(this));
    g_signal_connect ((gpointer) editor2->spelling_checked_signal, "clicked", G_CALLBACK(on_spelling_checked_signalled), gpointer(this));
    g_signal_connect ((gpointer) editor2->new_widget_signal, "clicked", G_CALLBACK(on_new_widget_signal_clicked), gpointer(this));
    last_focused_widget = editor2->last_focused_widget;
    break;
    
  case vtUSFM:
    usfmview = new USFMView (vbox, project);
    currView = usfmview;
    connect_focus_signals (usfmview->sourceview);
    g_signal_connect ((gpointer) usfmview->reload_signal, "clicked", G_CALLBACK(on_reload_signalled), gpointer(this));
    g_signal_connect ((gpointer) usfmview->changed_signal, "clicked", G_CALLBACK(on_changed_signalled), gpointer(this));
    g_signal_connect ((gpointer) usfmview->new_verse_signal, "clicked", G_CALLBACK(on_new_verse_signalled), gpointer(this));
    g_signal_connect ((gpointer) usfmview->word_double_clicked_signal, "clicked", G_CALLBACK(on_word_double_click_signalled), gpointer(this));
    last_focused_widget = usfmview->sourceview;
    break;

  case vtExperimental:
    break;
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
  currView->spelling_trigger ();
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
  currView->cut();
}


void WindowEditor::copy ()
{
  currView->copy ();
}


void WindowEditor::paste ()
{
  currView->paste();
}


vector <ustring> WindowEditor::spelling_get_misspelled ()
{
  vector <ustring> misspelled_words;
  currView->spelling_get_misspelled ();
  return misspelled_words;
}


void WindowEditor::spelling_approve (const vector <ustring>& words)
{
  currView->spelling_approve (words);
}

