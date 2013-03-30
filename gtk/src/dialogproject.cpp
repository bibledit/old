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
#include "dialogproject.h"
#include "utilities.h"
#include "projectutils.h"
#include "projectutils.h"
#include "dialogselectbooks.h"
#include "bible.h"
#include "directories.h"
#include <glib.h>
#include "stylesheetutils.h"
#include "constants.h"
#include "gwrappers.h"
#include "gtkwrappers.h"
#include "combobox.h"
#include "versification.h"
#include "shell.h"
#include "books.h"
#include "settings.h"
#include "date_time_utils.h"
#include "help.h"
#include "progresswindow.h"
#include "localizedbooks.h"
#include "versifications.h"
#include "shortcuts.h"
#include "tiny_utilities.h"
#include "scripts.h"
#include "dialogdictionary.h"


#define NEW_PROJECT "New Project"


ProjectDialog::ProjectDialog(bool newproject)
{
  // Settings.
  extern Settings *settings;

  // Save variables.
  if (newproject) {
    // Make "New Project".
    project_create_restore(NEW_PROJECT, "");
    currentprojectname = NEW_PROJECT;
  } else {
    currentprojectname = settings->genconfig.project_get();
  }

  // Get project information.
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());

  // Shortcuts.
  Shortcuts shortcuts(0);

  projectdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(projectdialog), "Project properties");
  gtk_window_set_position(GTK_WINDOW(projectdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(projectdialog), TRUE);
  gtk_window_set_type_hint(GTK_WINDOW(projectdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG(projectdialog));
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new_with_mnemonic("Name:");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

  shortcuts.label(label1);

  nameentry = gtk_entry_new();
  gtk_widget_show(nameentry);
  gtk_box_pack_start(GTK_BOX(vbox1), nameentry, FALSE, FALSE, 2);
  gtk_entry_set_text(GTK_ENTRY(nameentry), "name");
  gtk_entry_set_activates_default(GTK_ENTRY(nameentry), TRUE);

  // Set name of project
  gtk_entry_set_text(GTK_ENTRY(nameentry), currentprojectname.c_str());

  messagelabel = gtk_label_new("");
  gtk_widget_show(messagelabel);
  gtk_box_pack_start(GTK_BOX(vbox1), messagelabel, FALSE, FALSE, 0);

  checkbutton_editable = gtk_check_button_new_with_mnemonic("Editable");
  gtk_widget_show(checkbutton_editable);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_editable, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_editable);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_editable), projectconfig->editable_get());

  table1 = gtk_table_new(3, 3, FALSE);
  gtk_widget_show(table1);
  gtk_box_pack_start(GTK_BOX(vbox1), table1, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(table1), 4);
  gtk_table_set_row_spacings(GTK_TABLE(table1), 6);
  gtk_table_set_col_spacings(GTK_TABLE(table1), 8);

  label9 = gtk_label_new("Remove books");
  gtk_widget_show(label9);
  gtk_table_attach(GTK_TABLE(table1), label9, 2, 3, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  label8 = gtk_label_new("Add books");
  gtk_widget_show(label8);
  gtk_table_attach(GTK_TABLE(table1), label8, 0, 1, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  vseparator1 = gtk_vseparator_new();
  gtk_widget_show(vseparator1);
  gtk_table_attach(GTK_TABLE(table1), vseparator1, 1, 2, 0, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  addbutton = gtk_button_new();
  gtk_widget_show(addbutton);
  gtk_table_attach(GTK_TABLE(table1), addbutton, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(addbutton), alignment1);

  hbox2 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox2);
  gtk_container_add(GTK_CONTAINER(alignment1), hbox2);

  image1 = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX(hbox2), image1, FALSE, FALSE, 0);

  label5 = gtk_label_new_with_mnemonic("Templates");
  gtk_widget_show(label5);
  gtk_box_pack_start(GTK_BOX(hbox2), label5, FALSE, FALSE, 0);

  shortcuts.label(label5);

  deletebutton = gtk_button_new();
  gtk_widget_show(deletebutton);
  gtk_table_attach(GTK_TABLE(table1), deletebutton, 2, 3, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment2 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment2);
  gtk_container_add(GTK_CONTAINER(deletebutton), alignment2);

  hbox3 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox3);
  gtk_container_add(GTK_CONTAINER(alignment2), hbox3);

  image2 = gtk_image_new_from_stock("gtk-remove", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image2);
  gtk_box_pack_start(GTK_BOX(hbox3), image2, FALSE, FALSE, 0);

  label6 = gtk_label_new_with_mnemonic("Books");
  gtk_widget_show(label6);
  gtk_box_pack_start(GTK_BOX(hbox3), label6, FALSE, FALSE, 0);

  shortcuts.label(label6);

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator1, TRUE, TRUE, 2);

  table2 = gtk_table_new(2, 2, FALSE);
  gtk_widget_show(table2);
  gtk_box_pack_start(GTK_BOX(vbox1), table2, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(table2), 2);
  gtk_table_set_row_spacings(GTK_TABLE(table2), 2);
  gtk_table_set_col_spacings(GTK_TABLE(table2), 2);

  label11 = gtk_label_new_with_mnemonic("Versification");
  gtk_widget_show(label11);
  gtk_table_attach(GTK_TABLE(table2), label11, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label11), 0, 0.5);

  shortcuts.label(label11);

  combobox_versification = gtk_combo_box_new_text();
  gtk_widget_show(combobox_versification);
  gtk_table_attach(GTK_TABLE(table2), combobox_versification, 1, 2, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (GTK_FILL), 0, 0);

  // Load versifications.
  extern Versifications *versifications;
  vector < ustring > systems = versifications->systems_get();
  combobox_set_strings(combobox_versification, systems);
  combobox_set_string(combobox_versification, projectconfig->versification_get());

  label12 = gtk_label_new_with_mnemonic("Language");
  gtk_widget_show(label12);
  gtk_table_attach(GTK_TABLE(table2), label12, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label12), 0, 0.5);

  shortcuts.label(label12);

  combobox_language = gtk_combo_box_new_text();
  gtk_widget_show(combobox_language);
  gtk_table_attach(GTK_TABLE(table2), combobox_language, 1, 2, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_FILL), 0, 0);

  // Load languages.
  extern BookLocalizations *booklocalizations;
  vector < ustring > languages = booklocalizations->localizations_get();
  combobox_set_strings(combobox_language, languages);
  combobox_set_string(combobox_language, projectconfig->language_get());

  checkbutton_right_to_left = gtk_check_button_new_with_mnemonic("Predominantly right-to-left text");
  gtk_widget_show(checkbutton_right_to_left);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_right_to_left, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_right_to_left);

  // Set RTL.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_right_to_left), projectconfig->right_to_left_get());

  hbox_depend = gtk_hbox_new(FALSE, 5);
  gtk_widget_show(hbox_depend);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox_depend, TRUE, TRUE, 0);

  checkbutton_dependent = gtk_check_button_new_with_mnemonic("Depend upon project");
  gtk_widget_show(checkbutton_dependent);
  gtk_box_pack_start(GTK_BOX(hbox_depend), checkbutton_dependent, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_dependent);

  // Set depend-button state.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_dependent), projectconfig->depending_on_switch_get());

  button_depend = gtk_button_new_with_mnemonic(dependent_project(projectconfig->depending_on_project_get()).c_str());
  gtk_widget_show(button_depend);
  gtk_box_pack_start(GTK_BOX(hbox_depend), button_depend, FALSE, FALSE, 0);

  label_depend = gtk_label_new("through filter");
  gtk_widget_show(label_depend);
  gtk_box_pack_start(GTK_BOX(hbox_depend), label_depend, FALSE, FALSE, 0);

  combobox_depend = gtk_combo_box_new_text();
  gtk_widget_show(combobox_depend);
  gtk_box_pack_start(GTK_BOX(hbox_depend), combobox_depend, TRUE, TRUE, 0);

  // Set values.
  combobox_set_strings(combobox_depend, scripts_get_all());
  ustring script = projectconfig->depending_on_script_get();
  if (script_available(script))
    combobox_set_string(combobox_depend, script);
  else
    combobox_set_index(combobox_depend, 0);

  hbox_spelling = gtk_hbox_new(FALSE, 10);
  gtk_widget_show(hbox_spelling);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox_spelling, TRUE, TRUE, 0);

  checkbutton_spelling = gtk_check_button_new_with_mnemonic("Check spelling");
  gtk_widget_show(checkbutton_spelling);
  gtk_box_pack_start(GTK_BOX(hbox_spelling), checkbutton_spelling, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_spelling);

  // Set the spelling. A new project has the spelling on by default.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_spelling), projectconfig->spelling_check_get() || newproject);

  button_dictionaries = gtk_button_new_with_mnemonic("Dictionaries");
  gtk_widget_show(button_dictionaries);
  gtk_box_pack_start(GTK_BOX(hbox_spelling), button_dictionaries, FALSE, FALSE, 0);

  shortcuts.button(button_dictionaries);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(projectdialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(projectdialog, NULL, &shortcuts, "file/project/project-properties");

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(projectdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton1), true);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(projectdialog), okbutton1, GTK_RESPONSE_OK);
  gtk_widget_set_sensitive(okbutton1, FALSE);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton1), true);

  shortcuts.stockbutton(cancelbutton1);
  shortcuts.stockbutton(okbutton1);
  shortcuts.process();

  g_signal_connect((gpointer) nameentry, "changed", G_CALLBACK(projectdialog_on_nameentry_changed), gpointer(this));
  g_signal_connect((gpointer) checkbutton_editable, "toggled", G_CALLBACK(on_checkbutton_editable_toggled), gpointer(this));
  g_signal_connect((gpointer) addbutton, "clicked", G_CALLBACK(projectdialog_on_addbutton_clicked), gpointer(this));
  g_signal_connect((gpointer) deletebutton, "clicked", G_CALLBACK(projectdialog_on_deletebutton_clicked), gpointer(this));
  g_signal_connect((gpointer) cancelbutton1, "clicked", G_CALLBACK(projectdialog_on_cancelbutton1_clicked), gpointer(this));
  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(projectdialog_on_okbutton1_clicked), gpointer(this));
  g_signal_connect((gpointer) checkbutton_dependent, "toggled", G_CALLBACK(on_checkbutton_dependent_toggled), gpointer(this));
  g_signal_connect((gpointer) button_depend, "clicked", G_CALLBACK(on_button_depend_clicked), gpointer(this));
  g_signal_connect((gpointer) checkbutton_spelling, "toggled", G_CALLBACK(on_checkbutton_spelling_toggled), gpointer(this));
  g_signal_connect((gpointer) button_dictionaries, "clicked", G_CALLBACK(on_button_dictionaries_clicked), gpointer(this));

  gtk_label_set_mnemonic_widget(GTK_LABEL(label1), nameentry);

  gtk_label_set_mnemonic_widget(GTK_LABEL(label11), combobox_versification);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label12), combobox_language);

  gtk_widget_grab_focus(nameentry);
  gtk_widget_grab_default(okbutton1);

  // Handle gui.
  on_checkbutton_dependent();
  set_gui();
}


ProjectDialog::~ProjectDialog()
{
  gtk_widget_destroy(projectdialog);
}


int ProjectDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(projectdialog));
}


void ProjectDialog::set_gui()
{
  bool sensitive = true;
  newprojectname = gtk_entry_get_text(GTK_ENTRY(nameentry));
  newprojectname = trim(newprojectname);
  newprojectname = shell_clean_filename(newprojectname);
  // Reject "New Project" as a name.
  if (newprojectname == NEW_PROJECT) {
    sensitive = false;
    gtk_label_set_text(GTK_LABEL(messagelabel), "(the name of the project should be changed)");
  }
  // Reject empty names.
  if (newprojectname.empty()) {
    sensitive = false;
    gtk_label_set_text(GTK_LABEL(messagelabel), "(the project needs a name)");
  }
  // Reject names that already exist.
  if (currentprojectname != newprojectname) {
    if (project_exists(newprojectname)) {
      sensitive = false;
      gtk_label_set_text(GTK_LABEL(messagelabel), "(a project with this name already exists)");
    }
  }
  // Reject "data".
  if (newprojectname == "data") {
    sensitive = false;
    gtk_label_set_text(GTK_LABEL(messagelabel), "(this name is not allowed)");
  }
  // Deal with the sensitivity of the dialog.
  gtk_widget_set_sensitive(okbutton1, sensitive);
  if (sensitive)
    gtk_label_set_text(GTK_LABEL(messagelabel), "");

  // If non-editable, set widgets insensitive.
  bool editable = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_editable));
  gtk_widget_set_sensitive(nameentry, editable);
  gtk_widget_set_sensitive(messagelabel, editable);
  gtk_widget_set_sensitive(addbutton, editable);
  gtk_widget_set_sensitive(deletebutton, editable && project_get_books(currentprojectname).size() > 0);
  gtk_widget_set_sensitive(combobox_versification, editable);
  gtk_widget_set_sensitive(combobox_language, editable);
  gtk_widget_set_sensitive(label1, editable);
  gtk_widget_set_sensitive(label8, editable);
  gtk_widget_set_sensitive(label9, editable);
  gtk_widget_set_sensitive(label11, editable);
  gtk_widget_set_sensitive(label12, editable);
  gtk_widget_set_sensitive(checkbutton_right_to_left, editable);
  if (editable) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_dependent), false);
  }
  // Spelling widgets.
  sensitive = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_spelling));
  gtk_widget_set_sensitive(button_dictionaries, sensitive);
}


void ProjectDialog::on_ok()
{
  // Deal with possible new projectname.
  if (currentprojectname != newprojectname) {
    // Move project.
    project_move(currentprojectname, newprojectname);
  }
  // Save settings.
  extern Settings *settings;
  settings->genconfig.project_set(newprojectname);
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  projectconfig->versification_set(combobox_get_active_string(combobox_versification));
  projectconfig->language_set(combobox_get_active_string(combobox_language));
  projectconfig->editable_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_editable)));
  projectconfig->right_to_left_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_right_to_left)));
  projectconfig->spelling_check_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_spelling)));

  // Save diglot-related settings.
  bool depend_switch = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_dependent));
  projectconfig->depending_on_switch_set(depend_switch);
  ustring depend_project = dependent_project();
  projectconfig->depending_on_project_set(depend_project);
  ustring depend_script = combobox_get_active_string(combobox_depend);
  if (depend_script == scripts_straight_through())
    depend_script.clear();
  if (!script_available(depend_script))
    depend_script.clear();
  projectconfig->depending_on_script_set(depend_script);

  // If the project depends on another, do the copy through the script.
  if (depend_switch && (!depend_project.empty())) {

    // Progress information.
    ProgressWindow progresswindow("Updating project", false);

    progresswindow.set_fraction(0.1);

    // Get the list of book/chapters that is in the source project.
    vector < unsigned int >source_books;
    vector < unsigned int >source_chapters;
    {
      vector < unsigned int >books = project_get_books(depend_project);
      for (unsigned int b = 0; b < books.size(); b++) {
        vector < unsigned int >chapters = project_get_chapters(depend_project, books[b]);
        for (unsigned c = 0; c < chapters.size(); c++) {
          source_books.push_back(books[b]);
          source_chapters.push_back(chapters[c]);
        }
      }
    }

    progresswindow.set_fraction(0.2);

    // Delete the books and chapters from this project that are not in the source project.
    {
      vector < unsigned int >books = project_get_books(newprojectname);
      for (unsigned int b = 0; b < books.size(); b++) {
        if (project_book_exists(depend_project, books[b])) {
          vector < unsigned int >chapters = project_get_chapters(newprojectname, books[b]);
          for (unsigned c = 0; c < chapters.size(); c++) {
            bool exists = false;
            for (unsigned int i = 0; i < source_books.size(); i++) {
              if (source_books[i] == books[b]) {
                if (source_chapters[i] == chapters[c]) {
                  exists = true;
                }
              }
            }
            if (!exists) {
              project_remove_chapter(newprojectname, books[b], chapters[c]);
            }
          }
        } else {
          project_remove_book(newprojectname, books[b]);
        }
      }
    }

    progresswindow.set_iterate(0.2, 1, source_books.size());

    // Copy everything from the source project to this project.
    // We need to "touch" the files of the project it depends on to make them 
    // look newer, so as to ensure that they get loaded.
    for (unsigned int i = 0; i < source_books.size(); i++) {
      progresswindow.iterate();
      GwSpawn spawn("touch");
      spawn.arg(project_data_filename_chapter(depend_project, source_books[i], source_chapters[i], false));
      spawn.run();
      vector < ustring > lines = project_retrieve_chapter(newprojectname, source_books[i], source_chapters[i]);
    }

  }

}


void ProjectDialog::on_cancel()
{
  // Remove the "New Project". It was created but not used.
  project_delete(NEW_PROJECT);
}


void ProjectDialog::on_book_add()
{
/*
 * This gives the user the possibility to add books that are still missing in 
 * the project. Only those books that are not yet in the project are shown.
 * If the user adds book(s), templates of the book are placed in the project.
 * The templates contain markers for chapters, verses, and the basics.
 */
  vector < unsigned int >selectables;
  set < unsigned int >selection;
  vector < unsigned int >scripture_books = project_get_books(currentprojectname);
  set < unsigned int >currentbooks;
  for (unsigned int i = 0; i < scripture_books.size(); i++)
    currentbooks.insert(scripture_books[i]);
  {
    vector < unsigned int >ids = books_type_to_ids(btUnknown);
    for (unsigned int i = 0; i < ids.size(); i++) {
      if (currentbooks.find(ids[i]) == currentbooks.end())
        selectables.push_back(ids[i]);
    }
  }
  books_standard_order(selectables);
  SelectBooksDialog dialog(false);
  dialog.language(combobox_get_active_string(combobox_language));
  dialog.selectables(selectables);
  dialog.selection_set(selection);
  int result = dialog.run();
  if (result == GTK_RESPONSE_OK) {
    selection = dialog.selectionset;
    vector < unsigned int >ids = books_type_to_ids(btUnknown);
    ProgressWindow progresswindow("Adding books", false);
    progresswindow.set_iterate(0, 1, ids.size());
    for (unsigned int i = 0; i < ids.size(); i++) {
      progresswindow.iterate();
      if (!(selection.find(ids[i]) == selection.end())) {
        vector < ustring > booktemplate;
        // If the book is found in the templates, take that, else create it.
        ustring englishbook = books_id_to_english(ids[i]);
        ustring templatefile = englishbook.casefold() + ".usfm";
        replace_text(templatefile, " ", "_");
        templatefile = gw_build_filename(directories_get_package_data(), templatefile);
        if (g_file_test(templatefile.c_str(), G_FILE_TEST_IS_REGULAR)) {
          ReadText rt(templatefile, true, false);
          booktemplate.assign(rt.lines.begin(), rt.lines.end());
        } else {
          versification_create_book_template(combobox_get_active_string(combobox_versification), ids[i], booktemplate);
        }
        // Change any _year_ to the current year, so as to remain fresh always.
        ustring myyear;
        {
          int julianday = date_time_julian_day_get_current();
          guint year, month, day;
          date_time_normal_get_year_month_day(julianday, year, month, day);
          myyear = convert_to_string(year);
        }
        for (unsigned int bt = 0; bt < booktemplate.size(); bt++) {
          replace_text(booktemplate[bt], "_year_", myyear);
        }
        CategorizeChapterVerse ccv(booktemplate);
        project_store_book(currentprojectname, ids[i], ccv);
      }
    }
  }
  // Update GUI.
  set_gui();
}


void ProjectDialog::on_book_delete()
{
  vector < unsigned int >selectables;
  set < unsigned int >selection;
  vector < unsigned int >scripture_books = project_get_books(currentprojectname);
  set < unsigned int >currentbooks;
  for (unsigned int i = 0; i < scripture_books.size(); i++)
    currentbooks.insert(scripture_books[i]);
  {
    vector < unsigned int >ids = books_type_to_ids(btUnknown);
    for (unsigned int i = 0; i < ids.size(); i++) {
      if (currentbooks.find(ids[i]) != currentbooks.end())
        selectables.push_back(ids[i]);
    }
  }
  books_standard_order(selectables);
  SelectBooksDialog dialog(false);
  dialog.language(combobox_get_active_string(combobox_language));
  dialog.selectables(selectables);
  dialog.selection_set(selection);
  int result = dialog.run();
  if (result == GTK_RESPONSE_OK) {
    selection = dialog.selectionset;
    if (selection.empty())
      return;
    if (gtkw_dialog_question(projectdialog, "Are you sure you want to delete the books?") != GTK_RESPONSE_YES)
      return;
    if (gtkw_dialog_question(projectdialog, "Are you really sure to delete something worth perhaps months of work?") != GTK_RESPONSE_YES)
      return;
    vector < unsigned int >ids = books_type_to_ids(btUnknown);
    ProgressWindow progresswindow("Deleting books", false);
    progresswindow.set_iterate(0, 1, ids.size());
    for (unsigned int i = 0; i < ids.size(); i++) {
      // Remove the book.
      if (selection.find(ids[i]) != selection.end()) {
        project_remove_book(currentprojectname, ids[i]);
      }
      progresswindow.iterate();
    }

  }
  // Update GUI.

  set_gui();
}



void ProjectDialog::projectdialog_on_nameentry_changed(GtkEditable * editable, gpointer user_data)
{
  ((ProjectDialog *) user_data)->set_gui();
}


void ProjectDialog::projectdialog_on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((ProjectDialog *) user_data)->on_ok();
}


void ProjectDialog::projectdialog_on_addbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ProjectDialog *) user_data)->on_book_add();
}


void ProjectDialog::projectdialog_on_deletebutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ProjectDialog *) user_data)->on_book_delete();
}


void ProjectDialog::projectdialog_on_cancelbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((ProjectDialog *) user_data)->on_cancel();
}


void ProjectDialog::on_checkbutton_editable_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((ProjectDialog *) user_data)->set_gui();
}


void ProjectDialog::on_checkbutton_dependent_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((ProjectDialog *) user_data)->on_checkbutton_dependent();
}


void ProjectDialog::on_checkbutton_dependent()
{
  bool on = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_dependent));
  if (on) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_editable), false);
  }
  gtk_widget_set_sensitive(button_depend, on);
  gtk_widget_set_sensitive(label_depend, on);
  gtk_widget_set_sensitive(combobox_depend, on);
}


void ProjectDialog::on_button_depend_clicked(GtkButton * button, gpointer user_data)
{
  ((ProjectDialog *) user_data)->on_button_depend();
}


void ProjectDialog::on_button_depend()
{
  ustring project(dependent_project());
  if (project_select(project)) {
    gtk_button_set_label(GTK_BUTTON(button_depend), project.c_str());
  }
}


ustring ProjectDialog::dependent_project(const ustring & project)
{
  ustring label(project);
  if (label.empty() || !project_exists(project))
    label = none_project();
  return label;
}


ustring ProjectDialog::none_project()
{
  return "<none>";
}


ustring ProjectDialog::dependent_project()
// Gets the project name from the button.
{
  ustring project = gtk_button_get_label(GTK_BUTTON(button_depend));
  if (project == none_project())
    project.clear();
  return project;
}


void ProjectDialog::on_button_dictionaries_clicked(GtkButton * button, gpointer user_data)
{
  ((ProjectDialog *) user_data)->on_button_dictionaries();
}


void ProjectDialog::on_button_dictionaries()
{
  DictionaryDialog dialog(currentprojectname);
  dialog.run();
}


void ProjectDialog::on_checkbutton_spelling_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((ProjectDialog *) user_data)->set_gui();
}


