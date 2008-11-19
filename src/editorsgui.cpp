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
#include "editorsgui.h"
#include "settings.h"
#include "tiny_utilities.h"
#include "gwrappers.h"
#include "directories.h"
#include "books.h"
#include "resource_utils.h"
#include "dialogradiobutton.h"

EditorsGUI::EditorsGUI(GtkWidget * notebook_vbox) {
  // Save and initialize varables.
  my_notebook_vbox = notebook_vbox;
  pass_signals_on = false;

  // Build gui.
  notebook = gtk_notebook_new();
  gtk_widget_show(notebook);
  gtk_box_pack_start(GTK_BOX (notebook_vbox), notebook, TRUE, TRUE, 0);

  gtk_notebook_set_scrollable(GTK_NOTEBOOK (notebook), true);

  gtk_widget_grab_focus(notebook);

  // Create signalling buttons.
  focus_button = gtk_button_new();
  new_verse_button = gtk_button_new();
  new_styles_button = gtk_button_new();
  word_double_clicked_button = gtk_button_new();
  editor_reload_button = gtk_button_new();
  editor_changed_button = gtk_button_new();
  quick_references_button = gtk_button_new();

  // Load desktop.
  desktop_load();
}

EditorsGUI::~EditorsGUI() {
  // Destroy signalling buttons.
  gtk_widget_destroy(focus_button);
  gtk_widget_destroy(new_verse_button);
  gtk_widget_destroy(new_styles_button);
  gtk_widget_destroy(word_double_clicked_button);
  gtk_widget_destroy(editor_reload_button);
  gtk_widget_destroy(editor_changed_button);
  gtk_widget_destroy(quick_references_button);

  // Destroy all editors.
  for (unsigned int i = 0; i < editors.size(); i++) {
    delete editors[i];
  }
}

void EditorsGUI::jumpstart(const ustring& project)
// This jumpstarts the editor(s).
{
  // Pass subsequent signals on.
  pass_signals_on = true;

  // Open project - this will give a focus signal because the project artificially changes.
  extern Settings * settings;
  settings->genconfig.project_set("");
  open(project, 1);
}

void EditorsGUI::open(const ustring& project, int method)
{
  // Focus handlers.
  g_signal_connect ((gpointer) editor->focus_signal, "clicked", G_CALLBACK (on_focus_signal_clicked), gpointer(this));
  editor->focus();
  gtk_button_clicked(GTK_BUTTON (focus_button));

  // Other signal handlers.
  g_signal_connect ((gpointer) editor->new_verse_signal, "clicked", G_CALLBACK (on_new_verse_signalled), gpointer(this));
  g_signal_connect ((gpointer) editor->new_styles_signal, "clicked", G_CALLBACK (on_editor_style_changed), gpointer(this));
  g_signal_connect ((gpointer) editor->word_double_clicked_signal, "clicked", G_CALLBACK (on_word_double_clicked), gpointer(this));
  g_signal_connect ((gpointer) editor->reload_signal, "clicked", G_CALLBACK (on_editor_reload_clicked), gpointer(this));
  g_signal_connect ((gpointer) editor->changed_signal, "clicked", G_CALLBACK (on_editor_changed_clicked), gpointer(this));
  g_signal_connect ((gpointer) editor->quick_references_button, "clicked", G_CALLBACK (on_quick_references_signal_button_clicked), gpointer(this));
}

Editor * EditorsGUI::focused_editor()
// Returns the focused editor or NULL if there's none.
{
  for (unsigned int i = 0; i < editors.size(); i++) {
    if (editors[i]->focused()) {
      return editors[i];
    }
  }
  return NULL;
}

void EditorsGUI::go_to(const Reference& reference) {
  // Get the Editor. If none, bail out.
  Editor * editor = focused_editor();
  if (!editor)
    return;

  // Find out what needs to be changed: book, chapter and/or verse.
  bool new_book = (reference.book != editor->current_reference.book);
  for (unsigned int e = 0; e < editors.size(); e++) {
    editors[e]->current_reference.book = reference.book;
  }
  bool new_chapter = (reference.chapter != editor->current_reference.chapter);
  for (unsigned int e = 0; e < editors.size(); e++) {
    editors[e]->current_reference.chapter = reference.chapter;
  }
  bool new_verse = (reference.verse != editor->current_reference.verse);
  for (unsigned int e = 0; e < editors.size(); e++) {
    editors[e]->current_reference.verse = reference.verse;
  }

  // Save the editors if need be.
  if (new_book || new_chapter) {
    save();
  }

  // With a new book, also load a new chapter.
  if (new_book) {
    new_chapter = true;
    for (unsigned int e = 0; e < editors.size(); e++) {
      editors[e]->book_set(reference.book);
    }
  }

  // Deal with a new chapter.
  if (new_chapter) {
    // Load chapter in all Editors, if need be.
    // Handle all Editors.
    for (unsigned int e = 0; e < editors.size(); e++) {
      // Open chapter in editor.
      editors[e]->chapter_load(reference.chapter);
    }
    // When loading a new chapter, there is also a new verse.
    new_verse = true;
  }

  // New reference handling.  
  if (new_book || new_chapter || new_verse) {
    // Position the cursor properly.
    // The positioning will be done whenever Gtk is idle.
    // This is because sometimes Gtk is slow in loading a new chapter.
    // So if the cursor positioning is done straight after loading,
    // it will not work, as there is no text loaded yet.
    // But here we deal with that so that a delay is no longer needed. The trick:
    // Handle all pending events in GTK.
    while (gtk_events_pending())
      gtk_main_iteration();
    for (unsigned int e = 0; e < editors.size(); e++) {
      // Only focus the editor now showing.
      editors[e]->position_cursor_at_verse(reference.verse, editors[e] == editor);
    }
  }

  // Highlighting of searchwords.
  for (unsigned int e = 0; e < editors.size(); e++) {
    if (editors[e]->go_to_new_reference_highlight) {
      editors[e]->highlight_searchwords();
      editors[e]->go_to_new_reference_highlight = false;
    }
  }
}

void EditorsGUI::on_focus_signal_clicked(GtkButton *button, gpointer user_data) {
  ((EditorsGUI *) user_data)->on_focus_signal(button);
}

void EditorsGUI::on_focus_signal(GtkButton *button)
// Handle the change of focus, that is, another editor gets focused.
{
  // Defocus all editors except the focused one.
  for (unsigned int i = 0; i < editors.size(); i++) {
    if (GTK_WIDGET (button) != editors[i]->focus_signal) {
      editors[i]->defocus();
    }
  }

  // Settings.
  extern Settings * settings;

  // Get the focused project.
  ustring project;
  Editor * editor = focused_editor();
  if (editor)
    project = editor->project;

  // If the project changed, store it and give a signal.
  if (project != settings->genconfig.project_get()) {
    settings->genconfig.project_set(project);
    if (pass_signals_on) {
      gtk_button_clicked(GTK_BUTTON (focus_button));
    }
  }

  // The font and colour are tied to the project, 
  // but also stored in the general configuration.
  if (!project.empty()) {
    ProjectConfiguration * projectconfig = settings->projectconfig(project);
    settings->genconfig.text_editor_font_default_set(projectconfig->editor_font_default_get());
    settings->genconfig.text_editor_font_name_set(projectconfig->editor_font_name_get());
    settings->genconfig.text_editor_default_color_set(projectconfig->editor_default_color_get());
    settings->genconfig.text_editor_normal_text_color_set(projectconfig->editor_normal_text_color_get());
    settings->genconfig.text_editor_background_color_set(projectconfig->editor_background_color_get());
    settings->genconfig.text_editor_selected_text_color_set(projectconfig->editor_selected_text_color_get());
    settings->genconfig.text_editor_selection_color_set(projectconfig->editor_selection_color_get());

  }
}

void EditorsGUI::on_button_close_tab_clicked(GtkButton *button, gpointer user_data) {
  ((EditorsGUI *) user_data)->on_button_close_tab(button);
}

void EditorsGUI::on_button_close_tab(GtkButton *button) {
  // Get this button's parent notebook page.
  GtkWidget * notebook_page= NULL;
  for (int i = 0; i < gtk_notebook_get_n_pages(GTK_NOTEBOOK (notebook)); i++) {
    GtkWidget * page = gtk_notebook_get_nth_page(GTK_NOTEBOOK (notebook), i);
    GtkWidget * hbox_tab = gtk_notebook_get_tab_label(GTK_NOTEBOOK (notebook), page);
    GList * children = gtk_container_get_children(GTK_CONTAINER (hbox_tab));
    GList * list= g_list_next (children);
    GtkButton * tab_button= NULL;
    if (list)
      tab_button = GTK_BUTTON (list->data);
    g_list_free(children);
    if (tab_button == button) {
      notebook_page = page;
    }
  }

  // Gather pointers to all editors in the tab that closes.
  vector <Editor *> editors_to_close;
  for (unsigned int i = 0; i < editors.size(); i++) {
    if (notebook_page == editors[i]->parent_notebook_page) {
      editors_to_close.push_back(editors[i]);
    }
  }

  // Close the editors.
  for (unsigned int i = 0; i < editors_to_close.size(); i++) {
    close(editors_to_close[i]);
  }
}

void EditorsGUI::on_button_close_editor_clicked(GtkButton *button, gpointer user_data) {
  ((EditorsGUI *) user_data)->on_button_close_editor(button);
}

void EditorsGUI::on_button_close_editor(GtkButton *button) {
  for (unsigned int i = 0; i < editors.size(); i++) {
    if (GTK_WIDGET (button) == editors[i]->close_button) {
      close(editors[i]);
      break;
    }
  }
}

void EditorsGUI::on_new_verse_signalled(GtkButton *button, gpointer user_data) {
  ((EditorsGUI *) user_data)->on_new_verse();
}

void EditorsGUI::on_new_verse() {
  gtk_button_clicked(GTK_BUTTON (new_verse_button));
}

void EditorsGUI::on_editor_style_changed(GtkButton *button, gpointer user_data) {
  ((EditorsGUI *) user_data)->editor_style_changed();
}

void EditorsGUI::editor_style_changed() {
  gtk_button_clicked(GTK_BUTTON (new_styles_button));
}

void EditorsGUI::on_word_double_clicked(GtkButton *button, gpointer user_data) {
  ((EditorsGUI *) user_data)->word_double_clicked();
}

void EditorsGUI::word_double_clicked() {
  gtk_button_clicked(GTK_BUTTON (word_double_clicked_button));
}

void EditorsGUI::on_editor_reload_clicked(GtkButton *button, gpointer user_data) {
  ((EditorsGUI *) user_data)->on_editor_reload();
}

void EditorsGUI::on_editor_reload() {
  gtk_button_clicked(GTK_BUTTON (editor_reload_button));
}

void EditorsGUI::set_fonts() {
  for (unsigned int e = 0; e < editors.size(); e++) {
    editors[e]->set_font();
    editors[e]->create_or_update_formatting_data();
  }
}

void EditorsGUI::projects_pages_get(vector <ustring>& projects, vector <int> * pages)
// Get a list of projects loaded, and optionally the page numbers they're on.
// projects: To store the list of projects.
// pages: If non-NULL: To store the list of page numbers.
{
  for (int i = 0; i < gtk_notebook_get_n_pages(GTK_NOTEBOOK (notebook)); i++) {
    // Get the notebook page, which is a box for the split view.
    GtkWidget * split_box = gtk_notebook_get_nth_page(GTK_NOTEBOOK (notebook), i);
    // The box for the split view contains one or more vertical boxes for the editors.
    GList * children = gtk_container_get_children(GTK_CONTAINER (split_box));
    GList * list = children;
    do {
      GtkWidget * vbox= GTK_WIDGET (list->data);
      for (unsigned int i2 = 0; i2 < editors.size(); i2++) {
        if (vbox == editors[i2]->parent_vbox) {
          projects.push_back(editors[i2]->project);
          if (pages)
            pages->push_back(i);
        }
      }
      list = g_list_next (list);
    } while (list);
    g_list_free(children);
  }

}

void EditorsGUI::editors_pages_get(vector <Editor *>& editorpointers, vector <int> * pages)
// Get a list of Editors loaded, and optionally the page numbers they're on.
// editorpointers : To store the list of Editors.
// pages: If non-NULL: To store the list of page numbers.
{
  // If no editor is focused, bail out.
  Editor * editor = focused_editor();
  if (!editor)
    return;
  for (int i = 0; i < gtk_notebook_get_n_pages(GTK_NOTEBOOK (notebook)); i++) {
    // Get the notebook page, which is a box for the split view.
    GtkWidget * split_box = gtk_notebook_get_nth_page(GTK_NOTEBOOK (notebook), i);
    // The box for the split view contains one or more vertical boxes for the editors.
    GList * children = gtk_container_get_children(GTK_CONTAINER (split_box));
    GList * list = children;
    if (children) {
      do {
        GtkWidget * vbox= GTK_WIDGET (list->data);
        for (unsigned int i2 = 0; i2 < editors.size(); i2++) {
          if (vbox == editors[i2]->parent_vbox) {
            editorpointers.push_back(editors[i2]);
            if (pages)
              pages->push_back(i);
          }
        }
        list = g_list_next (list);
      } while (list);
      g_list_free(children);
    }
  }
}

void EditorsGUI::next_previous_project(bool next) {
  // Bail out if there are not enough editors to switch.
  if (editors.size() < 2)
    return;

  // The next (or previous) project to focus.
  ustring newproject;

  // Get the projects.
  vector <ustring> projects;
  projects_pages_get(projects, NULL);

  // Get current editor.
  Editor * editor = focused_editor();
  if (!editor)
    return;

  // Get next (or previous) project.
  for (unsigned int i = 0; i < projects.size(); i++) {
    if (editor->project == projects[i]) {
      if (next) {
        if (i < editors.size() - 1)
          newproject = projects[i + 1];
      } else {
        if (i > 0)
          newproject = projects[i - 1];
      }
    }
  }

  // Bail out if no next (previous) project was found.
  if (projects.empty())
    return;

  // Focus the new project.
  open(newproject, 0);
}

bool EditorsGUI::has_focus()
// Returns true if one of the editors has focus.
{
  for (unsigned int e = 0; e < editors.size(); e++) {
    if (editors[e]->has_focus()) {
      return true;
    }
  }
  return false;
}

void EditorsGUI::reload_styles() {
  for (unsigned int e = 0; e < editors.size(); e++) {
    editors[e]->create_or_update_formatting_data();
  }
}

void EditorsGUI::title_bar_display()
// If there are more than one editor in a tab, it shows the title bar.
// Else it hides it.
{
  vector <ustring> projects;
  vector <int> pages;
  projects_pages_get(projects, &pages);
  for (unsigned int i = 0; i < projects.size(); i++) {
    int projects_per_page = 0;
    for (unsigned int i2 = 0; i2 < pages.size(); i2++) {
      if (pages[i] == pages[i2])
        projects_per_page++;
    }
    for (unsigned int i2 = 0; i2 < editors.size(); i2++) {
      if (projects[i] == editors[i2]->project) {
        editors[i2]->title_bar_show(projects_per_page > 1);
      }
    }
  }
}

vector <Editor *> EditorsGUI::visible_editors_get()
// Gives a list of now visible editors.
{
  // Storage for the visible editors.
  vector <Editor *> visible_editors;

  // Get the focused editor.
  Editor * editor = focused_editor();

  // Get all editors, and on which pages they are.
  vector <Editor *> all_editors;
  vector <int> pages;
  editors_pages_get(all_editors, &pages);

  // Get the page number of the focused editor.
  int page = -1;
  for (unsigned int i = 0; i < all_editors.size(); i++) {
    if (editor == all_editors[i])
      page = pages[i];
  }

  // Get all the editors that are on that page.  
  for (unsigned int i = 0; i < all_editors.size(); i++) {
    if (page == pages[i]) {
      visible_editors.push_back(all_editors[i]);
    }
  }

  // Give the list.
  return visible_editors;
}

void EditorsGUI::on_editor_changed_clicked(GtkButton *button, gpointer user_data)
// This signal will be called if anything in any of the editors changed.
{
  ((EditorsGUI *) user_data)->on_editor_changed();
}

void EditorsGUI::on_editor_changed() {
  gtk_button_clicked(GTK_BUTTON (editor_changed_button));
}

ustring EditorsGUI::get_text(const ustring& project) {
  ustring text;
  for (unsigned int i = 0; i < editors.size(); i++) {
    if (editors[i]->project == project) {
      text = editors[i]->get_chapter();
    }
  }
  return text;
}

void EditorsGUI::reload_chapter(unsigned int book, unsigned int chapter) {
  for (unsigned int i = 0; i < editors.size(); i++) {
    editors[i]->chapter_load(chapter);
  }
}

void EditorsGUI::reload_dictionaries()
// Reloads the dictionaries of the focused project.
{
  Editor * editor = focused_editor();
  if (editor) {
    editor->load_dictionaries();
  }
}

void EditorsGUI::on_quick_references_signal_button_clicked(GtkButton *button, gpointer user_data) {
  ((EditorsGUI *) user_data)->on_quick_references_signal_button(button);
}

void EditorsGUI::on_quick_references_signal_button(GtkButton *button) {
  GtkWidget * widget = GTK_WIDGET (button);
  for (unsigned int i = 0; i < editors.size(); i++) {
    if (widget == editors[i]->quick_references_button) {
      quick_references = editors[i]->quick_references;
    }
  }

  gtk_button_clicked (GTK_BUTTON (quick_references_button));
}
