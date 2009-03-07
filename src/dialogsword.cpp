/*
** Copyright (©) 2003-2009 Teus Benschop.
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
#include "dialogsword.h"
#include "utilities.h"
#include "export_utils.h"
#include "dialoglistview.h"
#include "settings.h"
#include "help.h"

SwordDialog::SwordDialog(int dummy)
{
  // Build dialog.
  sworddialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(sworddialog), "SWORD module");
  gtk_window_set_position(GTK_WINDOW(sworddialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(sworddialog), TRUE);

  dialog_vbox1 = GTK_DIALOG(sworddialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label11 = gtk_label_new("Export a project to a SWORD module, and install it.\nIn most cases you can just press Ok, and it will work out fine.\nMore detailed settings can be made below.");
  gtk_widget_show(label11);
  gtk_box_pack_start(GTK_BOX(vbox1), label11, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label11), 0, 0.5);

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator1, TRUE, TRUE, 0);

  table1 = gtk_table_new(9, 2, FALSE);
  gtk_widget_show(table1);
  gtk_box_pack_start(GTK_BOX(vbox1), table1, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(table1), 2);
  gtk_table_set_row_spacings(GTK_TABLE(table1), 3);
  gtk_table_set_col_spacings(GTK_TABLE(table1), 3);

  label1 = gtk_label_new_with_mnemonic("_Name");
  gtk_widget_show(label1);
  gtk_table_attach(GTK_TABLE(table1), label1, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

  label2 = gtk_label_new_with_mnemonic("_Description");
  gtk_widget_show(label2);
  gtk_table_attach(GTK_TABLE(table1), label2, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label2), 0, 0.5);

  label3 = gtk_label_new_with_mnemonic("_About");
  gtk_widget_show(label3);
  gtk_table_attach(GTK_TABLE(table1), label3, 0, 1, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label3), 0, 0.5);

  label4 = gtk_label_new_with_mnemonic("_LCSH");
  gtk_widget_show(label4);
  gtk_table_attach(GTK_TABLE(table1), label4, 0, 1, 3, 4, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label4), 0, 0.5);

  label5 = gtk_label_new_with_mnemonic("L_icense");
  gtk_widget_show(label5);
  gtk_table_attach(GTK_TABLE(table1), label5, 0, 1, 4, 5, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label5), 0, 0.5);

  label6 = gtk_label_new_with_mnemonic("_Version");
  gtk_widget_show(label6);
  gtk_table_attach(GTK_TABLE(table1), label6, 0, 1, 5, 6, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label6), 0, 0.5);

  label7 = gtk_label_new_with_mnemonic("Lan_guage");
  gtk_widget_show(label7);
  gtk_table_attach(GTK_TABLE(table1), label7, 0, 1, 6, 7, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label7), 0, 0.5);

  label8 = gtk_label_new_with_mnemonic("In_stall path");
  gtk_widget_show(label8);
  gtk_table_attach(GTK_TABLE(table1), label8, 0, 1, 7, 8, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label8), 0, 0.5);

  label9 = gtk_label_new_with_mnemonic("_Module path");
  gtk_widget_show(label9);
  gtk_table_attach(GTK_TABLE(table1), label9, 0, 1, 8, 9, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label9), 0, 0.5);

  hbox7 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox7);
  gtk_table_attach(GTK_TABLE(table1), hbox7, 1, 2, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_FILL), 0, 0);

  labelname = gtk_label_new("");
  gtk_widget_show(labelname);
  gtk_box_pack_start(GTK_BOX(hbox7), labelname, FALSE, FALSE, 0);

  entry_name = gtk_entry_new();
  gtk_widget_show(entry_name);
  gtk_box_pack_start(GTK_BOX(hbox7), entry_name, TRUE, TRUE, 0);

  hbox8 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox8);
  gtk_table_attach(GTK_TABLE(table1), hbox8, 1, 2, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_FILL), 0, 0);

  labeldescription = gtk_label_new("");
  gtk_widget_show(labeldescription);
  gtk_box_pack_start(GTK_BOX(hbox8), labeldescription, FALSE, FALSE, 0);

  entry_description = gtk_entry_new();
  gtk_widget_show(entry_description);
  gtk_box_pack_start(GTK_BOX(hbox8), entry_description, TRUE, TRUE, 0);

  hbox9 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox9);
  gtk_table_attach(GTK_TABLE(table1), hbox9, 1, 2, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_FILL), 0, 0);

  labelabout = gtk_label_new("");
  gtk_widget_show(labelabout);
  gtk_box_pack_start(GTK_BOX(hbox9), labelabout, FALSE, FALSE, 0);

  entry_about = gtk_entry_new();
  gtk_widget_show(entry_about);
  gtk_box_pack_start(GTK_BOX(hbox9), entry_about, TRUE, TRUE, 0);

  entry_lcsh = gtk_entry_new();
  gtk_widget_show(entry_lcsh);
  gtk_table_attach(GTK_TABLE(table1), entry_lcsh, 1, 2, 3, 4, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  entry_license = gtk_entry_new();
  gtk_widget_show(entry_license);
  gtk_table_attach(GTK_TABLE(table1), entry_license, 1, 2, 4, 5, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  entry_version = gtk_entry_new();
  gtk_widget_show(entry_version);
  gtk_table_attach(GTK_TABLE(table1), entry_version, 1, 2, 5, 6, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  hbox5 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox5);
  gtk_table_attach(GTK_TABLE(table1), hbox5, 1, 2, 6, 7, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_FILL), 0, 0);

  entry_language = gtk_entry_new();
  gtk_widget_show(entry_language);
  gtk_box_pack_start(GTK_BOX(hbox5), entry_language, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox1);
  gtk_table_attach(GTK_TABLE(table1), hbox1, 1, 2, 7, 8, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_FILL), 0, 0);

  entry_installpath = gtk_entry_new();
  gtk_widget_show(entry_installpath);
  gtk_box_pack_start(GTK_BOX(hbox1), entry_installpath, TRUE, TRUE, 0);

  button_installpath = gtk_button_new();
  gtk_widget_show(button_installpath);
  gtk_box_pack_start(GTK_BOX(hbox1), button_installpath, FALSE, FALSE, 0);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(button_installpath), alignment1);

  hbox3 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox3);
  gtk_container_add(GTK_CONTAINER(alignment1), hbox3);

  image1 = gtk_image_new_from_stock("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX(hbox3), image1, FALSE, FALSE, 0);

  label12 = gtk_label_new_with_mnemonic("O_pen");
  gtk_widget_show(label12);
  gtk_box_pack_start(GTK_BOX(hbox3), label12, FALSE, FALSE, 0);

  hbox2 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox2);
  gtk_table_attach(GTK_TABLE(table1), hbox2, 1, 2, 8, 9, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_FILL), 0, 0);

  entry_modulepath = gtk_entry_new();
  gtk_widget_show(entry_modulepath);
  gtk_box_pack_start(GTK_BOX(hbox2), entry_modulepath, TRUE, TRUE, 0);

  button_modulepath = gtk_button_new();
  gtk_widget_show(button_modulepath);
  gtk_box_pack_start(GTK_BOX(hbox2), button_modulepath, FALSE, FALSE, 0);

  alignment2 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment2);
  gtk_container_add(GTK_CONTAINER(button_modulepath), alignment2);

  hbox4 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox4);
  gtk_container_add(GTK_CONTAINER(alignment2), hbox4);

  image2 = gtk_image_new_from_stock("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image2);
  gtk_box_pack_start(GTK_BOX(hbox4), image2, FALSE, FALSE, 0);

  label13 = gtk_label_new_with_mnemonic("Op_en");
  gtk_widget_show(label13);
  gtk_box_pack_start(GTK_BOX(hbox4), label13, FALSE, FALSE, 0);

  dialog_action_area1 = GTK_DIALOG(sworddialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(sworddialog, NULL, "sword_module");

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(sworddialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(sworddialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) button_installpath, "clicked", G_CALLBACK(on_button_installpath_clicked), gpointer(this));
  g_signal_connect((gpointer) button_modulepath, "clicked", G_CALLBACK(on_button_modulepath_clicked), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_label_set_mnemonic_widget(GTK_LABEL(label1), entry_name);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label2), entry_description);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label3), entry_about);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label4), entry_lcsh);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label5), entry_license);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label6), entry_version);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label7), entry_language);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label8), entry_installpath);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label9), entry_modulepath);

  gtk_widget_grab_default(okbutton);
  gtk_widget_grab_focus(okbutton);

  // Set values.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  gtk_label_set_text(GTK_LABEL(labelname), settings->genconfig.project_get().c_str());
  gtk_label_set_text(GTK_LABEL(labeldescription), settings->genconfig.project_get().c_str());
  gtk_label_set_text(GTK_LABEL(labelabout), settings->genconfig.project_get().c_str());
  gtk_entry_set_text(GTK_ENTRY(entry_name), projectconfig->sword_name_get().c_str());
  gtk_entry_set_text(GTK_ENTRY(entry_description), projectconfig->sword_description_get().c_str());
  gtk_entry_set_text(GTK_ENTRY(entry_about), projectconfig->sword_about_get().c_str());
  gtk_entry_set_text(GTK_ENTRY(entry_lcsh), projectconfig->sword_lcsh_get().c_str());
  gtk_entry_set_text(GTK_ENTRY(entry_license), projectconfig->sword_license_get().c_str());
  gtk_entry_set_text(GTK_ENTRY(entry_version), projectconfig->sword_version_get().c_str());
  gtk_entry_set_text(GTK_ENTRY(entry_language), projectconfig->sword_language_get().c_str());
  gtk_entry_set_text(GTK_ENTRY(entry_installpath), settings->genconfig.export_to_sword_install_path_get().c_str());
  gtk_entry_set_text(GTK_ENTRY(entry_modulepath), settings->genconfig.export_to_sword_module_path_get().c_str());
}

SwordDialog::~SwordDialog()
{
  gtk_widget_destroy(sworddialog);
}

int SwordDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(sworddialog));
}

void SwordDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((SwordDialog *) user_data)->on_okbutton();
}

void SwordDialog::on_okbutton()
{
  // Save values.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  ustring name = gtk_entry_get_text(GTK_ENTRY(entry_name));
  replace_text(name, " ", "_");
  projectconfig->sword_name_set(name);
  projectconfig->sword_description_set(gtk_entry_get_text(GTK_ENTRY(entry_description)));
  projectconfig->sword_about_set(gtk_entry_get_text(GTK_ENTRY(entry_about)));
  projectconfig->sword_lcsh_set(gtk_entry_get_text(GTK_ENTRY(entry_lcsh)));
  projectconfig->sword_license_set(gtk_entry_get_text(GTK_ENTRY(entry_license)));
  projectconfig->sword_version_set(gtk_entry_get_text(GTK_ENTRY(entry_version)));
  projectconfig->sword_language_set(gtk_entry_get_text(GTK_ENTRY(entry_language)));
  settings->genconfig.export_to_sword_install_path_set(gtk_entry_get_text(GTK_ENTRY(entry_installpath)));
  settings->genconfig.export_to_sword_module_path_set(gtk_entry_get_text(GTK_ENTRY(entry_modulepath)));
}

void SwordDialog::on_button_installpath_clicked(GtkButton * button, gpointer user_data)
{
  ((SwordDialog *) user_data)->on_button_installpath();
}

void SwordDialog::on_button_installpath()
{
  GtkWidget *dialog;
  dialog = gtk_file_chooser_dialog_new("Select a folder", GTK_WINDOW(sworddialog), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
  gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), gtk_entry_get_text(GTK_ENTRY(entry_installpath)));
  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
    gtk_entry_set_text(GTK_ENTRY(entry_installpath), gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
  }
  gtk_widget_destroy(dialog);
}

void SwordDialog::on_button_modulepath_clicked(GtkButton * button, gpointer user_data)
{
  ((SwordDialog *) user_data)->on_button_modulepath();
}

void SwordDialog::on_button_modulepath()
{
  GtkWidget *dialog;
  dialog = gtk_file_chooser_dialog_new("Select a folder", GTK_WINDOW(sworddialog), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
  gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), gtk_entry_get_text(GTK_ENTRY(entry_modulepath)));
  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
    gtk_entry_set_text(GTK_ENTRY(entry_modulepath), gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
  }
  gtk_widget_destroy(dialog);
}

