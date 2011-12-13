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
#include "dialogopenstylesheet.h"
#include "directories.h"
#include "listview.h"
#include "keyboard.h"
#include "stylesheetutils.h"
#include "help.h"

OpenStylesheetDialog::OpenStylesheetDialog(OpenStylesheetDialogType dialogtype, const ustring & currentstylesheet)
{
  // Get all stylesheets and the title.
  vector < ustring > stylesheets;

  ustring title;
  switch (dialogtype) {
  case osdtSwitch:
    {
      // Show all stylesheets.
      stylesheet_get_ones_available(stylesheets);
      title = "Switch stylesheet";
      break;
    }
  case osdtDelete:
    {
      // Show all sheets minus current one.
      vector < ustring > temp_sheets;
      stylesheet_get_ones_available(temp_sheets);
      for (unsigned int i = 0; i < temp_sheets.size(); i++)
        if (temp_sheets[i] != currentstylesheet)
          stylesheets.push_back(temp_sheets[i]);
      title = "Delete stylesheet";
      break;
    }
  }

  openstyledialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(openstyledialog), title.c_str());
  gtk_window_set_position(GTK_WINDOW(openstyledialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(openstyledialog), TRUE);

  dialog_vbox1 = GTK_DIALOG(openstyledialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new("Type the first couple of characters of the stylesheet, or select it,\nand then press OK or Enter");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 4);

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow1, FALSE, FALSE, 4);
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

  // Fill the treeview with the sorted projects.
  sort(stylesheets.begin(), stylesheets.end());
  listview_set_strings(treeview1, store1, stylesheets);
  // Set the project now opened.
  if (currentstylesheet.empty()) {
    if (stylesheets.size() > 0)
      listview_focus_string(treeview1, stylesheets[0]);
  } else {
    listview_focus_string(treeview1, currentstylesheet);
  }

  dialog_action_area1 = GTK_DIALOG(openstyledialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(openstyledialog, NULL, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(openstyledialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(openstyledialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  // Disable the OK button if there are no projects.
  if (stylesheets.size() == 0)
    gtk_widget_set_sensitive(okbutton, FALSE);

  g_signal_connect((gpointer) treeview1, "row_activated", G_CALLBACK(on_treeview1_row_activated), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(static_on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_focus(treeview1);
  gtk_widget_grab_default(okbutton);
}

OpenStylesheetDialog::~OpenStylesheetDialog()
{
  gtk_widget_destroy(openstyledialog);
}

int OpenStylesheetDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(openstyledialog));
}

void OpenStylesheetDialog::on_treeview1_row_activated(GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data)
{
  ((OpenStylesheetDialog *) user_data)->on_treeview();
}

void OpenStylesheetDialog::on_treeview()
{
  on_okbutton_clicked();
  gtk_dialog_response(GTK_DIALOG(openstyledialog), GTK_RESPONSE_OK);
}

void OpenStylesheetDialog::static_on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((OpenStylesheetDialog *) user_data)->on_okbutton_clicked();
}

void OpenStylesheetDialog::on_okbutton_clicked()
{
  stylesheet = listview_get_active_string(treeview1);
}
