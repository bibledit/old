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


EditorsGUI::EditorsGUI (GtkWidget * notebook_vbox)
{
  // Save and initialize varables.
  my_notebook_vbox = notebook_vbox;
  pass_signals_on = false;

  // Build gui.
  notebook = gtk_notebook_new ();
  gtk_widget_show (notebook);
  gtk_box_pack_start (GTK_BOX (notebook_vbox), notebook, TRUE, TRUE, 0);
  
  gtk_notebook_set_scrollable (GTK_NOTEBOOK (notebook), true);

  gtk_widget_grab_focus (notebook);

  // Create signalling buttons.
  focus_button = gtk_button_new ();
  new_verse_button = gtk_button_new ();
  new_styles_button = gtk_button_new ();
  word_double_clicked_button = gtk_button_new ();
  editor_reload_button = gtk_button_new ();
  editor_changed_button = gtk_button_new ();
  
  // Load desktop.
  desktop_load ();
}


EditorsGUI::~EditorsGUI ()
{
  // Destroy signalling buttons.
  gtk_widget_destroy (focus_button);
  gtk_widget_destroy (new_verse_button);
  gtk_widget_destroy (new_styles_button);
  gtk_widget_destroy (word_double_clicked_button);
  gtk_widget_destroy (editor_reload_button);
  gtk_widget_destroy (editor_changed_button);
  
  // Save desktop.
  desktop_save ();
  
  // Destroy all editors.
  for (unsigned int i = 0; i < editors.size (); i++) {
    delete editors[i];
  }
}


void EditorsGUI::jumpstart (const ustring& project)
// This jumpstarts the editor(s).
{
  // Pass subsequent signals on.
  pass_signals_on = true;
  
  // Open project - this will give a focus signal because the project artificially changes.
  extern Settings * settings;
  settings->genconfig.project_set ("");
  open (project, 1);
}


void EditorsGUI::open (const ustring& project, int method)
/*
Opens a new editor.
project : The project to open in the editor.
method  : -1 : Ask.
           0 : In current window.
           1 : In a new tab.
*/
{
  // Bail out if the project is empty.
  if (project.empty ()) return;
    
  // If this project is open already, switch to it and focus it.
  for (unsigned int i = 0; i < editors.size(); i++) {
    if (project == editors[i]->project) {
      for (int i2 = 0; i2 < gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook)); i2++) {
        if (editors[i]->parent_notebook_page == gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), i2)) {
          gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), i2);
          editors[i]->focus ();
          gtk_button_clicked (GTK_BUTTON (focus_button));
          return;
        }
      }
    }
  }
  
  // Ask where to open the editor if the method is negative 
  // and if there are no editors yet.
  if (editors.empty ()) {
    method = 1;
  } else {
    if (method < 0) {
      method = 0;
      vector <ustring> labels;
      labels.push_back ("In the current window");
      labels.push_back ("In a new tab");
      extern Settings * settings;
      RadiobuttonDialog dialog ("Open project", "Where would you like this editor to be displayed?", labels, settings->genconfig.split_view_open_method_get ());
      if (dialog.run () == GTK_RESPONSE_OK) {
        method = dialog.selection;
        settings->genconfig.split_view_open_method_set (method);
      }
    }
  }
  

  // The vertical (or horizontal) box for the split view, and the tab label.
  GtkWidget *box_split;
  GtkWidget *label;

  // Handle display method, whether a new tab or to add it to the current tab.
  if (method == 1) {
    
    // Display in a new tab.

    // Create the vertical or horizontal box for the split view.
    extern Settings * settings;
    if (settings->genconfig.split_view_editor_top_down_layout_get ()) {
      box_split = gtk_vbox_new (true, 0);
    } else {
      box_split = gtk_hbox_new (true, 0);
    }
    gtk_widget_show (box_split);

    // The label for the notebook tab, and the close button.
    GtkWidget *hbox_tab;
    hbox_tab = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox_tab);

    label = gtk_label_new ("");
    gtk_widget_show (label);
    gtk_box_pack_start (GTK_BOX (hbox_tab), label, FALSE, FALSE, 0);

    // Get the default height of the label.
    GtkRequisition sizerequisition;
    gtk_widget_size_request (label, &sizerequisition);
    gint default_label_height = sizerequisition.height;  
  
    GtkWidget *button_close;
    button_close = gtk_button_new ();
    gtk_widget_show (button_close);
    gtk_box_pack_start (GTK_BOX (hbox_tab), button_close, FALSE, FALSE, 0);
    gtk_button_set_focus_on_click (GTK_BUTTON (button_close), false);

    g_signal_connect ((gpointer) button_close, "clicked", G_CALLBACK (on_button_close_tab_clicked), gpointer(this));

    GtkWidget *alignment;
    alignment = gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_widget_show (alignment);
    gtk_container_add (GTK_CONTAINER (button_close), alignment);

    GtkWidget *hbox2;
    hbox2 = gtk_hbox_new (FALSE, 2);
    gtk_widget_show (hbox2);
    gtk_container_add (GTK_CONTAINER (alignment), hbox2);

    GtkWidget *image;
    image = gtk_image_new_from_stock ("gtk-close", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_box_pack_start (GTK_BOX (hbox2), image, FALSE, FALSE, 0);
    gtk_widget_set_size_request (image, default_label_height / 3, default_label_height / 3);

    // Append a page to the notebook with the paneds and the label and the button.
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), box_split, hbox_tab);
    gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), -1);
    
  } else {
    
    // Display in existing tab.
    gint pagenumber = gtk_notebook_get_current_page (GTK_NOTEBOOK (notebook));
    box_split = gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), pagenumber);
    GtkWidget *hbox = gtk_notebook_get_tab_label (GTK_NOTEBOOK (notebook), box_split);
    GList * children = gtk_container_get_children (GTK_CONTAINER (hbox));
    label = GTK_WIDGET (children->data);
    g_list_free (children);
    
  }

  // Add a new vertical box for holding the editor.
  GtkWidget *vbox;
  vbox = gtk_vbox_new (false, 0);
  gtk_widget_show (vbox);
  gtk_box_pack_start (GTK_BOX (box_split), vbox, TRUE, TRUE, 0);
  
  // Create the new editor.
  Editor * editor = new Editor (vbox, gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), -1), label, project);
  editors.push_back (editor);
  
  // Focus handlers.
  g_signal_connect ((gpointer) editor->focus_signal, "clicked", G_CALLBACK (on_focus_signal_clicked), gpointer(this));
  editor->focus ();
  gtk_button_clicked (GTK_BUTTON (focus_button));

  // Close handler.
  g_signal_connect ((gpointer) editor->close_button, "clicked", G_CALLBACK (on_button_close_editor_clicked), gpointer(this));
  
  // Other signal handlers.
  g_signal_connect ((gpointer) editor->new_verse_signal, "clicked", G_CALLBACK (on_new_verse_signalled), gpointer(this));
  g_signal_connect ((gpointer) editor->new_styles_signal, "clicked", G_CALLBACK (on_editor_style_changed), gpointer(this));
  g_signal_connect ((gpointer) editor->word_double_clicked_signal, "clicked", G_CALLBACK (on_word_double_clicked), gpointer(this));
  g_signal_connect ((gpointer) editor->reload_signal, "clicked", G_CALLBACK (on_editor_reload_clicked), gpointer(this));
  g_signal_connect ((gpointer) editor->changed_signal, "clicked", G_CALLBACK (on_editor_changed_clicked), gpointer(this));
  
  // Set the quick references view.
  editor->quick_references_textview_set (quick_references_textview);
  
  // If there is more than one tab, show them.
  gint page_count = gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook));
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (notebook), page_count > 1);
  
  // Show or hide title bars.
  title_bar_display ();
}


void EditorsGUI::close ()
// Closes the focused editor.
{
  Editor * editor = focused_editor ();
  close (editor);
}


void EditorsGUI::close (Editor * editor)
// Closes "editor".
{
  // Bail out if there was no editor.
  if (editor == NULL) return;
    
  // Erase the editor's pointer from the container.
  vector<Editor *>::iterator iter;
  iter = editors.begin();
  for (unsigned int i = 0; i < editors.size (); i++) {
    if (editor == editors[i]) {
      editors.erase (iter);
      break;
    }
    iter++;
  }

  // Get the page of this editor.
  GtkWidget * page = editor->parent_notebook_page;
  
  // Delete the editor.
  delete editor;

  // Count the number of remaining editors that use this notebook page.
  // At the same time focus the first one using it.
  unsigned int editors_using_this_page = 0;
  bool editor_focused = false;
  for (unsigned int i = 0; i < editors.size (); i++) {
    if (page == editors[i]->parent_notebook_page) {
      editors_using_this_page++;
      if (!editor_focused) {
        editors[i]->focus ();
        editor_focused = true;
      }
    }
  }
  
  // Delete the page from the notebook if no editor is using it anymore.
  if (editors_using_this_page == 0) {
    for (int i = 0; i < gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook)); i++) {
      if (page == gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), i)) {
        gtk_notebook_remove_page (GTK_NOTEBOOK (notebook), i);
        break;
      }
    }
  }
  
  // If there is only one tab left, or less, tabs hide.
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (notebook), gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook)) > 1);
  
  // Clear project is there's no project left.
  extern Settings * settings;
  if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook)) == 0) {
    settings->genconfig.project_set ("");
  }
  
  // Click focus button so as to signal the main window.
  gtk_button_clicked (GTK_BUTTON (focus_button));
  
  // Show or hide title bars.
  title_bar_display ();
}


Editor * EditorsGUI::focused_editor ()
// Returns the focused editor or NULL if there's none.
{
  for (unsigned int i = 0; i < editors.size (); i++) {
    if (editors[i]->focused ()) {
      return editors[i];
    }
  }
  return NULL;
}


void EditorsGUI::go_to (const Reference& reference)
{
  // Get the Editor. If none, bail out.
  Editor * editor = focused_editor ();
  if (!editor) return;

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
    save ();
  }

  // With a new book, also load a new chapter.
  if (new_book) {
    new_chapter = true;
    for (unsigned int e = 0; e < editors.size(); e++) {
      editors[e]->book_set (reference.book);
    }
  }

  // Deal with a new chapter.
  if (new_chapter) {
    // Load chapter in all Editors, if need be.
    // Handle all Editors.
    for (unsigned int e = 0; e < editors.size(); e++) {
      // Open chapter in editor.
      editors[e]->chapter_load (reference.chapter);
    }
    // When loading a new chapter, there is also a new verse.
    new_verse = true;
  }

  // New reference handling.  
  if (new_book || new_chapter || new_verse)
  {
    // Position the cursor properly.
    // The positioning will be done whenever Gtk is idle.
    // This is because sometimes Gtk is slow in loading a new chapter.
    // So if the cursor positioning is done straight after loading,
    // it will not work, as there is no text loaded yet.
    // But here we deal with that so that a delay is no longer needed. The trick:
    // Handle all pending events in GTK.
    while (gtk_events_pending ()) gtk_main_iteration ();
    for (unsigned int e = 0; e < editors.size(); e++) {
      // Only focus the editor now showing.
      editors[e]->position_cursor_at_verse (reference.verse, editors[e] == editor);
    }
  }

  // Highlighting of searchwords.
  for (unsigned int e = 0; e < editors.size(); e++) {
    if (editors[e]->go_to_new_reference_highlight) {
      editors[e]->highlight_searchwords ();
      editors[e]->go_to_new_reference_highlight = false;
    }
  }
}


void EditorsGUI::on_focus_signal_clicked (GtkButton *button, gpointer user_data)
{
  ((EditorsGUI *) user_data)->on_focus_signal (button);
}


void EditorsGUI::on_focus_signal (GtkButton *button)
// Handle the change of focus, that is, another editor gets focused.
{
  // Defocus all editors except the focused one.
  for (unsigned int i = 0; i < editors.size (); i++) {
    if (GTK_WIDGET (button) != editors[i]->focus_signal) {
      editors[i]->defocus ();
    }
  }

  // Settings.
  extern Settings * settings;
  
  // Get the focused project.
  ustring project;
  Editor * editor = focused_editor ();
  if (editor) project = editor->project;

  // If the project changed, store it and give a signal.
  if (project != settings->genconfig.project_get ()) {
    settings->genconfig.project_set (project);
    if (pass_signals_on) {
      gtk_button_clicked (GTK_BUTTON (focus_button));
    }
  }
  
  // The font and colour are tied to the project, 
  // but also stored in the general configuration.
  if (!project.empty ()) {
    ProjectConfiguration * projectconfig = settings->projectconfig (project);
    settings->genconfig.text_editor_font_default_set        (projectconfig->editor_font_default_get ());
    settings->genconfig.text_editor_font_name_set           (projectconfig->editor_font_name_get ());
    settings->genconfig.text_editor_default_color_set       (projectconfig->editor_default_color_get ());
    settings->genconfig.text_editor_normal_text_color_set   (projectconfig->editor_normal_text_color_get ());
    settings->genconfig.text_editor_background_color_set    (projectconfig->editor_background_color_get ());
    settings->genconfig.text_editor_selected_text_color_set (projectconfig->editor_selected_text_color_get ());
    settings->genconfig.text_editor_selection_color_set     (projectconfig->editor_selection_color_get ());
    
  }
}


void EditorsGUI::desktop_save ()
// Save the editor desktop.
{
  // Get the projects, and the notebook page they are on.
  vector <ustring> projectnames;
  vector <int> pagenumbers;
  projects_pages_get (projectnames, &pagenumbers);

  // Save them, including their page numbers.
  extern Settings * settings;
  settings->genconfig.projects_set (projectnames);
  settings->genconfig.project_pages_set (pagenumbers);
}


void EditorsGUI::desktop_load ()
// Load the saved resources desktop.
{
  // Get the projects and their page numbers in the notebook.
  extern Settings * settings;
  vector <ustring> projects = settings->genconfig.projects_get ();
  vector <int> pagenumbers = settings->genconfig.project_pages_get ();
  if (projects.size () != pagenumbers.size ()) {
    pagenumbers.clear ();
    for (unsigned int i = 0; i < projects.size (); i++) {
      pagenumbers.push_back (i);
    }
  }
  // Open the projects in the right notebook pages.
  int previouspage = -1;
  for (unsigned int i = 0; i < projects.size (); i++) {
    open (projects[i], pagenumbers[i] - previouspage);
    previouspage = pagenumbers[i];
  }
  // Clear projects and page numbers as these may no longer be in sync with 
  // the actual situation.
  projects.clear();
  settings->genconfig.projects_set (projects);
  pagenumbers.clear ();
  settings->genconfig.project_pages_set (pagenumbers);  
}


void EditorsGUI::on_button_close_tab_clicked (GtkButton *button, gpointer user_data)
{
  ((EditorsGUI *) user_data)->on_button_close_tab (button);
}


void EditorsGUI::on_button_close_tab (GtkButton *button)
{
  // Get this button's parent notebook page.
  GtkWidget * notebook_page = NULL;
  for (int i = 0; i < gtk_notebook_get_n_pages GTK_NOTEBOOK (notebook); i++) {
    GtkWidget * page = gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), i);
    GtkWidget * hbox_tab = gtk_notebook_get_tab_label (GTK_NOTEBOOK (notebook), page);
    GList * children = gtk_container_get_children (GTK_CONTAINER (hbox_tab));
    GList * list = g_list_next (children);
    GtkButton * tab_button = NULL;
    if (list) tab_button = GTK_BUTTON (list->data);
    g_list_free (children);
    if (tab_button == button) {
      notebook_page = page;
    }
  }
  
  // Gather pointers to all editors in the tab that closes.
  vector <Editor *> editors_to_close;
  for (unsigned int i = 0; i < editors.size (); i++) {
    if (notebook_page == editors[i]->parent_notebook_page) {
      editors_to_close.push_back (editors[i]);
    }
  }
  
  // Close the editors.
  for (unsigned int i = 0; i < editors_to_close.size (); i++) {
    close (editors_to_close[i]);
  }
}


void EditorsGUI::on_button_close_editor_clicked (GtkButton *button, gpointer user_data)
{
  ((EditorsGUI *) user_data)->on_button_close_editor (button);
}


void EditorsGUI::on_button_close_editor (GtkButton *button)
{
  for (unsigned int i = 0; i < editors.size (); i++) {
    if (GTK_WIDGET (button) == editors[i]->close_button) {
      close (editors[i]);
      break;
    }
  }
}


void EditorsGUI::save ()
// Save all the editors.
{
  for (unsigned int e = 0; e < editors.size(); e++) {
    editors[e]->chapter_save ();
  }
}


void EditorsGUI::on_new_verse_signalled (GtkButton *button, gpointer user_data)
{
  ((EditorsGUI *) user_data)->on_new_verse ();
}


void EditorsGUI::on_new_verse ()
{
  gtk_button_clicked (GTK_BUTTON (new_verse_button));
}


void EditorsGUI::on_editor_style_changed (GtkButton *button, gpointer user_data)
{
  ((EditorsGUI *) user_data)->editor_style_changed ();
}


void EditorsGUI::editor_style_changed ()
{
  gtk_button_clicked (GTK_BUTTON (new_styles_button));
}


void EditorsGUI::quick_references_textview_set (GtkWidget * widget)
{
  quick_references_textview = widget;
  for (unsigned int i = 0; i < editors.size (); i++) {
    editors[i]->quick_references_textview_set (quick_references_textview);
  }
}


void EditorsGUI::on_word_double_clicked (GtkButton *button, gpointer user_data)
{
  ((EditorsGUI *) user_data)->word_double_clicked ();
}


void EditorsGUI::word_double_clicked ()
{
  gtk_button_clicked (GTK_BUTTON (word_double_clicked_button));
}


void EditorsGUI::on_editor_reload_clicked (GtkButton *button, gpointer user_data)
{
  ((EditorsGUI *) user_data)->on_editor_reload ();
}


void EditorsGUI::on_editor_reload ()
{
  gtk_button_clicked (GTK_BUTTON (editor_reload_button));
}


void EditorsGUI::set_fonts ()
{
  for (unsigned int e = 0; e < editors.size(); e++) {
    editors[e]->set_font ();
    editors[e]->create_or_update_formatting_data ();
  }
}


void EditorsGUI::projects_pages_get (vector <ustring>& projects, vector <int> * pages)
// Get a list of projects loaded, and optionally the page numbers they're on.
// projects: To store the list of projects.
// pages: If non-NULL: To store the list of page numbers.
{
  for (int i = 0; i < gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook)); i++) {
    // Get the notebook page, which is a box for the split view.
    GtkWidget * split_box = gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), i);
    // The box for the split view contains one or more vertical boxes for the editors.
    GList * children = gtk_container_get_children (GTK_CONTAINER (split_box));
    GList * list = children;
    do {
      GtkWidget * vbox = GTK_WIDGET (list->data);
      for (unsigned int i2 = 0; i2 < editors.size (); i2++) {
        if (vbox == editors[i2]->parent_vbox) {
          projects.push_back (editors[i2]->project);
          if (pages) pages->push_back (i);
        }
      }
      list = g_list_next (list);
    } while (list);
    g_list_free (children);
  }

}


void EditorsGUI::editors_pages_get (vector <Editor *>& editorpointers, vector <int> * pages)
// Get a list of Editors loaded, and optionally the page numbers they're on.
// editorpointers : To store the list of Editors.
// pages: If non-NULL: To store the list of page numbers.
{
  // If no editor is focused, bail out.
  Editor * editor = focused_editor ();
  if (!editor) return;
  for (int i = 0; i < gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook)); i++) {
    // Get the notebook page, which is a box for the split view.
    GtkWidget * split_box = gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), i);
    // The box for the split view contains one or more vertical boxes for the editors.
    GList * children = gtk_container_get_children (GTK_CONTAINER (split_box));
    GList * list = children;
    if (children) {
      do {
        GtkWidget * vbox = GTK_WIDGET (list->data);
        for (unsigned int i2 = 0; i2 < editors.size (); i2++) {
          if (vbox == editors[i2]->parent_vbox) {
            editorpointers.push_back (editors[i2]);
            if (pages) pages->push_back (i);
          }
        }
        list = g_list_next (list);
      } while (list);
      g_list_free (children);
    }
  }
}


void EditorsGUI::next_previous_project (bool next)
{
  // Bail out if there are not enough editors to switch.
  if (editors.size () < 2) return;

  // The next (or previous) project to focus.
  ustring newproject;
  
  // Get the projects.
  vector <ustring> projects;
  projects_pages_get (projects, NULL);
  
  // Get current editor.
  Editor * editor = focused_editor ();
  if (!editor) return;
    
  // Get next (or previous) project.
  for (unsigned int i = 0; i < projects.size (); i++) {
    if (editor->project == projects[i]) {
      if (next) {
        if (i < editors.size () - 1)
          newproject = projects[i + 1];
      } else {
        if (i > 0)
          newproject = projects[i - 1];
      }
    }
  }

  // Bail out if no next (previous) project was found.
  if (projects.empty ()) return;
    
  // Focus the new project.
  open (newproject, 0);
}


bool EditorsGUI::has_focus ()
// Returns true if one of the editors has focus.
{
  for (unsigned int e = 0; e < editors.size(); e++) {
    if (editors[e]->has_focus ()) {
      return true;
    }
  }
  return false;
}


void EditorsGUI::reload_styles ()
{
  for (unsigned int e = 0; e < editors.size(); e++) {
    editors[e]->create_or_update_formatting_data ();
  }
}


void EditorsGUI::title_bar_display ()
// If there are more than one editor in a tab, it shows the title bar.
// Else it hides it.
{
  vector <ustring> projects;
  vector <int> pages;
  projects_pages_get (projects, &pages);
  for (unsigned int i = 0; i < projects.size (); i++) {
    int projects_per_page = 0;
    for (unsigned int i2 = 0; i2 < pages.size (); i2++ ) {
      if (pages[i] == pages[i2])
        projects_per_page++;
    }
    for (unsigned int i2 = 0; i2 < editors.size (); i2++) {
      if (projects[i] == editors[i2]->project) {
        editors[i2]->title_bar_show (projects_per_page > 1);
      }
    }
  }
}


vector <Editor *> EditorsGUI::visible_editors_get ()
// Gives a list of now visible editors.
{
  // Storage for the visible editors.
  vector <Editor *> visible_editors;
  
  // Get the focused editor.
  Editor * editor = focused_editor ();

  // Get all editors, and on which pages they are.
  vector <Editor * > all_editors;
  vector <int> pages;  
  editors_pages_get (all_editors, &pages);
  
  // Get the page number of the focused editor.
  int page = -1;
  for (unsigned int i = 0; i < all_editors.size (); i++) {
    if (editor == all_editors[i])
      page = pages[i];
  }

  // Get all the editors that are on that page.  
  for (unsigned int i = 0; i < all_editors.size (); i++) {
    if (page == pages[i]) {
      visible_editors.push_back (all_editors[i]);
    }
  }
  
  // Give the list.
  return visible_editors;
}


void EditorsGUI::on_editor_changed_clicked (GtkButton *button, gpointer user_data)
// This signal will be called if anything in any of the editors changed.
{
  ((EditorsGUI *) user_data)->on_editor_changed ();
}


void EditorsGUI::on_editor_changed ()
{
  gtk_button_clicked (GTK_BUTTON (editor_changed_button));
}


ustring EditorsGUI::get_text (const ustring& project)
{
  ustring text;
  for (unsigned int i = 0; i < editors.size (); i++) {
    if (editors[i]->project == project) {
      text = editors[i]->get_chapter ();
    }
  }
  return text;
}


void EditorsGUI::reload_chapter (unsigned int book, unsigned int chapter)
{
  for (unsigned int i = 0; i < editors.size (); i++) {
    editors[i]->chapter_load (chapter);
  }
}


void EditorsGUI::reload_dictionaries ()
// Reloads the dictionaries of the focused project.
{
  Editor * editor = focused_editor ();
  if (editor) {
    editor->load_dictionaries ();
  }
}
