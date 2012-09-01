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


#include "libraries.h"
#include "dialoglistview.h"
#include "projectutils.h"
#include "directories.h"
#include "listview.h"
#include "keyboard.h"
#include "help.h"
#include <gdk/gdkkeysyms.h>


ListviewDialog::ListviewDialog(const ustring & title, vector <ustring> &list, const ustring & focus, bool sortlist, gchar * help)
// This dialog shows "list".
// If the user selects one out of that, it returns it in "focus".
{
  // Initialize variables.
  initialfocus = focus;
  action_text_1 = NULL;
  action_text_2 = NULL;

  // Build dialog.
  listviewdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(listviewdialog), title.c_str());
  gtk_window_set_position(GTK_WINDOW(listviewdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(listviewdialog), TRUE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG (listviewdialog));
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new("Type the first couple of characters of the item, or select it,\nand then press OK or Enter");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 4);

  hbox1 = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox1, TRUE, TRUE, 0);

  label_shortcut = gtk_label_new("");
  gtk_widget_show(label_shortcut);
  gtk_box_pack_start(GTK_BOX(hbox1), label_shortcut, FALSE, FALSE, 0);

  label_action = gtk_label_new("");
  gtk_widget_show(label_action);
  gtk_box_pack_start(GTK_BOX(hbox1), label_action, FALSE, FALSE, 0);

  notebook1 = gtk_notebook_new();
  gtk_widget_show(notebook1);
  gtk_box_pack_start(GTK_BOX(vbox1), notebook1, TRUE, TRUE, 0);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook1), FALSE);
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook1), FALSE);

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_container_add(GTK_CONTAINER(notebook1), scrolledwindow1);
  gtk_widget_set_size_request(scrolledwindow1, -1, 400);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  treeview1 = gtk_tree_view_new();
  gtk_widget_show(treeview1);
  gtk_container_add(GTK_CONTAINER(scrolledwindow1), treeview1);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview1), FALSE);

  // Storage, renderer, column and selection.
  store1 = gtk_list_store_new(1, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview1), GTK_TREE_MODEL(store1));
  g_object_unref(store1);
  GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
  column1 = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview1), column1);
  select1 = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview1));
  gtk_tree_selection_set_mode(select1, GTK_SELECTION_SINGLE);

  // Fill the treeview with the items, sorted optionally
  if (sortlist)
    sort(list.begin(), list.end());
  listview_set_strings(treeview1, store1, list);

  // Focus item.
  if (list.size() > 0) {
    listview_focus_string(treeview1, list[0]);
  }
  listview_focus_string(treeview1, focus);

  label_tab_1 = gtk_label_new("");
  gtk_widget_show(label_tab_1);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook1), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook1), 0), label_tab_1);

  scrolledwindow2 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow2);
  gtk_container_add(GTK_CONTAINER(notebook1), scrolledwindow2);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow2), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  treeview2 = gtk_tree_view_new();
  gtk_widget_show(treeview2);
  gtk_container_add(GTK_CONTAINER(scrolledwindow2), treeview2);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview2), FALSE);

  // Storage, renderer, column and selection.
  store2 = gtk_list_store_new(1, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview2), GTK_TREE_MODEL(store2));
  g_object_unref(store2);
  GtkCellRenderer *renderer2 = gtk_cell_renderer_text_new();
  column2 = gtk_tree_view_column_new_with_attributes("", renderer2, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview2), column2);
  select2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview2));
  gtk_tree_selection_set_mode(select2, GTK_SELECTION_SINGLE);

  label_tab_2 = gtk_label_new("");
  gtk_widget_show(label_tab_2);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook1), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook1), 1), label_tab_2);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG (listviewdialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(listviewdialog, NULL, NULL, help);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(listviewdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton), true);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(listviewdialog), okbutton, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton), true);

  g_signal_connect_after((gpointer) notebook1, "switch_page", G_CALLBACK(on_notebook1_switch_page), gpointer(this));
  g_signal_connect((gpointer) treeview1, "row_activated", G_CALLBACK(on_treeview1_row_activated), gpointer(this));
  g_signal_connect((gpointer) treeview2, "row_activated", G_CALLBACK(on_treeview1_row_activated), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(static_on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_focus(treeview1);
  gtk_widget_grab_default(okbutton);

  // Gui.
  on_switch_page();
}


ListviewDialog::~ListviewDialog()
{
  gtk_widget_destroy(listviewdialog);
}


void ListviewDialog::two_pages(const gchar * tab1, const gchar * tab2, vector < ustring > &list, bool sortlist)
{
  // Modify GUI.
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook1), true);
  gtk_label_set_text(GTK_LABEL(label_tab_1), tab1);
  gtk_label_set_text(GTK_LABEL(label_tab_2), tab2);

  // Fill the treeview with the items, sorted optionally
  if (sortlist)
    sort(list.begin(), list.end());
  listview_set_strings(treeview2, store2, list);

  // Focus an item.
  if (list.size() > 0)
    listview_focus_string(treeview2, list[0]);
  listview_focus_string(treeview2, initialfocus);

  // Switch to the second page if it has the initially focused item.
  // Focus the right treeview.
  set < ustring > items(list.begin(), list.end());
  if (items.find(initialfocus) != items.end()) {
    on_focus();
    gtk_widget_grab_focus(treeview2);
  } else {
    gtk_widget_grab_focus(treeview1);
  }
}


void ListviewDialog::tab_switcher(guint accel_key, GdkModifierType accel_mods, const gchar * acceleratortext, const gchar * actiontext1, const gchar * actiontext2)
{
  // Integrate the accelerator.
  accel_group = gtk_accel_group_new();
  gtk_widget_add_accelerator(listviewdialog, "activate_focus", accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE);
  g_signal_connect((gpointer) listviewdialog, "activate_focus", G_CALLBACK(on_activate_focus), gpointer(this));
  gtk_window_add_accel_group(GTK_WINDOW(listviewdialog), accel_group);

  // Store both of the action texts for the accelerator.
  action_text_1 = actiontext1;
  action_text_2 = actiontext2;

  // Update GUI.
  ustring infotext("Press ");
  infotext.append(acceleratortext);
  gtk_label_set_text(GTK_LABEL(label_shortcut), infotext.c_str());
  on_switch_page();
}


int ListviewDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(listviewdialog));
}


void ListviewDialog::on_treeview1_row_activated(GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data)
{
  ((ListviewDialog *) user_data)->on_treeview();
}


void ListviewDialog::on_treeview()
{
  on_okbutton_clicked();
  gtk_dialog_response(GTK_DIALOG(listviewdialog), GTK_RESPONSE_OK);
}


void ListviewDialog::static_on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ListviewDialog *) user_data)->on_okbutton_clicked();
}


void ListviewDialog::on_okbutton_clicked()
{
  int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook1));
  if (page == 0) {
    focus = listview_get_active_string(treeview1);
  } else {
    focus = listview_get_active_string(treeview2);
  }
}


void ListviewDialog::on_activate_focus(GtkWindow * window, gpointer user_data)
{
  ((ListviewDialog *) user_data)->on_focus();
}


void ListviewDialog::on_focus()
{
  int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook1));
  gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook1), 1 - page);
}


void ListviewDialog::on_notebook1_switch_page(GtkNotebook * notebook, GtkNotebookPage * page, guint page_num, gpointer user_data)
{
  ((ListviewDialog *) user_data)->on_switch_page();
}


void ListviewDialog::on_switch_page()
{
  // Set the OK button (in)sensitive depending on whether an item is selected.
  int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook1));
  bool sensitive;
  if (page == 0) {
    sensitive = !listview_get_active_string(treeview1).empty();
  } else {
    sensitive = !listview_get_active_string(treeview2).empty();
  }
  gtk_widget_set_sensitive(okbutton, sensitive);
  // Update the GUI.
  const gchar *actiontext;
  if (page == 0) {
    actiontext = action_text_2;
  } else {
    actiontext = action_text_1;
  }
  if (actiontext)
    gtk_label_set_text(GTK_LABEL(label_action), actiontext);
}


