/*
** Copyright (©) 2003-2013 Teus Benschop.
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
#include "dialogplanningedit.h"
#include "help.h"
#include "settings.h"
#include "dialogeditlist.h"
#include "reporting.h"
#include "dialogtaskduration.h"
#include "date_time_utils.h"
#include "dialogdate.h"
#include "gwrappers.h"
#include "directories.h"
#include "dialogeditstatus.h"
#include "planning.h"


PlanningEditDialog::PlanningEditDialog(unsigned int book, unsigned int chapter)
{
  // Initialize variables
  mybook = book;
  mychapter = chapter;

  // Build gui.
  gtkbuilder = gtk_builder_new ();
  gtk_builder_add_from_file (gtkbuilder, gw_build_filename (directories_get_package_data(), "gtkbuilder.planningeditdialog.xml").c_str(), NULL);

  Shortcuts shortcuts(0);

  dialog = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "dialog"));

  button_status = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "button_status"));
  shortcuts.button (button_status);
  g_signal_connect((gpointer) button_status, "clicked", G_CALLBACK(on_button_status_clicked), gpointer(this));

  button_tasks = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "button_tasks"));
  shortcuts.button (button_tasks);
  g_signal_connect((gpointer) button_tasks, "clicked", G_CALLBACK(on_button_tasks_clicked), gpointer(this));

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


PlanningEditDialog::~PlanningEditDialog()
{
  g_object_unref (gtkbuilder);
  gtk_widget_destroy(dialog);
}


int PlanningEditDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dialog));
}


void PlanningEditDialog::on_button_status_clicked(GtkButton * button, gpointer user_data)
{
  ((PlanningEditDialog *) user_data)->on_button_status();
}

void PlanningEditDialog::on_button_status()
{
  extern Settings *settings;
  EditStatusDialog dialog(settings->genconfig.project_get(), mybook, mychapter);
  dialog.run();
}


void PlanningEditDialog::on_button_tasks_clicked(GtkButton * button, gpointer user_data)
{
  ((PlanningEditDialog *) user_data)->on_button_tasks();
}


void PlanningEditDialog::on_button_tasks()
{
  extern Settings *settings;
  planning_edit(settings->genconfig.project_get());
}


void PlanningEditDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((PlanningEditDialog *) user_data)->on_okbutton();
}


void PlanningEditDialog::on_okbutton()
{
}


void PlanningEditDialog::gui()
{
}


