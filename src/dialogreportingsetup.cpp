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
#include "dialogreportingsetup.h"
#include "help.h"
#include "settings.h"
#include "dialogeditlist.h"
#include "reporting.h"
#include "dialogtaskduration.h"

ReportingSetupDialog::ReportingSetupDialog(int dummy)
{
  // Initialize variables
  extern Settings *settings;
  tasks = settings->genconfig.project_tasks_names_get();
  durations = settings->genconfig.project_tasks_durations_get();
  reporting_check_tasks_and_durations(tasks, &durations);
  tasks_ever = tasks;
  durations_ever = durations;

  // Build gui.
  Shortcuts shortcuts(0);

  reportingsetupdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(reportingsetupdialog), "Reporting Setup");
  gtk_window_set_position(GTK_WINDOW(reportingsetupdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_type_hint(GTK_WINDOW(reportingsetupdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG(reportingsetupdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 5);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  table1 = gtk_table_new(2, 2, FALSE);
  gtk_widget_show(table1);
  gtk_box_pack_start(GTK_BOX(vbox1), table1, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(table1), 4);
  gtk_table_set_row_spacings(GTK_TABLE(table1), 4);
  gtk_table_set_col_spacings(GTK_TABLE(table1), 4);

  button_tasks = gtk_button_new();
  gtk_widget_show(button_tasks);
  gtk_table_attach(GTK_TABLE(table1), button_tasks, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(button_tasks), alignment1);

  hbox2 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox2);
  gtk_container_add(GTK_CONTAINER(alignment1), hbox2);

  image1 = gtk_image_new_from_stock("gtk-sort-ascending", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX(hbox2), image1, FALSE, FALSE, 0);

  label2 = gtk_label_new_with_mnemonic("Tasks");
  gtk_widget_show(label2);
  gtk_box_pack_start(GTK_BOX(hbox2), label2, FALSE, FALSE, 0);

  shortcuts.label(label2);

  label3 = gtk_label_new("The tasks that the project consists of");
  gtk_widget_show(label3);
  gtk_table_attach(GTK_TABLE(table1), label3, 1, 2, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label3), 0, 0.5);

  button_time = gtk_button_new();
  gtk_widget_show(button_time);
  gtk_table_attach(GTK_TABLE(table1), button_time, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment2 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment2);
  gtk_container_add(GTK_CONTAINER(button_time), alignment2);

  hbox4 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox4);
  gtk_container_add(GTK_CONTAINER(alignment2), hbox4);

  image2 = gtk_image_new_from_stock("gtk-index", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image2);
  gtk_box_pack_start(GTK_BOX(hbox4), image2, FALSE, FALSE, 0);

  label5 = gtk_label_new_with_mnemonic("Durations");
  gtk_widget_show(label5);
  gtk_box_pack_start(GTK_BOX(hbox4), label5, FALSE, FALSE, 0);

  shortcuts.label(label5);

  label4 = gtk_label_new("Time each task takes per chapter");
  gtk_widget_show(label4);
  gtk_table_attach(GTK_TABLE(table1), label4, 1, 2, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label4), 0, 0.5);

  dialog_action_area1 = GTK_DIALOG(reportingsetupdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(reportingsetupdialog, &shortcuts, "preferences_reporting");

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(reportingsetupdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(reportingsetupdialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton1);
  shortcuts.stockbutton(okbutton1);
  shortcuts.process();

  g_signal_connect((gpointer) button_tasks, "clicked", G_CALLBACK(on_button_tasks_clicked), gpointer(this));
  g_signal_connect((gpointer) button_time, "clicked", G_CALLBACK(on_button_time_clicked), gpointer(this));
  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_focus(okbutton1);
  gtk_widget_grab_default(okbutton1);
}

ReportingSetupDialog::~ReportingSetupDialog()
{
  gtk_widget_destroy(reportingsetupdialog);
}

int ReportingSetupDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(reportingsetupdialog));
}

void ReportingSetupDialog::on_button_tasks_clicked(GtkButton * button, gpointer user_data)
{
  ((ReportingSetupDialog *) user_data)->on_button_tasks();
}

void ReportingSetupDialog::on_button_tasks()
{
  // Run the dialog for editing the tasks.
  EditListDialog dialog(&tasks, "Tasks", "of tasks - add, remove, or re-order them", true, true, false, false, false, false, true, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    // Get the appropriate values for the durations, aligned to the tasks.
    durations.clear();
    for (unsigned int i = 0; i < tasks.size(); i++) {
      double duration = 0.5;
      for (unsigned int i2 = 0; i2 < tasks_ever.size(); i2++) {
        if (tasks[i] == tasks_ever[i2]) {
          duration = durations_ever[i2];
        }
      }
      durations.push_back(duration);
      // Store task and duration in the ones we've ever seen.
      tasks_ever.push_back(tasks[i]);
      durations_ever.push_back(duration);
    }
  }
}

void ReportingSetupDialog::on_button_time_clicked(GtkButton * button, gpointer user_data)
{
  ((ReportingSetupDialog *) user_data)->on_button_time();
}

void ReportingSetupDialog::on_button_time()
{
  TaskDurationDialog dialog(&tasks, &durations);
  dialog.run();
}

void ReportingSetupDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ReportingSetupDialog *) user_data)->on_okbutton();
}

void ReportingSetupDialog::on_okbutton()
{
  extern Settings *settings;
  settings->genconfig.project_tasks_names_set(tasks);
  settings->genconfig.project_tasks_durations_set(durations);
}
