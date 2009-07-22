/*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Library General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "libraries.h"
#include "utilities.h"
#include "references.h"
#include "directories.h"
#include "constants.h"
#include "bible.h"
#include "gwrappers.h"
#include "sqlite_reader.h"
#include "progresswindow.h"
#include "books.h"
#include "referenceutils.h"
#include "settings.h"
#include <sqlite3.h>
#include "tiny_utilities.h"

References::References(GtkListStore * liststore, GtkWidget * treeview, GtkTreeViewColumn * treecolumn)
{
  myliststore = liststore;
  mytreeview = treeview;
  mytreecolumn = treecolumn;
}

References::~References()
{
}

void References::fill_store(const ustring & language)
// Fill the list store with the data.
{
  // Clear the store first.
  gtk_list_store_clear(myliststore);
  // Now add new data.
  GtkTreeIter iter;
  for (unsigned int i = 0; i < references.size(); i++) {
    gtk_list_store_append(myliststore, &iter);
    gtk_list_store_set(myliststore, &iter, 0, references[i].human_readable(language).c_str(), 1, comments[i].c_str(), 2, references[i].book, 3, references[i].chapter, 4, references[i].verse.c_str(), -1);
  }
  // Show number of references in the title of the column
  set_header();
}

void References::set_header()
{
  // Sets the header in the treeview to the number of references.
  string s = TEXT_REFERENCES;
  s.append(" - ");
  s.append(convert_to_string(int (references.size())));
  gtk_tree_view_column_set_title(mytreecolumn, s.c_str());
}


void References::goto_next()
/* 
This selects the next reference, if there is any.
If no item has been selected it selects the first, if it's there.
*/
{
  goto_next_previous_internal(true);
}

void References::goto_previous()
/* 
This goes to the previous reference, if there is any.
If no item has been selected it chooses the first, if it's there.
*/
{
  goto_next_previous_internal(false);
}

void References::goto_next_previous_internal(bool next)
{
  // Continue only when references are available.
  if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(myliststore), &iterator)) {
    // See which is selected.
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(mytreeview));
    // See how many are selected.
    gint number_selected;
    number_selected = gtk_tree_selection_count_selected_rows(selection);
    if (number_selected > 0) {
      // One or more selected: Store pointer to last one selected, and select next.
      gtk_tree_selection_selected_foreach(selection, References::goto_foreach_function, gpointer(&iterator));
      if (next) {
        if (gtk_tree_model_iter_next(GTK_TREE_MODEL(myliststore), &iterator)) {
          gtk_tree_selection_unselect_all(selection);
          gtk_tree_selection_select_iter(selection, &iterator);
        }
      } else {
        GtkTreePath *path;
        path = gtk_tree_model_get_path(GTK_TREE_MODEL(myliststore), &iterator);
        if (gtk_tree_path_prev(path)) {
          gtk_tree_selection_unselect_all(selection);
          gtk_tree_model_get_iter(GTK_TREE_MODEL(myliststore), &iterator, path);
          gtk_tree_selection_select_iter(selection, &iterator);
        }
      }
    } else {
      // None selected: select the first one.
      gtk_tree_selection_select_iter(selection, &iterator);
    }
    // Scroll, if needed, to make the new selection visible.
    if (gtk_list_store_iter_is_valid(myliststore, &iterator)) {
      GtkTreePath *path;
      path = gtk_tree_model_get_path(GTK_TREE_MODEL(myliststore), &iterator);
      if (next)
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(mytreeview), path, NULL, true, 0.9, 0);
      else
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(mytreeview), path, NULL, true, 0.1, 0);
      gtk_tree_path_free(path);
    }
  }
}

void References::goto_foreach_function(GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
  GtkTreeIter *iterator;
  iterator = (GtkTreeIter *) data;
  *iterator = *iter;
}

void References::get_loaded()
// Gets the references and comments that are loaded in the editor.
{
  references.clear();
  gtk_tree_model_foreach(GTK_TREE_MODEL(myliststore), GtkTreeModelForeachFunc(loaded_get_reference_foreach_function), gpointer(&references));
  comments.clear();
  gtk_tree_model_foreach(GTK_TREE_MODEL(myliststore), GtkTreeModelForeachFunc(loaded_get_comment_foreach_function), gpointer(&comments));
}

gboolean References::loaded_get_reference_foreach_function(GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
  gint book;
  gint chapter;
  gchar *verse;
  gtk_tree_model_get(model, iter, 2, &book, 3, &chapter, 4, &verse, -1);
  Reference reference(book, chapter, verse);
  ((vector < Reference > *)data)->push_back(reference);
  g_free(verse);
  return false;
}

gboolean References::loaded_get_comment_foreach_function(GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
  gchar *comment;
  gtk_tree_model_get(model, iter, 1, &comment, -1);
  ((vector < string > *)data)->push_back(comment);
  g_free(comment);
  return false;
}

void References::get_references(vector < Reference > &refs)
{
  refs.assign(references.begin(), references.end());
}

void References::set_references(vector < Reference > &refs)
{
  vector < ustring > comments;
  for (unsigned int i = 0; i < refs.size(); i++)
    comments.push_back("Search result");
  set_references(refs, comments);
}

void References::set_references(vector < Reference > &references_in, vector < ustring > &comments_in)
{
  references_hidden_ones_filter(references_in, comments_in);
  references.assign(references_in.begin(), references_in.end());
  comments.assign(comments_in.begin(), comments_in.end());
}

ustring References::references_database_filename()
// Gives the filename of the database to save the references to.
{
  return gw_build_filename(directories_get_temp(), "references.sqlite3");
}
