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
#include "dialogbackup.h"
#include "help.h"
#include "settings.h"
#include "shortcuts.h"
#include "dialogdate.h"
#include "date_time_utils.h"

BackupDialog::BackupDialog(int dummy)
{
  Shortcuts shortcuts(0);

  backupdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(backupdialog), "Backup");
  gtk_window_set_position(GTK_WINDOW(backupdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_type_hint(GTK_WINDOW(backupdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG(backupdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 4);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label_info = gtk_label_new("");
  gtk_widget_show(label_info);
  gtk_box_pack_start(GTK_BOX(vbox1), label_info, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_info), 0, 0.5);
  gtk_label_set_line_wrap(GTK_LABEL(label_info), TRUE);

  hbox1 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox1, FALSE, FALSE, 0);

  GSList *radiobutton_group = NULL;

  radiobutton_incremental = gtk_radio_button_new_with_mnemonic(NULL, "Incremental backup");
  gtk_widget_show(radiobutton_incremental);
  gtk_box_pack_start(GTK_BOX(hbox1), radiobutton_incremental, TRUE, TRUE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_incremental), radiobutton_group);
  radiobutton_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_incremental));

  shortcuts.button(radiobutton_incremental);

  button_since = gtk_button_new();
  gtk_widget_show(button_since);
  gtk_box_pack_start(GTK_BOX(hbox1), button_since, FALSE, FALSE, 0);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(button_since), alignment1);

  hbox2 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox2);
  gtk_container_add(GTK_CONTAINER(alignment1), hbox2);

  image1 = gtk_image_new_from_stock("gtk-dialog-question", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX(hbox2), image1, FALSE, FALSE, 0);

  label2 = gtk_label_new_with_mnemonic("Since");
  gtk_widget_show(label2);
  gtk_box_pack_start(GTK_BOX(hbox2), label2, FALSE, FALSE, 0);

  shortcuts.label(label2);

  radiobutton_full = gtk_radio_button_new_with_mnemonic(NULL, "Full backup");
  gtk_widget_show(radiobutton_full);
  gtk_box_pack_start(GTK_BOX(vbox1), radiobutton_full, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_full), radiobutton_group);
  radiobutton_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_full));

  shortcuts.button(radiobutton_full);

  dialog_action_area1 = GTK_DIALOG(backupdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(backupdialog, &shortcuts, "backup");

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(backupdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton1);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(backupdialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(okbutton1);

  shortcuts.process();

  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_full), projectconfig->backup_full_get());

  g_signal_connect((gpointer) radiobutton_incremental, "toggled", G_CALLBACK(on_radiobutton_incremental_toggled), gpointer(this));
  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));
  g_signal_connect((gpointer) button_since, "clicked", G_CALLBACK(on_sincebutton_clicked), gpointer(this));

  gtk_widget_grab_focus(okbutton1);
  gtk_widget_grab_default(okbutton1);

  gui();
}

BackupDialog::~BackupDialog()
{
  gtk_widget_destroy(backupdialog);
}

int BackupDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(backupdialog));
}

void BackupDialog::on_radiobutton_incremental_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((BackupDialog *) user_data)->gui();
}

void BackupDialog::gui()
{
  ustring information;
  bool since_sensitive;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_full))) {
    information = "This will make a full backup of the data";
    since_sensitive = false;
  } else {
    information = "This will make a differential backup of all the data changed since the date under the Since button";
    since_sensitive = true;
  }
  gtk_label_set_text(GTK_LABEL(label_info), information.c_str());
  gtk_widget_set_sensitive(button_since, since_sensitive);
}

void BackupDialog::on_sincebutton_clicked(GtkButton * button, gpointer user_data)
{
  ((BackupDialog *) user_data)->on_sincebutton();
}

void BackupDialog::on_sincebutton()
{
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  guint32 seconds = projectconfig->backup_incremental_last_time_get();
  DateDialog dialog(&seconds, true);
  if (dialog.run() == GTK_RESPONSE_OK) {
    projectconfig->backup_incremental_last_time_set(seconds);
  }
}

void BackupDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((BackupDialog *) user_data)->on_okbutton();
}

void BackupDialog::on_okbutton()
{
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  projectconfig->backup_full_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_full)));
}
