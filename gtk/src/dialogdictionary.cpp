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
#include "dialogdictionary.h"
#include "utilities.h"
#include "bible.h"
#include "dialogselectchapters.h"
#include "projectutils.h"
#include "books.h"
#include "help.h"
#include "settings.h"
#include "listview.h"
#include "spelling.h"
#include "dialoglistview.h"
#include "dialogeditdictionary.h"
#include "git.h"


enum { COLUMN_BOOK, NUM_COLUMNS };


DictionaryDialog::DictionaryDialog(const ustring & project)
{
  // Store data.
  myproject = project;

  dictionarydialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dictionarydialog), "Dictionaries");
  gtk_window_set_position(GTK_WINDOW(dictionarydialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(dictionarydialog), TRUE);
  gtk_window_set_destroy_with_parent(GTK_WINDOW(dictionarydialog), TRUE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG (dictionarydialog));
  gtk_widget_show(dialog_vbox1);

  label_info = gtk_label_new_with_mnemonic("To change the order in which a dictionary is consulted, drag it to another location.");
  gtk_widget_show(label_info);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label_info, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_info), 0, 0.5);

  treeview_dictionaries = gtk_tree_view_new();
  gtk_widget_show(treeview_dictionaries);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), treeview_dictionaries, TRUE, TRUE, 0);
  gtk_widget_set_size_request(treeview_dictionaries, -1, 400);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview_dictionaries), FALSE);
  gtk_tree_view_set_reorderable(GTK_TREE_VIEW(treeview_dictionaries), TRUE);

  hbox_buttons = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox_buttons);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox_buttons, TRUE, TRUE, 0);

  button_add = gtk_button_new_from_stock("gtk-connect");
  gtk_widget_show(button_add);
  gtk_box_pack_start(GTK_BOX(hbox_buttons), button_add, FALSE, FALSE, 0);

  button_delete = gtk_button_new_from_stock("gtk-disconnect");
  gtk_widget_show(button_delete);
  gtk_box_pack_start(GTK_BOX(hbox_buttons), button_delete, FALSE, FALSE, 0);

  button_edit = gtk_button_new_from_stock("gtk-edit");
  gtk_widget_show(button_edit);
  gtk_box_pack_start(GTK_BOX(hbox_buttons), button_edit, FALSE, FALSE, 0);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(dictionarydialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(dictionarydialog, NULL, NULL, "file/project/project-properties/dictionaries");

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(dictionarydialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton1), true);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(dictionarydialog), okbutton1, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton1), true);

  g_signal_connect((gpointer) treeview_dictionaries, "cursor_changed", G_CALLBACK(on_treeview_dictionaries_cursor_changed), gpointer(this));
  g_signal_connect((gpointer) button_add, "clicked", G_CALLBACK(on_button_add_clicked), gpointer(this));
  g_signal_connect((gpointer) button_delete, "clicked", G_CALLBACK(on_button_delete_clicked), gpointer(this));
  g_signal_connect((gpointer) button_edit, "clicked", G_CALLBACK(on_button_edit_clicked), gpointer(this));
  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));

  gtk_widget_grab_focus(treeview_dictionaries);
  gtk_widget_grab_default(okbutton1);

  // Storage, renderer, column and selection.
  model = gtk_list_store_new(NUM_COLUMNS, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview_dictionaries), GTK_TREE_MODEL(model));
  g_object_unref(model);
  GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview_dictionaries), -1, "", renderer, "text", COLUMN_BOOK, NULL);
  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_dictionaries)), GTK_SELECTION_SINGLE);

  // Load content.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(myproject);
  vector < ustring > dictionaries = projectconfig->spelling_dictionaries_get();
  GtkTreeIter iter;
  for (unsigned int i = 0; i < dictionaries.size(); i++) {
    gtk_list_store_append(model, &iter);
    gtk_list_store_set(model, &iter, COLUMN_BOOK, dictionaries[i].c_str(), -1);
  }
  
  // Gui update.
  on_treeview_dictionaries_cursor ();
}


DictionaryDialog::~DictionaryDialog()
{
  gtk_widget_destroy(dictionarydialog);
}


int DictionaryDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dictionarydialog));
}


void DictionaryDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((DictionaryDialog *) user_data)->on_okbutton();
}


void DictionaryDialog::on_okbutton()
{
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(myproject);
  vector < ustring > dictionaries = listview_get_strings(treeview_dictionaries);
  projectconfig->spelling_dictionaries_set(dictionaries);
}


void DictionaryDialog::on_button_add_clicked(GtkButton * button, gpointer user_data)
{
  ((DictionaryDialog *) user_data)->on_button_add();
}


void DictionaryDialog::on_button_add()
{
  // Get the available dictionaries in a certain order:
  vector < ustring > dictionaries;
  // - current project's shared dictionary.
  dictionaries.push_back(spelling_project_dictionary(myproject));
  // - enchant provided dictionaries.
  vector < ustring > enchantdics = spelling_enchant_dictionaries();
  for (unsigned int i = 0; i < enchantdics.size(); i++) {
    dictionaries.push_back(enchantdics[i]);
  }
  // - global dictionary.
  dictionaries.push_back(spelling_global_dictionary());
  // - other projects' shared dictionaries.
  vector < ustring > projects = projects_get_all();
  for (unsigned int i = 0; i < projects.size(); i++) {
    if (projects[i] != myproject) {
      dictionaries.push_back(spelling_project_dictionary(projects[i]));
    }
  }

  // Take out the ones already loaded.
  {
    vector < ustring > loaded_dictionaries = listview_get_strings(treeview_dictionaries);
    set < ustring > dictionary_set(loaded_dictionaries.begin(), loaded_dictionaries.end());
    vector < ustring > temporal_dictionaries;
    for (unsigned int i = 0; i < dictionaries.size(); i++) {
      if (dictionary_set.find(dictionaries[i]) == dictionary_set.end()) {
        temporal_dictionaries.push_back(dictionaries[i]);
      }
    }
    dictionaries = temporal_dictionaries;
  }

  // Display these.
  ListviewDialog dialog("Select a dictionary", dictionaries, "", false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    // Add to the list.
    GtkTreeIter iter;
    gtk_list_store_append(model, &iter);
    gtk_list_store_set(model, &iter, COLUMN_BOOK, dialog.focus.c_str(), -1);
    // Focus new dictionary.
    listview_focus_string(treeview_dictionaries, dialog.focus, true);
  }
}


void DictionaryDialog::on_button_delete_clicked(GtkButton * button, gpointer user_data)
{
  ((DictionaryDialog *) user_data)->on_button_delete();
}


void DictionaryDialog::on_button_delete()
{
  list_view_erase_selection(treeview_dictionaries);
}


void DictionaryDialog::on_treeview_dictionaries_cursor_changed(GtkTreeView * treeview, gpointer user_data)
{
  ((DictionaryDialog *) user_data)->on_treeview_dictionaries_cursor();
}


void DictionaryDialog::on_treeview_dictionaries_cursor()
{
  ustring dictionary = listview_get_active_string(treeview_dictionaries);
  gtk_widget_set_sensitive(button_delete, !dictionary.empty());
  bool editable = spelling_dictionary_editable(dictionary);
  gtk_widget_set_sensitive(button_edit, editable);
}


void DictionaryDialog::on_button_edit_clicked(GtkButton * button, gpointer user_data)
{
  ((DictionaryDialog *) user_data)->on_button_edit();
}


void DictionaryDialog::on_button_edit()
{
  // Edit dictionary.
  EditDictionaryDialog dialog(listview_get_active_string(treeview_dictionaries));
  dialog.run();
}

