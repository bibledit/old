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
#include "dialogsearchspecial.h"
#include "dialogselectbooks.h"
#include "completion.h"
#include "combobox.h"
#include <gdk/gdkkeysyms.h>
#include "keyboard.h"
#include "bible.h"
#include "dialogarea.h"
#include "projectutils.h"
#include "help.h"
#include "settings.h"
#include "gui.h"


#define RESULTS_LOAD   "Load them in the references window"
#define RESULTS_ADD    "Add them to the ones already in the references window"
#define RESULTS_REMOVE "Remove them from the ones already in the references window"
#define RESULTS_SHARE  "Share them with the ones already in the references window"


SearchSpecialDialog::SearchSpecialDialog(int dummy)
{
  extern Settings *settings;

  accel_group = gtk_accel_group_new();

  searchspecialdialog = gtk_dialog_new();
  gtk_widget_add_accelerator(searchspecialdialog, "activate_focus", accel_group, GDK_F, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_window_set_title(GTK_WINDOW(searchspecialdialog), "Find");
  gtk_window_set_position(GTK_WINDOW(searchspecialdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(searchspecialdialog), TRUE);
  gtk_window_set_type_hint(GTK_WINDOW(searchspecialdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG(searchspecialdialog));
  gtk_widget_show(dialog_vbox1);

  vbox3 = gtk_vbox_new(FALSE, 4);
  gtk_widget_show(vbox3);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox3, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(vbox3), 4);

  notebook_mode = gtk_notebook_new();
  gtk_widget_show(notebook_mode);
  gtk_box_pack_start(GTK_BOX(vbox3), notebook_mode, TRUE, TRUE, 0);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook_mode), FALSE);

  label17 = gtk_label_new("Press Control-F for Advanced search");
  gtk_widget_show(label17);
  gtk_container_add(GTK_CONTAINER(notebook_mode), label17);
  gtk_misc_set_alignment(GTK_MISC(label17), 0.5, 1);

  label19 = gtk_label_new("Basic");
  gtk_widget_show(label19);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_mode), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_mode), 0), label19);

  label22 = gtk_label_new("Press Control-F for Basic search");
  gtk_widget_show(label22);
  gtk_container_add(GTK_CONTAINER(notebook_mode), label22);

  label20 = gtk_label_new("Advanced");
  gtk_widget_show(label20);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_mode), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_mode), 1), label20);

  hbox1 = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(vbox3), hbox1, TRUE, TRUE, 0);

  label_word_entry = gtk_label_new_with_mnemonic("_Text to search for:");
  gtk_widget_show(label_word_entry);
  gtk_box_pack_start(GTK_BOX(hbox1), label_word_entry, FALSE, FALSE, 0);

  word_entry = gtk_entry_new();
  gtk_widget_show(word_entry);
  gtk_box_pack_start(GTK_BOX(hbox1), word_entry, TRUE, TRUE, 0);

  if (!settings->genconfig.text_editor_font_default_get()) {
    PangoFontDescription *font_desc = NULL;
    font_desc = pango_font_description_from_string(settings->genconfig.text_editor_font_name_get().c_str());
    gtk_widget_modify_font(word_entry, font_desc);
    pango_font_description_free(font_desc);
  }

  // Set search text.
  gtk_entry_set_text(GTK_ENTRY(word_entry), settings->session.searchword.c_str());

  // Activation of dialog.
  allow_activate = true;

  notebook_case_current = gtk_notebook_new();
  gtk_widget_show(notebook_case_current);
  gtk_box_pack_start(GTK_BOX(vbox3), notebook_case_current, TRUE, TRUE, 0);
  gtk_widget_set_can_focus (notebook_case_current, false);
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook_case_current), FALSE);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook_case_current), FALSE);

  vbox6 = gtk_vbox_new(FALSE, 4);
  gtk_widget_show(vbox6);
  gtk_container_add(GTK_CONTAINER(notebook_case_current), vbox6);

  checkbutton_casesensitive = gtk_check_button_new_with_mnemonic("C_ase sensitive");
  gtk_widget_show(checkbutton_casesensitive);
  gtk_box_pack_start(GTK_BOX(vbox6), checkbutton_casesensitive, FALSE, FALSE, 0);

  // Set case sensitive.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_casesensitive), settings->session.search_case_sensitive);

  checkbutton_current_book = gtk_check_button_new_with_mnemonic("Search in c_urrent book only");
  gtk_widget_show(checkbutton_current_book);
  gtk_box_pack_start(GTK_BOX(vbox6), checkbutton_current_book, FALSE, FALSE, 0);

  // Set searching in current book.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_current_book), settings->session.search_current_book);

  checkbutton_current_chapter = gtk_check_button_new_with_mnemonic("Search in current cha_pter only");
  gtk_widget_show(checkbutton_current_chapter);
  gtk_box_pack_start(GTK_BOX(vbox6), checkbutton_current_chapter, FALSE, FALSE, 0);

  // Set searching in current chapter.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_current_chapter), settings->session.search_current_chapter);

  label27 = gtk_label_new("");
  gtk_widget_show(label27);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_case_current), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_case_current), 0), label27);

  empty_notebook_page = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(empty_notebook_page);
  gtk_container_add(GTK_CONTAINER(notebook_case_current), empty_notebook_page);

  label28 = gtk_label_new("");
  gtk_widget_show(label28);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_case_current), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_case_current), 1), label28);

  empty_notebook_page = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(empty_notebook_page);
  gtk_container_add(GTK_CONTAINER(notebook_case_current), empty_notebook_page);

  label29 = gtk_label_new("");
  gtk_widget_show(label29);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_case_current), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_case_current), 2), label29);

  empty_notebook_page = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(empty_notebook_page);
  gtk_container_add(GTK_CONTAINER(notebook_case_current), empty_notebook_page);

  label30 = gtk_label_new("");
  gtk_widget_show(label30);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_case_current), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_case_current), 3), label30);

  notebook_advanced = gtk_notebook_new();
  gtk_widget_show(notebook_advanced);
  gtk_box_pack_start(GTK_BOX(vbox3), notebook_advanced, TRUE, TRUE, 0);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook_advanced), FALSE);

  // No tabs visible.
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook_advanced), FALSE);

  vbox5 = gtk_vbox_new(FALSE, 4);
  gtk_widget_show(vbox5);
  gtk_container_add(GTK_CONTAINER(notebook_advanced), vbox5);

  checkbutton_glob = gtk_check_button_new_with_mnemonic("_Interprete * and ? as wildcards");
  gtk_widget_show(checkbutton_glob);
  gtk_box_pack_start(GTK_BOX(vbox5), checkbutton_glob, FALSE, FALSE, 0);

  // Set active or not.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_glob), settings->session.search_globbing);

  checkbutton_start_word = gtk_check_button_new_with_mnemonic("Match _start of word");
  gtk_widget_show(checkbutton_start_word);
  gtk_box_pack_start(GTK_BOX(vbox5), checkbutton_start_word, FALSE, FALSE, 0);

  // Set active or not.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_start_word), settings->session.search_start_word_match);

  checkbutton_end_word = gtk_check_button_new_with_mnemonic("Match _end of word");
  gtk_widget_show(checkbutton_end_word);
  gtk_box_pack_start(GTK_BOX(vbox5), checkbutton_end_word, FALSE, FALSE, 0);

  // Set active or not.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_end_word), settings->session.search_end_word_match);

  table2 = gtk_table_new(2, 2, FALSE);
  gtk_widget_show(table2);
  gtk_box_pack_start(GTK_BOX(vbox5), table2, TRUE, TRUE, 2);

  button_books = gtk_button_new();
  gtk_widget_show(button_books);
  gtk_table_attach(GTK_TABLE(table2), button_books, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(button_books), alignment1);

  hbox3 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox3);
  gtk_container_add(GTK_CONTAINER(alignment1), hbox3);

  image1 = gtk_image_new_from_stock("gtk-properties", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX(hbox3), image1, FALSE, FALSE, 0);

  label13 = gtk_label_new_with_mnemonic("Select _books");
  gtk_widget_show(label13);
  gtk_box_pack_start(GTK_BOX(hbox3), label13, FALSE, FALSE, 0);

  button_area = gtk_button_new();
  gtk_widget_show(button_area);
  gtk_table_attach(GTK_TABLE(table2), button_area, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment2 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment2);
  gtk_container_add(GTK_CONTAINER(button_area), alignment2);

  hbox5 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox5);
  gtk_container_add(GTK_CONTAINER(alignment2), hbox5);

  image2 = gtk_image_new_from_stock("gtk-zoom-in", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image2);
  gtk_box_pack_start(GTK_BOX(hbox5), image2, FALSE, FALSE, 0);

  label39 = gtk_label_new_with_mnemonic("Se_lect parts of text");
  gtk_widget_show(label39);
  gtk_box_pack_start(GTK_BOX(hbox5), label39, FALSE, FALSE, 0);

  label_book = gtk_label_new("");
  gtk_widget_show(label_book);
  gtk_table_attach(GTK_TABLE(table2), label_book, 1, 2, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 2, 0);
  gtk_misc_set_alignment(GTK_MISC(label_book), 0, 0.5);

  label_area = gtk_label_new("");
  gtk_widget_show(label_area);
  gtk_table_attach(GTK_TABLE(table2), label_area, 1, 2, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 2, 0);
  gtk_misc_set_alignment(GTK_MISC(label_area), 0, 0.5);

  empty_notebook_page = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(empty_notebook_page);
  gtk_container_add(GTK_CONTAINER(notebook_advanced), empty_notebook_page);

  label32 = gtk_label_new("");
  gtk_widget_show(label32);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_advanced), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_advanced), 1), label32);

  notebook_results = gtk_notebook_new();
  gtk_widget_show(notebook_results);
  gtk_box_pack_start(GTK_BOX(vbox3), notebook_results, TRUE, TRUE, 0);
  gtk_widget_set_can_focus (notebook_results, false);
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook_results), FALSE);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook_results), FALSE);

  vbox7 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox7);
  gtk_container_add(GTK_CONTAINER(notebook_results), vbox7);

  label14 = gtk_label_new_with_mnemonic("What to do with the search _results");
  gtk_widget_show(label14);
  gtk_box_pack_start(GTK_BOX(vbox7), label14, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label14), 0, 0.5);

  combobox_results = gtk_combo_box_new_text();
  gtk_widget_show(combobox_results);
  gtk_box_pack_start(GTK_BOX(vbox7), combobox_results, TRUE, TRUE, 0);
  gtk_combo_box_append_text(GTK_COMBO_BOX(combobox_results), RESULTS_LOAD);
  gtk_combo_box_append_text(GTK_COMBO_BOX(combobox_results), RESULTS_ADD);
  gtk_combo_box_append_text(GTK_COMBO_BOX(combobox_results), RESULTS_REMOVE);
  gtk_combo_box_append_text(GTK_COMBO_BOX(combobox_results), RESULTS_SHARE);

  // Set selected value in combobox.
  int index = 0;
  switch (settings->session.searchresultstype) {
  case sstLoad:
    index = 0;
    break;
  case sstAdd:
    index = 1;
    break;
  case sstSubtract:
    index = 2;
    break;
  case sstShare:
    index = 3;
    break;
  }
  gtk_combo_box_set_active(GTK_COMBO_BOX(combobox_results), index);

  label35 = gtk_label_new("");
  gtk_widget_show(label35);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_results), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_results), 0), label35);

  empty_notebook_page = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(empty_notebook_page);
  gtk_container_add(GTK_CONTAINER(notebook_results), empty_notebook_page);

  label36 = gtk_label_new("");
  gtk_widget_show(label36);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_results), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_results), 1), label36);

  empty_notebook_page = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(empty_notebook_page);
  gtk_container_add(GTK_CONTAINER(notebook_results), empty_notebook_page);

  label37 = gtk_label_new("");
  gtk_widget_show(label37);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_results), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_results), 2), label37);

  empty_notebook_page = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(empty_notebook_page);
  gtk_container_add(GTK_CONTAINER(notebook_results), empty_notebook_page);

  label38 = gtk_label_new("");
  gtk_widget_show(label38);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_results), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_results), 3), label38);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(searchspecialdialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(searchspecialdialog, NULL, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(searchspecialdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton), true);

  findbutton = gtk_button_new_from_stock("gtk-find");
  gtk_widget_show(findbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(searchspecialdialog), findbutton, GTK_RESPONSE_OK);
  gtk_widget_set_sensitive(findbutton, FALSE);
  gtk_widget_set_can_default (GTK_WIDGET (findbutton), true);

  g_signal_connect((gpointer) searchspecialdialog, "activate_focus", G_CALLBACK(on_searchspecialdialog_activate_focus), gpointer(this));
  g_signal_connect((gpointer) word_entry, "changed", G_CALLBACK(on_word_entry_changed), gpointer(this));
  g_signal_connect((gpointer) word_entry, "activate", G_CALLBACK(on_word_entry_activate), gpointer(this));
  g_signal_connect((gpointer) checkbutton_current_book, "toggled", G_CALLBACK(on_checkbutton_current_book_toggled), gpointer(this));
  g_signal_connect((gpointer) button_books, "clicked", G_CALLBACK(on_button_books_clicked), gpointer(this));
  g_signal_connect((gpointer) button_area, "clicked", G_CALLBACK(on_button_area_clicked), gpointer(this));
  g_signal_connect((gpointer) findbutton, "clicked", G_CALLBACK(on_findbutton_clicked), gpointer(this));
  g_signal_connect_after((gpointer) notebook_mode, "switch_page", G_CALLBACK(on_notebook_mode_switch_page), gpointer(this));

  gtk_label_set_mnemonic_widget(GTK_LABEL(label_word_entry), word_entry);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label14), combobox_results);

  gtk_widget_grab_focus(word_entry);
  gtk_widget_grab_default(findbutton);
  gtk_window_add_accel_group(GTK_WINDOW(searchspecialdialog), accel_group);

  // Entry completion
  completion_setup(word_entry, cpSearch);

  set_gui();

  // Set dialogbox to either simple or advanced search or ....
  if (settings->session.search_page == 0) {
    on_mode_change(settings->session.search_page);
  } else {
    for (int i = 0; i < settings->session.search_page; i++)
      on_control_f();
  }

  // Present the window after a while because other windows created may take away the focus.
  dialogpresenter = new DialogPresenter(searchspecialdialog);
}


SearchSpecialDialog::~SearchSpecialDialog()
{
  delete dialogpresenter;
  gtk_widget_destroy(searchspecialdialog);
}


int SearchSpecialDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(searchspecialdialog));
}


void SearchSpecialDialog::on_word_entry_changed(GtkEditable * editable, gpointer user_data)
{
  ((SearchSpecialDialog *) user_data)->set_gui();
}


void SearchSpecialDialog::set_gui()
{
  extern Settings *settings;
  bool findbuttonsensitive;
  string searchword = gtk_entry_get_text(GTK_ENTRY(word_entry));
  findbuttonsensitive = (searchword.size() > 0);
  gtk_widget_set_sensitive(findbutton, findbuttonsensitive);
  bool searchcurrentbook = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_current_book));
  gtk_widget_set_sensitive(button_books, !searchcurrentbook);
  if (!searchcurrentbook)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_current_chapter), false);
  gtk_widget_set_sensitive(checkbutton_current_chapter, searchcurrentbook);
  ustring message = gui_book_selection_information(settings->genconfig.project_get());
  gtk_label_set_text(GTK_LABEL(label_book), message.c_str());
  gtk_label_set_text(GTK_LABEL(label_area), area_information().c_str());
}


void SearchSpecialDialog::on_findbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((SearchSpecialDialog *) user_data)->on_find();
}


void SearchSpecialDialog::on_find()
{
  extern Settings *settings;
  settings->session.searchword = gtk_entry_get_text(GTK_ENTRY(word_entry));
  settings->session.search_case_sensitive = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_casesensitive));
  settings->session.search_current_book = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_current_book));
  settings->session.search_current_chapter = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_current_chapter));
  settings->session.search_globbing = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_glob));
  settings->session.search_start_word_match = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_start_word));
  settings->session.search_end_word_match = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_end_word));
  settings->session.search_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook_mode));
  ustring type = combobox_get_active_string(combobox_results);
  if (type == RESULTS_LOAD)
    settings->session.searchresultstype = sstLoad;
  if (type == RESULTS_ADD)
    settings->session.searchresultstype = sstAdd;
  if (type == RESULTS_REMOVE)
    settings->session.searchresultstype = sstSubtract;
  if (type == RESULTS_SHARE)
    settings->session.searchresultstype = sstShare;
  switch (settings->session.searchresultstype) {
  case sstLoad:
    settings->session.highlights.clear();       // Fall through, all the way to the bottom.
  case sstAdd:
  case sstSubtract:
  case sstShare:
    {
      SessionHighlights sessionhighlights(settings->session.searchword, settings->session.search_case_sensitive, settings->session.search_globbing, settings->session.search_start_word_match, settings->session.search_end_word_match, settings->session.area_type, settings->session.area_id, settings->session.area_intro, settings->session.area_heading, settings->session.area_chapter, settings->session.area_study, settings->session.area_notes, settings->session.area_xref, settings->session.area_verse);
      settings->session.highlights.push_back(sessionhighlights);
    }
  }
  completion_finish(word_entry, cpSearch);
}


void SearchSpecialDialog::on_button_books_clicked(GtkButton * button, gpointer user_data)
{
  ((SearchSpecialDialog *) user_data)->on_books();
}


void SearchSpecialDialog::on_books()
{
  extern Settings *settings;
  SelectBooksDialog dialog(false);
  dialog.selection_set(settings->session.selected_books);
  if (dialog.run() == GTK_RESPONSE_OK) {
    settings->session.selected_books = dialog.selectionset;
  }
  set_gui();
}


void SearchSpecialDialog::on_checkbutton_current_book_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((SearchSpecialDialog *) user_data)->set_gui();
}


void SearchSpecialDialog::on_button_area_clicked(GtkButton * button, gpointer user_data)
{
  ((SearchSpecialDialog *) user_data)->on_button_area();
}


void SearchSpecialDialog::on_button_area()
{
  AreaDialog dialog(0);
  dialog.run();
  set_gui();
}


void SearchSpecialDialog::on_searchspecialdialog_activate_focus(GtkWindow * window, gpointer user_data)
{
  ((SearchSpecialDialog *) user_data)->on_control_f();
}


void SearchSpecialDialog::on_control_f()
{
  allow_activate = false;
  g_timeout_add(1, GSourceFunc(on_timeout), gpointer(this));
  int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook_mode));
  page++;
  if (page > 1)
    page = 0;
  gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_mode), page);
  gtk_widget_grab_focus(word_entry);
}


void SearchSpecialDialog::on_notebook_mode_switch_page(GtkNotebook * notebook, GtkNotebookPage * page, guint page_num, gpointer user_data)
{
  ((SearchSpecialDialog *) user_data)->on_mode_change(page_num);
}


void SearchSpecialDialog::on_mode_change(guint page_num)
// When the search mode changes (Simple / Advanced / BibleTime / ...), then
// update the other notebooks too.
{
  // Show right page for casesensitive / search in current book
  if (page_num == 2)
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_case_current), 1);
  else
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_case_current), 0);
  // Show right page for advanced notebook.
  switch (page_num) {
  case 0:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_advanced), 1);
    break;
  case 1:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_advanced), 0);
    break;
  case 2:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_advanced), 2);
    break;
  }
  // Show right page for what to do with results.
  if (page_num == 0)
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_results), 1);
  else
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_results), 0);
  // Update rest of gui.
  set_gui();
}


void SearchSpecialDialog::on_word_entry_activate(GtkEntry * entry, gpointer user_data)
{
  ((SearchSpecialDialog *) user_data)->on_activate();
}


void SearchSpecialDialog::on_activate()
{
  // If the find button is disabled, bail out.
  if (!gtk_widget_get_sensitive (findbutton))
    return;
  if (allow_activate) {
    on_find();
    gtk_dialog_response(GTK_DIALOG(searchspecialdialog), GTK_RESPONSE_OK);
  }
}


bool SearchSpecialDialog::on_timeout(gpointer data)
{
  ((SearchSpecialDialog *) data)->timeout();
  return false;
}


void SearchSpecialDialog::timeout()
{
  allow_activate = true;
}


