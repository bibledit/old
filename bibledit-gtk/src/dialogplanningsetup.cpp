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
#include "dialogplanningsetup.h"
#include "help.h"
#include "settings.h"
#include "dialogeditlist.h"
#include "reporting.h"
#include "dialogtaskduration.h"
#include "date_time_utils.h"
#include "dialogdate.h"
#include "gwrappers.h"
#include "directories.h"


PlanningSetupDialog::PlanningSetupDialog(int dummy)
{
  // Initialize variables
  extern Settings *settings;
  startdate = settings->projectconfig(settings->genconfig.project_get())->planning_project_start_get();
  tasks = settings->genconfig.project_tasks_names_get();
  durations = settings->genconfig.project_tasks_durations_get();
  reporting_check_tasks_and_durations(tasks, &durations);
  tasks_ever = tasks;
  durations_ever = durations;

  // Build gui.
  gtkbuilder = gtk_builder_new ();
  gtk_builder_add_from_file (gtkbuilder, gw_build_filename (directories_get_package_data(), "gtkbuilder.planningsetupdialog.xml").c_str(), NULL);

  Shortcuts shortcuts(0);

  dialog = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "dialog"));

  button_start = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "button_start"));
  shortcuts.button (button_start);
  g_signal_connect((gpointer) button_start, "clicked", G_CALLBACK(on_button_start_clicked), gpointer(this));

  button_tasks = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "button_tasks"));
  shortcuts.button (button_tasks);
  g_signal_connect((gpointer) button_tasks, "clicked", G_CALLBACK(on_button_tasks_clicked), gpointer(this));

  button_time = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "button_time"));
  shortcuts.button (button_time);
  g_signal_connect((gpointer) button_time, "clicked", G_CALLBACK(on_button_time_clicked), gpointer(this));

  InDialogHelp * indialoghelp = new InDialogHelp(dialog, gtkbuilder, &shortcuts, NULL);

  cancelbutton = indialoghelp->cancelbutton;
  shortcuts.stockbutton (cancelbutton);
  
  okbutton = indialoghelp->okbutton;
  shortcuts.stockbutton (okbutton);
  gtk_widget_grab_default(okbutton);
  gtk_widget_grab_focus(okbutton);
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  shortcuts.process();

  gui();
}


PlanningSetupDialog::~PlanningSetupDialog()
{
  g_object_unref (gtkbuilder);
  gtk_widget_destroy(dialog);
}


int PlanningSetupDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dialog));
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


void PlanningSetupDialog::on_button_tasks_clicked(GtkButton * button, gpointer user_data)
{
  ((PlanningSetupDialog *) user_data)->on_button_tasks();
}


void PlanningSetupDialog::on_button_tasks()
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


void PlanningSetupDialog::on_button_time_clicked(GtkButton * button, gpointer user_data)
{
  ((PlanningSetupDialog *) user_data)->on_button_time();
}


void PlanningSetupDialog::on_button_time()
{
  TaskDurationDialog dialog(&tasks, &durations);
  dialog.run();
}


void PlanningSetupDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((PlanningSetupDialog *) user_data)->on_okbutton();
}


void PlanningSetupDialog::on_okbutton()
{
  extern Settings *settings;
  settings->projectconfig(settings->genconfig.project_get())->planning_project_start_set(startdate);
  settings->genconfig.project_tasks_names_set(tasks);
  settings->genconfig.project_tasks_durations_set(durations);
}


void PlanningSetupDialog::gui()
{
  gtk_button_set_label(GTK_BUTTON(button_start), date_time_julian_human_readable(startdate, false).c_str());
}


