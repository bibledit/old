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


#include "dialogscreenlayout.h"
#include "help.h"
#include "shortcuts.h"
#include "settings.h"
#include "restart.h"


ScreenLayoutDialog::ScreenLayoutDialog (int dummy)
{
  extern Settings * settings;
  Shortcuts shortcuts (0);
  
  screenlayoutdialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (screenlayoutdialog), "Screen Layout");
  gtk_window_set_position (GTK_WINDOW (screenlayoutdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal (GTK_WINDOW (screenlayoutdialog), TRUE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (screenlayoutdialog), TRUE);

  dialog_vbox1 = GTK_DIALOG (screenlayoutdialog)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  hbox3 = gtk_hbox_new (FALSE, 15);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox3, FALSE, FALSE, 0);

  label3 = gtk_label_new ("The Tools area is at the");
  gtk_widget_show (label3);
  gtk_box_pack_start (GTK_BOX (hbox3), label3, FALSE, FALSE, 0);

  GSList *radiobutton_tools_left_group = NULL;

  radiobutton_tools_left = gtk_radio_button_new_with_mnemonic (NULL, "left");
  gtk_widget_show (radiobutton_tools_left);
  gtk_box_pack_start (GTK_BOX (hbox3), radiobutton_tools_left, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_tools_left), radiobutton_tools_left_group);
  radiobutton_tools_left_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_tools_left));

  shortcuts.button (radiobutton_tools_left);

  radiobutton_tools_right = gtk_radio_button_new_with_mnemonic (NULL, "right");
  gtk_widget_show (radiobutton_tools_right);
  gtk_box_pack_start (GTK_BOX (hbox3), radiobutton_tools_right, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_tools_right), radiobutton_tools_left_group);
  radiobutton_tools_left_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_tools_right));

  shortcuts.button (radiobutton_tools_right);

  if (settings->genconfig.tools_area_left_get ()) {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton_tools_left), true);
  } else {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton_tools_right), true);
  }
  
  label4 = gtk_label_new ("side");
  gtk_widget_show (label4);
  gtk_box_pack_start (GTK_BOX (hbox3), label4, FALSE, FALSE, 0);

  hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (hseparator1);
  gtk_box_pack_start (GTK_BOX (vbox1), hseparator1, TRUE, TRUE, 0);

  hbox5 = gtk_hbox_new (FALSE, 15);
  gtk_widget_show (hbox5);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox5, TRUE, TRUE, 0);

  label5 = gtk_label_new ("The Editors display");
  gtk_widget_show (label5);
  gtk_box_pack_start (GTK_BOX (hbox5), label5, FALSE, FALSE, 0);

  GSList *radiobutton_editor_top_down_group = NULL;

  radiobutton_editor_top_down = gtk_radio_button_new_with_mnemonic (NULL, "one below the other");
  gtk_widget_show (radiobutton_editor_top_down);
  gtk_box_pack_start (GTK_BOX (hbox5), radiobutton_editor_top_down, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_editor_top_down), radiobutton_editor_top_down_group);
  radiobutton_editor_top_down_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_editor_top_down));

  shortcuts.button (radiobutton_editor_top_down);

  radiobutton_editor_side_by_side = gtk_radio_button_new_with_mnemonic (NULL, "side by side");
  gtk_widget_show (radiobutton_editor_side_by_side);
  gtk_box_pack_start (GTK_BOX (hbox5), radiobutton_editor_side_by_side, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_editor_side_by_side), radiobutton_editor_top_down_group);
  radiobutton_editor_top_down_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_editor_side_by_side));

  shortcuts.button (radiobutton_editor_side_by_side);

  if (settings->genconfig.split_view_editor_top_down_layout_get ()) {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton_editor_top_down), true);
  } else {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton_editor_side_by_side), true);
  }
  
  dialog_action_area1 = GTK_DIALOG (screenlayoutdialog)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (screenlayoutdialog, &shortcuts, NULL);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (screenlayoutdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (screenlayoutdialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  shortcuts.stockbutton (cancelbutton1);
  shortcuts.stockbutton (okbutton1);
  shortcuts.process ();

  g_signal_connect ((gpointer) okbutton1, "clicked", G_CALLBACK (on_okbutton1_clicked), gpointer(this));

  gtk_widget_grab_default (okbutton1);
}


ScreenLayoutDialog::~ScreenLayoutDialog ()
{
  gtk_widget_destroy (screenlayoutdialog);
}


int ScreenLayoutDialog::run()
{
  return gtk_dialog_run (GTK_DIALOG (screenlayoutdialog));
}


void ScreenLayoutDialog::on_okbutton1_clicked (GtkButton *button, gpointer user_data)
{
  ((ScreenLayoutDialog *) user_data)->on_ok ();
}

void ScreenLayoutDialog::on_ok ()
{
  // Get settings from gui.
  bool tools_left = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_tools_left));
  bool top_down = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_editor_top_down));

  // See whether restart is required.
  bool restart_flag = false;
  extern Settings * settings;
  if (settings->genconfig.tools_area_left_get () != tools_left)
    restart_flag = true;
  if (settings->genconfig.split_view_editor_top_down_layout_get () != top_down)
    restart_flag = true;
  
  // Store new settings.
  settings->genconfig.tools_area_left_set (tools_left);
  settings->genconfig.split_view_editor_top_down_layout_set (top_down);
  
  // Optionally do the restarting.
  if (restart_flag) restart ();
}
