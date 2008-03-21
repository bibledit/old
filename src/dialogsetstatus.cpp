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


#include "dialogsetstatus.h"
#include "help.h"
#include "settings.h"
#include "books.h"
#include "tiny_utilities.h"
#include "progresswindow.h"


#define MAX_CHAPTERS 150
#define MAX_VERSES 200


SetStatusDialog::SetStatusDialog (ProjectStatusRecord * statusrecord_in)
{
  // Initialize variables.
  statusrecord = statusrecord_in;
  
  // Load the possible tasks.
  extern Settings * settings;
  tasks = settings->genconfig.project_tasks_names_get ();
  vector <double> durations = settings->genconfig.project_tasks_durations_get ();
  reporting_check_tasks_and_durations (tasks, &durations);
  reporting_durations_to_percentages (durations, percentages);
  
  // Build dialog.  
  setstatusdialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (setstatusdialog), "Set Status");
  gtk_window_set_position (GTK_WINDOW (setstatusdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal (GTK_WINDOW (setstatusdialog), TRUE);

  dialog_vbox1 = GTK_DIALOG (setstatusdialog)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox4);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox4, TRUE, TRUE, 0);

  // Make as many entries as there are tasks.
  for (unsigned int i = 0; i < tasks.size (); i++) {

    checkbutton_status = gtk_check_button_new_with_mnemonic (tasks[i].c_str());
    gtk_widget_show (checkbutton_status);
    gtk_box_pack_start (GTK_BOX (vbox4), checkbutton_status, FALSE, FALSE, 0);
    
    if (statusrecord->tasks_done[i] > 0)
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_status), true);
    if (statusrecord->tasks_done[i] == 0)
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_status), false);
    if (statusrecord->tasks_done[i] < 0)
      gtk_toggle_button_set_inconsistent (GTK_TOGGLE_BUTTON (checkbutton_status), true);

    g_signal_connect ((gpointer) checkbutton_status, "toggled",
                      G_CALLBACK (on_checkbutton_status_toggled),
                      gpointer (this));
    
    checkbuttons.push_back (checkbutton_status);
    
  }
  
  dialog_action_area1 = GTK_DIALOG (setstatusdialog)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (setstatusdialog, NULL, "set_status");

  cancelbutton = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (setstatusdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (setstatusdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton, GTK_CAN_DEFAULT);

  g_signal_connect ((gpointer) okbutton, "clicked",
                    G_CALLBACK (on_okbutton_clicked),
                    gpointer (this));
  
  gtk_widget_grab_focus (okbutton);
  gtk_widget_grab_default (okbutton);
}


SetStatusDialog::~SetStatusDialog ()
{
  gtk_widget_destroy (setstatusdialog);
}


int SetStatusDialog::run ()
{
  return gtk_dialog_run (GTK_DIALOG (setstatusdialog));
}


void SetStatusDialog::on_checkbutton_status_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  if (gtk_toggle_button_get_inconsistent (togglebutton)) {
    gtk_toggle_button_set_inconsistent (togglebutton, false);
  }
}


void SetStatusDialog::on_okbutton_clicked (GtkButton *button, gpointer user_data)
{
  ((SetStatusDialog *) user_data)->on_okbutton ();
}


void SetStatusDialog::on_okbutton ()
{
  for (unsigned int i = 0; i < tasks.size (); i++) {
    if (gtk_toggle_button_get_inconsistent (GTK_TOGGLE_BUTTON (checkbuttons [i]))) {
      statusrecord->tasks_done[i] = -1;
    } else {
      if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbuttons [i]))) {
        statusrecord->tasks_done[i] = 1;
      } else {
        statusrecord->tasks_done[i] = 0;
      }
    }
  }
}
