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


#include "dialogviewplanning.h"
#include "help.h"
#include "settings.h"
#include "shortcuts.h"


ViewPlanningDialog::ViewPlanningDialog(int dummy)
{
  Shortcuts shortcuts(0);

  viewstatusdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(viewstatusdialog), "View Planning");
  gtk_window_set_position(GTK_WINDOW(viewstatusdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(viewstatusdialog), TRUE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG (viewstatusdialog));
  gtk_widget_show(dialog_vbox1);

  vbox4 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox4);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox4, TRUE, TRUE, 0);

  label1 = gtk_label_new("Include the ticked items in the report");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(vbox4), label1, FALSE, FALSE, 0);

  checkbutton_planning = gtk_check_button_new_with_mnemonic("Planning");
  gtk_widget_show(checkbutton_planning);
  gtk_box_pack_start(GTK_BOX(vbox4), checkbutton_planning, FALSE, FALSE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_planning), TRUE);

  shortcuts.button(checkbutton_planning);

  checkbutton_perc_done_project = gtk_check_button_new_with_mnemonic("Percentage complete whole project");
  gtk_widget_show(checkbutton_perc_done_project);
  gtk_box_pack_start(GTK_BOX(vbox4), checkbutton_perc_done_project, FALSE, FALSE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_perc_done_project), TRUE);

  shortcuts.button(checkbutton_perc_done_project);

  checkbutton_perc_done_book = gtk_check_button_new_with_mnemonic("Percentage complete per book");
  gtk_widget_show(checkbutton_perc_done_book);
  gtk_box_pack_start(GTK_BOX(vbox4), checkbutton_perc_done_book, FALSE, FALSE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_perc_done_book), TRUE);

  shortcuts.button(checkbutton_perc_done_book);

  checkbutton_tasks_book = gtk_check_button_new_with_mnemonic("Tasks per book");
  gtk_widget_show(checkbutton_tasks_book);
  gtk_box_pack_start(GTK_BOX(vbox4), checkbutton_tasks_book, FALSE, FALSE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_tasks_book), TRUE);

  shortcuts.button(checkbutton_tasks_book);

  checkbutton_tasks_chapter = gtk_check_button_new_with_mnemonic("Tasks per chapter");
  gtk_widget_show(checkbutton_tasks_chapter);
  gtk_box_pack_start(GTK_BOX(vbox4), checkbutton_tasks_chapter, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_tasks_chapter);

  checkbutton_csv_export = gtk_check_button_new_with_mnemonic("CSV exported data");
  gtk_widget_show(checkbutton_csv_export);
  gtk_box_pack_start(GTK_BOX(vbox4), checkbutton_csv_export, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_csv_export);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(viewstatusdialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(viewstatusdialog, NULL, &shortcuts, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(viewstatusdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton), true);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(viewstatusdialog), okbutton, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton), true);

  shortcuts.stockbutton(cancelbutton);
  shortcuts.stockbutton(okbutton);
  shortcuts.process();

  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_focus(okbutton);
  gtk_widget_grab_default(okbutton);
}


ViewPlanningDialog::~ViewPlanningDialog()
{
  gtk_widget_destroy(viewstatusdialog);
}


int ViewPlanningDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(viewstatusdialog));
}


void ViewPlanningDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ViewPlanningDialog *) user_data)->on_okbutton();
}


void ViewPlanningDialog::on_okbutton()
{
  extern Settings *settings;
  reporting_produce_status_report(settings->genconfig.project_get(), 
                                  gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_planning)),
                                  gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_perc_done_project)), 
                                  gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_perc_done_book)), 
                                  gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_tasks_book)), 
                                  gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_tasks_chapter)), 
                                  gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_csv_export)));
}
