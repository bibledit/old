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

#include "libraries.h"
#include <glib.h>
#include "windowreferences.h"
#include "help.h"
#include "windows.h"
#include "keyterms.h"
#include "tiny_utilities.h"
#include "projectutils.h"
#include "settings.h"
#include "references.h"
#include "keyboard.h"
#include "dialogentry3.h"
#include "gtkwrappers.h"
#include "referenceutils.h"

 WindowReferences::WindowReferences(GtkAccelGroup * accelerator_group, bool startup):
WindowBase(widReferences, "References", startup, 0), reference(0, 0, "")
// Window for showing the quick references.  
{
  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow);
  gtk_container_add(GTK_CONTAINER(window), scrolledwindow);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  // Manually added and changed.
  // 1. localized human readable reference
  // 2. comment
  // 3. book id
  // 4. chapter
  // 5. verse
  liststore = gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING);
  // Text cell renderer.
  GtkCellRenderer *renderer;
  renderer = gtk_cell_renderer_text_new();

  treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(liststore));
  gtk_widget_show(treeview);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), treeview);

  g_signal_connect((gpointer) treeview, "visibility-notify-event", G_CALLBACK(on_visibility_notify_event), gpointer(this));

  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), TRUE);
  // Unreference the store once, so it gets destroyed with the treeview.
  g_object_unref(liststore);
  // Add reference column.
  treecolumn = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), treecolumn);
  // Add comments column
  GtkTreeViewColumn *treecolumn2;
  treecolumn2 = gtk_tree_view_column_new_with_attributes("Comment", renderer, "text", 1, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), treecolumn2);
  treeselect = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  gtk_tree_selection_set_mode(treeselect, GTK_SELECTION_MULTIPLE);

  g_signal_connect((gpointer) treeview, "key_press_event", G_CALLBACK(on_treeview_key_press_event), gpointer(this));
  g_signal_connect((gpointer) treeview, "button_press_event", G_CALLBACK(on_treeview_button_press_event), gpointer(this));
  g_signal_connect((gpointer) treeview, "popup_menu", G_CALLBACK(on_treeview_popup_menu), gpointer(this));
  g_signal_connect((gpointer) treeview, "move_cursor", G_CALLBACK(on_treeview_move_cursor), gpointer(this));
  g_signal_connect((gpointer) treeview, "cursor_changed", G_CALLBACK(on_treeview_cursor_changed), gpointer(this));

  // Load previously saved references.
  References references(liststore, treeview, treecolumn);
  references.load();
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  references.fill_store(projectconfig->language_get());

  // Signal button.
  general_signal_button = gtk_button_new();
}

WindowReferences::~WindowReferences()
{
  // Save references.
  References references(liststore, treeview, treecolumn);
  references.get_loaded();
  references.save();
  // Destroy signal button.
  gtk_widget_destroy(general_signal_button);
}

void WindowReferences::display(vector < Reference > &refs)
{
  References references(liststore, treeview, treecolumn);
  references.set_references(refs);
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  references.fill_store(projectconfig->language_get());
}

gboolean WindowReferences::on_treeview_key_press_event(GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  // Pressing Return on the keyboard, or Enter on the numerical keypad make us go to the reference.
  if (keyboard_enter_pressed(event)) {
    ((WindowReferences *) user_data)->activate();
  }
  // Pressing Delete takes the reference(s) out that have been selected.
  if (keyboard_delete_pressed(event)) {
    ((WindowReferences *) user_data)->dismiss();
  }
  return FALSE;
}

gboolean WindowReferences::on_treeview_button_press_event(GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
  // Double-clicking a references makes us go to the reference.
  if (event->type == GDK_2BUTTON_PRESS) {
    ((WindowReferences *) user_data)->activate();
    return true;
  }
  // Popup menu handler.
  if (event->button == 3 && event->type == GDK_BUTTON_PRESS) {
    ((WindowReferences *) user_data)->show_popup_menu(widget, event);
    return true;
  }
  return false;
}

gboolean WindowReferences::on_treeview_popup_menu(GtkWidget * widget, gpointer user_data)
{
  ((WindowReferences *) user_data)->treeview_references_popup_menu(widget);
  return true;                  // Do not call the original handler.
}

gboolean WindowReferences::on_treeview_move_cursor(GtkTreeView * treeview, GtkMovementStep step, gint count, gpointer user_data)
{
  return false;
}

void WindowReferences::on_treeview_cursor_changed(GtkTreeView * treeview, gpointer user_data)
{
  ((WindowReferences *) user_data)->treeview_references_display_quick_reference();
}

void WindowReferences::activate()
{
  // Get the reference the user selected.
  GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  gtk_tree_selection_selected_foreach(selection, references_window_selection_foreach_function, gpointer(this));
  // Signal the action.
  action = wratReferenceActivated;
  gtk_button_clicked(GTK_BUTTON(general_signal_button));
}

void WindowReferences::references_window_selection_foreach_function(GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
  unsigned int book, chapter;
  gchar *verse;
  gtk_tree_model_get(model, iter, 2, &book, 3, &chapter, 4, &verse, -1);
  ((WindowReferences *) data)->reference.book = book;
  ((WindowReferences *) data)->reference.chapter = chapter;
  ((WindowReferences *) data)->reference.verse = verse;
  g_free(verse);
}

void WindowReferences::dismiss()
{
  // Delete each selected row.
  GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  vector < GtkTreeIter > iters;
  gtk_tree_selection_selected_foreach(selection, WindowReferences::on_collect_iters, gpointer(&iters));
  for (unsigned int i = 0; i < iters.size(); i++) {
    GtkTreeIter iter = iters[i];
    gtk_list_store_remove(liststore, &iter);
  }
  // Update heading.
  References references(liststore, treeview, treecolumn);
  references.get_loaded();
  references.set_header();
}

void WindowReferences::on_collect_iters(GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
  ((vector < GtkTreeIter > *)data)->push_back(*iter);
}

void WindowReferences::show_popup_menu(GtkWidget * my_widget, GdkEventButton * event)
{
  if (event) {
    popup_button = event->button;
    popup_event_time = event->time;
  } else {
    popup_button = 0;
    popup_event_time = gtk_get_current_event_time();
  }
  action = wratPopupMenu;
  gtk_button_clicked(GTK_BUTTON(general_signal_button));
}

gboolean WindowReferences::on_treeview_references_popup_menu(GtkWidget * widget, gpointer user_data)
{
  ((WindowReferences *) user_data)->treeview_references_popup_menu(widget);
  return true;                  // Do not call the original handler.
}

void WindowReferences::treeview_references_popup_menu(GtkWidget * widget)
{
  show_popup_menu(widget, NULL);
}

void WindowReferences::open()
{
  // Settings.
  extern Settings *settings;
  // Ask for a file.
  ustring filename = gtkw_file_chooser_open(window, "Open File", settings->genconfig.references_file_get());
  if (filename.empty())
    return;
  // Allow for up to three words to search for in these references.
  ustring searchword1, searchword2, searchword3;
  vector < ustring > import_references_searchwords = settings->session.import_references_searchwords;
  for (unsigned int i = 0; i < import_references_searchwords.size(); i++) {
    if (i == 0)
      searchword1 = import_references_searchwords[i];
    if (i == 1)
      searchword2 = import_references_searchwords[i];
    if (i == 2)
      searchword3 = import_references_searchwords[i];
  }
  Entry3Dialog dialog2("Search for", true, "Optionally enter _1st searchword", searchword1, "Optionally enter _2nd searchword", searchword2, "Optioanlly enter _3rd searchword", searchword3);
  int result = dialog2.run();
  if (result == GTK_RESPONSE_OK) {
    searchword1 = dialog2.entered_value1;
    searchword2 = dialog2.entered_value2;
    searchword3 = dialog2.entered_value3;
    import_references_searchwords.clear();
    if (!searchword1.empty())
      import_references_searchwords.push_back(searchword1);
    if (!searchword2.empty())
      import_references_searchwords.push_back(searchword2);
    if (!searchword3.empty())
      import_references_searchwords.push_back(searchword3);
    settings->session.import_references_searchwords = import_references_searchwords;
    settings->genconfig.references_file_set(filename);
    References references(liststore, treeview, treecolumn);
    references.load(settings->genconfig.references_file_get());
    ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
    references.fill_store(projectconfig->language_get());
    if (import_references_searchwords.size() > 0) {
      settings->session.highlights.clear();
      for (unsigned int i = 0; i < import_references_searchwords.size(); i++) {
        SessionHighlights sessionhighlights(import_references_searchwords[i], false, false, false, false, atRaw, false, false, false, false, false, false, false, false);
        settings->session.highlights.push_back(sessionhighlights);
      }
    }
  }
}

void WindowReferences::save()
{
  extern Settings *settings;
  try {
    ustring filename = gtkw_file_chooser_save(window, "", settings->genconfig.references_file_get());
    if (filename.empty())
      return;
    settings->genconfig.references_file_set(filename);
    References references(liststore, treeview, treecolumn);
    // Hack: set references with a dummy, then load the real ones from the editor.
    vector < Reference > dummy;
    references.set_references(dummy);
    references.get_loaded();
    references.save(filename);
  }
  catch(exception & ex) {
    cerr << "Saving references: " << ex.what() << endl;
  }
}

void WindowReferences::clear()
{
  References references(liststore, treeview, treecolumn);
  references.fill_store("");
  extern Settings *settings;
  settings->session.highlights.clear();
}

void WindowReferences::hide()
{
  // Load currently hidden references.
  vector < ustring > hidden_references = references_hidden_ones_load();
  // Get the model.
  GtkTreeModel *model;
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
  // Get all selected iterators.
  GtkTreeSelection *selection;
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  vector < GtkTreeIter > iters;
  gtk_tree_selection_selected_foreach(selection, WindowReferences::on_collect_iters, gpointer(&iters));
  // Get the strings describing the references, and add them to the ones already loaded.
  for (unsigned int i = 0; i < iters.size(); i++) {
    ustring hidden_reference;
    gint book, chapter;
    gchar *verse;
    gchar *comment;
    gtk_tree_model_get(model, &iters[i], 1, &comment, 2, &book, 3, &chapter, 4, &verse, -1);
    Reference reference(book, chapter, verse);
    hidden_reference = reference.human_readable("");
    hidden_reference.append(" ");
    hidden_reference.append(comment);
    g_free(verse);
    g_free(comment);
    hidden_references.push_back(hidden_reference);
  }
  // Save new list of hidden refs.
  references_hidden_ones_save(hidden_references);
  // Actually delete them from the window, for user feedback.
  dismiss();
}

void WindowReferences::treeview_references_display_quick_reference()
// Display the quick references.
{
  // Get the model.
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));

  // Get all selected iterators.
  GtkTreeSelection *selection;
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  vector < GtkTreeIter > iters;
  gtk_tree_selection_selected_foreach(selection, WindowReferences::on_collect_iters, gpointer(&iters));

  // Bail out if none was selected.
  if (iters.size() == 0)
    return;

  // Go through all the selected iterators and get their references.
  references.clear();
  for (unsigned int i = 0; i < iters.size(); i++) {
    gint book, chapter;
    gchar *verse;
    gtk_tree_model_get(model, &iters[i], 2, &book, 3, &chapter, 4, &verse, -1);
    Reference reference(book, chapter, verse);
    references.push_back(reference);
    // Free memory.
    g_free(verse);
  }

  // Give a signal that quick references can be viewed.
  action = wratReferencesSelected;
  gtk_button_clicked(GTK_BUTTON(general_signal_button));
}
