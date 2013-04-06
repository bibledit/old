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
#include "dialogeditlist.h"
#include "referenceutils.h"
#include "listview.h"
#include "dialogentry.h"
#include "gtkwrappers.h"
#include "utilities.h"
#include "help.h"
#include "tiny_utilities.h"
#include "dialoglistviewm.h"


EditListDialog::EditListDialog(vector <ustring> * lines, const ustring & title, const ustring & info, bool remove, bool add, bool sort, bool import, bool exprt, bool duplicates, bool reorderable, vector <ustring> * addables)
{
  // Save variables.
  mylines = lines;
  myaddables = addables;

  editlistdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(editlistdialog), title.c_str());
  gtk_window_set_position(GTK_WINDOW(editlistdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(editlistdialog), TRUE);
  gtk_window_set_resizable(GTK_WINDOW(editlistdialog), FALSE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG(editlistdialog));
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, FALSE, FALSE, 0);

  hbox3 = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(hbox3);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox3, FALSE, FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(hbox3), 2);

  label3 = gtk_label_new_with_mnemonic("_List");
  gtk_widget_show(label3);
  gtk_box_pack_start(GTK_BOX(hbox3), label3, FALSE, FALSE, 0);

  label_info = gtk_label_new(info.c_str());
  gtk_widget_show(label_info);
  gtk_box_pack_start(GTK_BOX(hbox3), label_info, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_info), 0, 0.5);

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_widget_set_size_request(scrolledwindow1, -1, 300);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  treeview1 = gtk_tree_view_new();
  gtk_widget_show(treeview1);
  gtk_container_add(GTK_CONTAINER(scrolledwindow1), treeview1);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview1), FALSE);
  if (reorderable)
    gtk_tree_view_set_reorderable(GTK_TREE_VIEW(treeview1), TRUE);

  table1 = gtk_table_new(1, 5, TRUE);
  gtk_widget_show(table1);
  gtk_box_pack_start(GTK_BOX(vbox1), table1, TRUE, TRUE, 0);

  // Pointer for properly ordering any buttons that get used.
  int buttonpointer = 0;
  buttonremove = NULL;
  if (remove) {
    buttonremove = gtk_button_new_from_stock("gtk-remove");
    gtk_widget_show(buttonremove);
    gtk_table_attach(GTK_TABLE(table1), buttonremove, buttonpointer, buttonpointer + 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    // Button remove is made sensitive only if there is something to remove.
    gtk_widget_set_sensitive(buttonremove, false);
    // Position of next button.
    buttonpointer++;
  }

  if (add) {
    button_add = gtk_button_new_from_stock("gtk-add");
    gtk_widget_show(button_add);
    gtk_table_attach(GTK_TABLE(table1), button_add, buttonpointer, buttonpointer + 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    // Position of next button.
    buttonpointer++;
  }

  if (sort) {
    button_sort = gtk_button_new();
    gtk_widget_show(button_sort);
    gtk_table_attach(GTK_TABLE(table1), button_sort, buttonpointer, buttonpointer + 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

    alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_widget_show(alignment1);
    gtk_container_add(GTK_CONTAINER(button_sort), alignment1);

    hbox4 = gtk_hbox_new(FALSE, 2);
    gtk_widget_show(hbox4);
    gtk_container_add(GTK_CONTAINER(alignment1), hbox4);

    image1 = gtk_image_new_from_stock("gtk-sort-ascending", GTK_ICON_SIZE_BUTTON);
    gtk_widget_show(image1);
    gtk_box_pack_start(GTK_BOX(hbox4), image1, FALSE, FALSE, 0);

    label5 = gtk_label_new_with_mnemonic("_Sort");
    gtk_widget_show(label5);
    gtk_box_pack_start(GTK_BOX(hbox4), label5, FALSE, FALSE, 0);

    // Position of next button.
    buttonpointer++;
  }

  if (import) {
    button_import = gtk_button_new();
    gtk_widget_show(button_import);
    gtk_table_attach(GTK_TABLE(table1), button_import, buttonpointer, buttonpointer + 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

    alignment2 = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_widget_show(alignment2);
    gtk_container_add(GTK_CONTAINER(button_import), alignment2);

    hbox5 = gtk_hbox_new(FALSE, 2);
    gtk_widget_show(hbox5);
    gtk_container_add(GTK_CONTAINER(alignment2), hbox5);

    image2 = gtk_image_new_from_stock("gtk-convert", GTK_ICON_SIZE_BUTTON);
    gtk_widget_show(image2);
    gtk_box_pack_start(GTK_BOX(hbox5), image2, FALSE, FALSE, 0);

    label6 = gtk_label_new_with_mnemonic("_Import");
    gtk_widget_show(label6);
    gtk_box_pack_start(GTK_BOX(hbox5), label6, FALSE, FALSE, 0);

    // Position of next button.
    buttonpointer++;
  }

  if (exprt) {
    button_export = gtk_button_new();
    gtk_widget_show(button_export);
    gtk_table_attach(GTK_TABLE(table1), button_export, buttonpointer, buttonpointer + 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

    alignment3 = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_widget_show(alignment3);
    gtk_container_add(GTK_CONTAINER(button_export), alignment3);

    hbox6 = gtk_hbox_new(FALSE, 2);
    gtk_widget_show(hbox6);
    gtk_container_add(GTK_CONTAINER(alignment3), hbox6);

    image3 = gtk_image_new_from_stock("gtk-convert", GTK_ICON_SIZE_BUTTON);
    gtk_widget_show(image3);
    gtk_box_pack_start(GTK_BOX(hbox6), image3, FALSE, FALSE, 0);

    label7 = gtk_label_new_with_mnemonic("_Export");
    gtk_widget_show(label7);
    gtk_box_pack_start(GTK_BOX(hbox6), label7, FALSE, FALSE, 0);

    // Position of next button.
    buttonpointer++;
  }

  if (duplicates) {
    button_doubles = gtk_button_new();
    gtk_widget_show(button_doubles);
    gtk_table_attach(GTK_TABLE(table1), button_doubles, buttonpointer, buttonpointer + 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

    alignment4 = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_widget_show(alignment4);
    gtk_container_add(GTK_CONTAINER(button_doubles), alignment4);

    hbox7 = gtk_hbox_new(FALSE, 2);
    gtk_widget_show(hbox7);
    gtk_container_add(GTK_CONTAINER(alignment4), hbox7);

    image4 = gtk_image_new_from_stock("gtk-clear", GTK_ICON_SIZE_BUTTON);
    gtk_widget_show(image4);
    gtk_box_pack_start(GTK_BOX(hbox7), image4, FALSE, FALSE, 0);

    label8 = gtk_label_new_with_mnemonic("_Duplicates");
    gtk_widget_show(label8);
    gtk_box_pack_start(GTK_BOX(hbox7), label8, FALSE, FALSE, 0);

    // Position of next button.
    buttonpointer++;
  }

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(editlistdialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(editlistdialog, NULL, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(editlistdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton), true);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(editlistdialog), okbutton, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton), true);

  g_signal_connect_after((gpointer) treeview1, "cursor_changed", G_CALLBACK(on_treeview1_cursor_changed), gpointer(this));
  g_signal_connect_after((gpointer) treeview1, "move_cursor", G_CALLBACK(on_treeview1_move_cursor), gpointer(this));
  g_signal_connect_after((gpointer) treeview1, "select_all", G_CALLBACK(on_treeview1_select_all), gpointer(this));
  g_signal_connect_after((gpointer) treeview1, "select_cursor_parent", G_CALLBACK(on_treeview1_select_cursor_parent), gpointer(this));
  g_signal_connect_after((gpointer) treeview1, "select_cursor_row", G_CALLBACK(on_treeview1_select_cursor_row), gpointer(this));
  g_signal_connect_after((gpointer) treeview1, "toggle_cursor_row", G_CALLBACK(on_treeview1_toggle_cursor_row), gpointer(this));
  g_signal_connect_after((gpointer) treeview1, "unselect_all", G_CALLBACK(on_treeview1_unselect_all), gpointer(this));
  if (remove)
    g_signal_connect((gpointer) buttonremove, "clicked", G_CALLBACK(on_removebutton_clicked), gpointer(this));
  if (add)
    g_signal_connect((gpointer) button_add, "clicked", G_CALLBACK(on_button_add_clicked), gpointer(this));
  if (sort)
    g_signal_connect((gpointer) button_sort, "clicked", G_CALLBACK(on_button_sort_clicked), gpointer(this));
  if (import)
    g_signal_connect((gpointer) button_import, "clicked", G_CALLBACK(on_button_import_clicked), gpointer(this));
  if (exprt)
    g_signal_connect((gpointer) button_export, "clicked", G_CALLBACK(on_button_export_clicked), gpointer(this));
  if (duplicates)
    g_signal_connect((gpointer) button_doubles, "clicked", G_CALLBACK(on_button_doubles_clicked), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_label_set_mnemonic_widget(GTK_LABEL(label3), treeview1);

  gtk_widget_grab_focus(treeview1);
  gtk_widget_grab_default(okbutton);

  // Storage.
  liststore = gtk_list_store_new(1, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview1), GTK_TREE_MODEL(liststore));
  g_object_unref(liststore);
  listview_set_strings(treeview1, liststore, *mylines);
  GtkCellRenderer *renderer;
  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview1), column);
  select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview1));
  gtk_tree_selection_set_mode(select, GTK_SELECTION_MULTIPLE);
}


EditListDialog::~EditListDialog()
{
  gtk_widget_destroy(editlistdialog);
}


int EditListDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(editlistdialog));
}


void EditListDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((EditListDialog *) user_data)->on_okbutton();
}


void EditListDialog::on_okbutton()
{
  vector < ustring > strings = listview_get_strings(treeview1);
  mylines->assign(strings.begin(), strings.end());
}


void EditListDialog::on_removebutton_clicked(GtkButton * button, gpointer user_data)
{
  ((EditListDialog *) user_data)->on_remove();
}


void EditListDialog::on_remove()
{
  // Delete each selected row.
  vector < GtkTreeIter > iters;
  gtk_tree_selection_selected_foreach(select, EditListDialog::on_collect_iters, gpointer(&iters));
  for (unsigned int i = 0; i < iters.size(); i++) {
    gtk_list_store_remove(liststore, &iters[i]);
  }
  // Set GUI.
  on_cursor();
}


void EditListDialog::on_collect_iters(GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
  ((vector < GtkTreeIter > *)data)->push_back(*iter);
}


void EditListDialog::on_button_add_clicked(GtkButton * button, gpointer user_data)
{
  ((EditListDialog *) user_data)->on_add();
}


void EditListDialog::on_add()
// Adds tasks.
{
  if (myaddables) {
    ListviewMDialog dialog("Select a task to add", *myaddables, false, NULL);
    if (dialog.run() == GTK_RESPONSE_OK) {
      for (unsigned int i = 0; i < dialog.foci.size(); i++) {
        GtkTreeIter iter;
        gtk_list_store_append(liststore, &iter);
        gtk_list_store_set(liststore, &iter, 0, dialog.foci[i].c_str(), -1);
      }
    }
  } else {
    EntryDialog dialog("New value", "Enter a new value", "");
    if (dialog.run() == GTK_RESPONSE_OK) {
      GtkTreeIter iter;
      gtk_list_store_append(liststore, &iter);
      gtk_list_store_set(liststore, &iter, 0, dialog.entered_value.c_str(), -1);
    }
  }
}


void EditListDialog::on_button_sort_clicked(GtkButton * button, gpointer user_data)
{
  ((EditListDialog *) user_data)->on_sort();
}


void EditListDialog::on_sort()
{
  vector < ustring > lines;
  lines = listview_get_strings(treeview1);
  sort(lines.begin(), lines.end());
  listview_set_strings(treeview1, liststore, lines);
}


void EditListDialog::on_button_import_clicked(GtkButton * button, gpointer user_data)
{
  ((EditListDialog *) user_data)->on_import();
}


void EditListDialog::on_import()
{
  ustring filename = gtkw_file_chooser_open(editlistdialog, "Open File", "");
  if (filename.empty())
    return;
  vector < ustring > lines;
  lines = listview_get_strings(treeview1);
  ReadText rt(filename, true);
  for (unsigned int i = 0; i < rt.lines.size(); i++)
    lines.push_back(rt.lines[i]);
  listview_set_strings(treeview1, liststore, lines);
}


void EditListDialog::on_button_export_clicked(GtkButton * button, gpointer user_data)
{
  ((EditListDialog *) user_data)->on_export();
}


void EditListDialog::on_export()
{
  ustring filename = gtkw_file_chooser_save(editlistdialog, "", "");
  if (filename.empty())
    return;
  vector < ustring > lines = listview_get_strings(treeview1);
  write_lines(filename, lines);
}


void EditListDialog::on_button_doubles_clicked(GtkButton * button, gpointer user_data)
{
  ((EditListDialog *) user_data)->on_duplicates();
}


void EditListDialog::on_duplicates()
{
  vector < ustring > lines;
  lines = listview_get_strings(treeview1);
  vector < ustring > lines_without_duplicates;
  set < ustring > checked_duplicates;
  for (unsigned int i = 0; i < lines.size(); i++) {
    if (checked_duplicates.find(lines[i]) == checked_duplicates.end()) {
      lines_without_duplicates.push_back(lines[i]);
      checked_duplicates.insert(lines[i]);
    }
  }
  unsigned int duplicates_removed;
  duplicates_removed = lines.size() - lines_without_duplicates.size();
  if (duplicates_removed == 0) {
    gtkw_dialog_info(editlistdialog, "There were no duplicates");
  } else {
    if (gtkw_dialog_question(editlistdialog, "Do you want to remove " + convert_to_string(duplicates_removed) + " duplicates?", GTK_RESPONSE_YES) == GTK_RESPONSE_YES) {
      listview_set_strings(treeview1, liststore, lines_without_duplicates);
    }
  }
}


void EditListDialog::on_treeview1_cursor_changed(GtkTreeView * treeview, gpointer user_data)
{
  ((EditListDialog *) user_data)->on_cursor();
}


void EditListDialog::on_cursor()
{
  if (buttonremove) {
    vector < GtkTreeIter > iters;
    gtk_tree_selection_selected_foreach(select, EditListDialog::on_collect_iters, gpointer(&iters));
    gtk_widget_set_sensitive(buttonremove, iters.size() > 0);
  }
}


gboolean EditListDialog::on_treeview1_move_cursor(GtkTreeView * treeview, GtkMovementStep step, gint count, gpointer user_data)
{
  return FALSE;
}


gboolean EditListDialog::on_treeview1_select_all(GtkTreeView * treeview, gpointer user_data)
{
  ((EditListDialog *) user_data)->on_cursor();
  return FALSE;
}


gboolean EditListDialog::on_treeview1_select_cursor_parent(GtkTreeView * treeview, gpointer user_data)
{
  return FALSE;
}


gboolean EditListDialog::on_treeview1_select_cursor_row(GtkTreeView * treeview, gboolean start_editing, gpointer user_data)
{
  return FALSE;
}


gboolean EditListDialog::on_treeview1_toggle_cursor_row(GtkTreeView * treeview, gpointer user_data)
{
  return FALSE;
}


gboolean EditListDialog::on_treeview1_unselect_all(GtkTreeView * treeview, gpointer user_data)
{
  ((EditListDialog *) user_data)->on_cursor();
  return FALSE;
}


