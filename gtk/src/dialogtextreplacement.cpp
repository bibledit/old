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
#include "dialogtextreplacement.h"
#include "utilities.h"
#include "projectutils.h"
#include "books.h"
#include "help.h"
#include "settings.h"
#include "shortcuts.h"
#include "textreplacement.h"
#include "screen.h"


enum { COLUMN_ORIGINALS, COLUMN_EDITABLE1, COLUMN_REPLACEMENTS, COLUMN_EDITABLE2, NUM_COLUMNS };


TextReplacementDialog::TextReplacementDialog(int dummy)
{
  // Get configurations.
  extern Settings *settings;

  // Shortcuts.
  Shortcuts shortcuts(0);

  textreplacementdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(textreplacementdialog), "Text Replacement");
  gtk_window_set_position(GTK_WINDOW(textreplacementdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(textreplacementdialog), TRUE);
  gtk_window_set_type_hint(GTK_WINDOW(textreplacementdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG(textreplacementdialog));
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 2);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  checkbutton1 = gtk_check_button_new_with_mnemonic("Replace text when printing and exporting");
  gtk_widget_show(checkbutton1);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton1, FALSE, FALSE, 0);

  shortcuts.button(checkbutton1);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton1), settings->genconfig.text_replacement_get());

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_SHADOW_IN);

  treeview1 = gtk_tree_view_new();
  gtk_widget_show(treeview1);
  gtk_container_add(GTK_CONTAINER(scrolledwindow1), treeview1);
  gtk_tree_view_set_reorderable(GTK_TREE_VIEW(treeview1), TRUE);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(textreplacementdialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(textreplacementdialog, NULL, &shortcuts, "menu-preferences/text-replacement");

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(textreplacementdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton1), true);

  shortcuts.stockbutton(cancelbutton1);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(textreplacementdialog), okbutton1, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton1), true);

  shortcuts.stockbutton(okbutton1);

  shortcuts.process();

  g_signal_connect((gpointer) checkbutton1, "toggled", G_CALLBACK(on_checkbutton1_toggled), gpointer(this));
  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));

  gtk_widget_grab_focus(okbutton1);
  gtk_widget_grab_default(okbutton1);

  // Toggle button.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton1), settings->genconfig.text_replacement_get());

  // Storage, renderer, column and selection.
  model = gtk_list_store_new(NUM_COLUMNS, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_BOOLEAN);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview1), GTK_TREE_MODEL(model));
  g_object_unref(model);
  GtkCellRenderer *renderer1 = gtk_cell_renderer_text_new();
  g_signal_connect(renderer1, "edited", G_CALLBACK(cell_text_edited), gpointer(this));

  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview1), -1, "Texts", renderer1, "text", COLUMN_ORIGINALS, "editable", COLUMN_EDITABLE1, NULL);

  GtkCellRenderer *renderer2 = gtk_cell_renderer_text_new();
  g_signal_connect(renderer2, "edited", G_CALLBACK(cell_replacement_edited), gpointer(this));

  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview1), -1, "Replacements", renderer2, "text", COLUMN_REPLACEMENTS, "editable", COLUMN_EDITABLE2, NULL);

  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview1)), GTK_SELECTION_SINGLE);

  // Load texts and replacements - these get initialized to defaults if there were none.
  vector < ustring > originals;
  vector < ustring > replacements;
  text_replacement_get_words(originals, replacements);

  GtkTreeIter iter;
  for (unsigned int i = 0; i < originals.size(); i++) {
    gtk_list_store_append(model, &iter);
    gtk_list_store_set(model, &iter, COLUMN_ORIGINALS, originals[i].c_str(), COLUMN_EDITABLE1, 1, COLUMN_REPLACEMENTS, replacements[i].c_str(), COLUMN_EDITABLE2, 1, -1);
  }

  gui();

  new DialogAutoScaler (textreplacementdialog, G_MAXINT);
}


TextReplacementDialog::~TextReplacementDialog()
{
  gtk_widget_destroy(textreplacementdialog);
}


int TextReplacementDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(textreplacementdialog));
}


void TextReplacementDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((TextReplacementDialog *) user_data)->on_okbutton();
}


void TextReplacementDialog::on_okbutton()
{
  extern Settings *settings;

  settings->genconfig.text_replacement_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton1)));

  vector < ustring > originals;
  vector < ustring > replacements;
  GtkTreeModel *model;
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview1));
  GtkTreeIter iter;
  gboolean valid;
  valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {
    ustring original = original_get(GTK_TREE_MODEL(model), &iter);
    ustring replacement = replacement_get(GTK_TREE_MODEL(model), &iter);
    if (original != enter_new_text_here())
      if (replacement != enter_new_replacement_here())
        if (!original.empty()) {
          originals.push_back(original);
          replacements.push_back(replacement);
        }
    valid = gtk_tree_model_iter_next(model, &iter);
  }
  settings->genconfig.text_replacement_originals_set(originals);
  settings->genconfig.text_replacement_replacements_set(replacements);
}


void TextReplacementDialog::on_checkbutton1_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((TextReplacementDialog *) user_data)->on_checkbutton1();
}


void TextReplacementDialog::on_checkbutton1()
{
  gui();
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton1)))
    gtk_widget_grab_focus(treeview1);
}


void TextReplacementDialog::cell_text_edited(GtkCellRendererText * cell, const gchar * path_string, const gchar * new_text, gpointer data)
{
  ((TextReplacementDialog *) data)->on_cell_edited(cell, path_string, new_text, COLUMN_ORIGINALS);
}


void TextReplacementDialog::cell_replacement_edited(GtkCellRendererText * cell, const gchar * path_string, const gchar * new_text, gpointer data)
{
  ((TextReplacementDialog *) data)->on_cell_edited(cell, path_string, new_text, COLUMN_REPLACEMENTS);
}


void TextReplacementDialog::on_cell_edited(GtkCellRendererText * cell, const gchar * path_string, const gchar * new_text, int column)
{
  // Set / initialize some variables.
  GtkTreeModel *tmodel = (GtkTreeModel *) model;
  GtkTreePath *path = gtk_tree_path_new_from_string(path_string);
  GtkTreeIter iter;
  gtk_tree_model_get_iter(tmodel, &iter, path);
  // Free memory for the old text.
  gchar *old_text;
  gtk_tree_model_get(tmodel, &iter, column, &old_text, -1);
  g_free(old_text);
  // Make copy of new text and put it in the store. 
  gtk_list_store_set(GTK_LIST_STORE(model), &iter, column, g_strdup(new_text), -1);
  // Free path.
  gtk_tree_path_free(path);
  // Update gui.
  gui();
}


void TextReplacementDialog::gui()
{
  // Sensitivity of the entry area.
  gtk_widget_set_sensitive(scrolledwindow1, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton1)));

  // There should always be one line for new data.
  bool newdatafound = false;
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview1));
  GtkTreeIter iter;
  gboolean valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {
    if (original_get(GTK_TREE_MODEL(model), &iter) == enter_new_text_here())
      if (replacement_get(GTK_TREE_MODEL(model), &iter) == enter_new_replacement_here())
        newdatafound = true;
    valid = gtk_tree_model_iter_next(model, &iter);
  }
  if (!newdatafound) {
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, COLUMN_ORIGINALS, enter_new_text_here(), COLUMN_EDITABLE1, 1, COLUMN_REPLACEMENTS, enter_new_replacement_here(), COLUMN_EDITABLE2, 1, -1);
  }
}


const char *TextReplacementDialog::enter_new_text_here()
{
  return "<Enter new text here>";
}


const char *TextReplacementDialog::enter_new_replacement_here()
{
  return "<Enter new replacement here>";
}


ustring TextReplacementDialog::original_get(GtkTreeModel * model, GtkTreeIter * iter)
{
  gchar *str;
  gtk_tree_model_get(model, iter, COLUMN_ORIGINALS, &str, -1);
  ustring s(str);
  g_free(str);
  return s;
}


ustring TextReplacementDialog::replacement_get(GtkTreeModel * model, GtkTreeIter * iter)
{
  gchar *str;
  gtk_tree_model_get(model, iter, COLUMN_REPLACEMENTS, &str, -1);
  ustring s(str);
  g_free(str);
  return s;
}


