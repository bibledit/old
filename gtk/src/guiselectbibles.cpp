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
#include "guiselectbibles.h"
#include "projectutils.h"
#include "dialoglistview.h"
#include <glib/gi18n.h>

SelectBiblesGui::SelectBiblesGui(GtkWidget * box, Shortcuts& shortcuts)
{
  // Create the GUI.
  vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox);
  gtk_box_pack_start(GTK_BOX(box), vbox, TRUE, TRUE, 0);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow, FALSE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);

  treeview = gtk_tree_view_new ();
  gtk_widget_show (treeview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), treeview);
  gtk_tree_view_set_reorderable (GTK_TREE_VIEW (treeview), TRUE);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

  button_add = gtk_button_new_from_stock ("gtk-add");
  gtk_widget_show (button_add);
  gtk_box_pack_start (GTK_BOX (hbox), button_add, FALSE, FALSE, 0);

  shortcuts.stockbutton (button_add);
  
  button_remove = gtk_button_new_from_stock ("gtk-remove");
  gtk_widget_show (button_remove);
  gtk_box_pack_start (GTK_BOX (hbox), button_remove, FALSE, FALSE, 0);

  shortcuts.stockbutton (button_remove);
  
  g_signal_connect ((gpointer) button_add, "clicked",
                    G_CALLBACK (on_button_add_clicked),
                    gpointer (this));
  g_signal_connect ((gpointer) button_remove, "clicked",
                    G_CALLBACK (on_button_remove_clicked),
                    gpointer (this));

  // List storage.
  store = gtk_list_store_new(2, G_TYPE_BOOLEAN, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
  g_object_unref(store);
  GtkCellRenderer *renderer0 = gtk_cell_renderer_toggle_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, _("Enabled"), renderer0, "active", 0, NULL);
  g_signal_connect(renderer0, "toggled", G_CALLBACK(on_enabled_toggled), gpointer(this));
  GtkCellRenderer *renderer1 = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, _("Bible"), renderer1, "text", 1, NULL);
  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)), GTK_SELECTION_MULTIPLE);
}


SelectBiblesGui::~SelectBiblesGui()
{
  gtk_widget_destroy (hbox);
}


void SelectBiblesGui::set (const vector <bool>& enabled, const vector <ustring>& bible)
{
  if (enabled.size() == bible.size()) {
    for (unsigned int i = 0; i < enabled.size(); i++) {
      add_bible (enabled[i], bible[i]);
    }
  }
}


void SelectBiblesGui::add_bible (bool enabled, const ustring& bible)
{
  GtkTreeIter iter;
  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter, 0, enabled, 1, bible.c_str(), -1);
}


void SelectBiblesGui::get (vector <bool>& enabled, vector <ustring>& bible)
{
  enabled.clear();
  bible.clear();  
  GtkTreeModel *model = (GtkTreeModel *) store;
  GtkTreeIter iter;
  gboolean valid;
  valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {
    int bool_data;
    gchar *str_data;
    gtk_tree_model_get(model, &iter, 0, &bool_data, 1, &str_data, -1);
    enabled.push_back (bool_data);
    bible.push_back (str_data);
    g_free(str_data);
    valid = gtk_tree_model_iter_next(model, &iter);
  }
}


void SelectBiblesGui::on_button_add_clicked(GtkButton * button, gpointer user_data)
{
  ((SelectBiblesGui *) user_data)->on_button_add();
}


void SelectBiblesGui::on_button_add()
{
  ustring bible;
  if (project_select(bible)) {
    add_bible (true, bible);
  }
}


void SelectBiblesGui::on_button_remove_clicked(GtkButton * button, gpointer user_data)
{
  ((SelectBiblesGui *) user_data)->on_button_remove();
}


void SelectBiblesGui::on_button_remove()
{
  GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  vector <GtkTreeIter> iters;
  gtk_tree_selection_selected_foreach(selection, SelectBiblesGui::on_collect_iters, gpointer(&iters));
  for (unsigned int i = 0; i < iters.size(); i++) {
    GtkTreeIter iter = iters[i];
    gtk_list_store_remove(store, &iter);
  }
}


void SelectBiblesGui::on_collect_iters(GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
  ((vector < GtkTreeIter > *)data)->push_back(*iter);
}


void SelectBiblesGui::focus()
{
  gtk_widget_grab_focus(treeview);
}


void SelectBiblesGui::on_enabled_toggled(GtkCellRendererToggle * cell, gchar * path_str, gpointer data)
{
  ((SelectBiblesGui *) data)->on_rendering_toggle(cell, path_str);
}


void SelectBiblesGui::on_rendering_toggle(GtkCellRendererToggle * cell, gchar * path_str)
{
  GtkTreeModel *model = (GtkTreeModel *) store;
  GtkTreeIter iter;
  GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
  gboolean setting;
  gtk_tree_model_get_iter(model, &iter, path);
  gtk_tree_model_get(model, &iter, 0, &setting, -1);
  setting = !setting;
  gtk_list_store_set(store, &iter, 0, setting, -1);
  gtk_tree_path_free(path);
}


