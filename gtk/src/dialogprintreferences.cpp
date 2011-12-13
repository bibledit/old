/*
** Copyright (©) 2003-2012 Teus Benschop.
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
#include "dialogprintreferences.h"
#include "utilities.h"
#include "projectutils.h"
#include "directories.h"
#include "combobox.h"
#include "dialogselectchapters.h"
#include "portion_utils.h"
#include "settings.h"
#include "help.h"
#include "books.h"
#include "tiny_utilities.h"


PrintReferencesDialog::PrintReferencesDialog(int dummy)
{
  extern Settings *settings;

  printreferencesdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(printreferencesdialog), "Print References");
  gtk_window_set_position(GTK_WINDOW(printreferencesdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(printreferencesdialog), TRUE);

  dialog_vbox1 = GTK_DIALOG(printreferencesdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 2);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, FALSE, FALSE, 0);

  label12 = gtk_label_new("This prints worksheets with the text of all the references on it.\n" "The text is taken from the project that is open. Other projects can be added.");
  gtk_widget_show(label12);
  gtk_box_pack_start(GTK_BOX(vbox1), label12, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label12), 0, 0.5);

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator1, TRUE, TRUE, 0);

  hbox3 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox3);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox3, TRUE, TRUE, 0);

  label13 = gtk_label_new("Main project:");
  gtk_widget_show(label13);
  gtk_box_pack_start(GTK_BOX(hbox3), label13, FALSE, FALSE, 0);

  label_main_project = gtk_label_new("");
  gtk_widget_show(label_main_project);
  gtk_box_pack_start(GTK_BOX(hbox3), label_main_project, FALSE, FALSE, 0);

  hseparator2 = gtk_hseparator_new();
  gtk_widget_show(hseparator2);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator2, TRUE, TRUE, 0);

  label15 = gtk_label_new("Additional projects:");
  gtk_widget_show(label15);
  gtk_box_pack_start(GTK_BOX(vbox1), label15, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label15), 0, 0.5);

  hbox7 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox7);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox7, TRUE, TRUE, 0);

  button_add = gtk_button_new();
  gtk_widget_show(button_add);
  gtk_box_pack_start(GTK_BOX(hbox7), button_add, FALSE, FALSE, 0);

  alignment2 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment2);
  gtk_container_add(GTK_CONTAINER(button_add), alignment2);

  hbox8 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox8);
  gtk_container_add(GTK_CONTAINER(alignment2), hbox8);

  image2 = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image2);
  gtk_box_pack_start(GTK_BOX(hbox8), image2, FALSE, FALSE, 0);

  label18 = gtk_label_new_with_mnemonic("_Add project");
  gtk_widget_show(label18);
  gtk_box_pack_start(GTK_BOX(hbox8), label18, FALSE, FALSE, 0);

  dialog_action_area1 = GTK_DIALOG(printreferencesdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(printreferencesdialog, NULL, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(printreferencesdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(printreferencesdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) button_add, "clicked", G_CALLBACK(on_button_add_clicked), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton);
  gtk_widget_grab_focus(okbutton);

  // Set the gui.
  gtk_label_set_text(GTK_LABEL(label_main_project), settings->genconfig.project_get().c_str());
  vector < ustring > versions = settings->genconfig.print_references_projects_get();
  for (unsigned int i = 0; i < versions.size(); i++) {
    on_button_add(versions[i]);
  }
}


PrintReferencesDialog::~PrintReferencesDialog()
{
  gtk_widget_destroy(printreferencesdialog);
}


int PrintReferencesDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(printreferencesdialog));
}


void PrintReferencesDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((PrintReferencesDialog *) user_data)->on_okbutton();
}


void PrintReferencesDialog::on_okbutton()
{
  // Save values.
  extern Settings *settings;
  vector < ustring > projects;
  for (unsigned int i = 0; i < selectprojectguis.size(); i++)
    projects.push_back(selectprojectguis[i]->project);
  settings->genconfig.print_references_projects_set(projects);
}


void PrintReferencesDialog::on_button_additional_project_clicked(GtkButton * button, gpointer user_data)
{
  ((PrintReferencesDialog *) user_data)->on_button_additional_project(button);
}


void PrintReferencesDialog::on_button_additional_project(GtkButton * button)
// Removes an "additional" project.
{
  // Get the offset of the widget to remove.
  int widget_offset = -1;
  for (unsigned int i = 0; i < buttons.size(); i++) {
    if (GTK_WIDGET(button) == buttons[i])
      widget_offset = i;
  }
  // Widget not found: bail out.
  if (widget_offset < 0)
    return;
  // Destroy the widgets.
  gtk_widget_destroy(hboxes[widget_offset]);
  // Remove widget's pointers.
  vector_remove_content(hboxes, widget_offset);
  vector_remove_content(selectprojectguis, widget_offset);
  vector_remove_content(buttons, widget_offset);
  // Rewrite the labels.
  rewrite_button_labels();
}


void PrintReferencesDialog::on_button_add_clicked(GtkButton * button, gpointer user_data)
{
  ((PrintReferencesDialog *) user_data)->on_button_add("");
}


void PrintReferencesDialog::on_button_add(const ustring & project)
// Adds a new project to the gui.
{
  // Get list of all projects.
  vector < ustring > projects = projects_get_all();

  // Only add the project if it exists.
  if (!project.empty()) {
    if (!project_exists(project)) {
      return;
    }
  }
  // All the widgets we're going to insert.  
  GtkWidget *hbox5;
  SelectProjectGui *selectprojectgui;
  GtkWidget *button_additional_project;
  GtkWidget *alignment1;
  GtkWidget *hbox6;
  GtkWidget *image1;
  GtkWidget *label17;

  // Build GUI.
  hbox5 = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(hbox5);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox5, TRUE, TRUE, 0);

  selectprojectgui = new SelectProjectGui(0);
  selectprojectgui->build(hbox5, "", project);

  button_additional_project = gtk_button_new();
  gtk_widget_show(button_additional_project);
  gtk_box_pack_start(GTK_BOX(hbox5), button_additional_project, FALSE, FALSE, 0);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(button_additional_project), alignment1);

  hbox6 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox6);
  gtk_container_add(GTK_CONTAINER(alignment1), hbox6);

  image1 = gtk_image_new_from_stock("gtk-remove", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX(hbox6), image1, FALSE, FALSE, 0);

  label17 = gtk_label_new_with_mnemonic("Remove");
  gtk_widget_show(label17);
  gtk_box_pack_start(GTK_BOX(hbox6), label17, FALSE, FALSE, 0);

  g_signal_connect((gpointer) button_additional_project, "clicked", G_CALLBACK(on_button_additional_project_clicked), gpointer(this));

  gtk_box_reorder_child(GTK_BOX(vbox1), hbox5, hboxes.size() + 7);

  // Store the relevant widgets.
  hboxes.push_back(hbox5);
  selectprojectguis.push_back(selectprojectgui);
  buttons.push_back(button_additional_project);

  // Reorder the accelerators on the remove buttons.
  rewrite_button_labels();
}


void PrintReferencesDialog::vector_remove_content(vector < GtkWidget * >&container, int offset)
{
  vector < GtkWidget * >::iterator iter;
  iter = container.begin();
  for (int i = 0; i < offset; i++)
    iter++;
  container.erase(iter);
}


void PrintReferencesDialog::vector_remove_content(vector < SelectProjectGui * >&container, int offset)
{
  delete container[offset];
  vector < SelectProjectGui * >::iterator iter;
  iter = container.begin();
  for (int i = 0; i < offset; i++)
    iter++;
  container.erase(iter);
}


void PrintReferencesDialog::rewrite_button_labels()
// Rewrites the accelerators on the buttons, , so that it goes from 1 till x.
{
  for (unsigned int i = 0; i < selectprojectguis.size(); i++) {
    ustring label = "_" + convert_to_string(i + 1);
    selectprojectguis[i]->set_label(label);
  }
}


