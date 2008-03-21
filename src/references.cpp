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
*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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


References::References (GtkListStore * liststore, GtkWidget * treeview, GtkTreeViewColumn * treecolumn)
{
  myliststore = liststore;
  mytreeview = treeview;
  mytreecolumn = treecolumn;
}


References::~References ()
{
}


void References::load (const ustring & filename)
// Loads references from a file.
{
  try
  {
    ReadText rt (filename, true);
    // Pick out the references and leave the rest.
    for (unsigned int i = 0; i < rt.lines.size (); i++) {
      unsigned int book, chapter;
      ustring verse;
      if (reference_discover (0, 0, "", rt.lines[i], book, chapter, verse)) {
        Reference reference (book, chapter, verse);
        references.push_back (reference);      
      }
    }
    sort_references (references);
  }
  catch (exception & ex)
  {
    cerr << "Loading references: " << ex.what () << endl;
  }
  comments.clear();
  for (unsigned int i = 0; i < references.size(); i++)
    comments.push_back ("Loaded from file");
}


void References::load ()
// Loads references from database.
{
  // Bail out if there are no references.
  if (!g_file_test (references_database_filename().c_str(), G_FILE_TEST_IS_REGULAR))
    return;
  // Database variables.  
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try
  {
    // Open database.
    rc = sqlite3_open (references_database_filename().c_str(), &db);
    if (rc) throw runtime_error (sqlite3_errmsg(db));
    sqlite3_busy_timeout (db, 1000);
    // Read the references.
    {
      SqliteReader sqlitereader (0);
      char * sql;
      sql = g_strdup_printf ("select book, chapter, verse, comment from refs;");
      rc = sqlite3_exec(db, sql, sqlitereader.callback, &sqlitereader, &error);
      g_free (sql);
      if (rc != SQLITE_OK) {
        throw runtime_error (error);
      }
      references.clear ();
      comments.clear();
      for (unsigned int i = 0; i < sqlitereader.ustring0.size(); i++) {
        Reference reference (convert_to_int (sqlitereader.ustring0[i]), convert_to_int (sqlitereader.ustring1[i]), sqlitereader.ustring2[i]);
        references.push_back (reference);
        comments.push_back (sqlitereader.ustring3[i]);
      }
    }
    // Read the searchwords.
    {
      SqliteReader sqlitereader (0);
      char * sql;
      sql = g_strdup_printf ("select word, casesensitive, glob, matchbegin, matchend, areatype, areaid, areaintro, areaheading, areachapter, areastudy, areanotes, areaxref, areaverse from highlights;");
      rc = sqlite3_exec(db, sql, sqlitereader.callback, &sqlitereader, &error);
      g_free (sql);
      if (rc != SQLITE_OK) {
        throw runtime_error (error);
      }
      extern Settings * settings;
      for (unsigned int i = 0; i < sqlitereader.ustring0.size(); i++) {
        SessionHighlights sessionhighlights (
                                sqlitereader.ustring0[i], 
                                convert_to_bool (sqlitereader.ustring1[i]), 
                                convert_to_bool (sqlitereader.ustring2[i]), 
                                convert_to_bool (sqlitereader.ustring3[i]), 
                                convert_to_bool (sqlitereader.ustring4[i]), 
                                (AreaType) convert_to_int (sqlitereader.ustring5[i]),
                                convert_to_bool (sqlitereader.ustring6[i]),
                                convert_to_bool (sqlitereader.ustring7[i]),
                                convert_to_bool (sqlitereader.ustring8[i]),
                                convert_to_bool (sqlitereader.ustring9[i]),
                                convert_to_bool (sqlitereader.ustring10[i]),
                                convert_to_bool (sqlitereader.ustring11[i]),
                                convert_to_bool (sqlitereader.ustring12[i]),
                                convert_to_bool (sqlitereader.ustring13[i]));
        settings->session.highlights.push_back (sessionhighlights);
      }
    }
  }
  catch (exception & ex)
  {
    gw_critical (ex.what ());
  }
  // Close connection.  
  sqlite3_close (db);
}


void References::fill_store (const ustring& language)
// Fill the list store with the data.
{
  // Clear the store first.
  gtk_list_store_clear (myliststore);
  // Now add new data.
  GtkTreeIter iter;
  for (unsigned int i = 0; i < references.size (); i++)
  {
    gtk_list_store_append (myliststore, &iter);
    gtk_list_store_set (myliststore, &iter, 
                        0, references[i].human_readable (language).c_str (),
                        1, comments[i].c_str(), 
                        2, references[i].book, 
                        3, references[i].chapter, 
                        4, references[i].verse.c_str(),
                        -1);
  }
  // Show number of references in the title of the column
  set_header ();
}


void References::set_header ()
{
  // Sets the header in the treeview to the number of references.
  string s = TEXT_REFERENCES;
  s.append (" - ");
  s.append (convert_to_string (int (references.size ())));
  gtk_tree_view_column_set_title (mytreecolumn, s.c_str ());
}


void References::save (const ustring & filename)
// Saves the references to a file.
{
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig (settings->genconfig.project_get());
  ustring language = projectconfig->language_get();
  vector <ustring> lines;
  for (unsigned int i = 0; i < references.size(); i++) {
    lines.push_back (references[i].human_readable (language));
  }
  try
  {
    write_lines (filename, lines);
  }
  catch (exception & ex)
  {
    cerr << "Saving references: " << ex.what () << endl;
  }
}


void References::save () 
// Saves references to database.
{
  // Remove existing database.
  unlink (references_database_filename().c_str());
  // Some database variables.
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try
  {
    // Open the database.
    rc = sqlite3_open (references_database_filename().c_str(), &db);
    if (rc) throw runtime_error (sqlite3_errmsg(db));
    sqlite3_busy_timeout (db, 1000);
    // Create table for the references.
    char * sql;
    sql = g_strdup_printf ("create table refs (book integer, chapter integer, verse text, comment text);");
    rc = sqlite3_exec (db, sql, NULL, NULL, &error);
    g_free (sql);
    if (rc) {
      throw runtime_error (sqlite3_errmsg(db));
    }
    // Set it to store references fast.
    sql = g_strdup_printf ("PRAGMA synchronous=OFF;");
    rc = sqlite3_exec (db, sql, NULL, NULL, &error);
    g_free (sql);
    if (rc) {
      throw runtime_error (sqlite3_errmsg(db));
    }
    // Store the references and the comments.
    for (unsigned int i = 0; i < references.size(); i++) {
      sql = g_strdup_printf ("insert into refs values (%d, %d, '%s', '%s')", references[i].book, references[i].chapter, references[i].verse.c_str(), double_apostrophy (comments[i]).c_str());
      rc = sqlite3_exec (db, sql, NULL, NULL, &error);
      g_free (sql);
      if (rc) {
        throw runtime_error (sqlite3_errmsg(db));
      }
    }    
    // Create table for the searchwords.
    sql = g_strdup_printf ("create table highlights (word text, casesensitive integer, glob integer, matchbegin integer, matchend integer, areatype integer, areaid integer, areaintro integer, areaheading integer, areachapter integer, areastudy integer, areanotes integer, areaxref integer, areaverse integer);");
    rc = sqlite3_exec (db, sql, NULL, NULL, &error);
    g_free (sql);
    if (rc) {
      throw runtime_error (sqlite3_errmsg(db));
    }
    // Store the searchwords and related data.
    extern Settings * settings;
    for (unsigned int i = 0; i < settings->session.highlights.size(); i++) {
      sql = g_strdup_printf ("insert into highlights values ('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)", 
                             double_apostrophy (settings->session.highlights[i].word).c_str(), 
                             (int) settings->session.highlights[i].casesensitive, 
                             (int) settings->session.highlights[i].globbing, 
                             (int) settings->session.highlights[i].matchbegin,
                             (int) settings->session.highlights[i].matchend,
                             (int) settings->session.highlights[i].areatype, 
                             (int) settings->session.highlights[i].id, 
                             (int) settings->session.highlights[i].intro,
                             (int) settings->session.highlights[i].heading,
                             (int) settings->session.highlights[i].chapter,
                             (int) settings->session.highlights[i].study,
                             (int) settings->session.highlights[i].notes,
                             (int) settings->session.highlights[i].xref,
                             (int) settings->session.highlights[i].verse);
      rc = sqlite3_exec (db, sql, NULL, NULL, &error);
      g_free (sql);
      if (rc) {
        throw runtime_error (sqlite3_errmsg(db));
      }
    }    
  }
  catch (exception & ex)
  {
    gw_critical (ex.what ());
  }
  // Close db.
  sqlite3_close (db);
}


void References::goto_next ()
/* 
This selects the next reference, if there is any.
If no item has been selected it selects the first, if it's there.
*/
{
  goto_next_previous_internal (true);
}


void References::goto_previous ()
/* 
This goes to the previous reference, if there is any.
If no item has been selected it chooses the first, if it's there.
*/
{
  goto_next_previous_internal (false);
}


void References::goto_next_previous_internal (bool next)
{
  // Continue only when references are available.
  if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (myliststore), &iterator)) {
    // See which is selected.
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (mytreeview));
    // See how many are selected.
    gint number_selected;
    number_selected = gtk_tree_selection_count_selected_rows (selection);
    if (number_selected > 0) {
      // One or more selected: Store pointer to last one selected, and select next.
      gtk_tree_selection_selected_foreach (selection, References::goto_foreach_function, gpointer (&iterator));
      if (next) {
        if (gtk_tree_model_iter_next (GTK_TREE_MODEL (myliststore), &iterator)) {
          gtk_tree_selection_unselect_all (selection);
          gtk_tree_selection_select_iter (selection, &iterator);
        }
      } 
      else {
        GtkTreePath *path;
        path = gtk_tree_model_get_path (GTK_TREE_MODEL (myliststore), &iterator);
        if (gtk_tree_path_prev (path)) {
          gtk_tree_selection_unselect_all (selection);
          gtk_tree_model_get_iter (GTK_TREE_MODEL (myliststore), &iterator, path);
          gtk_tree_selection_select_iter (selection, &iterator);
        }
      }
    }
    else {
      // None selected: select the first one.
      gtk_tree_selection_select_iter (selection, &iterator);
    }
    // Scroll, if needed, to make the new selection visible.
    if (gtk_list_store_iter_is_valid (myliststore, &iterator)) {
      GtkTreePath *path;
      path = gtk_tree_model_get_path (GTK_TREE_MODEL (myliststore), &iterator);
      if (next)
        gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (mytreeview), path, NULL, true, 0.9, 0);
      else
        gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (mytreeview), path, NULL, true, 0.1, 0);
      gtk_tree_path_free (path);
    }
  }
}


void References::goto_foreach_function (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
  GtkTreeIter *iterator;
  iterator = (GtkTreeIter *) data;
  *iterator = *iter;
}


void References::get_loaded ()
// Gets the references and comments that are loaded in the editor.
{
  references.clear ();
  gtk_tree_model_foreach (GTK_TREE_MODEL (myliststore), GtkTreeModelForeachFunc (loaded_get_reference_foreach_function), gpointer (&references));
  comments.clear ();
  gtk_tree_model_foreach (GTK_TREE_MODEL (myliststore), GtkTreeModelForeachFunc (loaded_get_comment_foreach_function), gpointer (&comments));
}


gboolean References::loaded_get_reference_foreach_function (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
  gint book;
  gint chapter;
  gchar * verse;
  gtk_tree_model_get (model, iter, 2, &book, 3, &chapter, 4, &verse, -1);
  Reference reference (book, chapter, verse);
  ((vector < Reference > *)data)->push_back (reference);
  g_free (verse);
  return false;
}


gboolean References::loaded_get_comment_foreach_function (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
  gchar * comment;
  gtk_tree_model_get (model, iter, 1, &comment, -1);
  ((vector < string > *)data)->push_back (comment);
  g_free (comment);
  return false;
}


void References::get_references (vector <Reference> &refs)
{
  refs.assign (references.begin (), references.end ());
}


void References::set_references (vector <Reference>& refs)
{
  vector<ustring> comments;
  for (unsigned int i = 0; i < refs.size(); i++)
    comments.push_back ("Search result");
  set_references (refs, comments);
}


void References::set_references (vector <Reference>& references_in, vector<ustring>& comments_in)
{
  references_hidden_ones_filter (references_in, comments_in);
  references.assign (references_in.begin (), references_in.end ());
  comments.assign (comments_in.begin(), comments_in.end());
}


ustring References::references_database_filename ()
// Gives the filename of the database to save the references to.
{
  return gw_build_filename (directories_get_temp(), "references.sqlite3");
}
