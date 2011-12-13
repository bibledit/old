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
#include <glib.h>
#include "dialogbooknames.h"
#include "utilities.h"
#include "projectutils.h"
#include "books.h"
#include "help.h"
#include "settings.h"
#include "shortcuts.h"
#include "screen.h"


enum { COLUMN_BIBLEDIT, COLUMN_EDITABLE, COLUMN_URL, NUM_COLUMNS };


BooknamesDialog::BooknamesDialog(const map <unsigned int, ustring>& books, const gchar * info, const gchar * heading2)
{
  // Shortcuts.
  Shortcuts shortcuts(0);

  resourcebooksdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(resourcebooksdialog), "Books");
  gtk_window_set_position(GTK_WINDOW(resourcebooksdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(resourcebooksdialog), TRUE);
  gtk_window_set_type_hint(GTK_WINDOW(resourcebooksdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG(resourcebooksdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 2);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label_info = gtk_label_new(info);
  gtk_widget_show(label_info);
  gtk_box_pack_start(GTK_BOX(vbox1), label_info, FALSE, FALSE, 0);
  gtk_label_set_line_wrap(GTK_LABEL(label_info), TRUE);
  gtk_misc_set_alignment(GTK_MISC(label_info), 0, 0.5);

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator1, FALSE, FALSE, 0);

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_SHADOW_IN);

  treeview1 = gtk_tree_view_new();
  gtk_widget_show(treeview1);
  gtk_container_add(GTK_CONTAINER(scrolledwindow1), treeview1);
  gtk_tree_view_set_reorderable(GTK_TREE_VIEW(treeview1), TRUE);

  dialog_action_area1 = GTK_DIALOG(resourcebooksdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(resourcebooksdialog, NULL, &shortcuts, NULL);

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(resourcebooksdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton1);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(resourcebooksdialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(okbutton1);

  shortcuts.process();

  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));

  gtk_widget_grab_focus(okbutton1);
  gtk_widget_grab_default(okbutton1);

  // Storage, renderer, column and selection.
  model = gtk_list_store_new(NUM_COLUMNS, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview1), GTK_TREE_MODEL(model));
  g_object_unref(model);
  GtkCellRenderer *renderer1 = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview1), -1, "Book in Bibledit-Gtk", renderer1, "text", COLUMN_BIBLEDIT, NULL);
  GtkCellRenderer *renderer2 = gtk_cell_renderer_text_new();
  g_signal_connect(renderer2, "edited", G_CALLBACK(cell_edited), gpointer(this));
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview1), -1, heading2, renderer2, "text", COLUMN_URL, "editable", COLUMN_EDITABLE, NULL);
  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview1)), GTK_SELECTION_SINGLE);

  // Load texts.
  vector < unsigned int >ids = books_type_to_ids(btUnknown);
  map <unsigned int, ustring> books_copy = books;
  for (unsigned int i = 0; i < ids.size(); i++) {
    GtkTreeIter iter;
    gtk_list_store_append(model, &iter);
    unsigned int id = ids[i];
    ustring text = books_copy[id];
    gtk_list_store_set(model, &iter, COLUMN_BIBLEDIT, books_id_to_english(ids[i]).c_str(), COLUMN_EDITABLE, 1, COLUMN_URL, text.c_str(), -1);
  }
  
  new DialogAutoScaler (resourcebooksdialog, G_MAXINT);
}


BooknamesDialog::~BooknamesDialog()
{
  gtk_widget_destroy(resourcebooksdialog);
}


int BooknamesDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(resourcebooksdialog));
}


void BooknamesDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((BooknamesDialog *) user_data)->on_okbutton();
}


void BooknamesDialog::on_okbutton()
{
  GtkTreeModel *model;
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview1));
  GtkTreeIter iter;
  gboolean valid;
  int index = 0;
  valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {
    GtkTreePath *path = gtk_tree_path_new_from_indices(index, -1);
    gchar *str_abbrev;
    gchar *str_text;
    gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, COLUMN_BIBLEDIT, &str_abbrev, COLUMN_URL, &str_text, -1);
    if (strcmp(str_text, "") != 0) {
      unsigned int id = books_english_to_id(str_abbrev);
      ustring text = str_text;
      if (!text.empty()) {
        newbooks[id] = text;
      }
    }
    gtk_tree_path_free(path);
    g_free(str_abbrev);
    g_free(str_text);
    valid = gtk_tree_model_iter_next(model, &iter);
    index++;
  }
}


void BooknamesDialog::cell_edited(GtkCellRendererText * cell, const gchar * path_string, const gchar * new_text, gpointer data)
{
  ((BooknamesDialog *) data)->on_cell_edited(cell, path_string, new_text);
}


void BooknamesDialog::on_cell_edited(GtkCellRendererText * cell, const gchar * path_string, const gchar * new_text)
{
  // Set / initialize some variables.
  GtkTreeModel *tmodel = (GtkTreeModel *) model;
  GtkTreePath *path = gtk_tree_path_new_from_string(path_string);
  GtkTreeIter iter;
  gtk_tree_model_get_iter(tmodel, &iter, path);
  // Free memory for the old text.
  gchar *old_text;
  gtk_tree_model_get(tmodel, &iter, COLUMN_URL, &old_text, -1);
  g_free(old_text);
  // Make copy of new text and put it in the store. 
  gtk_list_store_set(GTK_LIST_STORE(model), &iter, COLUMN_URL, g_strdup(new_text), -1);
  // Free path.
  gtk_tree_path_free(path);
}


