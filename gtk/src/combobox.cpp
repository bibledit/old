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

#include "combobox.h"
#include "tiny_utilities.h"

ustring combobox_get_active_string(GtkWidget * combobox)
// Gets the currently active string in a combobox.
{
  ustring active_string;
  GtkTreeModel *model;
  model = gtk_combo_box_get_model(GTK_COMBO_BOX(combobox));
  GtkTreeIter iter;
  if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(combobox), &iter)) {
    gchar *value;
    gtk_tree_model_get(model, &iter, 0, &value, -1);
    active_string = value;
    g_free(value);
  }
  return active_string;
}

unsigned int combobox_get_active_index(GtkWidget * combobox)
// Gets the currently active index in a combobox.
{
  return gtk_combo_box_get_active(GTK_COMBO_BOX(combobox));
}

unsigned int combobox_get_string_count(GtkWidget * combobox)
// Gets the number of strings loaded in the combobox.
{
  unsigned int string_count = 0;
  GtkTreeModel *model;
  model = gtk_combo_box_get_model(GTK_COMBO_BOX(combobox));
  GtkTreeIter iter;
  if (gtk_tree_model_get_iter_first(model, &iter)) {
    string_count++;
    while (gtk_tree_model_iter_next(model, &iter))
      string_count++;
  }
  return string_count;
}

void combobox_clear_strings(GtkWidget * combobox)
// Clear the strings loaded in the combobox.
{
  for (int i = combobox_get_string_count(combobox) - 1; i >= 0; i--)
    gtk_combo_box_remove_text(GTK_COMBO_BOX(combobox), i);
}

vector < ustring > combobox_get_strings(GtkWidget * combobox)
{
  vector < ustring > strings;
  GtkTreeModel *model;
  model = gtk_combo_box_get_model(GTK_COMBO_BOX(combobox));
  GtkTreeIter iter;
  if (gtk_tree_model_get_iter_first(model, &iter)) {
    gchar *value;
    gtk_tree_model_get(model, &iter, 0, &value, -1);
    strings.push_back(value);
    g_free(value);
    while (gtk_tree_model_iter_next(model, &iter)) {
      gchar *value;
      gtk_tree_model_get(model, &iter, 0, &value, -1);
      strings.push_back(value);
      g_free(value);
    }
  }
  return strings;
}

void combobox_set_strings(GtkWidget * combobox, const vector < ustring > &strings)
{
  combobox_clear_strings(combobox);
  for (unsigned int i = 0; i < strings.size(); i++)
    gtk_combo_box_append_text(GTK_COMBO_BOX(combobox), strings[i].c_str());
}

void combobox_set_strings(GtkWidget * combobox, const vector < unsigned int >&strings)
{
  combobox_clear_strings(combobox);
  for (unsigned int i = 0; i < strings.size(); i++)
    gtk_combo_box_append_text(GTK_COMBO_BOX(combobox), convert_to_string(strings[i]).c_str());
}

void combobox_set_string(GtkWidget * combobox, const ustring & string)
// Sets the string that should be selected.
// If this string does not exist, it gets set to the first string, if there are
// strings loaded, else it will be empty.
{
  vector < ustring > strings;
  strings = combobox_get_strings(combobox);
  int index = -1;
  for (unsigned int i = 0; i < strings.size(); i++)
    if (string == strings[i])
      index = i;
  gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), index);
}

void combobox_set_string(GtkWidget * combobox, unsigned int string)
{
  combobox_set_string(combobox, convert_to_string(string));
}

void combobox_set_index(GtkWidget * combobox, unsigned int index)
{
  gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), index);
}
