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
#include "dialogreplace.h"
#include "utilities.h"
#include "bible.h"
#include "dialogselectbooks.h"
#include "search_utils.h"
#include "completion.h"
#include "projectutils.h"
#include "help.h"
#include "shortcuts.h"
#include "settings.h"
#include "tiny_utilities.h"


ReplaceDialog::ReplaceDialog(int dummy)
{
  Shortcuts shortcuts(0);

  extern Settings *settings;

  replacedialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(replacedialog), "Replace");
  gtk_window_set_position(GTK_WINDOW(replacedialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(replacedialog), TRUE);
  gtk_window_set_destroy_with_parent(GTK_WINDOW(replacedialog), TRUE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG(replacedialog));
  gtk_widget_show(dialog_vbox1);

  table1 = gtk_table_new(6, 2, FALSE);
  gtk_widget_show(table1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), table1, TRUE, TRUE, 0);

  label7 = gtk_label_new("Search for");
  gtk_widget_show(label7);
  gtk_table_attach(GTK_TABLE(table1), label7, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 4, 4);
  gtk_misc_set_alignment(GTK_MISC(label7), 1, 0.5);

  shortcuts.label(label7);

  label8 = gtk_label_new("Replace with");
  gtk_widget_show(label8);
  gtk_table_attach(GTK_TABLE(table1), label8, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 4, 4);
  gtk_misc_set_alignment(GTK_MISC(label8), 1, 0.5);

  shortcuts.label(label8);

  entry2 = gtk_entry_new();
  // Manually added code.
  gtk_entry_set_text(GTK_ENTRY(entry2), settings->session.searchword.c_str());
  gtk_widget_show(entry2);
  gtk_table_attach(GTK_TABLE(table1), entry2, 1, 2, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 2);
  gtk_entry_set_activates_default(GTK_ENTRY(entry2), TRUE);

  entry3 = gtk_entry_new();
  // Manually added code.
  gtk_entry_set_text(GTK_ENTRY(entry3), settings->session.replaceword.c_str());
  gtk_widget_show(entry3);
  gtk_table_attach(GTK_TABLE(table1), entry3, 1, 2, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 2);
  gtk_entry_set_activates_default(GTK_ENTRY(entry3), TRUE);

  if (!settings->genconfig.text_editor_font_default_get()) {
    PangoFontDescription *font_desc = NULL;
    font_desc = pango_font_description_from_string(settings->genconfig.text_editor_font_name_get().c_str());
    gtk_widget_modify_font(entry2, font_desc);
    gtk_widget_modify_font(entry3, font_desc);
    pango_font_description_free(font_desc);
  }

  checkbuttoncase = gtk_check_button_new_with_mnemonic("Case sensitive");
  gtk_widget_show(checkbuttoncase);
  gtk_table_attach(GTK_TABLE(table1), checkbuttoncase, 0, 2, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  shortcuts.button(checkbuttoncase);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbuttoncase), settings->session.search_case_sensitive);

  checkbuttonbook = gtk_check_button_new_with_mnemonic("Current book only");
  gtk_widget_show(checkbuttonbook);
  gtk_table_attach(GTK_TABLE(table1), checkbuttonbook, 0, 2, 3, 4, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  shortcuts.button(checkbuttonbook);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbuttonbook), settings->session.search_current_book);

  checkbuttonchapter = gtk_check_button_new_with_mnemonic("Current chapter only");
  gtk_widget_show(checkbuttonchapter);
  gtk_table_attach(GTK_TABLE(table1), checkbuttonchapter, 0, 2, 4, 5, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  shortcuts.button(checkbuttonchapter);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbuttonchapter), settings->session.search_current_chapter);

  selectbutton = gtk_button_new();
  gtk_widget_show(selectbutton);
  gtk_table_attach(GTK_TABLE(table1), selectbutton, 0, 1, 5, 6, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(selectbutton), alignment1);

  hbox1 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox1);
  gtk_container_add(GTK_CONTAINER(alignment1), hbox1);

  image1 = gtk_image_new_from_stock("gtk-properties", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX(hbox1), image1, FALSE, FALSE, 0);

  label9 = gtk_label_new_with_mnemonic("Select books");
  gtk_widget_show(label9);
  gtk_box_pack_start(GTK_BOX(hbox1), label9, FALSE, FALSE, 0);

  shortcuts.label(label9);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(replacedialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(replacedialog, NULL, &shortcuts, NULL);

  buttonfind = gtk_button_new_from_stock("gtk-find");
  gtk_widget_show(buttonfind);
  gtk_dialog_add_action_widget(GTK_DIALOG(replacedialog), buttonfind, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (buttonfind), true);

  buttoncancel = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(buttoncancel);
  gtk_dialog_add_action_widget(GTK_DIALOG(replacedialog), buttoncancel, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (buttoncancel), true);

  shortcuts.stockbutton(buttonfind);
  shortcuts.stockbutton(buttoncancel);
  shortcuts.process();

  g_signal_connect((gpointer) checkbuttonbook, "toggled", G_CALLBACK(on_checkbuttonbook_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbuttonchapter, "toggled", G_CALLBACK(on_checkbuttonchapter_toggled), gpointer(this));
  g_signal_connect((gpointer) selectbutton, "clicked", G_CALLBACK(on_selectbutton_clicked), gpointer(this));
  g_signal_connect((gpointer) buttonfind, "clicked", G_CALLBACK(replacedialog_on_buttonfind_clicked), gpointer(this));
  g_signal_connect((gpointer) entry2, "changed", G_CALLBACK(on_word_entry_changed), gpointer(this));

  gtk_label_set_mnemonic_widget(GTK_LABEL(label7), entry2);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label8), entry3);

  gtk_widget_grab_focus(entry2);
  gtk_widget_grab_default(buttonfind);

  // Select books feature.
  selectable_books = project_get_books(settings->genconfig.project_get());

  // Entry completion
  completion_setup(entry2, cpSearch);
  completion_setup(entry3, cpReplace);

  set_gui();
}


ReplaceDialog::~ReplaceDialog()
{
  gtk_widget_destroy(replacedialog);
}


int ReplaceDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(replacedialog));
}


void ReplaceDialog::on_checkbuttonbook_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((ReplaceDialog *) user_data)->set_gui();
}


void ReplaceDialog::on_checkbuttonchapter_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((ReplaceDialog *) user_data)->set_gui();
}


void ReplaceDialog::replacedialog_on_buttonfind_clicked(GtkButton * button, gpointer user_data)
{
  ((ReplaceDialog *) user_data)->on_buttonfind_clicked();
}


void ReplaceDialog::on_buttonfind_clicked()
{
  extern Settings *settings;

  // Get data from the user interface.
  ustring searchword = gtk_entry_get_text(GTK_ENTRY(entry2));
  settings->session.searchword = searchword;
  ustring replaceword = gtk_entry_get_text(GTK_ENTRY(entry3));
  settings->session.replaceword = replaceword;
  settings->session.search_case_sensitive = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttoncase));
  settings->session.search_current_book = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttonbook));
  settings->session.search_current_chapter = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttonchapter));

  // Save the current book selection in case we modify it.
  set <unsigned int> saved_book_selection = settings->session.selected_books;
  
  // In case we search in the current book only, modify the book selection.
  if (settings->session.search_current_book) {
    settings->session.selected_books.clear();
    settings->session.selected_books.insert(settings->genconfig.book_get());
  }

  // Search.
  unsigned int chapter = convert_to_int(settings->genconfig.chapter_get());
  search_string_basic(settings->genconfig.project_get(), true, chapter, results);
  sort_references(results);
  
  // Restore current book selection.
  settings->session.selected_books = saved_book_selection;
  
  // Highlighting words in editor.
  settings->session.highlights.clear();
  SessionHighlights sessionhighlights1(searchword, settings->session.search_case_sensitive, false, false, false, atRaw, false, false, false, false, false, false, false, false);
  settings->session.highlights.push_back(sessionhighlights1);
  SessionHighlights sessionhighlights2(replaceword, settings->session.search_case_sensitive, false, false, false, atRaw, false, false, false, false, false, false, false, false);
  settings->session.highlights.push_back(sessionhighlights2);

  // Entry completion
  completion_finish(entry2, cpSearch);
  completion_finish(entry3, cpReplace);
}


void ReplaceDialog::on_selectbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ReplaceDialog *) user_data)->on_selectbutton_clicked2();
}


void ReplaceDialog::on_selectbutton_clicked2()
// The user can select which books to search and replace in.
{
  extern Settings *settings;
  SelectBooksDialog dialog(false);
  dialog.selectables(selectable_books);
  dialog.selection_set(settings->session.selected_books);
  if (dialog.run() == GTK_RESPONSE_OK) {
    settings->session.selected_books = dialog.selectionset;
  }
}


void ReplaceDialog::on_word_entry_changed(GtkEditable * editable, gpointer user_data)
{
  ((ReplaceDialog *) user_data)->set_gui();
}


void ReplaceDialog::set_gui()
{
  // Search only works when there's a word to look for.
  ustring searchword = gtk_entry_get_text(GTK_ENTRY(entry2));
  gtk_widget_set_sensitive(buttonfind, !searchword.empty());
  
  // Current book and current chapter selectors.
  bool current_book_active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttonbook));
  if (!current_book_active) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbuttonchapter), false);
  }
  bool current_chapter_active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttonchapter));
  if (current_chapter_active) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbuttonbook), true);
  }
  gtk_widget_set_sensitive (checkbuttonchapter, current_book_active);

  // Sensitivity of book selection.
  gtk_widget_set_sensitive (selectbutton, !(current_book_active || current_chapter_active));
}


