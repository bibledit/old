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


#include "utilities.h"
#include "dialogshownotes.h"
#include <glib.h>
#include "notes_utils.h"
#include "dialogdate.h"
#include "date_time_utils.h"
#include "combobox.h"
#include "settings.h"
#include "help.h"
#include "shortcuts.h"
#include "directories.h"
#include "gwrappers.h"
#include "tiny_utilities.h"
#include "books.h"


ShowNotesDialog::ShowNotesDialog(int dummy)
{
  event_id = 0;
  extern Settings *settings;

  gtkbuilder = gtk_builder_new ();
  gtk_builder_add_from_file (gtkbuilder, gw_build_filename (directories_get_package_data(), "gtkbuilder.showprojectnotesdialog.xml").c_str(), NULL);

  dialog = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "dialog"));

  GSList *verse_reference_group = NULL;

  radiobutton_current_verse = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_current_verse"));
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_current_verse), verse_reference_group);
  verse_reference_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_current_verse));
  g_signal_connect ((gpointer) radiobutton_current_verse, "clicked", G_CALLBACK (on_button_clicked), gpointer (this));

  radiobutton_current_chapter = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_current_chapter"));
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_current_chapter), verse_reference_group);
  verse_reference_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_current_chapter));
  g_signal_connect ((gpointer) radiobutton_current_chapter, "clicked", G_CALLBACK (on_button_clicked), gpointer (this));

  radiobutton_current_book = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_current_book"));
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_current_book), verse_reference_group);
  verse_reference_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_current_book));
  g_signal_connect ((gpointer) radiobutton_current_book, "clicked", G_CALLBACK (on_button_clicked), gpointer (this));

  radiobutton_any_verse = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_any_verse"));
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_any_verse), verse_reference_group);
  verse_reference_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_any_verse));
  g_signal_connect ((gpointer) radiobutton_any_verse, "clicked", G_CALLBACK (on_button_clicked), gpointer (this));

  GSList *radiobutton_date_group = NULL;

  radiobutton_today = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_today"));
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_today), radiobutton_date_group);
  radiobutton_date_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_today));
  g_signal_connect ((gpointer) radiobutton_today, "clicked", G_CALLBACK (on_button_clicked), gpointer (this));

  radiobutton_between = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_between"));
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_between), radiobutton_date_group);
  radiobutton_date_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_between));
  g_signal_connect ((gpointer) radiobutton_between, "clicked", G_CALLBACK (on_button_clicked), gpointer (this));

  button_start = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "button_start"));
  g_signal_connect((gpointer) button_start, "clicked", G_CALLBACK(on_fromdatebutton_clicked), gpointer(this));
  g_signal_connect ((gpointer) button_start, "clicked", G_CALLBACK (on_button_clicked), gpointer (this));

  button_end = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "button_end"));
  g_signal_connect((gpointer) button_end, "clicked", G_CALLBACK(on_todatebutton_clicked), gpointer(this));
  g_signal_connect ((gpointer) button_end, "clicked", G_CALLBACK (on_button_clicked), gpointer (this));

  radiobutton_at_any_time = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_at_any_time"));
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_at_any_time), radiobutton_date_group);
  radiobutton_date_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_at_any_time));
  g_signal_connect ((gpointer) radiobutton_at_any_time, "clicked", G_CALLBACK (on_button_clicked), gpointer (this));

  // Glade-3 does not seem to be able to work with gtk_combo_box_new_text yet. Workaround below.
  label_category = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_category"));
  hbox_category = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "hbox_category"));
  combobox_category = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "combobox_category"));
  gtk_widget_destroy (combobox_category);
  combobox_category = gtk_combo_box_new_text();
  gtk_widget_show(combobox_category);
  gtk_box_pack_start(GTK_BOX(hbox_category), combobox_category, TRUE, TRUE, 0);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label_category), combobox_category);
  g_signal_connect ((gpointer) combobox_category, "changed", G_CALLBACK (on_combobox_changed), gpointer (this));

  GSList *radiobutton_project_group = NULL;

  radiobutton_current_project = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_current_project"));
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_current_project), radiobutton_project_group);
  radiobutton_project_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_current_project));
  g_signal_connect ((gpointer) radiobutton_current_project, "clicked", G_CALLBACK (on_button_clicked), gpointer (this));

  radiobutton_any_project = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_any_project"));
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_any_project), radiobutton_project_group);
  radiobutton_project_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_any_project));
  g_signal_connect ((gpointer) radiobutton_any_project, "clicked", G_CALLBACK (on_button_clicked), gpointer (this));

  checkbutton_title = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_title"));
  g_signal_connect((gpointer) checkbutton_title, "toggled", G_CALLBACK(on_checkbutton_show_title_toggled), gpointer(this));
  g_signal_connect ((gpointer) checkbutton_title, "clicked", G_CALLBACK (on_button_clicked), gpointer (this));

  checkbutton_project = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_project"));
  g_signal_connect ((gpointer) checkbutton_project, "clicked", G_CALLBACK (on_button_clicked), gpointer (this));

  checkbutton_category = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_category"));
  g_signal_connect ((gpointer) checkbutton_category, "clicked", G_CALLBACK (on_button_clicked), gpointer (this));

  checkbutton_date_created = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_date_created"));
  g_signal_connect ((gpointer) checkbutton_date_created, "clicked", G_CALLBACK (on_button_clicked), gpointer (this));

  checkbutton_created_by = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_created_by"));
  g_signal_connect ((gpointer) checkbutton_created_by, "clicked", G_CALLBACK (on_button_clicked), gpointer (this));

  GSList *radiobutton_text_group = NULL;

  radiobutton_full = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_full"));
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_full), radiobutton_text_group);
  radiobutton_text_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_full));

  radiobutton_summary = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_summary"));
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_summary), radiobutton_text_group);
  radiobutton_text_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_summary));

  checkbutton_add_ref_text = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_add_ref_text"));

  // Set the reference selection.
  GtkToggleButton *refbutton = reference_get_button(settings->genconfig.notes_selection_reference_get());
  if (refbutton) {
    gtk_toggle_button_set_active(refbutton, true);
  }

  // Set the edited selection.
  GtkToggleButton *editbutton = edited_get_button(settings->genconfig.notes_selection_edited_get());
  if (editbutton) {
    gtk_toggle_button_set_active(editbutton, true);
  }
  from_day = settings->genconfig.notes_selection_date_from_get();
  to_day = settings->genconfig.notes_selection_date_to_get();

  // Load available categories and set the combo.
  ReadText rt(notes_categories_filename());
  rt.lines.push_back(all_categories());
  combobox_set_strings(combobox_category, rt.lines);
  ustring category = settings->genconfig.notes_selection_category_get();
  if (category.empty())
    category = all_categories();
  combobox_set_string(combobox_category, category);

  // Project selection.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_any_project), !settings->genconfig.notes_selection_current_project_get());

  // Title and inclusions.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_title), settings->session.project_notes_show_title);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_project), settings->genconfig.notes_display_project_get());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_category), settings->genconfig.notes_display_category_get());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_date_created), settings->genconfig.notes_display_date_created_get());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_created_by), settings->genconfig.notes_display_created_by_get());

  // Notes text display.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_summary), settings->genconfig.notes_display_summary_get());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_add_ref_text), settings->genconfig.notes_display_reference_text_get());

  label_result = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_result"));

  Shortcuts shortcuts(0);
  InDialogHelp * indialoghelp = new InDialogHelp(dialog, gtkbuilder, &shortcuts, "view/project-notes");
  shortcuts.process();
  cancelbutton = indialoghelp->cancelbutton;
  okbutton = indialoghelp->okbutton;
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));
  gtk_widget_grab_default(okbutton);

  set_gui();
}


ShowNotesDialog::~ShowNotesDialog()
{
  gtk_widget_destroy(dialog);
}


int ShowNotesDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dialog));
}


void ShowNotesDialog::on_fromdatebutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ShowNotesDialog *) user_data)->on_from_date();
}


void ShowNotesDialog::on_todatebutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ShowNotesDialog *) user_data)->on_to_date();
}


void ShowNotesDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ShowNotesDialog *) user_data)->on_ok();
}


void ShowNotesDialog::on_from_date()
{
  guint seconds = date_time_julian_to_seconds(from_day);
  DateDialog dialog(&seconds);
  if (dialog.run() == GTK_RESPONSE_OK) {
    from_day = date_time_seconds_to_julian(seconds);
    set_gui();
  }
}


void ShowNotesDialog::on_to_date()
{
  guint seconds = date_time_julian_to_seconds(to_day);
  DateDialog dialog(&seconds);
  if (dialog.run() == GTK_RESPONSE_OK) {
    to_day = date_time_seconds_to_julian(seconds);
    set_gui();
  }
}


void ShowNotesDialog::on_ok()
{
  extern Settings *settings;

  // Reference selection.
  settings->genconfig.notes_selection_reference_set(get_reference_selection ());

  // Edited selection.  
  settings->genconfig.notes_selection_edited_set(get_edited_selection ());
  settings->genconfig.notes_selection_date_from_set(from_day);
  settings->genconfig.notes_selection_date_to_set(to_day);

  // Category.
  ustring category = combobox_get_active_string(combobox_category);
  if (category == all_categories())
    category.clear();
  settings->genconfig.notes_selection_category_set(category);

  // Project selection.
  settings->genconfig.notes_selection_current_project_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_current_project)));

  // Title and inclusions.
  settings->session.project_notes_show_title = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_title));
  settings->genconfig.notes_display_project_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_project)));
  settings->genconfig.notes_display_category_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_category)));
  settings->genconfig.notes_display_date_created_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_date_created)));
  settings->genconfig.notes_display_created_by_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_created_by)));

  // Notes text display.
  settings->genconfig.notes_display_summary_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_summary)));
  settings->genconfig.notes_display_reference_text_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_add_ref_text)));
}


void ShowNotesDialog::set_gui()
{
  // Set the labels on the date buttons.
  gtk_button_set_label(GTK_BUTTON(button_start), date_time_julian_human_readable(from_day, true).c_str());
  gtk_button_set_label(GTK_BUTTON(button_end), date_time_julian_human_readable(to_day, true).c_str());

  // Rewrite shortcuts.
  Shortcuts shortcuts(0);
  shortcuts.button(radiobutton_current_verse);
  shortcuts.button(radiobutton_current_chapter);
  shortcuts.button(radiobutton_current_book);
  shortcuts.button(radiobutton_any_verse);
  shortcuts.button(radiobutton_today);
  shortcuts.button(radiobutton_between);
  shortcuts.button(button_start);
  shortcuts.button(button_end);
  shortcuts.button(radiobutton_at_any_time);
  shortcuts.label(label_category);
  shortcuts.button(radiobutton_current_project);
  shortcuts.button(radiobutton_any_project);
  shortcuts.button(checkbutton_title);
  shortcuts.button(checkbutton_project);
  shortcuts.button(checkbutton_category);
  shortcuts.button(checkbutton_date_created);
  shortcuts.button(checkbutton_created_by);
  shortcuts.button(radiobutton_full);
  shortcuts.button(radiobutton_summary);
  shortcuts.button(checkbutton_add_ref_text);
  // Temporal helpbutton for correct processing of the _H.  
  GtkWidget *helpbutton = gtk_button_new_from_stock("gtk-help");
  shortcuts.stockbutton(helpbutton);
  shortcuts.stockbutton(cancelbutton);
  shortcuts.stockbutton(okbutton);
  shortcuts.process();
  gtk_widget_destroy(helpbutton);
  on_checkbutton_show_title();
}


ustring ShowNotesDialog::all_categories()
{
  return "All categories";
}


GtkToggleButton *ShowNotesDialog::reference_get_button(int selector)
{
  GtkToggleButton *button = NULL;
  NotesSelectionReferenceType ref_selection = (NotesSelectionReferenceType) selector;
  switch (ref_selection) {
  case nsrtCurrentVerse:
    {
      button = GTK_TOGGLE_BUTTON(radiobutton_current_verse);
      break;
    }
  case nsrtCurrentChapter:
    {
      button = GTK_TOGGLE_BUTTON(radiobutton_current_chapter);
      break;
    }
  case nsrtCurrentBook:
    {
      button = GTK_TOGGLE_BUTTON(radiobutton_current_book);
      break;
    }
  case nsrtAny:
    {
      button = GTK_TOGGLE_BUTTON(radiobutton_any_verse);
      break;
    }
  }
  return button;
}


GtkToggleButton *ShowNotesDialog::edited_get_button(int selector)
{
  GtkToggleButton *button = NULL;
  NotesSelectionEditedType edit_selection = (NotesSelectionEditedType) selector;
  switch (edit_selection) {
  case nsetToday:
    {
      button = GTK_TOGGLE_BUTTON(radiobutton_today);
      break;
    }
  case nsetDateRange:
    {
      button = GTK_TOGGLE_BUTTON(radiobutton_between);
      break;
    }
  case nsetAny:
    {
      button = GTK_TOGGLE_BUTTON(radiobutton_at_any_time);
      break;
    }
  }
  return button;
}


void ShowNotesDialog::on_checkbutton_show_title_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((ShowNotesDialog *) user_data)->on_checkbutton_show_title();
}


void ShowNotesDialog::on_checkbutton_show_title()
// Whether to show the title.
{
  bool active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_title));
  gtk_widget_set_sensitive(checkbutton_project, active);
  gtk_widget_set_sensitive(checkbutton_category, active);
  gtk_widget_set_sensitive(checkbutton_date_created, active);
  gtk_widget_set_sensitive(checkbutton_created_by, active);
}


NotesSelectionReferenceType ShowNotesDialog::get_reference_selection ()
{
  if (gtk_toggle_button_get_active(reference_get_button(nsrtCurrentVerse)))
    return nsrtCurrentVerse;
  if (gtk_toggle_button_get_active(reference_get_button(nsrtCurrentChapter)))
    return nsrtCurrentChapter;
  if (gtk_toggle_button_get_active(reference_get_button(nsrtCurrentBook)))
    return nsrtCurrentBook;
  if (gtk_toggle_button_get_active(reference_get_button(nsrtAny)))
    return nsrtAny;
  return nsrtCurrentVerse;  
}


NotesSelectionEditedType ShowNotesDialog::get_edited_selection ()
{
  if (gtk_toggle_button_get_active(edited_get_button(nsetToday)))
    return nsetToday;
  if (gtk_toggle_button_get_active(edited_get_button(nsetDateRange)))
    return nsetDateRange;
  if (gtk_toggle_button_get_active(edited_get_button(nsetAny)))
    return nsetAny;
  return nsetToday;
}


void ShowNotesDialog::on_button_clicked (GtkButton *button, gpointer user_data)
{
  ((ShowNotesDialog *) user_data)->restart_timeout();
}


void ShowNotesDialog::on_combobox_changed (GtkComboBox *combobox, gpointer user_data)
{
  ((ShowNotesDialog *) user_data)->restart_timeout();
}


void ShowNotesDialog::restart_timeout ()
{
  gw_destroy_source(event_id);
  event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 100, GSourceFunc(on_timeout), gpointer(this), NULL);
}


bool ShowNotesDialog::on_timeout(gpointer user_data)
{
  ((ShowNotesDialog *) user_data)->timeout();
  return false;
}


void ShowNotesDialog::timeout()
{
  // Clear the event id.
  event_id = 0;

  // Get the parameters from the dialog, not from the Settings object, since these have not yet been stored.
  extern Settings * settings;
  ustring currentreference = books_id_to_english(settings->genconfig.book_get()) + " " + settings->genconfig.chapter_get() + ":" + settings->genconfig.verse_get();
  NotesSelectionReferenceType refselection = get_reference_selection ();
  NotesSelectionEditedType editedselection = get_edited_selection ();
  ustring category = combobox_get_active_string(combobox_category);
  if (category == all_categories())
    category.clear();
  bool currentprojectselection = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_current_project));
  
  // Get notes count.
  vector <unsigned int> ids;
  unsigned int id_cursor;
  notes_select (ids, id_cursor, currentreference, category, refselection, editedselection, currentprojectselection, from_day, to_day);
  
  // Update GUI.
  ustring message = "This selection would display " + convert_to_string (ids.size()) + " ";
  if (ids.size() == 1)
    message.append ("note");
  else
    message.append ("notes");
  gtk_label_set_text (GTK_LABEL (label_result), message.c_str());
}

