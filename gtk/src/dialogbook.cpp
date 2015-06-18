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
#include "dialogbook.h"
#include "utilities.h"
#include "bible.h"
#include "dialogselectchapters.h"
#include "projectutils.h"
#include "books.h"
#include "help.h"
#include "settings.h"
#include "listview.h"
#include "screen.h"
#include <glib/gi18n.h>

enum { COLUMN_BOOK, NUM_COLUMNS };


BookDialog::BookDialog(const ustring & project)
{
  // Store data.
  myproject = project;

  bookdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(bookdialog), _("Books"));
  gtk_window_set_position(GTK_WINDOW(bookdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(bookdialog), TRUE);
  gtk_window_set_destroy_with_parent(GTK_WINDOW(bookdialog), TRUE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG(bookdialog));
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 2);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label11 = gtk_label_new(_("To reorder a book, drag it to the new location."));
  gtk_widget_show(label11);
  gtk_box_pack_start(GTK_BOX(vbox1), label11, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label11), 0, 0.5);

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_SHADOW_IN);

  treeview1 = gtk_tree_view_new();
  gtk_widget_show(treeview1);
  gtk_container_add(GTK_CONTAINER(scrolledwindow1), treeview1);
  gtk_tree_view_set_reorderable(GTK_TREE_VIEW(treeview1), TRUE);

  hbox8 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox8);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox8, FALSE, FALSE, 0);

  button_standard_order = gtk_button_new();
  gtk_widget_show(button_standard_order);
  gtk_box_pack_start(GTK_BOX(hbox8), button_standard_order, FALSE, FALSE, 0);

  alignment5 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment5);
  gtk_container_add(GTK_CONTAINER(button_standard_order), alignment5);

  hbox10 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox10);
  gtk_container_add(GTK_CONTAINER(alignment5), hbox10);

  image5 = gtk_image_new_from_stock("gtk-sort-ascending", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image5);
  gtk_box_pack_start(GTK_BOX(hbox10), image5, FALSE, FALSE, 0);

  label9 = gtk_label_new_with_mnemonic(_("_Standard order"));
  gtk_widget_show(label9);
  gtk_box_pack_start(GTK_BOX(hbox10), label9, FALSE, FALSE, 0);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(bookdialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(bookdialog, NULL, NULL, NULL);

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(bookdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton1), true);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(bookdialog), okbutton1, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton1), true);

  g_signal_connect((gpointer) button_standard_order, "clicked", G_CALLBACK(on_button_standard_order_clicked), gpointer(this));
  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));

  gtk_widget_grab_focus(treeview1);
  gtk_widget_grab_default(okbutton1);

  // Storage, renderer, column and selection.
  model = gtk_list_store_new(NUM_COLUMNS, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_INT);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview1), GTK_TREE_MODEL(model));
  g_object_unref(model);
  GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview1), -1, "Name", renderer, "text", COLUMN_BOOK, NULL);
  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview1)), GTK_SELECTION_SINGLE);

  // Load content.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(myproject);
  vector < unsigned int >books = books_type_to_ids(btUnknown);
  books_order(myproject, books);
  GtkTreeIter iter;
  for (unsigned int i = 0; i < books.size(); i++) {
    gtk_list_store_append(model, &iter);
    gtk_list_store_set(model, &iter, COLUMN_BOOK, books_id_to_name(projectconfig->language_get(), books[i]).c_str(), -1);
  }
  
  new DialogAutoScaler (bookdialog, G_MAXINT);
}


BookDialog::~BookDialog()
{
  gtk_widget_destroy(bookdialog);
}


int BookDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(bookdialog));
}


void BookDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((BookDialog *) user_data)->on_okbutton();
}


void BookDialog::on_okbutton()
{
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(myproject);
  vector < ustring > ubooks = listview_get_strings(treeview1);
  vector < int >books;
  for (unsigned int i = 0; i < ubooks.size(); i++) {
    books.push_back(books_name_to_id(projectconfig->language_get(), ubooks[i]));
  }
  projectconfig->book_order_set(books);
}


void BookDialog::on_button_standard_order_clicked(GtkButton * button, gpointer user_data)
{
  ((BookDialog *) user_data)->on_button_standard_order();
}


void BookDialog::on_button_standard_order()
{
  // Get all the book ids.
  vector < ustring > ubooks = listview_get_strings(treeview1);
  vector < unsigned int >books;
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(myproject);
  for (unsigned int i = 0; i < ubooks.size(); i++) {
    books.push_back(books_name_to_id(projectconfig->language_get(), ubooks[i]));
  }
  // Order them.
  books_standard_order(books);
  // Make localized books of them.
  ubooks.clear();
  for (unsigned int i = 0; i < books.size(); i++) {
    ustring book = books_id_to_name(projectconfig->language_get(), books[i]);
    ubooks.push_back(book);
  }
  // Show them.
  GtkTreeModel *model;
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview1));
  listview_set_strings(treeview1, GTK_LIST_STORE(model), ubooks);
}


