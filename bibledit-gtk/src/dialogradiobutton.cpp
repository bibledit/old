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
#include "dialogradiobutton.h"
#include "help.h"
#include "screen.h"


RadiobuttonDialog::RadiobuttonDialog(const ustring & title, const ustring & info, const vector < ustring > &labels, unsigned int selection, bool autoscale)
{
  Shortcuts shortcuts(0);

  radiobuttondialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(radiobuttondialog), title.c_str());
  gtk_window_set_position(GTK_WINDOW(radiobuttondialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(radiobuttondialog), TRUE);

  dialog_vbox1 = GTK_DIALOG(radiobuttondialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  label = gtk_label_new(info.c_str());
  gtk_widget_show(label);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);

  if (autoscale) {

    scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow1);
    gtk_box_pack_start (GTK_BOX (dialog_vbox1), scrolledwindow1, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_SHADOW_IN);

    viewport1 = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport1);
    gtk_container_add (GTK_CONTAINER (scrolledwindow1), viewport1);

  }

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  if (autoscale)
    gtk_container_add (GTK_CONTAINER (viewport1), vbox1);
  else
    gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  GSList *radiobutton_group = NULL;
  GtkWidget *radiobutton;

  for (unsigned int i = 0; i < labels.size(); i++) {

    radiobutton = gtk_radio_button_new_with_mnemonic(NULL, labels[i].c_str());
    gtk_widget_show(radiobutton);
    gtk_box_pack_start (GTK_BOX (vbox1), radiobutton, FALSE, FALSE, 0);
    gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton), radiobutton_group);
    radiobutton_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton));

    radiobuttons.push_back(radiobutton);

    shortcuts.button(radiobutton);

    if (i == selection) {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton), true);
      gtk_widget_grab_focus(radiobutton);
    }

    g_signal_connect ((gpointer) radiobutton, "button_press_event", G_CALLBACK (on_radiobutton_button_press_event), gpointer (this));

  }

  dialog_action_area1 = GTK_DIALOG(radiobuttondialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(radiobuttondialog, NULL, &shortcuts, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(radiobuttondialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(radiobuttondialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton);
  shortcuts.stockbutton(okbutton);
  shortcuts.process();

  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton);
  
  if (autoscale) {
    new DialogAutoScaler (radiobuttondialog, G_MAXINT);
  }
}


RadiobuttonDialog::~RadiobuttonDialog()
{
  gtk_widget_destroy(radiobuttondialog);
}


int RadiobuttonDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(radiobuttondialog));
}


gboolean RadiobuttonDialog::on_radiobutton_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  ((RadiobuttonDialog *) user_data)->radiobutton_button_press_event(event);
  return false;  
}


void RadiobuttonDialog::radiobutton_button_press_event (GdkEventButton *event)
{
  if (event->type == GDK_2BUTTON_PRESS) {
    on_okbutton();
    gtk_dialog_response(GTK_DIALOG(radiobuttondialog), GTK_RESPONSE_OK);
  }
}


void RadiobuttonDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((RadiobuttonDialog *) user_data)->on_okbutton();
}


void RadiobuttonDialog::on_okbutton()
{
  selection = 0;
  for (unsigned int i = 0; i < radiobuttons.size(); i++) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobuttons[i])))
      selection = i;
  }
}


