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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/

#include "libraries.h"
#include <glib.h>
#include "dialogrefexchange.h"
#include "settings.h"
#include "shell.h"
#include "windowsoutpost.h"
#include "help.h"

ReferenceExchangeDialog::ReferenceExchangeDialog(int dummy)
{
  // Save variables.
  extern Settings *settings;

  dialogsynchronize = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dialogsynchronize), "Reference exchange");
  gtk_window_set_position(GTK_WINDOW(dialogsynchronize), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(dialogsynchronize), TRUE);
  gtk_window_set_type_hint(GTK_WINDOW(dialogsynchronize), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG(dialogsynchronize)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(vbox1), 4);

  label6 = gtk_label_new("Name of the BibleWorks executable:");
  gtk_widget_show(label6);
  gtk_box_pack_start(GTK_BOX(vbox1), label6, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label6), 0, 0.5);

  entry_bw_exe = gtk_entry_new();
  gtk_widget_show(entry_bw_exe);
  gtk_box_pack_start(GTK_BOX(vbox1), entry_bw_exe, FALSE, FALSE, 0);
  gtk_entry_set_activates_default(GTK_ENTRY(entry_bw_exe), TRUE);

  // Set value.
  gtk_entry_set_text(GTK_ENTRY(entry_bw_exe), settings->genconfig.bibleworks_executable_get().c_str());

  hseparator3 = gtk_hseparator_new();
  gtk_widget_show(hseparator3);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator3, TRUE, TRUE, 0);

  label2 = gtk_label_new("Send references to: ");
  gtk_widget_show(label2);
  gtk_box_pack_start(GTK_BOX(vbox1), label2, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label2), 0, 0.5);

  vbox2 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox2);
  gtk_box_pack_start(GTK_BOX(vbox1), vbox2, TRUE, TRUE, 0);

  checkbutton_send_bw = gtk_check_button_new_with_mnemonic("Bible_Works");
  gtk_widget_show(checkbutton_send_bw);
  gtk_box_pack_start(GTK_BOX(vbox2), checkbutton_send_bw, FALSE, FALSE, 0);

  // Set state.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_send_bw), settings->genconfig.reference_exchange_send_to_bibleworks_get());

  checkbutton_send_bt = gtk_check_button_new_with_mnemonic("Bible_Time");
  gtk_widget_show(checkbutton_send_bt);
  gtk_box_pack_start(GTK_BOX(vbox2), checkbutton_send_bt, FALSE, FALSE, 0);

  // Set state.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_send_bt), settings->genconfig.reference_exchange_send_to_bibletime_get());

  checkbutton_send_paratext = gtk_check_button_new_with_mnemonic("_Paratext, BART, Translator's Workplace");
  gtk_widget_show(checkbutton_send_paratext);
  gtk_box_pack_start(GTK_BOX(vbox2), checkbutton_send_paratext, FALSE, FALSE, 0);

  // Set state.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_send_paratext), settings->genconfig.reference_exchange_send_to_santafefocus_get());

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator1, TRUE, TRUE, 0);

  label4 = gtk_label_new("Receive references: ");
  gtk_widget_show(label4);
  gtk_box_pack_start(GTK_BOX(vbox1), label4, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label4), 0, 0.5);

  vbox3 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox3);
  gtk_box_pack_start(GTK_BOX(vbox1), vbox3, TRUE, TRUE, 0);

  GSList *radiobutton_receive_none_group = NULL;

  radiobutton_receive_none = gtk_radio_button_new_with_mnemonic(NULL, "Do not receive");
  gtk_widget_show(radiobutton_receive_none);
  gtk_box_pack_start(GTK_BOX(vbox3), radiobutton_receive_none, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_receive_none), radiobutton_receive_none_group);
  radiobutton_receive_none_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_receive_none));

  radiobutton_receive_bw = gtk_radio_button_new_with_mnemonic(NULL, "From _BibleWorks (not yet implemented)");
  gtk_widget_show(radiobutton_receive_bw);
  gtk_box_pack_start(GTK_BOX(vbox3), radiobutton_receive_bw, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_receive_bw), radiobutton_receive_none_group);
  radiobutton_receive_none_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_receive_bw));

  // Set state.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_receive_bw), settings->genconfig.reference_exchange_receive_from_bibleworks_get());

  radiobutton_receive_bt = gtk_radio_button_new_with_mnemonic(NULL, "From B_ibleTime");
  gtk_widget_show(radiobutton_receive_bt);
  gtk_box_pack_start(GTK_BOX(vbox3), radiobutton_receive_bt, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_receive_bt), radiobutton_receive_none_group);
  radiobutton_receive_none_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_receive_bt));

  // Set state.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_receive_bt), settings->genconfig.reference_exchange_receive_from_bibletime_get());

  radiobutton_receive_paratext = gtk_radio_button_new_with_mnemonic(NULL, "From P_aratext, BART, Translator's Workplace (not yet implemented)");
  gtk_widget_show(radiobutton_receive_paratext);
  gtk_box_pack_start(GTK_BOX(vbox3), radiobutton_receive_paratext, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_receive_paratext), radiobutton_receive_none_group);
  radiobutton_receive_none_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_receive_paratext));

  // Set state.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_receive_paratext), settings->genconfig.reference_exchange_receive_from_santafefocus_get());

  hseparator_warning_outpost = gtk_hseparator_new();
  gtk_widget_show(hseparator_warning_outpost);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator_warning_outpost, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox1, TRUE, TRUE, 0);

  image_warning_outpost = gtk_image_new_from_stock("gtk-dialog-warning", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image_warning_outpost);
  gtk_box_pack_start(GTK_BOX(hbox1), image_warning_outpost, FALSE, FALSE, 0);

  label_warning_outpost = gtk_label_new("Windows Outpost is needed but is not running.\nSet it under menu Preferences.");
  gtk_widget_show(label_warning_outpost);
  gtk_box_pack_start(GTK_BOX(hbox1), label_warning_outpost, FALSE, FALSE, 0);

  dialog_action_area1 = GTK_DIALOG(dialogsynchronize)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(dialogsynchronize, NULL, NULL);

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(dialogsynchronize), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(dialogsynchronize), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));
  g_signal_connect((gpointer) checkbutton_send_bw, "toggled", G_CALLBACK(on_checkbutton_send_bw_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_receive_bw, "toggled", G_CALLBACK(on_radiobutton_receive_bw_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_send_paratext, "toggled", G_CALLBACK(on_checkbutton_send_paratext_toggled), gpointer(this));
  g_signal_connect_after((gpointer) radiobutton_receive_paratext, "toggled", G_CALLBACK(on_radiobutton_receive_paratext_toggled), gpointer(this));

  // Set gui.
  on_bibleworks();
  on_outpost();
}

ReferenceExchangeDialog::~ReferenceExchangeDialog()
{
  gtk_widget_destroy(dialogsynchronize);
}

int ReferenceExchangeDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dialogsynchronize));
}

void ReferenceExchangeDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ReferenceExchangeDialog *) user_data)->on_okbutton();
}

void ReferenceExchangeDialog::on_okbutton()
{
  extern Settings *settings;
  settings->genconfig.bibleworks_executable_set(gtk_entry_get_text(GTK_ENTRY(entry_bw_exe)));
  settings->genconfig.reference_exchange_send_to_bibleworks_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_send_bw)));
  settings->genconfig.reference_exchange_receive_from_bibleworks_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_receive_bw)));
  settings->genconfig.reference_exchange_send_to_bibletime_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_send_bt)));
  settings->genconfig.reference_exchange_receive_from_bibletime_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_receive_bt)));
  settings->genconfig.reference_exchange_send_to_santafefocus_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_send_paratext)));
  settings->genconfig.reference_exchange_receive_from_santafefocus_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_receive_paratext)));
}

void ReferenceExchangeDialog::on_checkbutton_send_bw_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((ReferenceExchangeDialog *) user_data)->on_bibleworks();
}

void ReferenceExchangeDialog::on_radiobutton_receive_bw_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((ReferenceExchangeDialog *) user_data)->on_bibleworks();
}

void ReferenceExchangeDialog::on_bibleworks()
{
  // Enable the entry for entering the BibleWorks executable if needed.
  bool sensitive = false;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_send_bw)))
    sensitive = true;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_receive_bw)))
    sensitive = true;
  gtk_widget_set_sensitive(entry_bw_exe, sensitive);
}

void ReferenceExchangeDialog::on_checkbutton_send_paratext_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((ReferenceExchangeDialog *) user_data)->on_outpost();
}

void ReferenceExchangeDialog::on_radiobutton_receive_paratext_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((ReferenceExchangeDialog *) user_data)->on_outpost();
}

void ReferenceExchangeDialog::on_outpost()
// Show warning if Outpost is needed but not running.
{
  bool outpost_needed = false;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_send_paratext)))
    outpost_needed = true;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_receive_paratext)))
    outpost_needed = true;
  bool outpost_running = program_is_running(BIBLEDIT_WINDOWS_OUTPOST);
  if (outpost_needed && (!outpost_running)) {
    gtk_widget_show(hseparator_warning_outpost);
    gtk_widget_show(image_warning_outpost);
    gtk_widget_show(label_warning_outpost);
  } else {
    gtk_widget_hide(hseparator_warning_outpost);
    gtk_widget_hide(image_warning_outpost);
    gtk_widget_hide(label_warning_outpost);
  }
}
