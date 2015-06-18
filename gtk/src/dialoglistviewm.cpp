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
#include "dialoglistviewm.h"
#include "projectutils.h"
#include "directories.h"
#include "listview.h"
#include "keyboard.h"
#include "help.h"
#include "screen.h"
#include <glib/gi18n.h>

ListviewMDialog::ListviewMDialog(const ustring & title, vector <ustring> &list, bool sortlist, gchar * help)
// This dialog shows "list".
// It returns the selected rows in "foci".
{
  // Build dialog.
  dialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dialog), title.c_str());
  gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG(dialog));
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new(_("Select one or more items, and then press OK or Enter"));
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 4);

  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow);
  gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  treeview = gtk_tree_view_new();
  gtk_widget_show(treeview);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), treeview);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);

  // Storage, renderer, column and selection.
  store = gtk_list_store_new(1, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
  g_object_unref(store);
  GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "", renderer, "text", 0, NULL);
  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)), GTK_SELECTION_MULTIPLE);

  // Fill the treeview with the items, sorted optionally
  if (sortlist)
    sort(list.begin(), list.end());
  listview_set_strings(treeview, store, list);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(dialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(dialog, NULL, NULL, help);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(dialog), cancelbutton, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton), true);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(dialog), okbutton, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton), true);

  g_signal_connect_after((gpointer) treeview, "button_release_event", G_CALLBACK(on_treeview_button_event), gpointer(this));
  g_signal_connect_after((gpointer) treeview, "key_release_event", G_CALLBACK(on_treeview_key_event), gpointer(this));
  g_signal_connect((gpointer) treeview, "row_activated", G_CALLBACK(on_treeview_row_activated), gpointer(this));

  gtk_widget_grab_focus(treeview);
  gtk_widget_grab_default(okbutton);
  
  new DialogAutoScaler (dialog, G_MAXINT);
}


ListviewMDialog::~ListviewMDialog()
{
  gtk_widget_destroy(dialog);
}


int ListviewMDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dialog));
}


gboolean ListviewMDialog::on_treeview_button_event(GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
  ((ListviewMDialog *) user_data)->on_treeview_cursor();
  return FALSE;
}


gboolean ListviewMDialog::on_treeview_key_event(GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  ((ListviewMDialog *) user_data)->on_treeview_cursor();
  return FALSE;
}


void ListviewMDialog::on_treeview_cursor()
{
  foci = listview_get_active_strings(treeview);
}


void ListviewMDialog::on_treeview_row_activated(GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data)
{
  ((ListviewMDialog *) user_data)->on_treeview();
}


void ListviewMDialog::on_treeview()
{
  gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
}


