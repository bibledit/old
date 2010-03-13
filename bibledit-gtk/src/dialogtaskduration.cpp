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
#include "dialogtaskduration.h"
#include "help.h"
#include "reporting.h"
#include "shortcuts.h"
#include "screen.h"


TaskDurationDialog::TaskDurationDialog(vector < ustring > *tasks, vector < double >*durations)
{
  // Initialize variables
  mytasks = tasks;
  mydurations = durations;

  // Build gui.
  Shortcuts shortcuts(0);

  taskdurationdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(taskdurationdialog), "Task Duration Setup");
  gtk_window_set_position(GTK_WINDOW(taskdurationdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_type_hint(GTK_WINDOW(taskdurationdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG(taskdurationdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  label4 = gtk_label_new("Set the number of days a task takes to complete one chapter");
  gtk_widget_show(label4);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label4, FALSE, FALSE, 0);
  gtk_misc_set_padding(GTK_MISC(label4), 5, 5);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), scrolledwindow, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);

  viewport = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

  table1 = gtk_table_new(2, 2, FALSE);
  gtk_widget_show(table1);
  gtk_container_add (GTK_CONTAINER (viewport), table1);
  gtk_container_set_border_width(GTK_CONTAINER(table1), 5);
  gtk_table_set_col_spacings(GTK_TABLE(table1), 5);

  label5 = gtk_label_new("Task");
  gtk_widget_show(label5);
  gtk_table_attach(GTK_TABLE(table1), label5, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label5), 0, 0.5);

  label6 = gtk_label_new("Days per chapter");
  gtk_widget_show(label6);
  gtk_table_attach(GTK_TABLE(table1), label6, 1, 2, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label6), 0, 0.5);

  // Build gui for each task.
  for (unsigned int i = 0; i < mytasks->size(); i++) {

    label_task = gtk_label_new_with_mnemonic(mytasks->at(i).c_str());
    gtk_widget_show(label_task);
    gtk_table_attach(GTK_TABLE(table1), label_task, 0, 1, i + 1, i + 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC(label_task), 0, 0.5);

    shortcuts.label(label_task);

    spinbutton1_adj = gtk_adjustment_new(mydurations->at(i), 0, 5, 0.1, 1, 0);
    spinbutton1 = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton1_adj), 0.1, 1);
    gtk_widget_show(spinbutton1);
    gtk_table_attach(GTK_TABLE(table1), spinbutton1, 1, 2, i + 1, i + 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton1), TRUE);

    gtk_label_set_mnemonic_widget(GTK_LABEL(label_task), spinbutton1);

    spinbuttons.push_back(spinbutton1);

  }

  dialog_action_area1 = GTK_DIALOG(taskdurationdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(taskdurationdialog, NULL, &shortcuts, NULL);

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(taskdurationdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(taskdurationdialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton1);
  shortcuts.stockbutton(okbutton1);
  shortcuts.process();

  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));

  gtk_widget_grab_focus(okbutton1);
  gtk_widget_grab_default(okbutton1);

  new DialogAutoScaler (taskdurationdialog, G_MAXINT);
}


TaskDurationDialog::~TaskDurationDialog()
{
  gtk_widget_destroy(taskdurationdialog);
}


int TaskDurationDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(taskdurationdialog));
}


void TaskDurationDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((TaskDurationDialog *) user_data)->on_okbutton1();
}


void TaskDurationDialog::on_okbutton1()
{
  mydurations->clear();
  for (unsigned int i = 0; i < spinbuttons.size(); i++) {
    mydurations->push_back(gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbuttons[i])));
  }
}


