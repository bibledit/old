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
#include "windowshowverses.h"
#include "help.h"
#include "windows.h"
#include "projectutils.h"
#include "settings.h"
#include "listview.h"

WindowShowVerses::WindowShowVerses(GtkAccelGroup * accelerator_group, bool startup):
WindowBase(widShowVerses, "Verses", startup, 0), myreference(0)
// Window for showing the quick references.  
{
  expander1 = gtk_expander_new (NULL);  vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  gtk_widget_show (expander1);
  gtk_box_pack_start (GTK_BOX (vbox), expander1, FALSE, TRUE, 0);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (expander1), vbox1);

  scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow2);
  gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow2, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_SHADOW_IN);

  treeview1 = gtk_tree_view_new ();
  gtk_widget_show (treeview1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow2), treeview1);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview1), FALSE);
  gtk_tree_view_set_reorderable (GTK_TREE_VIEW (treeview1), TRUE);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

  button_add = gtk_button_new_from_stock ("gtk-add");
  gtk_widget_show (button_add);
  gtk_box_pack_start (GTK_BOX (hbox1), button_add, FALSE, FALSE, 0);

  button_delete = gtk_button_new_from_stock ("gtk-delete");
  gtk_widget_show (button_delete);
  gtk_box_pack_start (GTK_BOX (hbox1), button_delete, FALSE, FALSE, 0);

  label1 = gtk_label_new_with_mnemonic ("_Settings");
  gtk_widget_show (label1);
  gtk_expander_set_label_widget (GTK_EXPANDER (expander1), label1);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_SHADOW_IN);

  textview1 = gtk_text_view_new ();
  gtk_widget_show (textview1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), textview1);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (textview1), FALSE);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (textview1), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview1), GTK_WRAP_WORD);
  
  g_signal_connect((gpointer) textview1, "visibility-notify-event", G_CALLBACK(on_visibility_notify_event), gpointer(this));
  
  g_signal_connect ((gpointer) treeview1, "drag_end",  G_CALLBACK (on_treeview1_drag_end), gpointer(this));
  g_signal_connect ((gpointer) button_add, "clicked", G_CALLBACK (on_button_add_clicked), gpointer(this));
  g_signal_connect ((gpointer) button_delete, "clicked",  G_CALLBACK (on_button_delete_clicked),  gpointer(this));
  
  // Storage, renderer, column and selection.
  store = gtk_list_store_new(1, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview1), GTK_TREE_MODEL(store));
  g_object_unref(store);
  GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview1), -1, "Name", renderer, "text", 0, NULL);
  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview1)), GTK_SELECTION_MULTIPLE);

  // Load the projects.
  load_tree();
}


WindowShowVerses::~WindowShowVerses()
{
}


void WindowShowVerses::go_to(Reference& reference)
// Show the references.
{
  if (myreference.equals (reference))
    return;
  myreference.assign (reference);
  load_verses();
}


void WindowShowVerses::on_button_add_clicked (GtkButton *button, gpointer user_data)
{
  ((WindowShowVerses *) user_data)->on_button_add();
}


void WindowShowVerses::on_button_add ()
{
  ustring project;
  if (project_select(project)) {
    vector <ustring> projects = listview_get_strings (treeview1);
    projects.push_back (project);
    extern Settings * settings;
    settings->genconfig.projects_displaying_verses_set (projects);
    load_tree();
  }
}

  
void WindowShowVerses::on_button_delete_clicked (GtkButton *button, gpointer user_data)
{
  ((WindowShowVerses *) user_data)->on_button_delete();
}


void WindowShowVerses::on_button_delete ()
{
  GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview1));
  vector < GtkTreeIter > iters;
  gtk_tree_selection_selected_foreach(selection, WindowShowVerses::on_collect_iters, gpointer(&iters));
  for (unsigned int i = 0; i < iters.size(); i++) {
    GtkTreeIter iter = iters[i];
    gtk_list_store_remove(store, &iter);
  }
  vector <ustring> projects = listview_get_strings (treeview1);
  extern Settings * settings;
  settings->genconfig.projects_displaying_verses_set (projects);
  load_verses();
}


void WindowShowVerses::on_collect_iters(GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
  ((vector < GtkTreeIter > *)data)->push_back(*iter);
}


void WindowShowVerses::load_tree ()
{
  extern Settings *settings;
  vector <ustring> projects = settings->genconfig.projects_displaying_verses_get();
  listview_set_strings (treeview1, store, projects);  
  load_verses();
}


void WindowShowVerses::on_treeview1_drag_end (GtkWidget *widget, GdkDragContext *drag_context, gpointer user_data)
{
  ((WindowShowVerses *) user_data)->treeview1_drag_end();
}


void WindowShowVerses::treeview1_drag_end ()
{
  vector <ustring> projects = listview_get_strings (treeview1);
  extern Settings * settings;
  settings->genconfig.projects_displaying_verses_set (projects);
  load_verses();
}


void WindowShowVerses::load_verses()
{
  ustring text;
  extern Settings * settings;
  vector <ustring> projects = settings->genconfig.projects_displaying_verses_get();
  for (unsigned int i = 0; i < projects.size(); i++) {
    text.append (projects[i]);
    text.append ("\n");
    ustring verse = project_retrieve_verse(projects[i], myreference.book, myreference.chapter, myreference.verse);
    if (text.empty()) {
      text.append("<empty>");
    } else {
      CategorizeLine cl(verse);
      text.append(cl.verse);
    }
    text.append("\n");
  }
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1));
  gtk_text_buffer_set_text(buffer, text.c_str(), -1);
}


