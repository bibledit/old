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


#ifndef INCLUDED_REFERENCES_H
#define INCLUDED_REFERENCES_H


#include "libraries.h"
#include <gtk/gtk.h>
#include "reference.h"


class References
{
public:
  References (GtkListStore * liststore, GtkWidget * treeview, GtkTreeViewColumn * treecolumn);
  ~References ();
  void load ();
  void load (const ustring & filename);
  void fill_store (const ustring& language);
  void set_header ();
  void save (const ustring & filename);
  void save ();
  void goto_next ();
  void goto_previous ();
  void get_loaded ();
  void get_references (vector <Reference> &refs);
  void set_references (vector <Reference> &refs);
  void set_references (vector <Reference>& references_in, vector<ustring>& comments_in);
private:
  vector <Reference> references;
  vector <ustring> comments;
  GtkListStore *myliststore;
  GtkWidget *mytreeview;
  GtkTreeViewColumn *mytreecolumn;
  GtkTreeIter iterator;
  void goto_next_previous_internal (bool next);
  static void goto_foreach_function (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data);
  static gboolean loaded_get_reference_foreach_function (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data);
  static gboolean loaded_get_comment_foreach_function (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data);
  ustring references_database_filename ();
};


#endif
