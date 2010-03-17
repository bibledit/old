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
#include "dialogimportnotes.h"
#include "bible.h"
#include "utilities.h"
#include "dialogwait.h"
#include "utilities.h"
#include "notes_utils.h"
#include "date_time_utils.h"
#include <sqlite3.h>
#include "gtkwrappers.h"
#include "gwrappers.h"
#include "shell.h"
#include "books.h"
#include "help.h"
#include "unixwrappers.h"
#include "tiny_utilities.h"


#define TEMP_FILE "bibledit.import.notes"


ImportNotesDialog::ImportNotesDialog(int dummy)
{
  importnotesdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(importnotesdialog), "Import Notes");
  gtk_window_set_position(GTK_WINDOW(importnotesdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(importnotesdialog), TRUE);
  gtk_window_set_type_hint(GTK_WINDOW(importnotesdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG(importnotesdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  notebook = gtk_notebook_new();
  gtk_widget_show(notebook);
  gtk_box_pack_start(GTK_BOX(vbox1), notebook, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(notebook), 4);
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), FALSE);

  vbox4 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox4);
  gtk_container_add(GTK_CONTAINER(notebook), vbox4);

  label13 = gtk_label_new("You are going to import notes into your notes editor.\n\nClick Forward to continue.");
  gtk_widget_show(label13);
  gtk_box_pack_start(GTK_BOX(vbox4), label13, FALSE, FALSE, 5);
  gtk_misc_set_alignment(GTK_MISC(label13), 0, 0.5);

  label24 = gtk_label_new("");
  gtk_widget_show(label24);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 0), label24);

  vbox2 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox2);
  gtk_container_add(GTK_CONTAINER(notebook), vbox2);

  label6 = gtk_label_new("Select the file to import");
  gtk_widget_show(label6);
  gtk_box_pack_start(GTK_BOX(vbox2), label6, FALSE, FALSE, 10);

  select_directory_button = gtk_button_new();
  gtk_widget_show(select_directory_button);
  gtk_box_pack_start(GTK_BOX(vbox2), select_directory_button, FALSE, FALSE, 0);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(select_directory_button), alignment1);

  hbox2 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox2);
  gtk_container_add(GTK_CONTAINER(alignment1), hbox2);

  image1 = gtk_image_new_from_stock("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX(hbox2), image1, FALSE, FALSE, 0);

  label8 = gtk_label_new_with_mnemonic("");
  gtk_widget_show(label8);
  gtk_box_pack_start(GTK_BOX(hbox2), label8, FALSE, FALSE, 0);

  xml_file_label = gtk_label_new("");
  gtk_widget_show(xml_file_label);
  gtk_box_pack_start(GTK_BOX(vbox2), xml_file_label, FALSE, FALSE, 10);

  label26 = gtk_label_new("");
  gtk_widget_show(label26);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 1), label26);

  vbox5 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox5);
  gtk_container_add(GTK_CONTAINER(notebook), vbox5);

  label18 = gtk_label_new("We now know which file to import.\nThe next thing is to check if the file is in Unicode format.\n\nIf it is not in Unicode format, you will be asked to convert it.\n\nNote: The conversion may take time, depending on the size of the file.");
  gtk_widget_show(label18);
  gtk_box_pack_start(GTK_BOX(vbox5), label18, FALSE, FALSE, 10);
  gtk_misc_set_alignment(GTK_MISC(label18), 0, 0.5);

  label28 = gtk_label_new("");
  gtk_widget_show(label28);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 2), label28);

  vbox6 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox6);
  gtk_container_add(GTK_CONTAINER(notebook), vbox6);

  label19 = gtk_label_new("It's now in Unicode format.");
  gtk_widget_show(label19);
  gtk_box_pack_start(GTK_BOX(vbox6), label19, FALSE, FALSE, 10);
  gtk_misc_set_alignment(GTK_MISC(label19), 0, 0.5);

  label30 = gtk_label_new("");
  gtk_widget_show(label30);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 3), label30);

  vbox8 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox8);
  gtk_container_add(GTK_CONTAINER(notebook), vbox8);

  label23 = gtk_label_new("All preparation is now ready.\n\nPress Apply to actually import the notes.");
  gtk_widget_show(label23);
  gtk_box_pack_start(GTK_BOX(vbox8), label23, FALSE, FALSE, 10);
  gtk_misc_set_alignment(GTK_MISC(label23), 0, 0.5);

  label32 = gtk_label_new("");
  gtk_widget_show(label32);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 4), label32);

  hbox1 = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox1, FALSE, FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(hbox1), 4);

  backbutton = gtk_button_new_from_stock("gtk-go-back");
  gtk_widget_show(backbutton);
  gtk_box_pack_start(GTK_BOX(hbox1), backbutton, FALSE, FALSE, 0);

  forwardbutton = gtk_button_new_from_stock("gtk-go-forward");
  gtk_widget_show(forwardbutton);
  gtk_box_pack_start(GTK_BOX(hbox1), forwardbutton, FALSE, FALSE, 0);
  GTK_WIDGET_SET_FLAGS(forwardbutton, GTK_CAN_DEFAULT);

  dialog_action_area1 = GTK_DIALOG(importnotesdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(importnotesdialog, NULL, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(importnotesdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  applybutton = gtk_button_new_from_stock("gtk-apply");
  gtk_widget_show(applybutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(importnotesdialog), applybutton, GTK_RESPONSE_APPLY);
  gtk_widget_set_sensitive(applybutton, FALSE);
  GTK_WIDGET_SET_FLAGS(applybutton, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) select_directory_button, "clicked", G_CALLBACK(on_select_file_button_clicked), gpointer(this));
  g_signal_connect((gpointer) backbutton, "clicked", G_CALLBACK(on_backbutton_clicked), gpointer(this));
  g_signal_connect((gpointer) forwardbutton, "clicked", G_CALLBACK(on_forwardbutton_clicked), gpointer(this));
  g_signal_connect((gpointer) applybutton, "clicked", G_CALLBACK(on_applybutton_clicked), gpointer(this));

  gtk_widget_grab_focus(forwardbutton);
  gtk_widget_grab_default(forwardbutton);

  set_gui();
}


ImportNotesDialog::~ImportNotesDialog()
{
  // Clean up temporary file.
  unlink(temporary_file(TEMP_FILE).c_str());
  gtk_widget_destroy(importnotesdialog);
}


int ImportNotesDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(importnotesdialog));
}


void ImportNotesDialog::on_backbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ImportNotesDialog *) user_data)->on_backbutton();
}


void ImportNotesDialog::on_backbutton()
{
  gtk_notebook_prev_page(GTK_NOTEBOOK(notebook));
  set_gui();
}


void ImportNotesDialog::on_forwardbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ImportNotesDialog *) user_data)->on_forwardbutton();
}


void ImportNotesDialog::on_forwardbutton()
{
  gtk_notebook_next_page(GTK_NOTEBOOK(notebook));
  set_gui();
}


void ImportNotesDialog::on_select_file_button_clicked(GtkButton * button, gpointer user_data)
{
  ((ImportNotesDialog *) user_data)->on_select_file_button();
}


void ImportNotesDialog::on_select_file_button()
{
  ustring filename = gtkw_file_chooser_open(importnotesdialog, "Open a file", "");
  if (filename.empty())
    return;
  file_to_import = filename;
  select_file_page();
}


void ImportNotesDialog::set_gui()
{
  gtk_widget_set_sensitive(backbutton, TRUE);
  gtk_widget_set_sensitive(forwardbutton, TRUE);
  gtk_widget_set_sensitive(applybutton, FALSE);
  gint currentpage = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
  switch (currentpage) {
  case 0:
    {
      gtk_widget_set_sensitive(backbutton, FALSE);
      break;
    }
  case 1:
    {
      select_file_page();
      break;
    }
  case 2:
    {
      // Nothing to do, text is already in the notebook.
      break;
    }
  case 3:
    {
      unicode_page();
      break;
    }
  case 4:
    {
      // Some eye candy, because the process might take time.
      WaitDialog wd(500, 450, 300);
      wd.run();
      // Make known corrections to the xml file.
      make_known_corrections_to_xml_file();
      // Activate apply button.
      gtk_widget_set_sensitive(forwardbutton, FALSE);
      gtk_widget_set_sensitive(applybutton, TRUE);
      break;
    }
  }
}


void ImportNotesDialog::select_file_page()
{
  // Show filename.
  gtk_label_set_text(GTK_LABEL(label8), file_to_import.c_str());
  // See whether the file is of a known type.
  import_format = ifUnknown;
  if (!file_to_import.empty()) {
    GwSpawn spawn("head");
    spawn.arg(file_to_import);
    spawn.read();
    spawn.run();
    for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
      if (spawn.standardout[i].find("<bibledit-notes version=\"3\">") != string::npos)
        import_format = ifBibleditNotesVersion3;
      if (spawn.standardout[i].find("<ScriptureNotes Version=\"2.0\">") != string::npos)
        import_format = ifScriptureNotesVersion20;
    }
    // Info for user.
    ustring info_for_user;
    switch (import_format) {
    case ifUnknown:
      {
        info_for_user = "Bibledit-Gtk does not recognize the format of this file";
        break;
      }
    case ifBibleditNotesVersion3:
      {
        note_element = "note";
        reference_element = "references";
        notetext_element = "text";
        category_element = "category";
        project_element = "project";
        datemodified_element = "date-modified";
        datecreated_element = "date-created";
        createdby_element = "created-by";
        info_for_user = "Found: Bibledit-Gtk Notes Version 3";
        break;
      }
    case ifScriptureNotesVersion20:
      {
        note_element = "Note";
        reference_element = "References";
        notetext_element = "Contents";
        category_element = "Status";
        project_element = "Project";
        datemodified_element = "ModificationDate";
        datecreated_element = "CreationDate";
        createdby_element = "CreatedBy";
        info_for_user = "Found: TNE Scripture Notes Version 2.0";
        break;
      }
    }
    gtk_label_set_text(GTK_LABEL(xml_file_label), info_for_user.c_str());
  }
  // We only can go forward if we've found a known file.
  gtk_widget_set_sensitive(forwardbutton, import_format != ifUnknown);
  // Copy the xml file to the temporary location.
  if (import_format != ifUnknown) {
    unix_cp(file_to_import, temporary_file(TEMP_FILE));
  }
}


void ImportNotesDialog::unicode_page()
{
  // Filename of the file to import.
  ustring path = temporary_file(TEMP_FILE);
  // See whether the file contents is proper Unicode
  ustring file_contents;
  bool unicode_valid = true;
  {
    gchar *s;
    if (!g_file_get_contents(path.c_str(), &s, NULL, NULL))
      unicode_valid = false;
    file_contents = s;
    g_free(s);
  }
}


void ImportNotesDialog::make_known_corrections_to_xml_file()
{
  // Make a number of corrections to the xml file.
  ustring filename = temporary_file(TEMP_FILE);
  ReadText rt(filename, true);
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    // Get the line.
    ustring line = rt.lines[i];
    // The Glib xml parser gives an error on lines like:
    // ﻿<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
    // So remove that line.
    if (line.find("xml") != string::npos)
      line = "";
    // Store the line again in the vector.
    rt.lines[i] = line;
  }
  // Write the corrected file back to disk.
  write_lines(filename, rt.lines);
}


void ImportNotesDialog::on_applybutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ImportNotesDialog *) user_data)->on_apply();
}


void ImportNotesDialog::on_apply()
/*
Parse the xml file.
The various handlers will then store the data in the database.
*/
{
  // Some error handling.
  bool was_error = false;
  ustring error_message;
  // Initialisation of depth control variables.
  depth = 0;
  depth_of_notetext = 100;
  // Get contents of file.
  ustring filename = temporary_file(TEMP_FILE);
  gchar *contents;
  gsize length;
  GError *error;
  error = NULL;
  if (!g_file_get_contents(filename.c_str(), &contents, &length, &error)) {
    cerr << error->message << endl;
    g_error_free(error);
    was_error = true;
    error_message = "Error reading xml file";
    return;
  }
  // Set up parser.
  GMarkupParseContext *context;
  GMarkupParser parser = {
    start_element_handler,
    end_element_handler,
    text_handler,
    passthrough_handler,
    error_handler
  };
  // Parse xml file.
  if (!was_error) {
    context = g_markup_parse_context_new(&parser, GMarkupParseFlags(0), gpointer(this), NULL);
    if (!g_markup_parse_context_parse(context, contents, length, NULL)) {
      g_markup_parse_context_free(context);
      was_error = true;
      error_message = "Error parsing the xml file";
    }
  }
  if (!was_error) {
    if (!g_markup_parse_context_end_parse(context, NULL)) {
      g_markup_parse_context_free(context);
      was_error = true;
      error_message = "Error finishing parsing the xml file";
    }
  }
  if (!was_error)
    g_markup_parse_context_free(context);
  // Display message?
  if (was_error) {
    gtkw_dialog_error(importnotesdialog, error_message);
  }
  // Update the notes categories with possible new ones imported.
  ReadText rt(notes_categories_filename());
  notes_categories_add_from_database(rt.lines);
  write_lines(notes_categories_filename(), rt.lines);
}


void ImportNotesDialog::start_element_handler(GMarkupParseContext * context, const gchar * element_name, const gchar ** attribute_names, const gchar ** attribute_values, gpointer user_data, GError ** error)
{
  ustring element;
  element = element_name;
  ((ImportNotesDialog *) user_data)->start_element_handler(element);
}


void ImportNotesDialog::end_element_handler(GMarkupParseContext * context, const gchar * element_name, gpointer user_data, GError ** error)
{
  ustring element;
  element = element_name;
  ((ImportNotesDialog *) user_data)->end_element_handler(element);
}


void ImportNotesDialog::text_handler(GMarkupParseContext * context, const gchar * text, gsize text_len, gpointer user_data, GError ** error)
{
  ustring utext;
  utext = text;
  ((ImportNotesDialog *) user_data)->text_handler(utext);
}


void ImportNotesDialog::passthrough_handler(GMarkupParseContext * context, const gchar * passthrough_text, gsize text_len, gpointer user_data, GError ** error)
{
}


void ImportNotesDialog::error_handler(GMarkupParseContext * context, GError * error, gpointer user_data)
{
  cerr << error->message << endl;
}


void ImportNotesDialog::start_element_handler(const ustring & element_name)
/*
When we encounter a new element that starts data, this handler deals with that.
*/
{
  // Deal with depth of the elements.
  depth++;
  // Store depth of the note, so that we can catch everything nested deeper,
  // and treat it as note text also.
  if (element_name == notetext_element)
    depth_of_notetext = depth;
  if (element_name == note_element) {
    // New note starts from now. Clear storage area for new data.
    reference_content.clear();
    notetext_content.clear();
    status_content.clear();
    category_content.clear();
    project_content.clear();
    datemodified_content.clear();
    datecreated_content.clear();
    createdby_content.clear();
  }
  // Store element currently open.
  current_element = element_name;
}


void ImportNotesDialog::end_element_handler(const ustring & element_name)
/*
When we encounter an element that ends data, this handler deals with that.
*/
{
  // If new paragraph, deal with that.
  if (current_element == "P")
    notetext_content.append("\n");
  if (element_name == note_element) {
    // We are at the end of a note. Go through the data, and fill up 
    // missing bits and pieces, and then store a new note in the database.

    // Right, let's get started. First the reference.
    ustring osis_reference;
    {
      // There may be more than one reference, so deal with each of them.
      ParseLine parse(reference_content);
      reference_content.clear();
      for (unsigned int i = 0; i < parse.lines.size(); i++) {
        // Get the reference.
        Reference reference(0);
        if (reference_discover(0, 0, "", trim(parse.lines[i]), reference.book, reference.chapter, reference.verse)) {
          vector < int >verses = verses_encode(reference.verse);
          int book_chapter = reference_to_numerical_equivalent(books_id_to_english(reference.book), convert_to_string(reference.chapter), "0");
          for (unsigned int i2 = 0; i2 < verses.size(); i2++) {
            reference_content.append(" ");
            reference_content.append(convert_to_string(int (book_chapter + verses[i2])));
          }
          // Store the reference in OSIS format too.
          ustring osis_book = books_id_to_osis(reference.book);
          ustring osis_ref = osis_book + "." + convert_to_string(reference.chapter) + "." + reference.verse;
          if (!osis_reference.empty())
            osis_reference.append(" ");
          osis_reference.append(osis_ref);
        }
      }
      reference_content.append(" ");
      // If we did not find any reference, take Genesis 0:0.
      if (reference_content.empty()) {
        reference_content = "0";
        osis_reference = "Gen.0.0";
      }
    }

    // Deal with the text of the note.
    notetext_content = trim(notetext_content);

    // Deal with the category of the note.
    category_content = trim(category_content);

    // Deal with the project name.
    project_content = trim(project_content);

    // Deal with the date modified.
    int julian_modified;
    julian_modified = date_time_julian_day_get_parse(datemodified_content);

    // Deal with the date created.
    int julian_created;
    julian_created = date_time_julian_day_get_parse(datecreated_content);

    // Deal with the user.
    createdby_content = trim(createdby_content);

    // We've cleaned and verified all data. Store this note.

    // ID (integer)
    gint unique_id;
    unique_id = notes_database_get_unique_id();
    // References (text)
    // Project (text)
    // Category (text)
    // Note (text) Apostrophies need to be doubled before storing them.
    notetext_content = double_apostrophy(notetext_content);
    // Casefolded (text)
    ustring casefolded = notetext_content.casefold();
    // Date created.
    // Date modified.
    // Username.
    // Logbook.
    ustring logbook;
    // Store the note to file.
    notes_store_one_in_file(unique_id, notetext_content, project_content, osis_reference, category_content, julian_created, createdby_content, julian_modified, logbook);

  }
  // Deal with depth of the elements.
  depth--;
}


void ImportNotesDialog::text_handler(const ustring & text)
/*
When we encounter the text inside an element, this handler deals with that.
*/
{
  if (current_element == reference_element) {
    reference_content.append(text);
  } else if (current_element == notetext_element) {
    notetext_content.append(text);
  } else if (current_element == category_element) {
    category_content.append(text);
  } else if (current_element == project_element) {
    project_content.append(text);
  } else if (current_element == datemodified_element) {
    datemodified_content.append(text);
  } else if (current_element == datecreated_element) {
    datecreated_content.append(text);
  } else if (current_element == createdby_element) {
    createdby_content.append(text);
  }
  if (depth > depth_of_notetext)
    notetext_content.append(text);
}
