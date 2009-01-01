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
#include "dialogparallelbible.h"
#include "utilities.h"
#include "projectutils.h"
#include "directories.h"
#include "combobox.h"
#include "dialogselectchapters.h"
#include "portion_utils.h"
#include "settings.h"
#include "help.h"
#include "books.h"
#include "shortcuts.h"
#include "tiny_utilities.h"

ParallelBibleDialog::ParallelBibleDialog(int dummy)
{
  extern Settings *settings;

  Shortcuts shortcuts(0);

  parallelbibledialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(parallelbibledialog), "Parallel Bible");
  gtk_window_set_position(GTK_WINDOW(parallelbibledialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(parallelbibledialog), TRUE);

  dialog_vbox1 = GTK_DIALOG(parallelbibledialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 2);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, FALSE, FALSE, 0);

  label12 = gtk_label_new("Parallel Bible.\n" "This prints worksheets with parallel versions on it.\n" "Each verse of one book in your project gets the parallel verses of other projects printed below it.");
  gtk_widget_show(label12);
  gtk_box_pack_start(GTK_BOX(vbox1), label12, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label12), 0, 0.5);

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator1, TRUE, TRUE, 0);

  hbox3 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox3);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox3, TRUE, TRUE, 0);

  label13 = gtk_label_new("First project:");
  gtk_widget_show(label13);
  gtk_box_pack_start(GTK_BOX(hbox3), label13, FALSE, FALSE, 0);

  label_main_project = gtk_label_new(settings->genconfig.project_get().c_str());
  gtk_widget_show(label_main_project);
  gtk_box_pack_start(GTK_BOX(hbox3), label_main_project, FALSE, FALSE, 0);

  hbox4 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox4);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox4, TRUE, TRUE, 0);

  label16 = gtk_label_new("Book:");
  gtk_widget_show(label16);
  gtk_box_pack_start(GTK_BOX(hbox4), label16, FALSE, FALSE, 0);

  label_book = gtk_label_new(books_id_to_english(settings->genconfig.book_get()).c_str());
  gtk_widget_show(label_book);
  gtk_box_pack_start(GTK_BOX(hbox4), label_book, FALSE, FALSE, 0);

  hbox10 = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(hbox10);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox10, FALSE, FALSE, 0);

  label19 = gtk_label_new("Portion:");
  gtk_widget_show(label19);
  gtk_box_pack_start(GTK_BOX(hbox10), label19, FALSE, FALSE, 0);

  label_chapters = gtk_label_new("");
  gtk_widget_show(label_chapters);
  gtk_box_pack_start(GTK_BOX(hbox10), label_chapters, FALSE, FALSE, 0);

  button_chapters = gtk_button_new();
  gtk_widget_show(button_chapters);
  gtk_box_pack_start(GTK_BOX(hbox10), button_chapters, FALSE, FALSE, 0);

  alignment3 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment3);
  gtk_container_add(GTK_CONTAINER(button_chapters), alignment3);

  hbox11 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox11);
  gtk_container_add(GTK_CONTAINER(alignment3), hbox11);

  image3 = gtk_image_new_from_stock("gtk-preferences", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image3);
  gtk_box_pack_start(GTK_BOX(hbox11), image3, FALSE, FALSE, 0);

  label20 = gtk_label_new_with_mnemonic("Change");
  gtk_widget_show(label20);
  gtk_box_pack_start(GTK_BOX(hbox11), label20, FALSE, FALSE, 0);

  shortcuts.label(label20);

  hseparator2 = gtk_hseparator_new();
  gtk_widget_show(hseparator2);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator2, TRUE, TRUE, 0);

  label15 = gtk_label_new("Additional projects:");
  gtk_widget_show(label15);
  gtk_box_pack_start(GTK_BOX(vbox1), label15, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label15), 0, 0.5);

  hseparator4 = gtk_hseparator_new();
  gtk_widget_show(hseparator4);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator4, TRUE, TRUE, 0);

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

  label18 = gtk_label_new_with_mnemonic("Add project");
  gtk_widget_show(label18);
  gtk_box_pack_start(GTK_BOX(hbox8), label18, FALSE, FALSE, 0);

  shortcuts.label(label18);

  hseparator5 = gtk_hseparator_new();
  gtk_widget_show(hseparator5);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator5, TRUE, TRUE, 0);

  checkbutton_keep_together = gtk_check_button_new_with_mnemonic("Keep all parallel verses together on the page");
  gtk_widget_show(checkbutton_keep_together);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_keep_together, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_keep_together);

  checkbutton_include_verse0 = gtk_check_button_new_with_mnemonic("Include verses 0");
  gtk_widget_show(checkbutton_include_verse0);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_include_verse0, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_include_verse0);

  dialog_action_area1 = GTK_DIALOG(parallelbibledialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(parallelbibledialog, &shortcuts, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(parallelbibledialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(parallelbibledialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton);
  shortcuts.stockbutton(okbutton);
  shortcuts.process();

  g_signal_connect((gpointer) button_chapters, "clicked", G_CALLBACK(on_button_chapters_clicked), gpointer(this));
  g_signal_connect((gpointer) button_add, "clicked", G_CALLBACK(on_button_add_clicked), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton);
  gtk_widget_grab_focus(okbutton);

  // Set the gui.
  vector < ustring > versions = settings->genconfig.parallel_bible_projects_get();
  for (unsigned int i = 0; i < versions.size(); i++) {
    on_button_add(versions[i]);
  }
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_keep_together), settings->genconfig.parallel_bible_keep_verses_together_get());
  vector < unsigned int >chapters_from, chapters_to;
  vector < ustring > verses_from, verses_to;
  select_portion_get_values(settings->genconfig.project_get(), settings->genconfig.book_get(), settings->genconfig.parallel_bible_chapters_verses_get(), chapters_from, verses_from, chapters_to, verses_to);
  ustring label;
  for (unsigned int i = 0; i < chapters_from.size(); i++) {
    if (i)
      label.append(" | ");
    label.append(select_portion_get_label(settings->genconfig.project_get(), settings->genconfig.book_get(), chapters_from[i], verses_from[i], chapters_to[i], verses_to[i]));
  }
  gtk_label_set_text(GTK_LABEL(label_chapters), label.c_str());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_include_verse0), settings->genconfig.parallel_bible_include_verse_zero_get());
}

ParallelBibleDialog::~ParallelBibleDialog()
{
  gtk_widget_destroy(parallelbibledialog);
}

int ParallelBibleDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(parallelbibledialog));
}

void ParallelBibleDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ParallelBibleDialog *) user_data)->on_okbutton();
}

void ParallelBibleDialog::on_okbutton()
{
  // Save values.
  extern Settings *settings;
  vector < ustring > projects;
  for (unsigned int i = 0; i < selectprojectguis.size(); i++)
    projects.push_back(selectprojectguis[i]->project);
  settings->genconfig.parallel_bible_projects_set(projects);
  settings->genconfig.parallel_bible_keep_verses_together_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_keep_together)));
  settings->genconfig.parallel_bible_chapters_verses_set(gtk_label_get_text(GTK_LABEL(label_chapters)));
  settings->genconfig.parallel_bible_include_verse_zero_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_include_verse0)));
}

void ParallelBibleDialog::on_button_additional_project_clicked(GtkButton * button, gpointer user_data)
{
  ((ParallelBibleDialog *) user_data)->on_button_additional_project(button);
}

void ParallelBibleDialog::on_button_additional_project(GtkButton * button)
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

void ParallelBibleDialog::on_button_add_clicked(GtkButton * button, gpointer user_data)
{
  ((ParallelBibleDialog *) user_data)->on_button_add("");
}

void ParallelBibleDialog::on_button_add(const ustring & project)
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

void ParallelBibleDialog::vector_remove_content(vector < GtkWidget * >&container, int offset)
{
  vector < GtkWidget * >::iterator iter;
  iter = container.begin();
  for (int i = 0; i < offset; i++)
    iter++;
  container.erase(iter);
}

void ParallelBibleDialog::vector_remove_content(vector < SelectProjectGui * >&container, int offset)
{
  delete container[offset];
  vector < SelectProjectGui * >::iterator iter;
  iter = container.begin();
  for (int i = 0; i < offset; i++)
    iter++;
  container.erase(iter);
}

void ParallelBibleDialog::rewrite_button_labels()
// Rewrites the accelerators on the buttons, , so that it goes from 1 till x.
{
  for (unsigned int i = 0; i < selectprojectguis.size(); i++) {
    ustring label = "_" + convert_to_string(i + 1);
    selectprojectguis[i]->set_label(label);
  }
}

void ParallelBibleDialog::on_button_chapters_clicked(GtkButton * button, gpointer user_data)
{
  ((ParallelBibleDialog *) user_data)->on_button_chapters();
}

void ParallelBibleDialog::on_button_chapters()
{
  extern Settings *settings;
  SelectChaptersDialog dialog(settings->genconfig.project_get(), settings->genconfig.book_get(), gtk_label_get_text(GTK_LABEL(label_chapters)));
  if (dialog.run() == GTK_RESPONSE_OK) {
    gtk_label_set_text(GTK_LABEL(label_chapters), dialog.new_selection.c_str());
  }
}
