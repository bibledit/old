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


#include "dialognotesupdate.h"
#include "progresswindow.h"
#include "gtkwrappers.h"
#include "help.h"
#include "gui.h"
#include "shortcuts.h"
#include "notes_utils.h"
#include "combobox.h"
#include "projectutils.h"
#include "utilities.h"


NotesUpdateDialog::NotesUpdateDialog(int dummy)
{
  // Shortcuts.
  Shortcuts shortcuts1(0);
  Shortcuts shortcuts2(0);

  notesupdatedialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(notesupdatedialog), "Notes Mass Update");
  gtk_window_set_position(GTK_WINDOW(notesupdatedialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_type_hint(GTK_WINDOW(notesupdatedialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG(notesupdatedialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  notebook1 = gtk_notebook_new();
  gtk_widget_show(notebook1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), notebook1, TRUE, TRUE, 0);
  gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook1), GTK_POS_LEFT);

  label6 = gtk_label_new("This tool provides a way to update many project notes at once. It is a powerful tool, and changes made here cannot be made undone easily.\n\nAt the left, select the action you'd like to take.");
  gtk_widget_show(label6);
  gtk_container_add(GTK_CONTAINER(notebook1), label6);
  gtk_label_set_line_wrap(GTK_LABEL(label6), TRUE);
  gtk_misc_set_alignment(GTK_MISC(label6), 0, 0.5);

  label4 = gtk_label_new_with_mnemonic("Information");
  gtk_widget_show(label4);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook1), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook1), 0), label4);

  shortcuts1.label(label4);
  shortcuts2.label(label4);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_container_add(GTK_CONTAINER(notebook1), vbox1);
  gtk_container_set_border_width(GTK_CONTAINER(vbox1), 4);

  label7 = gtk_label_new("Move all notes that belong to project");
  gtk_widget_show(label7);
  gtk_box_pack_start(GTK_BOX(vbox1), label7, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label7), 0, 0.5);

  combobox_project_from = gtk_combo_box_new_text();
  gtk_widget_show(combobox_project_from);
  gtk_box_pack_start(GTK_BOX(vbox1), combobox_project_from, TRUE, TRUE, 0);

  shortcuts1.label(label7);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label7), combobox_project_from);

  label8 = gtk_label_new("to project");
  gtk_widget_show(label8);
  gtk_box_pack_start(GTK_BOX(vbox1), label8, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label8), 0, 0.5);

  combobox_project_to = gtk_combo_box_new_text();
  gtk_widget_show(combobox_project_to);
  gtk_box_pack_start(GTK_BOX(vbox1), combobox_project_to, TRUE, TRUE, 0);

  shortcuts1.label(label8);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label8), combobox_project_to);

  label5 = gtk_label_new_with_mnemonic("Project");
  gtk_widget_show(label5);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook1), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook1), 1), label5);

  shortcuts1.label(label5);
  shortcuts2.label(label5);

  vbox2 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox2);
  gtk_container_add(GTK_CONTAINER(notebook1), vbox2);
  gtk_container_set_border_width(GTK_CONTAINER(vbox2), 4);

  label10 = gtk_label_new("Move all notes that are in category");
  gtk_widget_show(label10);
  gtk_box_pack_start(GTK_BOX(vbox2), label10, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label10), 0, 0.5);

  combobox_category_from = gtk_combo_box_new_text();
  gtk_widget_show(combobox_category_from);
  gtk_box_pack_start(GTK_BOX(vbox2), combobox_category_from, TRUE, TRUE, 0);

  shortcuts2.label(label10);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label10), combobox_category_from);

  label11 = gtk_label_new("to category");
  gtk_widget_show(label11);
  gtk_box_pack_start(GTK_BOX(vbox2), label11, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label11), 0, 0.5);

  combobox_category_to = gtk_combo_box_new_text();
  gtk_widget_show(combobox_category_to);
  gtk_box_pack_start(GTK_BOX(vbox2), combobox_category_to, TRUE, TRUE, 0);

  shortcuts2.label(label11);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label11), combobox_category_to);

  label9 = gtk_label_new_with_mnemonic("Category");
  gtk_widget_show(label9);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook1), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook1), 2), label9);

  shortcuts1.label(label9);
  shortcuts2.label(label9);

  dialog_action_area1 = GTK_DIALOG(notesupdatedialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(notesupdatedialog, NULL, &shortcuts1, NULL);
  shortcuts2.stockbutton(shortcuts1.lastwidget);

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(notesupdatedialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

  shortcuts1.stockbutton(cancelbutton1);
  shortcuts2.stockbutton(cancelbutton1);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(notesupdatedialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);

  shortcuts1.stockbutton(okbutton1);
  shortcuts2.stockbutton(okbutton1);

  shortcuts1.process();
  shortcuts2.process();

  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton1);

  // Load all projects that are now in the notes.
  vector < ustring > databaseprojects;
  notes_projects_add_from_database(databaseprojects);
  combobox_set_strings(combobox_project_from, databaseprojects);
  if (!databaseprojects.empty())
    combobox_set_string(combobox_project_from, databaseprojects[0]);
  // Load all projects where we possibly can transfer to.
  vector < ustring > projects = projects_get_all();
  combobox_set_strings(combobox_project_to, projects);
  if (!projects.empty())
    combobox_set_string(combobox_project_to, projects[0]);
  // Load all categories that are now in the notes database.
  vector < ustring > databasecategories;
  notes_categories_add_from_database(databasecategories);
  sort(databasecategories.begin(), databasecategories.end());
  combobox_set_strings(combobox_category_from, databasecategories);
  if (!databasecategories.empty())
    combobox_set_string(combobox_category_from, databasecategories[0]);
  // Load all categories that we possibly can transfer to.
  ReadText categories(notes_categories_filename());
  notes_categories_add_from_database(categories.lines);
  combobox_set_strings(combobox_category_to, categories.lines);
  if (!categories.lines.empty())
    combobox_set_string(combobox_category_to, categories.lines[0]);
}


NotesUpdateDialog::~NotesUpdateDialog()
{
  gtk_widget_destroy(notesupdatedialog);
}


int NotesUpdateDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(notesupdatedialog));
}


void NotesUpdateDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((NotesUpdateDialog *) user_data)->on_ok();
}


void NotesUpdateDialog::on_ok()
{
  // Get the page we are on.
  int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook1));
  // Message to give.
  ustring message;
  // Give a double warning if we're going to do something serious.
  if (page)
    if (!gui_double_question(notesupdatedialog, "", ""))
      return;
  // Okay, take the action.
  switch (page) {
  case 0:
    {
      // Do nothing.
      message = "Nothing was done";
      break;
    }
  case 1:
    {
      // Change project.
      notes_change_project(combobox_get_active_string(combobox_project_from), combobox_get_active_string(combobox_project_to));
      message = "Notes were moved to the other project";
      break;
    }
  case 2:
    {
      // Change category.
      notes_change_category(combobox_get_active_string(combobox_category_from), combobox_get_active_string(combobox_category_to));
      message = "Notes were moved to the other category";
      break;
    }
  }
  // Give message.
  gtkw_dialog_info(notesupdatedialog, message);
}


