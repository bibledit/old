/*
** Copyright (©) 2003 The Free Software Foundation.
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
#include "dialognotes.h"
#include "notes_utils.h"
#include "utilities.h"
#include "dialogentry.h"
#include "gtkwrappers.h"
#include "combobox.h"
#include "settings.h"
#include "help.h"


NotesDialog::NotesDialog (int dummy)
{
  // Save variables.
  extern Settings * settings;
  
  // Read all the categories and add possible new ones in the database.
  ReadText rt (notes_categories_filename());
  notes_categories_add_from_database (rt.lines);
  for (unsigned int i  = 0; i < rt.lines.size(); i++)
    mycategories.push_back (rt.lines[i]);
  
  notesdialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (notesdialog), "Project notes");
  gtk_window_set_position (GTK_WINDOW (notesdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal (GTK_WINDOW (notesdialog), TRUE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (notesdialog), TRUE);

  dialog_vbox1 = GTK_DIALOG (notesdialog)->vbox;
  gtk_widget_show (dialog_vbox1);

  notebook1 = gtk_notebook_new ();
  gtk_widget_show (notebook1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), notebook1, TRUE, TRUE, 0);
  
  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox1);
  gtk_container_set_border_width (GTK_CONTAINER (vbox1), 2);
  
  label3 = gtk_label_new ("While editing a project note, there are some shortcuts\nthat enable you to quickly insert some text in the editor.\nEnter the texts that will be inserted. There are four.");
  gtk_widget_show (label3);
  gtk_box_pack_start (GTK_BOX (vbox1), label3, FALSE, FALSE, 2);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  table1 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, TRUE, TRUE, 4);
  gtk_table_set_row_spacings (GTK_TABLE (table1), 4);
  gtk_table_set_col_spacings (GTK_TABLE (table1), 4);

  label4 = gtk_label_new ("1");
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  label5 = gtk_label_new ("2");
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table1), label5, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  label6 = gtk_label_new ("3");
  gtk_widget_show (label6);
  gtk_table_attach (GTK_TABLE (table1), label6, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label6), 0, 0.5);

  label7 = gtk_label_new ("4");
  gtk_widget_show (label7);
  gtk_table_attach (GTK_TABLE (table1), label7, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label7), 0, 0.5);

  entry1 = gtk_entry_new ();
  gtk_widget_show (entry1);
  gtk_table_attach (GTK_TABLE (table1), entry1, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_entry_set_text (GTK_ENTRY (entry1), settings->genconfig.edit_note_standard_text_one_get ().c_str());
  gtk_entry_set_activates_default (GTK_ENTRY (entry1), TRUE);

  entry2 = gtk_entry_new ();
  gtk_widget_show (entry2);
  gtk_table_attach (GTK_TABLE (table1), entry2, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_entry_set_text (GTK_ENTRY (entry2), settings->genconfig.edit_note_standard_text_two_get ().c_str());
  gtk_entry_set_activates_default (GTK_ENTRY (entry2), TRUE);

  entry3 = gtk_entry_new ();
  gtk_widget_show (entry3);
  gtk_table_attach (GTK_TABLE (table1), entry3, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_entry_set_text (GTK_ENTRY (entry3), settings->genconfig.edit_note_standard_text_three_get ().c_str());
  gtk_entry_set_activates_default (GTK_ENTRY (entry3), TRUE);

  entry4 = gtk_entry_new ();
  gtk_widget_show (entry4);
  gtk_table_attach (GTK_TABLE (table1), entry4, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_entry_set_text (GTK_ENTRY (entry4), settings->genconfig.edit_note_standard_text_four_get ().c_str());
  gtk_entry_set_activates_default (GTK_ENTRY (entry4), TRUE);

  label8 = gtk_label_new_with_mnemonic ("_Shortcuts");
  gtk_widget_show (label8);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label8);

  vbox2 = gtk_vbox_new (FALSE, 2);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox2);
  gtk_container_set_border_width (GTK_CONTAINER (vbox2), 2);

  label10 = gtk_label_new ("While editing a project note, there are some categories that can be assigned to this note.\nThe list of categories can be edited here.");
  gtk_widget_show (label10);
  gtk_box_pack_start (GTK_BOX (vbox2), label10, FALSE, FALSE, 0);
  gtk_label_set_line_wrap (GTK_LABEL (label10), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label10), 0, 0.5);

  combobox1 = gtk_combo_box_new_text ();
  gtk_widget_show (combobox1);
  gtk_box_pack_start (GTK_BOX (vbox2), combobox1, FALSE, FALSE, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox1, FALSE, FALSE, 0);

  buttondelete = gtk_button_new ();
  gtk_widget_show (buttondelete);
  gtk_box_pack_start (GTK_BOX (hbox1), buttondelete, FALSE, FALSE, 0);

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (buttondelete), alignment1);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox2);

  image1 = gtk_image_new_from_stock ("gtk-delete", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox2), image1, FALSE, FALSE, 0);

  label11 = gtk_label_new_with_mnemonic ("_Delete");
  gtk_widget_show (label11);
  gtk_box_pack_start (GTK_BOX (hbox2), label11, FALSE, FALSE, 0);

  buttonadd = gtk_button_new ();
  gtk_widget_show (buttonadd);
  gtk_box_pack_start (GTK_BOX (hbox1), buttonadd, FALSE, FALSE, 0);

  alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (buttonadd), alignment2);

  hbox3 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox3);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox3);

  image2 = gtk_image_new_from_stock ("gtk-add", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_box_pack_start (GTK_BOX (hbox3), image2, FALSE, FALSE, 0);

  label12 = gtk_label_new_with_mnemonic ("_Add");
  gtk_widget_show (label12);
  gtk_box_pack_start (GTK_BOX (hbox3), label12, FALSE, FALSE, 0);

  label9 = gtk_label_new_with_mnemonic ("Ca_tegories");
  gtk_widget_show (label9);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label9);

  dialog_action_area1 = GTK_DIALOG (notesdialog)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (notesdialog, NULL, NULL);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (notesdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (notesdialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  g_signal_connect ((gpointer) okbutton1, "clicked", G_CALLBACK (on_okbutton1_clicked), gpointer(this));
  g_signal_connect ((gpointer) buttondelete, "clicked", G_CALLBACK (on_buttondelete_clicked), gpointer(this));
  g_signal_connect ((gpointer) buttonadd, "clicked", G_CALLBACK (on_buttonadd_clicked), gpointer(this));

  gtk_widget_grab_focus (entry1);
  gtk_widget_grab_default (okbutton1);
  
  set_gui("");
}


NotesDialog::~NotesDialog ()
{
  gtk_widget_destroy (notesdialog);
}


int NotesDialog::run ()
{
  return gtk_dialog_run (GTK_DIALOG (notesdialog));
}


void NotesDialog::on_okbutton1_clicked (GtkButton *button, gpointer user_data)
{
  ((NotesDialog *) user_data)->on_okbutton ();
}


void NotesDialog::on_okbutton ()
{
  // Store values for standard texts.
  extern Settings * settings;
  settings->genconfig.edit_note_standard_text_one_set (gtk_entry_get_text (GTK_ENTRY (entry1)));
  settings->genconfig.edit_note_standard_text_two_set (gtk_entry_get_text (GTK_ENTRY (entry2)));
  settings->genconfig.edit_note_standard_text_three_set (gtk_entry_get_text (GTK_ENTRY (entry3)));
  settings->genconfig.edit_note_standard_text_four_set (gtk_entry_get_text (GTK_ENTRY (entry4)));
  // Save possibly edited categories.
  vector<ustring> categories (mycategories.begin(), mycategories.end());
  write_lines (notes_categories_filename(), categories);
}


void NotesDialog::on_buttondelete_clicked (GtkButton *button, gpointer user_data)
{
  ((NotesDialog *) user_data)->on_delete_category ();
}


void NotesDialog::on_delete_category ()
{
  // Get category to delete.
  ustring category = combobox_get_active_string (combobox1);
  // If it is not in the database, delete it.
  vector <ustring> categories_in_database;
  notes_categories_add_from_database (categories_in_database);
  set <ustring> categories_in_db (categories_in_database.begin(), categories_in_database.end());
  set<ustring>::const_iterator found;
  found = categories_in_db.find (category);
  if (found == categories_in_db.end()) {
    // Not in database: delete category.
    vector<ustring> temporary (mycategories.begin(), mycategories.end());
    mycategories.clear();
    for (unsigned int i = 0; i < temporary.size(); i++) {
      if (temporary[i] != category)
        mycategories.push_back (temporary[i]);
    }
    set_gui("");
  } else {
    // Category is in database: Can't be deleted.
    ustring message = "This category could not be deleted, because there are still notes\n"
                     "in the database with this category.";
    gtkw_dialog_error (notesdialog, message);
  }
}


void NotesDialog::on_buttonadd_clicked (GtkButton *button, gpointer user_data)
{
  ((NotesDialog *) user_data)->on_add_category ();
}


void NotesDialog::on_add_category ()
{
  EntryDialog dialog ("New category name", 
                      "Enter a new category.",
                      "");
  if (dialog.run () == GTK_RESPONSE_OK) {
    mycategories.push_back (dialog.entered_value);
    set_gui(dialog.entered_value);
  }
}


void NotesDialog::set_gui(const ustring& display_value)
{
  // Fill combo box with the categories we have.
  vector<ustring> categories_container (mycategories.begin(), mycategories.end());
  combobox_set_strings (combobox1, categories_container);
  if (!display_value.empty()) 
    combobox_set_string (combobox1, display_value);
  else if (categories_container.size() > 0)
    combobox_set_string (combobox1, categories_container[0]);
}
