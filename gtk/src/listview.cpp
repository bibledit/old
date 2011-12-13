/*
 ** Copyright (©) 2003-2012 Teus Benschop.
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


#include "listview.h"
#include "utilities.h"
#include "tiny_utilities.h"


ustring listview_get_active_string(GtkWidget * listview)
// Gets the currently active string in a combobox.
{
  // Storage for result
  ustring active_string;
  // Get the model
  GtkTreeModel *model;
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(listview));
  // Get the selection.
  GtkTreeSelection *selection;
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(listview));
  // Some variables needed.
  GtkTreeIter iter;
  // Get the selected iterator.
  if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
    gchar *str_data;
    // Make sure you terminate calls to gtk_tree_model_get() with a '-1' value.
    gtk_tree_model_get(model, &iter, 0, &str_data, -1);
    active_string = str_data;
    g_free(str_data);
  }
  // Return result.
  return active_string;
}


int listview_get_active_offset(GtkWidget * listview)
// Gets the offset of the item now selected.
// If nothing is selected, it returns -1.
{
  // Get model and selection.
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(listview));
  GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(listview));
  // Initialize iterator that points to the selected row.  
  GtkTreeIter selectediter;
  bool somethingselected = gtk_tree_selection_get_selected(selection, &model, &selectediter);
  if (!somethingselected)
    return -1;

  // Offset we look for.
  int offset = 0;
  int result = 0;

  // Iterate through the model. Compare iterators to find offset.
  GtkTreeIter iter;
  gboolean valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {
    if (selectediter.user_data == iter.user_data)
      result = offset;
    valid = gtk_tree_model_iter_next(model, &iter);
    offset++;
  }

  // Return offset.  
  return result;
}


static void listview_collect_iterators(GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
  ((vector < GtkTreeIter > *)data)->push_back(*iter);
}


vector <int> listview_get_active_offsets(GtkWidget * listview)
// Gets the offsets of the items now selected.
{
  // Container with offsets.
  vector < int >offsets;

  // Get model and selected iterators.
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(listview));
  GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(listview));
  vector <GtkTreeIter> iters;
  gtk_tree_selection_selected_foreach(selection, listview_collect_iterators, gpointer(&iters));

  // Iterate through the model. Compare iterators to find offset.
  int offset = 0;
  GtkTreeIter iter;
  gboolean valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {
    for (unsigned int i = 0; i < iters.size(); i++) {
      if (iter.user_data == iters[i].user_data) {
        offsets.push_back(offset);
      }
    }
    valid = gtk_tree_model_iter_next(model, &iter);
    offset++;
  }

  // Return offsets.  
  return offsets;
}


vector <ustring> listview_get_active_strings(GtkWidget * listview)
{
  // Storage for result.
  vector <ustring> strings;

  // Get model and selected iterators.
  GtkTreeSelection *selection =  gtk_tree_view_get_selection(GTK_TREE_VIEW(listview));
  vector <GtkTreeIter> selected_iterators;
  gtk_tree_selection_selected_foreach(selection, listview_collect_iterators, gpointer(&selected_iterators));

  // Get selected strings.
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(listview));
  for (unsigned int i = 0; i < selected_iterators.size(); i++) {
    gchar *str_data;
    gtk_tree_model_get(model, &selected_iterators[i], 0, &str_data, -1);
    strings.push_back(str_data);
    g_free(str_data);
  }

  // Return the results.
  return strings;
}


void listview_clear_strings(GtkWidget * listview, GtkListStore * store)
// Clear the strings loaded in the combobox.
{
  gtk_list_store_clear(store);
}


void listview_set_strings(GtkWidget * listview, GtkListStore * store, const vector < ustring > &strings)
{
  listview_clear_strings(listview, store);
  GtkTreeIter iter;
  for (unsigned int i = 0; i < strings.size(); i++) {
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, strings[i].c_str(), -1);
  }
}


void listview_set_strings(GtkWidget * listview, GtkListStore * store, const vector < unsigned int >&strings)
{
  vector < ustring > strings2;
  for (unsigned int i = 0; i < strings.size(); i++)
    strings2.push_back(convert_to_string(strings[i]));
  listview_set_strings(listview, store, strings2);
}


void listview_focus_string(GtkWidget * listview, const ustring & string, bool grabfocus)
// Sets the string that is focused and scrolls to it.
// If grabfocus = true, the listview grabs the focus.
{
  // Get the model
  GtkTreeModel *model;
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(listview));
  // Get the selection.
  GtkTreeSelection *selection;
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(listview));
  // Some variables needed.
  GtkTreeIter iter;
  gboolean valid;
  gint index = 0;
  // Get the first iter in the store.
  valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {
    // Walk through the list, reading each row
    gchar *str_data;
    // Make sure you terminate calls to gtk_tree_model_get() with a '-1' value.
    gtk_tree_model_get(model, &iter, 0, &str_data, -1);
    // If this is the marker we wish to display, select it, put the cursor on it,
    // and focus on the listview.
    if (str_data == string) {
      gtk_tree_selection_select_iter(selection, &iter);
      GtkTreePath *path = NULL;
      path = gtk_tree_path_new_from_indices(index, -1);
      if (path) {
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(listview), path, NULL, false);
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(listview), path, NULL, true, 0.5, 0.5);
      }
      gtk_tree_path_free(path);
      if (grabfocus)
        gtk_widget_grab_focus(listview);
    }
    g_free(str_data);
    valid = gtk_tree_model_iter_next(model, &iter);
    index++;
  }
}


void listview_focus_string(GtkWidget * listview, unsigned int string, bool grabfocus)
{
  listview_focus_string(listview, convert_to_string(string), grabfocus);
}


vector < ustring > listview_get_strings(GtkWidget * listview)
// Gets the strings loaded in the listview.
{
  // Storage for the result.
  vector < ustring > strings;
  // Get the model
  GtkTreeModel *model;
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(listview));
  // Some variables needed.
  GtkTreeIter iter;
  gboolean valid;
  // Get the first iter in the store.
  valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {
    // Walk through the list, reading each row
    gchar *str_data;
    // Make sure you terminate calls to gtk_tree_model_get() with a '-1' value.
    gtk_tree_model_get(model, &iter, 0, &str_data, -1);
    // Store.
    strings.push_back(str_data);
    // Free memory and go to next.
    g_free(str_data);
    valid = gtk_tree_model_iter_next(model, &iter);
  }
  // Return the results.
  return strings;
}


void list_view_erase_selection(GtkWidget * listview)
// Erase the current selected string.
{
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(listview));
  GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(listview));
  GtkTreeIter iter;
  if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
  }
}


void listview_set_row(GtkWidget * listview, GtkListStore * store, unsigned int offset, const ustring row)
{
  // Get the model
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(listview));

  // Some variables needed.
  GtkTreeIter iter;
  gboolean valid;
  unsigned int index = 0;

  // Get the first iter in the store and walk through the list.
  valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {

    // If this is the right row, set it.
    if (index == offset) {
      gtk_list_store_set(store, &iter, 0, row.c_str(), -1);
    }
    // Next iteration.
    valid = gtk_tree_model_iter_next(model, &iter);
    index++;
  }
}
