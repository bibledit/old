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
#include "dialogcheck.h"
#include "dialogselectbooks.h"
#include "utilities.h"
#include "bible.h"
#include "dialoglistview.h"
#include "scripturechecks.h"
#include "dialogeditlist.h"
#include "dialogarea.h"
#include "projectutils.h"
#include "directories.h"
#include "checks.h"
#include "gtkwrappers.h"
#include "settings.h"
#include "gui.h"
#include "books.h"


CheckDialog::CheckDialog(CheckDialogType checkdialogtype)
{
  // Save and initialize variables.
  mycheckdialogtype = checkdialogtype;

  checkdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(checkdialog), "Check");
  gtk_window_set_position(GTK_WINDOW(checkdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(checkdialog), TRUE);

  dialog_vbox1 = GTK_DIALOG(checkdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 4);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(vbox1), 4);

  // Initialize some variables in case they're not used.
  radiobutton_sort0 = NULL;
  radiobutton_sort1 = NULL;
  radiobutton_sort2 = NULL;
  radiobutton_sort3 = NULL;
  selectprojectgui = NULL;
  entry_capitalization_punctuation = NULL;
  checkbutton_repetition_case = NULL;
  entry_matching_pairs_ignore = NULL;
  wordcount_checkbutton1 = NULL;
  usfm_spacing_entry1 = NULL;
  checkbutton_include_verse_text = NULL;
  checkbutton_output_in_ot_order = NULL;

  // Build the gui for this check.
  switch (checkdialogtype) {
  case cdtChaptersVerses:
    information_setup("Checks the chapter and verse markers, to see whether they are correct according to the versification of the project.");
    book_selection_setup();
    break;
  case cdtMarkersCount:
    information_setup("Counts the number of times a marker is used.");
    sorting_setup("_No sorting", "Sort on _marker", "_Sort on count", NULL);
    book_selection_setup();
    break;
  case cdtMarkersValidate:
    information_setup("Verifies that the markers are valid.");
    book_selection_setup();
    break;
  case cdtMarkersCompare:
    information_setup("Compares the markers of the current project with the ones in another project. This other project is taken as the standard to compare against.");
    markers_comparison_setup();
    book_selection_setup();
    break;
  case cdtMarkersSpacing:
    information_setup("Checks whether the text that belongs to selected markers has one space after it.");
    markers_spacing_setup();
    book_selection_setup();
    break;
  case cdtCharactersCount:
    information_setup("Counts the number of characters.");
    sorting_setup("_No sorting", "_Sort on character", "Sort on co_unt", NULL);
    book_selection_setup();
    break;
  case cdtWordsCapitalization:
    information_setup("Check capitalization of the text. Looks for missing capitals after selected punctuation marks, and for suspicious use of capitals such as \"hOuse\" or \"HOuse\".");
    words_capitalization_setup();
    book_selection_setup();
    break;
  case cdtWordsRepetition:
    information_setup("Checks for repeating contiguous words.");
    word_repetition_setup();
    book_selection_setup();
    break;
  case cdtMatchingPairs:
    information_setup("Check matching pairs of punctuation. Checks that opening and closing punctuation match, e.g. a [ needs a ]. Also checks that punctuation is closed in the reverse order that it was opened.");
    punctuation_matching_pairs_setup();
    book_selection_setup();
    break;
  case cdtSentenceStructure:
    information_setup("Check punctuation of sentences and headings.");
    book_selection_setup();
    break;
  case cdtUnwantedPatterns:
    information_setup("Check for unwanted patterns of characters.");
    characters_unwanted_patterns_setup();
    book_selection_setup();
    break;
  case cdtWordsUnwanted:
    information_setup("Check for unwanted whole words in the text.");
    unwanted_words_setup();
    book_selection_setup();
    break;
  case cdtWordsCount:
    information_setup("Count the frequency of the words.");
    sorting_setup("_No sorting", "Sort on _word", "_Sort on count", NULL);
    word_count_setup();
    book_selection_setup();
    break;
  case cdtReferencesInventory:
    information_setup("Makes an inventory of the references contained in the text.");
    include_verse_text_setup();
    book_selection_setup();
    break;
  case cdtReferencesValidate:
    information_setup("Validates the references in the text and notes.");
    book_selection_setup();
    break;
  case cdtNTQuotationsFromOT:
    information_setup("Shows the places where the New Testament quotes the Old Testament.");
    include_verse_text_setup();
    output_in_ot_order_setup();
    book_selection_setup();
    break;
  case cdtSynopticParallelsNT:
    information_setup("Shows the synoptic parallel passages of the New Testament.");
    include_verse_text_setup();
    book_selection_setup();
    break;
  case cdtParallelsOT:
    information_setup("Shows the parallel passages of the Old Testament.");
    include_verse_text_setup();
    book_selection_setup();
    break;
  }

  dialog_action_area1 = GTK_DIALOG(checkdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(checkdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(checkdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  // Default / focus on OK.  
  gtk_widget_grab_default(okbutton);
  gtk_widget_grab_focus(okbutton);
 
  // Set relevant books.
  set_relevant_books ();
  
  // Set gui elements.
  set_gui();
}

CheckDialog::~CheckDialog()
{
  if (selectprojectgui)
    delete selectprojectgui;
  gtk_widget_destroy(checkdialog);
}

int CheckDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(checkdialog));
}

void CheckDialog::information_setup(const gchar * information)
{
  infolabel = gtk_label_new(information);
  gtk_widget_show(infolabel);
  gtk_container_add(GTK_CONTAINER(vbox1), infolabel);
  gtk_label_set_line_wrap(GTK_LABEL(infolabel), TRUE);
  gtk_misc_set_alignment(GTK_MISC(infolabel), 0, 0);
}

void CheckDialog::sorting_setup(const gchar * sort0, const gchar * sort1, const gchar * sort2, const gchar * sort3)
{
  GSList *radiobutton_sort_group = NULL;

  if (sort0) {
    radiobutton_sort0 = gtk_radio_button_new_with_mnemonic(NULL, sort0);
    gtk_widget_show(radiobutton_sort0);
    gtk_box_pack_start(GTK_BOX(vbox1), radiobutton_sort0, FALSE, FALSE, 0);
    gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_sort0), radiobutton_sort_group);
    radiobutton_sort_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_sort0));
  }

  if (sort1) {
    radiobutton_sort1 = gtk_radio_button_new_with_mnemonic(NULL, sort1);
    gtk_widget_show(radiobutton_sort1);
    gtk_box_pack_start(GTK_BOX(vbox1), radiobutton_sort1, FALSE, FALSE, 0);
    gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_sort1), radiobutton_sort_group);
    radiobutton_sort_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_sort1));
  }

  if (sort2) {
    radiobutton_sort2 = gtk_radio_button_new_with_mnemonic(NULL, sort2);
    gtk_widget_show(radiobutton_sort2);
    gtk_box_pack_start(GTK_BOX(vbox1), radiobutton_sort2, FALSE, FALSE, 0);
    gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_sort2), radiobutton_sort_group);
    radiobutton_sort_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_sort2));
  }

  if (sort3) {
    radiobutton_sort3 = gtk_radio_button_new_with_mnemonic(NULL, sort3);
    gtk_widget_show(radiobutton_sort3);
    gtk_box_pack_start(GTK_BOX(vbox1), radiobutton_sort3, FALSE, FALSE, 0);
    gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_sort3), radiobutton_sort_group);
    radiobutton_sort_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_sort3));
  }
  // Set value in sorting buttons.
  extern Settings *settings;
  switch (settings->session.checksorttype) {
  case cstSort0:
  default:
    if (sort0)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_sort0), true);
    break;
  case cstSort1:
    if (sort1)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_sort1), true);
    break;
  case cstSort2:
    if (sort2)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_sort2), true);
    break;
  case cstSort3:
    if (sort3)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_sort3), true);
    break;
  }
}

void CheckDialog::book_selection_setup()
{
  hbox1 = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox1, TRUE, TRUE, 0);

  button_books = gtk_button_new();
  gtk_widget_show(button_books);
  gtk_box_pack_start(GTK_BOX(hbox1), button_books, FALSE, FALSE, 0);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(button_books), alignment1);

  hbox2 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox2);
  gtk_container_add(GTK_CONTAINER(alignment1), hbox2);

  image1 = gtk_image_new_from_stock("gtk-properties", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX(hbox2), image1, FALSE, FALSE, 0);

  label5 = gtk_label_new_with_mnemonic("Select _books");
  gtk_widget_show(label5);
  gtk_box_pack_start(GTK_BOX(hbox2), label5, FALSE, FALSE, 0);

  label_book = gtk_label_new("");
  gtk_widget_show(label_book);
  gtk_box_pack_start(GTK_BOX(hbox1), label_book, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_book), 0, 0.5);

  g_signal_connect((gpointer) button_books, "clicked", G_CALLBACK(on_button_books_clicked), gpointer(this));

  extern Settings *settings;
  vector < unsigned int >scripture_books = project_get_books(settings->genconfig.project_get());
  for (unsigned int i = 0; i < scripture_books.size(); i++) {
    selectable_books.push_back(scripture_books[i]);
  }

  set_gui_books();
}

void CheckDialog::on_button_books_clicked(GtkButton * button, gpointer user_data)
{
  ((CheckDialog *) user_data)->on_booksbutton();
}

void CheckDialog::on_booksbutton()
{
  extern Settings *settings;
  SelectBooksDialog dialog(false);
  dialog.selectables(selectable_books);
  dialog.selection_set(settings->session.selected_books);
  if (dialog.run() == GTK_RESPONSE_OK) {
    settings->session.selected_books = dialog.selectionset;
  }
  set_gui_books();
  set_gui();
}

void CheckDialog::set_gui_books()
{
  extern Settings *settings;
  ustring message = gui_book_selection_information(settings->genconfig.project_get());
  gtk_label_set_text(GTK_LABEL(label_book), message.c_str());
}

void CheckDialog::area_selection_setup()
{
  hbox15 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox15);
  gtk_container_add(GTK_CONTAINER(vbox1), hbox15);

  button_area = gtk_button_new();
  gtk_widget_show(button_area);
  gtk_box_pack_start(GTK_BOX(hbox15), button_area, FALSE, FALSE, 0);

  alignment8 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment8);
  gtk_container_add(GTK_CONTAINER(button_area), alignment8);

  hbox16 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox16);
  gtk_container_add(GTK_CONTAINER(alignment8), hbox16);

  image8 = gtk_image_new_from_stock("gtk-zoom-in", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image8);
  gtk_box_pack_start(GTK_BOX(hbox16), image8, FALSE, FALSE, 0);

  label36 = gtk_label_new_with_mnemonic("Select _area");
  gtk_widget_show(label36);
  gtk_box_pack_start(GTK_BOX(hbox16), label36, FALSE, FALSE, 0);

  label_area = gtk_label_new("");
  gtk_widget_show(label_area);
  gtk_box_pack_start(GTK_BOX(hbox15), label_area, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_area), 0, 0.5);

  g_signal_connect((gpointer) button_area, "clicked", G_CALLBACK(on_button_area_clicked), gpointer(this));

  gtk_label_set_text(GTK_LABEL(label_area), area_information().c_str());
}

void CheckDialog::on_button_area_clicked(GtkButton * button, gpointer user_data)
{
  ((CheckDialog *) user_data)->on_area();
}

void CheckDialog::on_area()
{
  AreaDialog dialog(0);
  dialog.run();
  gtk_label_set_text(GTK_LABEL(label_area), area_information().c_str());
}

void CheckDialog::markers_comparison_setup()
{
  extern Settings *settings;

  vbox3 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox3);
  gtk_container_add(GTK_CONTAINER(vbox1), vbox3);

  hseparator2 = gtk_hseparator_new();
  gtk_widget_show(hseparator2);
  gtk_box_pack_start(GTK_BOX(vbox3), hseparator2, TRUE, TRUE, 0);

  // Project selection gui.
  selectprojectgui = new SelectProjectGui(0);
  selectprojectgui->build(vbox3, "Compare _with: ", settings->genconfig.check_markers_compare_project_get());

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(vbox3), hseparator1, TRUE, TRUE, 0);

  checkbutton_compare_all = gtk_check_button_new_with_mnemonic("Compare _all markers");
  gtk_widget_show(checkbutton_compare_all);
  gtk_box_pack_start(GTK_BOX(vbox3), checkbutton_compare_all, FALSE, FALSE, 0);

  // Set whether active.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_compare_all), settings->genconfig.check_markers_compare_all_markers_get());

  label17 = gtk_label_new("Below provide a list of markers to be included in the comparison, separated with spaces and without the backslashes, e.g.: id c v");
  gtk_widget_show(label17);
  gtk_box_pack_start(GTK_BOX(vbox3), label17, FALSE, FALSE, 0);
  gtk_label_set_line_wrap(GTK_LABEL(label17), TRUE);
  gtk_misc_set_alignment(GTK_MISC(label17), 0, 0.5);

  entry_compare_include_only = gtk_entry_new();
  gtk_widget_show(entry_compare_include_only);
  gtk_box_pack_start(GTK_BOX(vbox3), entry_compare_include_only, FALSE, FALSE, 0);
  gtk_entry_set_activates_default(GTK_ENTRY(entry_compare_include_only), TRUE);

  // Set the markers.
  gtk_entry_set_text(GTK_ENTRY(entry_compare_include_only), settings->genconfig.check_markers_compare_include_only_get().c_str());

  hseparator3 = gtk_hseparator_new();
  gtk_widget_show(hseparator3);
  gtk_box_pack_start(GTK_BOX(vbox3), hseparator3, TRUE, TRUE, 0);

  label18 = gtk_label_new_with_mnemonic("_Ignore the following markers:");
  gtk_widget_show(label18);
  gtk_box_pack_start(GTK_BOX(vbox3), label18, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label18), 0, 0.5);

  entry_compare_ignore = gtk_entry_new();
  gtk_widget_show(entry_compare_ignore);
  gtk_box_pack_start(GTK_BOX(vbox3), entry_compare_ignore, FALSE, FALSE, 0);
  gtk_entry_set_activates_default(GTK_ENTRY(entry_compare_ignore), TRUE);

  // Set the markers.
  gtk_entry_set_text(GTK_ENTRY(entry_compare_ignore), settings->genconfig.check_markers_compare_ignore_get().c_str());

  hseparator4 = gtk_hseparator_new();
  gtk_widget_show(hseparator4);
  gtk_box_pack_start(GTK_BOX(vbox3), hseparator4, TRUE, TRUE, 0);

  checkbutton_compare_ignore_verse_zero = gtk_check_button_new_with_mnemonic("Ignore anything before the _first verse");
  gtk_widget_show(checkbutton_compare_ignore_verse_zero);
  gtk_box_pack_start(GTK_BOX(vbox3), checkbutton_compare_ignore_verse_zero, FALSE, FALSE, 0);

  // Set the state.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_compare_ignore_verse_zero), settings->genconfig.check_markers_compare_ignore_verse_zero_get());

  hseparator5 = gtk_hseparator_new();
  gtk_widget_show(hseparator5);
  gtk_box_pack_start(GTK_BOX(vbox3), hseparator5, TRUE, TRUE, 0);

  g_signal_connect_after((gpointer) checkbutton_compare_all, "toggled", G_CALLBACK(on_checkbutton_compare_all_toggled), gpointer(this));

  gtk_label_set_mnemonic_widget(GTK_LABEL(label18), entry_compare_ignore);

  // Set gui.
  on_checkbutton_compare_all();
}

void CheckDialog::markers_spacing_setup()
{
  extern Settings *settings;

  usfm_spacing_hseparator1 = gtk_hseparator_new();
  gtk_widget_show(usfm_spacing_hseparator1);
  gtk_box_pack_start(GTK_BOX(vbox1), usfm_spacing_hseparator1, TRUE, TRUE, 0);

  usfm_spacing_label2 = gtk_label_new_with_mnemonic("_Provide a list of markers to be checked, separated with spaces, and without the backslashes, e.g.: xo xt");
  gtk_widget_show(usfm_spacing_label2);
  gtk_box_pack_start(GTK_BOX(vbox1), usfm_spacing_label2, FALSE, FALSE, 0);
  gtk_label_set_line_wrap(GTK_LABEL(usfm_spacing_label2), TRUE);
  gtk_misc_set_alignment(GTK_MISC(usfm_spacing_label2), 0, 0.5);

  usfm_spacing_entry1 = gtk_entry_new();
  gtk_widget_show(usfm_spacing_entry1);
  gtk_box_pack_start(GTK_BOX(vbox1), usfm_spacing_entry1, FALSE, FALSE, 0);
  gtk_entry_set_invisible_char(GTK_ENTRY(usfm_spacing_entry1), 8226);
  gtk_entry_set_activates_default(GTK_ENTRY(usfm_spacing_entry1), TRUE);

  gtk_label_set_mnemonic_widget(GTK_LABEL(usfm_spacing_label2), usfm_spacing_entry1);

  // Set the markers.
  gtk_entry_set_text(GTK_ENTRY(usfm_spacing_entry1), settings->genconfig.check_markers_spacing_include_get().c_str());
}

void CheckDialog::on_checkbutton_compare_all_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((CheckDialog *) user_data)->on_checkbutton_compare_all();
}

void CheckDialog::on_checkbutton_compare_all()
{
  bool all;
  all = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_compare_all));
  gtk_widget_set_sensitive(label17, !all);
  gtk_widget_set_sensitive(entry_compare_include_only, !all);
  if (!all)
    gtk_widget_grab_focus(entry_compare_include_only);
}

void CheckDialog::characters_unwanted_patterns_setup()
{
  vbox12 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox12);
  gtk_container_add(GTK_CONTAINER(vbox1), vbox12);

  hseparator15 = gtk_hseparator_new();
  gtk_widget_show(hseparator15);
  gtk_box_pack_start(GTK_BOX(vbox12), hseparator15, FALSE, FALSE, 0);

  hbox19 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox19);
  gtk_box_pack_start(GTK_BOX(vbox12), hbox19, FALSE, FALSE, 0);

  label46 = gtk_label_new("Enter a list of sequences of characters that are unwanted.\nOne sequence per line.");
  gtk_widget_show(label46);
  gtk_box_pack_start(GTK_BOX(hbox19), label46, TRUE, TRUE, 0);
  gtk_label_set_line_wrap(GTK_LABEL(label46), TRUE);
  gtk_misc_set_alignment(GTK_MISC(label46), 0, 0.5);

  button_unwanted_patterns = gtk_button_new();
  gtk_widget_show(button_unwanted_patterns);
  gtk_box_pack_start(GTK_BOX(hbox19), button_unwanted_patterns, FALSE, FALSE, 0);

  alignment11 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment11);
  gtk_container_add(GTK_CONTAINER(button_unwanted_patterns), alignment11);

  hbox20 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox20);
  gtk_container_add(GTK_CONTAINER(alignment11), hbox20);

  image11 = gtk_image_new_from_stock("gtk-properties", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image11);
  gtk_box_pack_start(GTK_BOX(hbox20), image11, FALSE, FALSE, 0);

  label47 = gtk_label_new_with_mnemonic("_Patterns");
  gtk_widget_show(label47);
  gtk_box_pack_start(GTK_BOX(hbox20), label47, FALSE, FALSE, 0);

  g_signal_connect((gpointer) button_unwanted_patterns, "clicked", G_CALLBACK(on_button_unwanted_patterns_clicked), gpointer(this));
}

void CheckDialog::on_button_unwanted_patterns_clicked(GtkButton * button, gpointer user_data)
{
  ((CheckDialog *) user_data)->on_button_unwanted_patterns();
}

void CheckDialog::on_button_unwanted_patterns()
{
  extern Settings *settings;
  ReadText rt(checks_unwanted_patterns_get_filename(settings->genconfig.project_get()), true, false);
  EditListDialog dialog(&rt.lines, "Unwanted patterns", "of patterns that are not wanted", true, true, true, true, true, true, false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    write_lines(checks_unwanted_patterns_get_filename(settings->genconfig.project_get()), rt.lines);
  }
}

void CheckDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((CheckDialog *) user_data)->on_okbutton();
}

void CheckDialog::words_capitalization_setup()
{
  extern Settings *settings;

  vbox5 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox5);
  gtk_container_add(GTK_CONTAINER(vbox1), vbox5);

  hseparator6 = gtk_hseparator_new();
  gtk_widget_show(hseparator6);
  gtk_box_pack_start(GTK_BOX(vbox5), hseparator6, TRUE, TRUE, 0);

  label23 = gtk_label_new_with_mnemonic("Enter a list of p_unctuation characters, which must be followed by a capitalized word.");
  gtk_widget_show(label23);
  gtk_box_pack_start(GTK_BOX(vbox5), label23, FALSE, FALSE, 0);
  gtk_label_set_line_wrap(GTK_LABEL(label23), TRUE);
  gtk_misc_set_alignment(GTK_MISC(label23), 0, 0.5);

  hbox5 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox5);
  gtk_box_pack_start(GTK_BOX(vbox5), hbox5, TRUE, TRUE, 0);

  entry_capitalization_punctuation = gtk_entry_new();
  gtk_widget_show(entry_capitalization_punctuation);
  gtk_box_pack_start(GTK_BOX(hbox5), entry_capitalization_punctuation, TRUE, TRUE, 0);
  gtk_entry_set_activates_default(GTK_ENTRY(entry_capitalization_punctuation), TRUE);

  // Enter value.
  gtk_entry_set_text(GTK_ENTRY(entry_capitalization_punctuation), settings->genconfig.check_capitalization_punctuation_get().c_str());

  button_capitalization_punctuation = gtk_button_new();
  gtk_widget_show(button_capitalization_punctuation);
  gtk_box_pack_start(GTK_BOX(hbox5), button_capitalization_punctuation, FALSE, FALSE, 0);

  alignment3 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment3);
  gtk_container_add(GTK_CONTAINER(button_capitalization_punctuation), alignment3);

  hbox6 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox6);
  gtk_container_add(GTK_CONTAINER(alignment3), hbox6);

  image3 = gtk_image_new_from_stock("gtk-home", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image3);
  gtk_box_pack_start(GTK_BOX(hbox6), image3, FALSE, FALSE, 0);

  label24 = gtk_label_new_with_mnemonic("Default");
  gtk_widget_show(label24);
  gtk_box_pack_start(GTK_BOX(hbox6), label24, FALSE, FALSE, 0);

  hseparator7 = gtk_hseparator_new();
  gtk_widget_show(hseparator7);
  gtk_box_pack_start(GTK_BOX(vbox5), hseparator7, TRUE, TRUE, 0);

  label25 = gtk_label_new_with_mnemonic("Enter a list of characters, after which a _lower case letter is acceptable.\nExample: The '>' in the following makes the lowercase \"and\" correct:\n<<I order this one, <Go!> and he goes.>>");
  gtk_widget_show(label25);
  gtk_box_pack_start(GTK_BOX(vbox5), label25, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label25), 0, 0.5);

  hbox7 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox7);
  gtk_box_pack_start(GTK_BOX(vbox5), hbox7, TRUE, TRUE, 0);

  entry_capitalization_lowercase = gtk_entry_new();
  gtk_widget_show(entry_capitalization_lowercase);
  gtk_box_pack_start(GTK_BOX(hbox7), entry_capitalization_lowercase, TRUE, TRUE, 0);
  gtk_entry_set_activates_default(GTK_ENTRY(entry_capitalization_lowercase), TRUE);

  // Enter value.
  gtk_entry_set_text(GTK_ENTRY(entry_capitalization_lowercase), settings->genconfig.check_capitalization_ignore_get().c_str());

  button_capitalization_lowercase = gtk_button_new();
  gtk_widget_show(button_capitalization_lowercase);
  gtk_box_pack_start(GTK_BOX(hbox7), button_capitalization_lowercase, FALSE, FALSE, 0);

  alignment4 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment4);
  gtk_container_add(GTK_CONTAINER(button_capitalization_lowercase), alignment4);

  hbox8 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox8);
  gtk_container_add(GTK_CONTAINER(alignment4), hbox8);

  image4 = gtk_image_new_from_stock("gtk-home", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image4);
  gtk_box_pack_start(GTK_BOX(hbox8), image4, FALSE, FALSE, 0);

  label26 = gtk_label_new_with_mnemonic("Default");
  gtk_widget_show(label26);
  gtk_box_pack_start(GTK_BOX(hbox8), label26, FALSE, FALSE, 0);

  hseparator8 = gtk_hseparator_new();
  gtk_widget_show(hseparator8);
  gtk_box_pack_start(GTK_BOX(vbox5), hseparator8, TRUE, TRUE, 0);

  hbox9 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox9);
  gtk_box_pack_start(GTK_BOX(vbox5), hbox9, FALSE, FALSE, 0);

  label27 = gtk_label_new("Enter a list of abbreviations that prevent errors saying that a period is not followed by an upper case letter.");
  gtk_widget_show(label27);
  gtk_box_pack_start(GTK_BOX(hbox9), label27, TRUE, TRUE, 0);
  gtk_label_set_line_wrap(GTK_LABEL(label27), TRUE);
  gtk_misc_set_alignment(GTK_MISC(label27), 0, 0.5);

  vbox6 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox6);
  gtk_box_pack_start(GTK_BOX(hbox9), vbox6, FALSE, FALSE, 0);

  button_capitalization_abbreviations = gtk_button_new();
  gtk_widget_show(button_capitalization_abbreviations);
  gtk_box_pack_start(GTK_BOX(vbox6), button_capitalization_abbreviations, FALSE, FALSE, 0);

  alignment5 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment5);
  gtk_container_add(GTK_CONTAINER(button_capitalization_abbreviations), alignment5);

  hbox10 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox10);
  gtk_container_add(GTK_CONTAINER(alignment5), hbox10);

  image5 = gtk_image_new_from_stock("gtk-properties", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image5);
  gtk_box_pack_start(GTK_BOX(hbox10), image5, FALSE, FALSE, 0);

  label28 = gtk_label_new_with_mnemonic("_Abbrev.");
  gtk_widget_show(label28);
  gtk_box_pack_start(GTK_BOX(hbox10), label28, FALSE, FALSE, 0);

  hseparator9 = gtk_hseparator_new();
  gtk_widget_show(hseparator9);
  gtk_box_pack_start(GTK_BOX(vbox5), hseparator9, TRUE, TRUE, 0);

  hbox11 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox11);
  gtk_box_pack_start(GTK_BOX(vbox5), hbox11, TRUE, TRUE, 0);

  vbox9 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox9);
  gtk_box_pack_start(GTK_BOX(hbox11), vbox9, TRUE, TRUE, 0);

  label29 = gtk_label_new("Enter a list of prefixes which are allowed to occur uncapitalized before the first capital letter in a word.");
  gtk_widget_show(label29);
  gtk_box_pack_start(GTK_BOX(vbox9), label29, TRUE, TRUE, 0);
  gtk_label_set_line_wrap(GTK_LABEL(label29), TRUE);
  gtk_misc_set_alignment(GTK_MISC(label29), 0, 0.5);

  checkbutton_capitalization_prefixes = gtk_check_button_new_with_mnemonic("Allow an_y prefixes");
  gtk_widget_show(checkbutton_capitalization_prefixes);
  gtk_box_pack_start(GTK_BOX(vbox9), checkbutton_capitalization_prefixes, FALSE, FALSE, 0);

  // Set value
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_capitalization_prefixes), settings->genconfig.check_capitalization_allow_any_prefixes_get());

  vbox7 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox7);
  gtk_box_pack_start(GTK_BOX(hbox11), vbox7, FALSE, FALSE, 0);

  button_capitalization_prefixes = gtk_button_new();
  gtk_widget_show(button_capitalization_prefixes);
  gtk_box_pack_start(GTK_BOX(vbox7), button_capitalization_prefixes, FALSE, FALSE, 0);

  alignment6 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment6);
  gtk_container_add(GTK_CONTAINER(button_capitalization_prefixes), alignment6);

  hbox12 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox12);
  gtk_container_add(GTK_CONTAINER(alignment6), hbox12);

  image6 = gtk_image_new_from_stock("gtk-properties", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image6);
  gtk_box_pack_start(GTK_BOX(hbox12), image6, FALSE, FALSE, 0);

  label30 = gtk_label_new_with_mnemonic("_Prefixes");
  gtk_widget_show(label30);
  gtk_box_pack_start(GTK_BOX(hbox12), label30, FALSE, FALSE, 0);

  hseparator10 = gtk_hseparator_new();
  gtk_widget_show(hseparator10);
  gtk_box_pack_start(GTK_BOX(vbox5), hseparator10, TRUE, TRUE, 0);

  hbox13 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox13);
  gtk_box_pack_start(GTK_BOX(vbox5), hbox13, TRUE, TRUE, 0);

  label31 = gtk_label_new("Enter a list of suffixes which are allowed to occur capitalized at the end of a word.\nExample: If \"seaCan\" is a valid word because the \"Can\" suffix must be capitalized, enter \"Can\" in this list.");
  gtk_widget_show(label31);
  gtk_box_pack_start(GTK_BOX(hbox13), label31, TRUE, TRUE, 0);
  gtk_label_set_line_wrap(GTK_LABEL(label31), TRUE);
  gtk_misc_set_alignment(GTK_MISC(label31), 0, 0.5);

  vbox8 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox8);
  gtk_box_pack_start(GTK_BOX(hbox13), vbox8, FALSE, FALSE, 0);

  button_capitalization_suffixes = gtk_button_new();
  gtk_widget_show(button_capitalization_suffixes);
  gtk_box_pack_start(GTK_BOX(vbox8), button_capitalization_suffixes, FALSE, FALSE, 0);

  alignment7 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment7);
  gtk_container_add(GTK_CONTAINER(button_capitalization_suffixes), alignment7);

  hbox14 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox14);
  gtk_container_add(GTK_CONTAINER(alignment7), hbox14);

  image7 = gtk_image_new_from_stock("gtk-properties", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image7);
  gtk_box_pack_start(GTK_BOX(hbox14), image7, FALSE, FALSE, 0);

  label32 = gtk_label_new_with_mnemonic("_Suffixes");
  gtk_widget_show(label32);
  gtk_box_pack_start(GTK_BOX(hbox14), label32, FALSE, FALSE, 0);

  hseparator11 = gtk_hseparator_new();
  gtk_widget_show(hseparator11);
  gtk_box_pack_start(GTK_BOX(vbox5), hseparator11, TRUE, TRUE, 0);

  g_signal_connect((gpointer) button_capitalization_punctuation, "clicked", G_CALLBACK(on_button_capitalization_punctuation_clicked), gpointer(this));
  g_signal_connect((gpointer) button_capitalization_lowercase, "clicked", G_CALLBACK(on_button_capitalization_lowercase_clicked), gpointer(this));
  g_signal_connect((gpointer) button_capitalization_abbreviations, "clicked", G_CALLBACK(on_button_capitalization_abbreviations_clicked), gpointer(this));
  g_signal_connect((gpointer) checkbutton_capitalization_prefixes, "toggled", G_CALLBACK(on_checkbutton_capitalization_prefixes_toggled), gpointer(this));
  g_signal_connect((gpointer) button_capitalization_prefixes, "clicked", G_CALLBACK(on_button_capitalization_prefixes_clicked), gpointer(this));
  g_signal_connect((gpointer) button_capitalization_suffixes, "clicked", G_CALLBACK(on_button_capitalization_suffixes_clicked), gpointer(this));

  on_checkbutton_prefixes();

  gtk_label_set_mnemonic_widget(GTK_LABEL(label23), entry_capitalization_punctuation);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label25), entry_capitalization_lowercase);
}

void CheckDialog::on_button_capitalization_punctuation_clicked(GtkButton * button, gpointer user_data)
{
  ((CheckDialog *) user_data)->capitalization_punctuation_default();
}

void CheckDialog::capitalization_punctuation_default()
{
  gtk_entry_set_text(GTK_ENTRY(entry_capitalization_punctuation), CAPITALIZATION_PUNCTUATION);
}

void CheckDialog::on_button_capitalization_lowercase_clicked(GtkButton * button, gpointer user_data)
{
  ((CheckDialog *) user_data)->capitalization_lowercase_default();
}

void CheckDialog::capitalization_lowercase_default()
{
  gtk_entry_set_text(GTK_ENTRY(entry_capitalization_lowercase), CAPITALIZATION_LOWERCASE);
}

void CheckDialog::on_button_capitalization_abbreviations_clicked(GtkButton * button, gpointer user_data)
{
  ((CheckDialog *) user_data)->capitalization_abbreviations_edit();
}

void CheckDialog::capitalization_abbreviations_edit()
{
  extern Settings *settings;
  ReadText rt(checks_abbreviations_get_filename(settings->genconfig.project_get()), true);
  EditListDialog dialog(&rt.lines, "Abbreviations", "of abbreviations", true, true, true, true, true, true, false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    write_lines(checks_abbreviations_get_filename(settings->genconfig.project_get()), rt.lines);
  }
}

void CheckDialog::on_checkbutton_capitalization_prefixes_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((CheckDialog *) user_data)->on_checkbutton_prefixes();
}

void CheckDialog::on_checkbutton_prefixes()
{
  // If ticked, the user cannot edit the prefixes anymore.
  gtk_widget_set_sensitive(button_capitalization_prefixes, !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_capitalization_prefixes)));
}

void CheckDialog::on_button_capitalization_prefixes_clicked(GtkButton * button, gpointer user_data)
{
  ((CheckDialog *) user_data)->button_capitalization_prefixes_edit();
}

void CheckDialog::button_capitalization_prefixes_edit()
{
  extern Settings *settings;
  ReadText rt(checks_uncapitalized_prefixes_get_filename(settings->genconfig.project_get()), true);
  EditListDialog dialog(&rt.lines, "Uncapitalized prefixes", "of uncapitalized prefixes", true, true, true, true, true, true, false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    write_lines(checks_uncapitalized_prefixes_get_filename(settings->genconfig.project_get()), rt.lines);
  }
}

void CheckDialog::on_button_capitalization_suffixes_clicked(GtkButton * button, gpointer user_data)
{
  ((CheckDialog *) user_data)->capitalization_suffixes_edit();
}

void CheckDialog::capitalization_suffixes_edit()
{
  extern Settings *settings;
  ReadText rt(checks_capitalized_suffixes_get_filename(settings->genconfig.project_get()), true);
  EditListDialog dialog(&rt.lines, "Capitalized suffixes", "of capitalized suffixes", true, true, true, true, true, true, false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    write_lines(checks_capitalized_suffixes_get_filename(settings->genconfig.project_get()), rt.lines);
  }
}

void CheckDialog::word_repetition_setup()
{
  extern Settings *settings;

  vbox10 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox10);
  gtk_container_add(GTK_CONTAINER(vbox1), vbox10);

  hseparator12 = gtk_hseparator_new();
  gtk_widget_show(hseparator12);
  gtk_box_pack_start(GTK_BOX(vbox10), hseparator12, FALSE, FALSE, 0);

  checkbutton_repetition_case = gtk_check_button_new_with_mnemonic("C_ase insensitive");
  gtk_widget_show(checkbutton_repetition_case);
  gtk_box_pack_start(GTK_BOX(vbox10), checkbutton_repetition_case, FALSE, FALSE, 0);

  // Set value.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_repetition_case), settings->genconfig.check_repetition_ignore_case_get());

  hseparator13 = gtk_hseparator_new();
  gtk_widget_show(hseparator13);
  gtk_box_pack_start(GTK_BOX(vbox10), hseparator13, FALSE, FALSE, 0);

  table1 = gtk_table_new(2, 2, FALSE);
  gtk_widget_show(table1);
  gtk_box_pack_start(GTK_BOX(vbox10), table1, FALSE, FALSE, 0);

  button_repetition_show = gtk_button_new();
  gtk_widget_show(button_repetition_show);
  gtk_table_attach(GTK_TABLE(table1), button_repetition_show, 1, 2, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment9 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment9);
  gtk_container_add(GTK_CONTAINER(button_repetition_show), alignment9);

  hbox17 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox17);
  gtk_container_add(GTK_CONTAINER(alignment9), hbox17);

  image9 = gtk_image_new_from_stock("gtk-properties", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image9);
  gtk_box_pack_start(GTK_BOX(hbox17), image9, FALSE, FALSE, 0);

  label39 = gtk_label_new_with_mnemonic("S_how");
  gtk_widget_show(label39);
  gtk_box_pack_start(GTK_BOX(hbox17), label39, FALSE, FALSE, 0);

  checkbutton_repetition_show = gtk_check_button_new_with_mnemonic("Only _show repeating words in this list ->");
  gtk_widget_show(checkbutton_repetition_show);
  gtk_table_attach(GTK_TABLE(table1), checkbutton_repetition_show, 0, 1, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  // Set state.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_repetition_show), settings->genconfig.check_repetition_show_only_these_get());

  checkbutton_repetition_ignore = gtk_check_button_new_with_mnemonic("_Ignore repeating words given in this list ->\nor repeating word pairs that have texts in between them from this list ->");
  gtk_widget_show(checkbutton_repetition_ignore);
  gtk_table_attach(GTK_TABLE(table1), checkbutton_repetition_ignore, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  // Set state
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_repetition_ignore), settings->genconfig.check_repetition_ignore_these_get());

  button_repetition_ignore = gtk_button_new();
  gtk_widget_show(button_repetition_ignore);
  gtk_table_attach(GTK_TABLE(table1), button_repetition_ignore, 1, 2, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment10 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment10);
  gtk_container_add(GTK_CONTAINER(button_repetition_ignore), alignment10);

  hbox18 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox18);
  gtk_container_add(GTK_CONTAINER(alignment10), hbox18);

  image10 = gtk_image_new_from_stock("gtk-properties", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image10);
  gtk_box_pack_start(GTK_BOX(hbox18), image10, FALSE, FALSE, 0);

  label40 = gtk_label_new_with_mnemonic("I_gnore");
  gtk_widget_show(label40);
  gtk_box_pack_start(GTK_BOX(hbox18), label40, FALSE, FALSE, 0);

  g_signal_connect((gpointer) button_repetition_show, "clicked", G_CALLBACK(on_button_repetition_show_clicked), gpointer(this));
  g_signal_connect((gpointer) checkbutton_repetition_show, "toggled", G_CALLBACK(on_checkbutton_repetition_show_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_repetition_ignore, "toggled", G_CALLBACK(on_checkbutton_repetition_ignore_toggled), gpointer(this));
  g_signal_connect((gpointer) button_repetition_ignore, "clicked", G_CALLBACK(on_button_repetition_ignore_clicked), gpointer(this));
}

void CheckDialog::on_checkbutton_repetition_show_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((CheckDialog *) user_data)->on_checkbutton_repetition_show();
}

void CheckDialog::on_checkbutton_repetition_show()
{
  set_gui();
}

void CheckDialog::on_button_repetition_show_clicked(GtkButton * button, gpointer user_data)
{
  ((CheckDialog *) user_data)->on_button_repetition_show();
}

void CheckDialog::on_button_repetition_show()
{
  extern Settings *settings;
  ReadText rt(checks_repetition_show_only_get_filename(settings->genconfig.project_get()), true);
  EditListDialog dialog(&rt.lines, "Repeating words", "of contiguous repeating words to show only in the report", true, true, true, true, true, true, false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    write_lines(checks_repetition_show_only_get_filename(settings->genconfig.project_get()), rt.lines);
  }
}

void CheckDialog::on_checkbutton_repetition_ignore_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((CheckDialog *) user_data)->on_checkbutton_repetition_ignore();
}

void CheckDialog::on_checkbutton_repetition_ignore()
{
  set_gui();
}

void CheckDialog::on_button_repetition_ignore_clicked(GtkButton * button, gpointer user_data)
{
  ((CheckDialog *) user_data)->on_button_repetition_ignore();
}

void CheckDialog::on_button_repetition_ignore()
{
  extern Settings *settings;
  ReadText rt(checks_repetition_ignore_get_filename(settings->genconfig.project_get()), true);
  EditListDialog dialog(&rt.lines, "Repeating words", "of contiguous repeating words to ignore or text between these pairs of words", true, true, true, true, true, true, false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    write_lines(checks_repetition_ignore_get_filename(settings->genconfig.project_get()), rt.lines);
  }
}

void CheckDialog::unwanted_words_setup()
{
  vbox13 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox13);
  gtk_container_add(GTK_CONTAINER(vbox1), vbox13);

  hseparator16 = gtk_hseparator_new();
  gtk_widget_show(hseparator16);
  gtk_box_pack_start(GTK_BOX(vbox13), hseparator16, FALSE, FALSE, 0);

  hbox21 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox21);
  gtk_box_pack_start(GTK_BOX(vbox13), hbox21, FALSE, FALSE, 0);

  label50 = gtk_label_new("Enter a list of whole words that are unwanted in the text.\nOne word per line.");
  gtk_widget_show(label50);
  gtk_box_pack_start(GTK_BOX(hbox21), label50, TRUE, TRUE, 0);
  gtk_misc_set_alignment(GTK_MISC(label50), 0, 0.5);

  button_unwanted_words = gtk_button_new();
  gtk_widget_show(button_unwanted_words);
  gtk_box_pack_start(GTK_BOX(hbox21), button_unwanted_words, FALSE, FALSE, 0);

  alignment12 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment12);
  gtk_container_add(GTK_CONTAINER(button_unwanted_words), alignment12);

  hbox22 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox22);
  gtk_container_add(GTK_CONTAINER(alignment12), hbox22);

  image12 = gtk_image_new_from_stock("gtk-properties", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image12);
  gtk_box_pack_start(GTK_BOX(hbox22), image12, FALSE, FALSE, 0);

  label51 = gtk_label_new_with_mnemonic("_Words");
  gtk_widget_show(label51);
  gtk_box_pack_start(GTK_BOX(hbox22), label51, FALSE, FALSE, 0);

  g_signal_connect((gpointer) button_unwanted_words, "clicked", G_CALLBACK(on_button_unwanted_words_clicked), gpointer(this));
}

void CheckDialog::on_button_unwanted_words_clicked(GtkButton * button, gpointer user_data)
{
  ((CheckDialog *) user_data)->on_button_unwanted_words();
}

void CheckDialog::on_button_unwanted_words()
{
  extern Settings *settings;
  ReadText rt(checks_unwanted_words_get_filename(settings->genconfig.project_get()), true);
  EditListDialog dialog(&rt.lines, "Unwanted words", "of whole words that are not wanted", true, true, true, true, true, true, false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    write_lines(checks_unwanted_words_get_filename(settings->genconfig.project_get()), rt.lines);
  }
}

void CheckDialog::punctuation_matching_pairs_setup()
{
  extern Settings *settings;

  vbox11 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox11);
  gtk_container_add(GTK_CONTAINER(vbox1), vbox11);

  hseparator14 = gtk_hseparator_new();
  gtk_widget_show(hseparator14);
  gtk_box_pack_start(GTK_BOX(vbox11), hseparator14, FALSE, FALSE, 0);

  label43 = gtk_label_new_with_mnemonic("Enter a list of punctuation characters to be _ignored.\nEnter them without spaces.\nOnly one of a pair is needed, the matching one is added automatically.");
  gtk_widget_show(label43);
  gtk_box_pack_start(GTK_BOX(vbox11), label43, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label43), 0, 0.5);

  entry_matching_pairs_ignore = gtk_entry_new();
  gtk_widget_show(entry_matching_pairs_ignore);
  gtk_box_pack_start(GTK_BOX(vbox11), entry_matching_pairs_ignore, FALSE, FALSE, 0);
  gtk_entry_set_activates_default(GTK_ENTRY(entry_matching_pairs_ignore), TRUE);

  // Set value.
  gtk_entry_set_text(GTK_ENTRY(entry_matching_pairs_ignore), settings->genconfig.check_matching_pairs_ignore_get().c_str());

  gtk_label_set_mnemonic_widget(GTK_LABEL(label43), entry_matching_pairs_ignore);
}

void CheckDialog::set_gui()
// Update the GUI.
{
  extern Settings *settings;
  bool sensitive = settings->session.selected_books.size();
  gtk_widget_set_sensitive(okbutton, sensitive);
  if (checkbutton_repetition_case) {
    gtk_widget_set_sensitive(button_repetition_show, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_repetition_show)));
    gtk_widget_set_sensitive(button_repetition_ignore, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_repetition_ignore)));
  }
}

void CheckDialog::word_count_setup()
{
  extern Settings *settings;

  wordcount_hbox1 = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(wordcount_hbox1);
  gtk_box_pack_start(GTK_BOX(vbox1), wordcount_hbox1, TRUE, TRUE, 0);

  wordcount_checkbutton1 = gtk_check_button_new_with_mnemonic("Do not _include words with a count of");
  gtk_widget_show(wordcount_checkbutton1);
  gtk_box_pack_start(GTK_BOX(wordcount_hbox1), wordcount_checkbutton1, FALSE, FALSE, 0);

  wordcount_spinbutton1_adj = gtk_adjustment_new(0, 0, 1000000, 1, 10, 0);
  wordcount_spinbutton1 = gtk_spin_button_new(GTK_ADJUSTMENT(wordcount_spinbutton1_adj), 1, 0);
  gtk_widget_show(wordcount_spinbutton1);
  gtk_box_pack_start(GTK_BOX(wordcount_hbox1), wordcount_spinbutton1, FALSE, FALSE, 0);

  wordcount_label2 = gtk_label_new("and up");
  gtk_widget_show(wordcount_label2);
  gtk_box_pack_start(GTK_BOX(wordcount_hbox1), wordcount_label2, FALSE, FALSE, 0);

  wordcount_label1 = gtk_label_new_with_mnemonic("Bibledit-Gtk normally knows the right word boundaries, but if this is not done satisfactorily, you can enter _extra characters that should be part of a word");
  gtk_widget_show(wordcount_label1);
  gtk_box_pack_start(GTK_BOX(vbox1), wordcount_label1, FALSE, FALSE, 0);
  gtk_label_set_line_wrap(GTK_LABEL(wordcount_label1), TRUE);
  gtk_misc_set_alignment(GTK_MISC(wordcount_label1), 0, 0.5);

  wordcount_entry1 = gtk_entry_new();
  gtk_widget_show(wordcount_entry1);
  gtk_box_pack_start(GTK_BOX(vbox1), wordcount_entry1, FALSE, FALSE, 0);

  g_signal_connect_after((gpointer) wordcount_checkbutton1, "clicked", G_CALLBACK(on_wordcount_checkbutton1_activate), gpointer(this));
  g_signal_connect((gpointer) wordcount_entry1, "changed", G_CALLBACK(on_wordcount_entry1_changed), gpointer(this));

  gtk_label_set_mnemonic_widget(GTK_LABEL(wordcount_label1), wordcount_entry1);

  // Set GUI.
  unsigned int count = settings->genconfig.check_words_inventory_not_include_words_count_get();
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(wordcount_spinbutton1), count);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wordcount_checkbutton1), count > 0);
  gtk_entry_set_text(GTK_ENTRY(wordcount_entry1), settings->genconfig.check_words_inventory_word_forming_characters_get().c_str());
  on_wordcount_checkbutton1();
  on_wordcount_entry1();
}

void CheckDialog::on_wordcount_checkbutton1_activate(GtkButton * button, gpointer user_data)
{
  ((CheckDialog *) user_data)->on_wordcount_checkbutton1();
}

void CheckDialog::on_wordcount_checkbutton1()
{
  bool active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wordcount_checkbutton1));
  gtk_widget_set_sensitive(wordcount_spinbutton1, active);
  if (active)
    gtk_widget_grab_focus(wordcount_spinbutton1);
}

void CheckDialog::on_wordcount_entry1_changed(GtkEditable * editable, gpointer user_data)
{
  ((CheckDialog *) user_data)->on_wordcount_entry1();
}

void CheckDialog::on_wordcount_entry1()
{
  ustring chars = gtk_entry_get_text(GTK_ENTRY(wordcount_entry1));
  ustring non_alphas = get_non_alphanumericals(chars);
  if (chars != non_alphas) {
    gtk_entry_set_text(GTK_ENTRY(wordcount_entry1), non_alphas.c_str());
    gtkw_dialog_info(checkdialog, "Bibledit-Gtk already recognizes this character as part of a word.\nIt has been removed as superfluous.\nThere is no need to enter your alphabet.\nEntering the special cases is enough.");
  }
}

void CheckDialog::include_verse_text_setup()
{
  checkbutton_include_verse_text = gtk_check_button_new_with_mnemonic("_Include verse text");
  gtk_widget_show(checkbutton_include_verse_text);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_include_verse_text, TRUE, TRUE, 0);

  extern Settings *settings;
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_include_verse_text), settings->session.check_include_verse_text);
}

void CheckDialog::output_in_ot_order_setup()
{
  checkbutton_output_in_ot_order = gtk_check_button_new_with_mnemonic("O_utput in Old Testament order");
  gtk_widget_show(checkbutton_output_in_ot_order);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_output_in_ot_order, TRUE, TRUE, 0);

  extern Settings *settings;
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_output_in_ot_order), settings->session.check_output_in_ot_order);
}

void CheckDialog::on_okbutton()
{
  // Save sorting mechanism.
  extern Settings *settings;
  if (radiobutton_sort0)
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_sort0)))
      settings->session.checksorttype = cstSort0;

  if (radiobutton_sort1)
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_sort1)))
      settings->session.checksorttype = cstSort1;

  if (radiobutton_sort2)
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_sort2)))
      settings->session.checksorttype = cstSort2;

  if (radiobutton_sort3)
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_sort3)))
      settings->session.checksorttype = cstSort3;

  // Compare markers settings.
  if (selectprojectgui) {
    settings->genconfig.check_markers_compare_project_set(selectprojectgui->project);
    settings->genconfig.check_markers_compare_all_markers_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_compare_all)));
    settings->genconfig.check_markers_compare_include_only_set(gtk_entry_get_text(GTK_ENTRY(entry_compare_include_only)));
    settings->genconfig.check_markers_compare_ignore_set(gtk_entry_get_text(GTK_ENTRY(entry_compare_ignore)));
    settings->genconfig.check_markers_compare_ignore_verse_zero_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_compare_ignore_verse_zero)));
  }
  // Words - capitalization.
  if (entry_capitalization_punctuation) {
    settings->genconfig.check_capitalization_punctuation_set(remove_spaces(gtk_entry_get_text(GTK_ENTRY(entry_capitalization_punctuation))));
    settings->genconfig.check_capitalization_ignore_set(remove_spaces(gtk_entry_get_text(GTK_ENTRY(entry_capitalization_lowercase))));
    settings->genconfig.check_capitalization_allow_any_prefixes_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_capitalization_prefixes)));
  }
  // Words - repetition
  if (checkbutton_repetition_case) {
    settings->genconfig.check_repetition_ignore_case_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_repetition_case)));
    settings->genconfig.check_repetition_show_only_these_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_repetition_show)));
    settings->genconfig.check_repetition_ignore_these_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_repetition_ignore)));
  }
  // Punctuation - matching pairs.
  if (entry_matching_pairs_ignore) {
    settings->genconfig.check_matching_pairs_ignore_set(gtk_entry_get_text(GTK_ENTRY(entry_matching_pairs_ignore)));
  }
  // Words - Inventory / Count.
  if (wordcount_checkbutton1) {
    int count = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(wordcount_spinbutton1));
    if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wordcount_checkbutton1)))
      count = 0;
    settings->genconfig.check_words_inventory_not_include_words_count_set(count);
    settings->genconfig.check_words_inventory_word_forming_characters_set(gtk_entry_get_text(GTK_ENTRY(wordcount_entry1)));
  }
  // Markers - Spacing.
  if (usfm_spacing_entry1) {
    settings->genconfig.check_markers_spacing_include_set(gtk_entry_get_text(GTK_ENTRY(usfm_spacing_entry1)));
  }
  // Include verse text.
  if (checkbutton_include_verse_text) {
    settings->session.check_include_verse_text = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_include_verse_text));
  }
  // Output in OT order.
  if (checkbutton_output_in_ot_order) {
    settings->session.check_output_in_ot_order = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_output_in_ot_order));
  }
}


void CheckDialog::set_relevant_books ()
{
  // Create a list of relevant books.
  vector <unsigned int> relevant_list;
  if (mycheckdialogtype == cdtSynopticParallelsNT) {
    vector <unsigned int> ntbooks = books_type_to_ids(btNewTestament);
    for (unsigned int i = 0; i < 4; i++) {
      relevant_list.push_back (ntbooks[i]);
    }  
  }
  else if (mycheckdialogtype == cdtParallelsOT) {
    relevant_list = books_type_to_ids(btOldTestament);
  }
  else {
    // No relevant books to set: bail out.
    return;
  }
  set <unsigned int> relevant_set (relevant_list.begin(), relevant_list.end());

  // If the books in the current selection are all within the relevant list, bail out.
  extern Settings *settings;
  vector <unsigned int> current_selection (settings->session.selected_books.begin(), settings->session.selected_books.end());
  bool all_books_are_relevant = true;
  for (unsigned int i = 0; i < current_selection.size(); i++) {
    if (relevant_set.find (current_selection[i]) == relevant_set.end())
      all_books_are_relevant = false;
  }
  if (all_books_are_relevant)
    return;
 
  // Set the relevant list.
  settings->session.selected_books = relevant_set;
  
  // Update the GUI.
  set_gui_books();
}


