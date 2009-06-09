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


ShowNotesDialog::ShowNotesDialog(int dummy)
{
  extern Settings *settings;

  shownotesdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(shownotesdialog), "Show Project Notes");
  gtk_window_set_position(GTK_WINDOW(shownotesdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(shownotesdialog), TRUE);
  // Hints for skipping the pager and the taskbar only cause trouble on some 
  // distributions, like what we had on Suse 9.2, when these hints were set, 
  // the whole dialog went under the main window, and so became invisible.
  // The gtk_window_set_transient_for function gives no benefits to a modal dialog.
  // This was tested, but no difference could be seen in any behaviour.

  dialog_vbox1 = GTK_DIALOG(shownotesdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  label3 = gtk_label_new("Selection");
  gtk_widget_show(label3);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label3, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label3), 0, 0.5);

  hbox6 = gtk_hbox_new(FALSE, 8);
  gtk_widget_show(hbox6);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox6, FALSE, FALSE, 0);

  label5 = gtk_label_new("Notes that have the");
  gtk_widget_show(label5);
  gtk_box_pack_start(GTK_BOX(hbox6), label5, FALSE, FALSE, 0);

  GSList *radiobutton_reference_verse_group = NULL;

  radiobutton_reference_verse = gtk_radio_button_new_with_mnemonic(NULL, "current verse");
  gtk_widget_show(radiobutton_reference_verse);
  gtk_box_pack_start(GTK_BOX(hbox6), radiobutton_reference_verse, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_reference_verse), radiobutton_reference_verse_group);
  radiobutton_reference_verse_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_reference_verse));

  radiobutton_reference_chapter = gtk_radio_button_new_with_mnemonic(NULL, "current chapter");
  gtk_widget_show(radiobutton_reference_chapter);
  gtk_box_pack_start(GTK_BOX(hbox6), radiobutton_reference_chapter, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_reference_chapter), radiobutton_reference_verse_group);
  radiobutton_reference_verse_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_reference_chapter));

  radiobutton_reference_book = gtk_radio_button_new_with_mnemonic(NULL, "current book");
  gtk_widget_show(radiobutton_reference_book);
  gtk_box_pack_start(GTK_BOX(hbox6), radiobutton_reference_book, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_reference_book), radiobutton_reference_verse_group);
  radiobutton_reference_verse_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_reference_book));

  radiobutton_reference_any = gtk_radio_button_new_with_mnemonic(NULL, "any verse");
  gtk_widget_show(radiobutton_reference_any);
  gtk_box_pack_start(GTK_BOX(hbox6), radiobutton_reference_any, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_reference_any), radiobutton_reference_verse_group);
  radiobutton_reference_verse_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_reference_any));

  hbox7 = gtk_hbox_new(FALSE, 10);
  gtk_widget_show(hbox7);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox7, TRUE, TRUE, 0);

  label6 = gtk_label_new("Notes edited");
  gtk_widget_show(label6);
  gtk_box_pack_start(GTK_BOX(hbox7), label6, FALSE, FALSE, 0);

  GSList *radiobutton_date_today_group = NULL;

  radiobutton_date_today = gtk_radio_button_new_with_mnemonic(NULL, "today");
  gtk_widget_show(radiobutton_date_today);
  gtk_box_pack_start(GTK_BOX(hbox7), radiobutton_date_today, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_date_today), radiobutton_date_today_group);
  radiobutton_date_today_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_date_today));

  hbox8 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox8);
  gtk_box_pack_start(GTK_BOX(hbox7), hbox8, FALSE, FALSE, 0);

  radiobutton_date_range = gtk_radio_button_new_with_mnemonic(NULL, "between");
  gtk_widget_show(radiobutton_date_range);
  gtk_box_pack_start(GTK_BOX(hbox8), radiobutton_date_range, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_date_range), radiobutton_date_today_group);
  radiobutton_date_today_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_date_range));

  button_date_from = gtk_button_new_with_mnemonic("");
  gtk_widget_show(button_date_from);
  gtk_box_pack_start(GTK_BOX(hbox8), button_date_from, FALSE, FALSE, 0);

  label4 = gtk_label_new("and");
  gtk_widget_show(label4);
  gtk_box_pack_start(GTK_BOX(hbox8), label4, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label4), 0, 0.5);

  button_date_to = gtk_button_new_with_mnemonic("");
  gtk_widget_show(button_date_to);
  gtk_box_pack_start(GTK_BOX(hbox8), button_date_to, FALSE, FALSE, 0);

  radiobutton_date_any = gtk_radio_button_new_with_mnemonic(NULL, "at any time");
  gtk_widget_show(radiobutton_date_any);
  gtk_box_pack_start(GTK_BOX(hbox7), radiobutton_date_any, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_date_any), radiobutton_date_today_group);
  radiobutton_date_today_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_date_any));

  hbox9 = gtk_hbox_new(FALSE, 6);
  gtk_widget_show(hbox9);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox9, TRUE, TRUE, 0);

  label7 = gtk_label_new("Notes of category");
  gtk_widget_show(label7);
  gtk_box_pack_start(GTK_BOX(hbox9), label7, FALSE, FALSE, 0);

  combobox_category = gtk_combo_box_new_text();
  gtk_widget_show(combobox_category);
  gtk_box_pack_start(GTK_BOX(hbox9), combobox_category, TRUE, TRUE, 0);

  gtk_label_set_mnemonic_widget(GTK_LABEL(label7), combobox_category);

  hbox10 = gtk_hbox_new(FALSE, 6);
  gtk_widget_show(hbox10);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox10, TRUE, TRUE, 0);

  label8 = gtk_label_new("Notes of project");
  gtk_widget_show(label8);
  gtk_box_pack_start(GTK_BOX(hbox10), label8, FALSE, FALSE, 0);

  GSList *radiobutton_project_current_group = NULL;

  radiobutton_project_current = gtk_radio_button_new_with_mnemonic(NULL, "current");
  gtk_widget_show(radiobutton_project_current);
  gtk_box_pack_start(GTK_BOX(hbox10), radiobutton_project_current, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_project_current), radiobutton_project_current_group);
  radiobutton_project_current_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_project_current));

  radiobutton_project_any = gtk_radio_button_new_with_mnemonic(NULL, "any");
  gtk_widget_show(radiobutton_project_any);
  gtk_box_pack_start(GTK_BOX(hbox10), radiobutton_project_any, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_project_any), radiobutton_project_current_group);
  radiobutton_project_current_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_project_any));

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hseparator1, TRUE, TRUE, 0);

  label9 = gtk_label_new("Display");
  gtk_widget_show(label9);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label9, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label9), 0, 0.5);

  hbox12 = gtk_hbox_new(FALSE, 6);
  gtk_widget_show(hbox12);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox12, TRUE, TRUE, 0);

  checkbutton_show_title = gtk_check_button_new_with_mnemonic("Title, including");
  gtk_widget_show(checkbutton_show_title);
  gtk_box_pack_start(GTK_BOX(hbox12), checkbutton_show_title, FALSE, FALSE, 0);

  checkbutton_showproject = gtk_check_button_new_with_mnemonic("project");
  gtk_widget_show(checkbutton_showproject);
  gtk_box_pack_start(GTK_BOX(hbox12), checkbutton_showproject, FALSE, FALSE, 0);

  checkbutton_show_category = gtk_check_button_new_with_mnemonic("category");
  gtk_widget_show(checkbutton_show_category);
  gtk_box_pack_start(GTK_BOX(hbox12), checkbutton_show_category, FALSE, FALSE, 0);

  checkbutton_date_created = gtk_check_button_new_with_mnemonic("date created");
  gtk_widget_show(checkbutton_date_created);
  gtk_box_pack_start(GTK_BOX(hbox12), checkbutton_date_created, FALSE, FALSE, 0);

  checkbutton_show_created_by = gtk_check_button_new_with_mnemonic("created by");
  gtk_widget_show(checkbutton_show_created_by);
  gtk_box_pack_start(GTK_BOX(hbox12), checkbutton_show_created_by, FALSE, FALSE, 0);

  hbox13 = gtk_hbox_new(FALSE, 6);
  gtk_widget_show(hbox13);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox13, TRUE, TRUE, 0);

  label12 = gtk_label_new("Text");
  gtk_widget_show(label12);
  gtk_box_pack_start(GTK_BOX(hbox13), label12, FALSE, FALSE, 0);

  GSList *radiobutton_text_full_group = NULL;

  radiobutton_text_full = gtk_radio_button_new_with_mnemonic(NULL, "full");
  gtk_widget_show(radiobutton_text_full);
  gtk_box_pack_start(GTK_BOX(hbox13), radiobutton_text_full, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_text_full), radiobutton_text_full_group);
  radiobutton_text_full_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_text_full));

  radiobutton_text_summary = gtk_radio_button_new_with_mnemonic(NULL, "summary");
  gtk_widget_show(radiobutton_text_summary);
  gtk_box_pack_start(GTK_BOX(hbox13), radiobutton_text_summary, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_text_summary), radiobutton_text_full_group);
  radiobutton_text_full_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_text_summary));

  checkbutton_show_versetext = gtk_check_button_new_with_mnemonic("add text of the references");
  gtk_widget_show(checkbutton_show_versetext);
  gtk_box_pack_start(GTK_BOX(hbox13), checkbutton_show_versetext, FALSE, FALSE, 0);

  // Set the reference selection.
  GtkToggleButton *refbutton = reference_get_button(settings->genconfig.notes_selection_reference_get());
  if (refbutton)
    gtk_toggle_button_set_active(refbutton, true);

  // Set the edited selection.
  GtkToggleButton *editbutton = edited_get_button(settings->genconfig.notes_selection_edited_get());
  if (editbutton)
    gtk_toggle_button_set_active(editbutton, true);
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
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_project_any), !settings->genconfig.notes_selection_current_project_get());

  // Title and inclusions.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_show_title), settings->session.project_notes_show_title);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_showproject), settings->genconfig.notes_display_project_get());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_show_category), settings->genconfig.notes_display_category_get());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_date_created), settings->genconfig.notes_display_date_created_get());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_show_created_by), settings->genconfig.notes_display_created_by_get());

  // Notes text display.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_text_summary), settings->genconfig.notes_display_summary_get());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_show_versetext), settings->genconfig.notes_display_reference_text_get());

  dialog_action_area1 = GTK_DIALOG(shownotesdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  Shortcuts shortcuts(0);
  new InDialogHelp(shownotesdialog, NULL, &shortcuts, "view_project_notes");
  shortcuts.process();

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(shownotesdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(shownotesdialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) button_date_from, "clicked", G_CALLBACK(on_fromdatebutton_clicked), gpointer(this));
  g_signal_connect((gpointer) button_date_to, "clicked", G_CALLBACK(on_todatebutton_clicked), gpointer(this));
  g_signal_connect((gpointer) checkbutton_show_title, "toggled", G_CALLBACK(on_checkbutton_show_title_toggled), gpointer(this));
  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton1);

  set_gui();
}


ShowNotesDialog::~ShowNotesDialog()
{
  gtk_widget_destroy(shownotesdialog);
}


int ShowNotesDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(shownotesdialog));
}


void ShowNotesDialog::on_fromdatebutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ShowNotesDialog *) user_data)->on_from_date();
}


void ShowNotesDialog::on_todatebutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ShowNotesDialog *) user_data)->on_to_date();
}


void ShowNotesDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
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
  int i;

  // Referenfe selection.
  if (gtk_toggle_button_get_active(reference_get_button(nsrtCurrentVerse)))
    i = nsrtCurrentVerse;
  if (gtk_toggle_button_get_active(reference_get_button(nsrtCurrentChapter)))
    i = nsrtCurrentChapter;
  if (gtk_toggle_button_get_active(reference_get_button(nsrtCurrentBook)))
    i = nsrtCurrentBook;
  if (gtk_toggle_button_get_active(reference_get_button(nsrtAny)))
    i = nsrtAny;
  settings->genconfig.notes_selection_reference_set(i);

  // Edited selection.  
  if (gtk_toggle_button_get_active(edited_get_button(nsetToday)))
    i = nsetToday;
  if (gtk_toggle_button_get_active(edited_get_button(nsetDateRange)))
    i = nsetDateRange;
  if (gtk_toggle_button_get_active(edited_get_button(nsetAny)))
    i = nsetAny;
  settings->genconfig.notes_selection_edited_set(i);
  settings->genconfig.notes_selection_date_from_set(from_day);
  settings->genconfig.notes_selection_date_to_set(to_day);

  // Category.
  ustring category = combobox_get_active_string(combobox_category);
  if (category == all_categories())
    category.clear();
  settings->genconfig.notes_selection_category_set(category);

  // Project selection.
  settings->genconfig.notes_selection_current_project_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_project_current)));

  // Title and inclusions.
  settings->session.project_notes_show_title = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_show_title));
  settings->genconfig.notes_display_project_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_showproject)));
  settings->genconfig.notes_display_category_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_show_category)));
  settings->genconfig.notes_display_date_created_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_date_created)));
  settings->genconfig.notes_display_created_by_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_show_created_by)));

  // Notes text display.
  settings->genconfig.notes_display_summary_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_text_summary)));
  settings->genconfig.notes_display_reference_text_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_show_versetext)));
}


void ShowNotesDialog::set_gui()
{
  // Set the labels on the date buttons.
  gtk_button_set_label(GTK_BUTTON(button_date_from), date_time_julian_human_readable(from_day, true).c_str());
  gtk_button_set_label(GTK_BUTTON(button_date_to), date_time_julian_human_readable(to_day, true).c_str());

  // Rewrite shortcuts.
  Shortcuts shortcuts(0);
  shortcuts.button(radiobutton_reference_verse);
  shortcuts.button(radiobutton_reference_chapter);
  shortcuts.button(radiobutton_reference_book);
  shortcuts.button(radiobutton_reference_any);
  shortcuts.button(radiobutton_date_today);
  shortcuts.button(radiobutton_date_range);
  shortcuts.button(button_date_from);
  shortcuts.button(button_date_to);
  shortcuts.button(radiobutton_date_any);
  shortcuts.label(label7);
  shortcuts.button(radiobutton_project_current);
  shortcuts.button(radiobutton_project_any);
  shortcuts.button(checkbutton_show_title);
  shortcuts.button(checkbutton_showproject);
  shortcuts.button(checkbutton_show_category);
  shortcuts.button(checkbutton_date_created);
  shortcuts.button(checkbutton_show_created_by);
  shortcuts.button(radiobutton_text_full);
  shortcuts.button(radiobutton_text_summary);
  shortcuts.button(checkbutton_show_versetext);
  // Temporal helpbutton for correct processing of the _H.  
  GtkWidget *helpbutton = gtk_button_new_from_stock("gtk-help");
  shortcuts.stockbutton(helpbutton);
  shortcuts.stockbutton(cancelbutton1);
  shortcuts.stockbutton(okbutton1);
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
      button = GTK_TOGGLE_BUTTON(radiobutton_reference_verse);
      break;
    }
  case nsrtCurrentChapter:
    {
      button = GTK_TOGGLE_BUTTON(radiobutton_reference_chapter);
      break;
    }
  case nsrtCurrentBook:
    {
      button = GTK_TOGGLE_BUTTON(radiobutton_reference_book);
      break;
    }
  case nsrtAny:
    {
      button = GTK_TOGGLE_BUTTON(radiobutton_reference_any);
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
      button = GTK_TOGGLE_BUTTON(radiobutton_date_today);
      break;
    }
  case nsetDateRange:
    {
      button = GTK_TOGGLE_BUTTON(radiobutton_date_range);
      break;
    }
  case nsetAny:
    {
      button = GTK_TOGGLE_BUTTON(radiobutton_date_any);
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
  bool active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_show_title));
  gtk_widget_set_sensitive(checkbutton_showproject, active);
  gtk_widget_set_sensitive(checkbutton_show_category, active);
  gtk_widget_set_sensitive(checkbutton_date_created, active);
  gtk_widget_set_sensitive(checkbutton_show_created_by, active);
}


/*


Todo notes display optimizations

1. The reference sorting mechanism should not only take account of the initial reference, but also sort on the final reference. 
E.g. if references are 10-11,, and 10-12,, and 10,, then it should sort them like:

10
10-11
10-12,

and not like:

10-12
10-11...

2. Once this sorting works, then the cursor in the notes should always be placed at the first reference that the editor has. 
If the editor has verse 11, then in the above example, the cursor should be placed on the note that has reference 10-11.











*/

