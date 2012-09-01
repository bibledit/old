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
#include "dialogtidy.h"
#include "utilities.h"
#include "projectutils.h"
#include "books.h"
#include "help.h"
#include "settings.h"
#include "shortcuts.h"

enum { COLUMN_ABBREV, COLUMN_EDITABLE, COLUMN_TEXT, NUM_COLUMNS };

TidyDialog::TidyDialog(int dummy)
{
  // Get configurations.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());

  // Shortcuts.
  Shortcuts shortcuts(0);

  tidydialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(tidydialog), "Tidy Text");
  gtk_window_set_position(GTK_WINDOW(tidydialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(tidydialog), TRUE);
  gtk_window_set_type_hint(GTK_WINDOW(tidydialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG(tidydialog));
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 2);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label11 = gtk_label_new("Tidy Text can be used while inserting Bible notes. It makes changes in the text that is inserted, following the settings made below.");
  gtk_widget_show(label11);
  gtk_box_pack_start(GTK_BOX(vbox1), label11, FALSE, FALSE, 0);
  gtk_label_set_line_wrap(GTK_LABEL(label11), TRUE);
  gtk_misc_set_alignment(GTK_MISC(label11), 0, 0.5);

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator1, FALSE, FALSE, 0);

  checkbutton_translate_books = gtk_check_button_new_with_mnemonic("Translate certain text to book abbreviations");
  gtk_widget_show(checkbutton_translate_books);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_translate_books, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_translate_books);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_translate_books), settings->genconfig.tidy_translate_get());

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_SHADOW_IN);

  treeview1 = gtk_tree_view_new();
  gtk_widget_show(treeview1);
  gtk_container_add(GTK_CONTAINER(scrolledwindow1), treeview1);
  gtk_widget_set_size_request(treeview1, -1, 300);
  gtk_tree_view_set_reorderable(GTK_TREE_VIEW(treeview1), TRUE);

  checkbutton_normalize_hyphens = gtk_check_button_new_with_mnemonic("Normalize hyphens");
  gtk_widget_show(checkbutton_normalize_hyphens);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_normalize_hyphens, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_normalize_hyphens);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_normalize_hyphens), settings->genconfig.tidy_normalize_hyphens_get());

  checkbutton_space_between_chapter_verse = gtk_check_button_new_with_mnemonic("Remove the space between chapter and verse, e.g. Mat. 1. 1 becomes Mat. 1.1");
  gtk_widget_show(checkbutton_space_between_chapter_verse);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_space_between_chapter_verse, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_space_between_chapter_verse);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_space_between_chapter_verse), settings->genconfig.tidy_space_between_chapter_verse_get());

  checkbutton_space_series_verses = gtk_check_button_new_with_mnemonic("Remove the space after a comma in a series of verses, e.g. 10, 11 becomes 10,11");
  gtk_widget_show(checkbutton_space_series_verses);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_space_series_verses, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_space_series_verses);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_space_series_verses), settings->genconfig.tidy_space_series_verses_get());

  checkbutton_full_stop_ends_text = gtk_check_button_new_with_mnemonic("Always end the text with a full stop");
  gtk_widget_show(checkbutton_full_stop_ends_text);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_full_stop_ends_text, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_full_stop_ends_text);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_full_stop_ends_text), settings->genconfig.tidy_full_stop_ends_text_get());

  checkbutton_ampersand_semicolon = gtk_check_button_new_with_mnemonic("Change the ampersand in, e.g., Mat. 10.1 & 11.2 to a semicolon: Mat. 10.1; 11.2");
  gtk_widget_show(checkbutton_ampersand_semicolon);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_ampersand_semicolon, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_ampersand_semicolon);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_ampersand_semicolon), settings->genconfig.tidy_ampersand_semicolon_get());

  checkbutton_space_before_punctuation = gtk_check_button_new_with_mnemonic("Remove a space before a (semi)colon, full stop, etc.");
  gtk_widget_show(checkbutton_space_before_punctuation);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_space_before_punctuation, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_space_before_punctuation);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_space_before_punctuation), settings->genconfig.tidy_space_before_punctuation_get());

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(tidydialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(tidydialog, NULL, &shortcuts, "menu-preferences/tidy-text");

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(tidydialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton1), true);

  shortcuts.stockbutton(cancelbutton1);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(tidydialog), okbutton1, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton1), true);

  shortcuts.stockbutton(okbutton1);

  shortcuts.process();

  g_signal_connect((gpointer) checkbutton_translate_books, "toggled", G_CALLBACK(on_checkbutton_translate_books_toggled), gpointer(this));
  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));

  gtk_widget_grab_focus(okbutton1);
  gtk_widget_grab_default(okbutton1);

  // Storage, renderer, column and selection.
  model = gtk_list_store_new(NUM_COLUMNS, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview1), GTK_TREE_MODEL(model));
  g_object_unref(model);
  GtkCellRenderer *renderer1 = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview1), -1, "Abbreviation", renderer1, "text", COLUMN_ABBREV, NULL);
  GtkCellRenderer *renderer2 = gtk_cell_renderer_text_new();
  g_signal_connect(renderer2, "edited", G_CALLBACK(cell_edited), gpointer(this));
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview1), -1, "Texts", renderer2, "text", COLUMN_TEXT, "editable", COLUMN_EDITABLE, NULL);
  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview1)), GTK_SELECTION_SINGLE);

  // Load translation texts.
  vector < int >ids = settings->genconfig.tidy_books_get();
  vector < ustring > texts = settings->genconfig.tidy_texts_get();
  vector < unsigned int >bible_ids = books_type_to_ids(btOldTestament);
  {
    vector < unsigned int >ids2 = books_type_to_ids(btNewTestament);
    for (unsigned int i = 0; i < ids2.size(); i++)
      bible_ids.push_back(ids2[i]);
  }
  for (unsigned int i = 0; i < bible_ids.size(); i++) {
    GtkTreeIter iter;
    gtk_list_store_append(model, &iter);
    ustring text;
    for (unsigned int i2 = 0; i2 < ids.size(); i2++) {
      if (ids[i2] == (int)bible_ids[i])
        text = texts[i2];
    }
    gtk_list_store_set(model, &iter, COLUMN_ABBREV, books_id_to_abbreviation(projectconfig->language_get(), bible_ids[i]).c_str(), COLUMN_EDITABLE, 1, COLUMN_TEXT, text.c_str(), -1);
  }

  gui();
}

TidyDialog::~TidyDialog()
{
  gtk_widget_destroy(tidydialog);
}

int TidyDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(tidydialog));
}

void TidyDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((TidyDialog *) user_data)->on_okbutton();
}

void TidyDialog::on_okbutton()
{
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());

  settings->genconfig.tidy_translate_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_translate_books)));

  vector < int >ids;
  vector < ustring > texts;
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
    gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, COLUMN_ABBREV, &str_abbrev, COLUMN_TEXT, &str_text, -1);
    if (strcmp(str_text, "") != 0) {
      ids.push_back(books_abbreviation_to_id(projectconfig->language_get(), str_abbrev));
      texts.push_back(str_text);
    }
    gtk_tree_path_free(path);
    g_free(str_abbrev);
    g_free(str_text);
    valid = gtk_tree_model_iter_next(model, &iter);
    index++;
  }
  settings->genconfig.tidy_books_set(ids);
  settings->genconfig.tidy_texts_set(texts);

  settings->genconfig.tidy_normalize_hyphens_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_normalize_hyphens)));
  settings->genconfig.tidy_space_between_chapter_verse_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_space_between_chapter_verse)));
  settings->genconfig.tidy_space_series_verses_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_space_series_verses)));
  settings->genconfig.tidy_full_stop_ends_text_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_full_stop_ends_text)));
  settings->genconfig.tidy_ampersand_semicolon_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_space_before_punctuation)));
  settings->genconfig.tidy_space_before_punctuation_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_space_before_punctuation)));
}

void TidyDialog::on_checkbutton_translate_books_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((TidyDialog *) user_data)->on_checkbutton_translate_books();
}

void TidyDialog::on_checkbutton_translate_books()
{
  gui();
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_translate_books)))
    gtk_widget_grab_focus(treeview1);
}

void TidyDialog::cell_edited(GtkCellRendererText * cell, const gchar * path_string, const gchar * new_text, gpointer data)
{
  ((TidyDialog *) data)->on_cell_edited(cell, path_string, new_text);
}

void TidyDialog::on_cell_edited(GtkCellRendererText * cell, const gchar * path_string, const gchar * new_text)
{
  // Set / initialize some variables.
  GtkTreeModel *tmodel = (GtkTreeModel *) model;
  GtkTreePath *path = gtk_tree_path_new_from_string(path_string);
  GtkTreeIter iter;
  gtk_tree_model_get_iter(tmodel, &iter, path);
  // Free memory for the old text.
  gchar *old_text;
  gtk_tree_model_get(tmodel, &iter, COLUMN_TEXT, &old_text, -1);
  g_free(old_text);
  // Make copy of new text and put it in the store. 
  gtk_list_store_set(GTK_LIST_STORE(model), &iter, COLUMN_TEXT, g_strdup(new_text), -1);
  // Free path.
  gtk_tree_path_free(path);
}

void TidyDialog::gui()
{
  gtk_widget_set_sensitive(scrolledwindow1, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_translate_books)));
}
