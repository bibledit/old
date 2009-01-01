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
#include "dialogplanningsetup.h"
#include "help.h"
#include "settings.h"
#include "dialogeditlist.h"
#include "reporting.h"
#include "dialogtaskduration.h"
#include "date_time_utils.h"
#include "dialogdate.h"

PlanningSetupDialog::PlanningSetupDialog(int dummy)
{
  // Initialize variables
  extern Settings *settings;
  startdate = settings->projectconfig(settings->genconfig.project_get())->planning_project_start_get();

  // Build gui.
  Shortcuts shortcuts(0);

  planningsetupdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(planningsetupdialog), "Planning Setup");
  gtk_window_set_position(GTK_WINDOW(planningsetupdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_type_hint(GTK_WINDOW(planningsetupdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG(planningsetupdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 5);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  table1 = gtk_table_new(1, 2, FALSE);
  gtk_widget_show(table1);
  gtk_box_pack_start(GTK_BOX(vbox1), table1, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(table1), 4);
  gtk_table_set_row_spacings(GTK_TABLE(table1), 4);
  gtk_table_set_col_spacings(GTK_TABLE(table1), 4);

  label3 = gtk_label_new("The project started on");
  gtk_widget_show(label3);
  gtk_table_attach(GTK_TABLE(table1), label3, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  button_start = gtk_button_new();
  gtk_widget_show(button_start);
  gtk_table_attach(GTK_TABLE(table1), button_start, 1, 2, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(button_start), alignment1);

  hbox2 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox2);
  gtk_container_add(GTK_CONTAINER(alignment1), hbox2);

  image1 = gtk_image_new_from_stock("gtk-connect", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX(hbox2), image1, FALSE, FALSE, 0);

  label2 = gtk_label_new_with_mnemonic("");
  gtk_widget_show(label2);
  gtk_box_pack_start(GTK_BOX(hbox2), label2, FALSE, FALSE, 0);

  shortcuts.label(label2);

  dialog_action_area1 = GTK_DIALOG(planningsetupdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(planningsetupdialog, &shortcuts, NULL);

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(planningsetupdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(planningsetupdialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton1);
  shortcuts.stockbutton(okbutton1);
  shortcuts.process();

  g_signal_connect((gpointer) button_start, "clicked", G_CALLBACK(on_button_start_clicked), gpointer(this));
  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_focus(okbutton1);
  gtk_widget_grab_default(okbutton1);

  gui();
}

PlanningSetupDialog::~PlanningSetupDialog()
{
  gtk_widget_destroy(planningsetupdialog);
}

int PlanningSetupDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(planningsetupdialog));
}

void PlanningSetupDialog::on_button_start_clicked(GtkButton * button, gpointer user_data)
{
  ((PlanningSetupDialog *) user_data)->on_button_start();
}

void PlanningSetupDialog::on_button_start()
{
  guint seconds = date_time_julian_to_seconds(startdate);
  DateDialog dialog(&seconds);
  if (dialog.run() == GTK_RESPONSE_OK) {
    startdate = date_time_seconds_to_julian(seconds);
    gui();
  }
}

void PlanningSetupDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((PlanningSetupDialog *) user_data)->on_okbutton();
}

void PlanningSetupDialog::on_okbutton()
{
  extern Settings *settings;
  settings->projectconfig(settings->genconfig.project_get())->planning_project_start_set(startdate);
}

void PlanningSetupDialog::gui()
{
  gtk_button_set_label(GTK_BUTTON(button_start), date_time_julian_human_readable(startdate, false).c_str());
}
