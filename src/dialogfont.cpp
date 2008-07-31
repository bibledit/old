/*
** Copyright (©) 2003-2008 Teus Benschop.
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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/


#include "libraries.h"
#include <glib.h>
#include "dialogfont.h"
#include "fonts.h"
#include "listview.h"
#include "utilities.h"
#include "projectutils.h"
#include "combobox.h"
#include "settings.h"
#include "help.h"
#include "books.h"
#include "tiny_utilities.h"


enum { COLUMN_FONT, COLUMN_USE, NUM_COLUMNS };


FontDialog::FontDialog (int dummy)
{
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig (settings->genconfig.project_get());
  
  fontdialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (fontdialog), "Font");
  gtk_window_set_position (GTK_WINDOW (fontdialog), GTK_WIN_POS_CENTER_ON_PARENT);

  dialog_vbox1 = GTK_DIALOG (fontdialog)->vbox;
  gtk_widget_show (dialog_vbox1);

  label24 = gtk_label_new ("1. Select one or more fonts you wish to use for printing this project.\n2. If more than one font was selected, reorder them: drag the more important fonts higher than the others.");
  gtk_widget_show (label24);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), label24, FALSE, FALSE, 4);
  gtk_misc_set_alignment (GTK_MISC (label24), 0, 0.5);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox3, TRUE, TRUE, 0);

  vbox5 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox5);
  gtk_box_pack_start (GTK_BOX (hbox3), vbox5, TRUE, TRUE, 4);
  
  scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow2);
  gtk_box_pack_start (GTK_BOX (vbox5), scrolledwindow2, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  treeview1 = gtk_tree_view_new ();
  gtk_widget_show (treeview1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow2), treeview1);
  gtk_widget_set_size_request (treeview1, 300, -1);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview1), true);
  gtk_tree_view_set_reorderable (GTK_TREE_VIEW (treeview1), TRUE);

  hbox4 = gtk_hbox_new (FALSE, 4);
  gtk_widget_show (hbox4);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox4, FALSE, TRUE, 0);

  label25 = gtk_label_new_with_mnemonic ("_Size");
  gtk_widget_show (label25);
  gtk_box_pack_start (GTK_BOX (hbox4), label25, FALSE, FALSE, 0);

  combobox1 = gtk_combo_box_new_text ();
  gtk_widget_show (combobox1);
  gtk_box_pack_start (GTK_BOX (hbox4), combobox1, FALSE, FALSE, 0);

  label26 = gtk_label_new ("points");
  gtk_widget_show (label26);
  gtk_box_pack_start (GTK_BOX (hbox4), label26, FALSE, FALSE, 0);

  vseparator1 = gtk_vseparator_new ();
  gtk_widget_show (vseparator1);
  gtk_box_pack_start (GTK_BOX (hbox4), vseparator1, FALSE, FALSE, 0);

  label27 = gtk_label_new_with_mnemonic ("_Line height");
  gtk_widget_show (label27);
  gtk_box_pack_start (GTK_BOX (hbox4), label27, FALSE, FALSE, 0);

  spinbutton1_adj = gtk_adjustment_new (100, 50, 300, 1, 10, 10);
  spinbutton1 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton1_adj), 1, 0);
  gtk_widget_show (spinbutton1);
  gtk_box_pack_start (GTK_BOX (hbox4), spinbutton1, FALSE, TRUE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton1), TRUE);

  // Set value.
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinbutton1), projectconfig->text_line_height_get ());

  label28 = gtk_label_new ("%");
  gtk_widget_show (label28);
  gtk_box_pack_start (GTK_BOX (hbox4), label28, FALSE, FALSE, 0);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (hbox3), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  textview1 = gtk_text_view_new ();
  gtk_widget_show (textview1);
  gtk_widget_set_size_request (textview1, 500, 400);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), textview1);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (textview1), FALSE);
  gtk_text_view_set_accepts_tab (GTK_TEXT_VIEW (textview1), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview1), GTK_WRAP_WORD);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (textview1), FALSE);

  dialog_action_area1 = GTK_DIALOG (fontdialog)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (fontdialog, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (fontdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (fontdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton, GTK_CAN_DEFAULT);

  g_signal_connect ((gpointer) treeview1, "cursor_changed", G_CALLBACK (on_treeview1_cursor_changed), gpointer(this));
  g_signal_connect ((gpointer) okbutton, "clicked", G_CALLBACK (on_okbutton_clicked), gpointer(this));
  g_signal_connect ((gpointer) combobox1, "changed", G_CALLBACK (on_combobox1_changed), gpointer(this));

  gtk_label_set_mnemonic_widget (GTK_LABEL (label25), combobox1);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label27), spinbutton1);

  gtk_widget_grab_focus (treeview1);
  gtk_widget_grab_default (okbutton);
  
  // Set the fontsize(s).
  combobox_set_strings (combobox1, font_get_sizes());
  combobox_set_string (combobox1, convert_to_string (12));

  // Storage, renderer, column and selection.
  store = gtk_list_store_new (NUM_COLUMNS, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeview1), GTK_TREE_MODEL (store));
  g_object_unref (store);
  GtkCellRenderer * renderer;
  renderer = gtk_cell_renderer_text_new ();
  GtkTreeViewColumn *column1;
  column1 = gtk_tree_view_column_new_with_attributes ("Font", renderer, "text", COLUMN_FONT, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview1), column1);
  renderer = gtk_cell_renderer_toggle_new ();
  g_signal_connect (renderer, "toggled", G_CALLBACK (use_toggled), store);
  GtkTreeViewColumn *column2;
  column2 = gtk_tree_view_column_new_with_attributes ("Use", renderer, "active", COLUMN_USE, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview1), column2);
  GtkTreeSelection *select;
  select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview1));
  gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);

  // Set the fonts in the view.
  PrintingFonts printingfonts (settings->genconfig.project_get());
  set<ustring> selected_fonts (printingfonts.printing_families.begin(), printingfonts.printing_families.end());  
  // Move the selected fonts to the top.
  vector <ustring> reorderedfonts = printingfonts.printing_families;
  for (unsigned int i = 0; i < printingfonts.available_families.size(); i++) {
    if (selected_fonts.find (printingfonts.available_families[i]) == selected_fonts.end())
      reorderedfonts.push_back (printingfonts.available_families[i]);
  }
  GtkTreeIter iter;
  for (unsigned int i = 0; i < reorderedfonts.size(); i++) {
    gtk_list_store_append (store, &iter);
    bool use = selected_fonts.find (reorderedfonts[i]) != selected_fonts.end();
    gtk_list_store_set (store, &iter, COLUMN_FONT, reorderedfonts[i].c_str (), COLUMN_USE, use, -1);
    // Focus the first font.
    if (i == 0) listview_focus_string (treeview1, reorderedfonts[i]);
  }
}


FontDialog::~FontDialog ()
{
  gtk_widget_destroy (fontdialog);
}


int FontDialog::run ()
{
  return gtk_dialog_run (GTK_DIALOG (fontdialog));
}


void FontDialog::on_okbutton_clicked (GtkButton * button, gpointer user_data)
{
  ((FontDialog *) user_data)->on_okbutton ();
}


void FontDialog::on_okbutton ()
{
  // Get the fonts that are to be used.
  vector <ustring> fonts;
  {
    // Get the model and the selection.
    GtkTreeModel * model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview1));
    // Some variables needed.
    GtkTreeIter iter;
    gboolean valid;
    gint index = 0;
    // Get the first iter in the store.
    valid = gtk_tree_model_get_iter_first (model, &iter);
    while (valid) {
      // Produce path.
      GtkTreePath * path = NULL;
      path = gtk_tree_path_new_from_indices (index, -1);
      // Read this row.
      gchar *str_data;
      gboolean used;
      // Make sure you terminate calls to gtk_tree_model_get() with a '-1' value.
      gtk_tree_model_get (model, &iter,COLUMN_FONT, &str_data, COLUMN_USE, &used, -1);
      // If used, list this font.
      if (used) fonts.push_back (str_data);
      // Free some data.
      gtk_tree_path_free (path);
      g_free (str_data);
      // Next row.
      valid = gtk_tree_model_iter_next (model, &iter);
      index++;
    }
  }
  // Save list of fonts.
  PrintingFonts pfonts ("");
  pfonts.printing_families_store (fonts);
  // Save font size.
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig (settings->genconfig.project_get());
  gtk_spin_button_update (GTK_SPIN_BUTTON (spinbutton1));
  projectconfig->text_line_height_set (gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (spinbutton1)));
}


void FontDialog::on_treeview1_cursor_changed (GtkTreeView *treeview, gpointer user_data)
{
  ((FontDialog *) user_data)->on_treeview ();
}


void FontDialog::on_treeview ()
{
  // Set the font in the editor.
  extern Settings * settings;
  ustring font = listview_get_active_string (treeview1);
  PangoFontDescription *font_desc = pango_font_description_from_string (font.c_str ());
  pango_font_description_set_size (font_desc, convert_to_int (combobox_get_active_string (combobox1)) * PANGO_SCALE);
  gtk_widget_modify_font (textview1, font_desc);
  pango_font_description_free (font_desc);
  // Put the currently opened chapter in the textview, ...
  ustring contents;
  if (!settings->genconfig.project_get().empty()) {
    vector<ustring> text;
    text = project_retrieve_chapter (settings->genconfig.project_get(), settings->genconfig.book_get (), convert_to_int (settings->genconfig.chapter_get ()));
    for (unsigned int i2 = 0; i2 < text.size(); i2++) {
      contents.append (text[i2]);
      contents.append ("\n");
    }
  }
  // ... or the alphabet.
  if (contents.empty()) {
    contents = ALPHABET + upperCase (ALPHABET);
  }
  gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview1)), contents.c_str(), -1);
}


void FontDialog::on_combobox1_changed (GtkComboBox *combobox,  gpointer user_data)
{
  ((FontDialog *) user_data)->on_treeview ();
}


void FontDialog::use_toggled (GtkCellRendererToggle *cell, gchar *path_str, gpointer data)
{
  GtkTreeModel *model = (GtkTreeModel *)data;
  GtkTreeIter iter;
  GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
  gboolean use;
  // Get include iter
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_model_get (model, &iter, COLUMN_USE, &use, -1);
  // Toggle include 
  use = !use;
  // Set new value
  gtk_list_store_set (GTK_LIST_STORE (model), &iter, COLUMN_USE, use, -1);
  // Clean up
  gtk_tree_path_free (path);
}
