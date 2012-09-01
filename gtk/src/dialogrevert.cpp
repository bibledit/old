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
#include "dialogrevert.h"
#include "help.h"
#include "shortcuts.h"
#include "settings.h"
#include "combobox.h"
#include "projectutils.h"
#include "books.h"
#include "utilities.h"
#include "gwrappers.h"
#include "gtkwrappers.h"
#include "listview.h"
#include "date_time_utils.h"
#include "tiny_utilities.h"
#include "directories.h"
#include "unixwrappers.h"
#include "compareutils.h"
#include "snapshots.h"


RevertDialog::RevertDialog(Reference * reference)
{
  // Save variables.
  extern Settings *settings;
  project = settings->genconfig.project_get();
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  language = projectconfig->language_get();
  reference_chapter = reference->chapter;
  revisionloaded = false;
  branch = 0;

  // Build dialog.
  Shortcuts shortcuts(0);

  revertdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(revertdialog), "Revert");
  gtk_window_set_position(GTK_WINDOW(revertdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_type_hint(GTK_WINDOW(revertdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG (revertdialog));
  gtk_widget_show(dialog_vbox1);

  hbox1 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox1, TRUE, TRUE, 0);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(hbox1), vbox1, TRUE, TRUE, 0);

  table1 = gtk_table_new(2, 2, FALSE);
  gtk_widget_show(table1);
  gtk_box_pack_start(GTK_BOX(vbox1), table1, FALSE, TRUE, 0);

  comboboxbook = gtk_combo_box_new_text();
  gtk_widget_show(comboboxbook);
  gtk_table_attach(GTK_TABLE(table1), comboboxbook, 0, 1, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  comboboxchapter = gtk_combo_box_new_text();
  gtk_widget_show(comboboxchapter);
  gtk_table_attach(GTK_TABLE(table1), comboboxchapter, 1, 2, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (GTK_FILL), 0, 0);

  labelbook = gtk_label_new("Book");
  gtk_widget_show(labelbook);
  gtk_table_attach(GTK_TABLE(table1), labelbook, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(labelbook), 0, 0.5);

  shortcuts.label(labelbook);

  gtk_label_set_mnemonic_widget(GTK_LABEL(labelbook), comboboxbook);

  labelchapter = gtk_label_new("Chapter");
  gtk_widget_show(labelchapter);
  gtk_table_attach(GTK_TABLE(table1), labelchapter, 1, 2, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(labelchapter), 0, 0.5);

  shortcuts.label(labelchapter);

  gtk_label_set_mnemonic_widget(GTK_LABEL(labelchapter), comboboxchapter);

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator1, FALSE, TRUE, 0);

  labelrevisions = gtk_label_new("Revisions");
  gtk_widget_show(labelrevisions);
  gtk_box_pack_start(GTK_BOX(vbox1), labelrevisions, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(labelrevisions), 0, 0.5);

  shortcuts.label(labelrevisions);

  scrolledwindowrevisions = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindowrevisions);
  gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindowrevisions, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindowrevisions), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindowrevisions), GTK_SHADOW_IN);

  treeviewrevisions = gtk_tree_view_new();
  gtk_widget_show(treeviewrevisions);
  gtk_container_add(GTK_CONTAINER(scrolledwindowrevisions), treeviewrevisions);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeviewrevisions), FALSE);

  gtk_label_set_mnemonic_widget(GTK_LABEL(labelrevisions), treeviewrevisions);

  // Create treeview related stuff.
  store = gtk_list_store_new(1, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeviewrevisions), GTK_TREE_MODEL(store));
  g_object_unref(store);
  GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeviewrevisions), column);
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeviewrevisions));
  gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);

  vseparator1 = gtk_vseparator_new();
  gtk_widget_show(vseparator1);
  gtk_box_pack_start(GTK_BOX(hbox1), vseparator1, FALSE, TRUE, 0);

  vbox2 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox2);
  gtk_box_pack_start(GTK_BOX(hbox1), vbox2, TRUE, TRUE, 0);

  GSList *radiobutton_current_group = NULL;

  radiobutton_current = gtk_radio_button_new_with_mnemonic (NULL, "View current version");
  gtk_widget_show (radiobutton_current);
  gtk_box_pack_start (GTK_BOX (vbox2), radiobutton_current, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_current), radiobutton_current_group);
  radiobutton_current_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_current));

  shortcuts.button(radiobutton_current);

  radiobutton_previous = gtk_radio_button_new_with_mnemonic (NULL, "View previous revision");
  gtk_widget_show (radiobutton_previous);
  gtk_box_pack_start (GTK_BOX (vbox2), radiobutton_previous, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_previous), radiobutton_current_group);
  radiobutton_current_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_previous));

  shortcuts.button(radiobutton_previous);

  radiobutton_changes = gtk_radio_button_new_with_mnemonic (NULL, "View changes if reverting to previous revision");
  gtk_widget_show (radiobutton_changes);
  gtk_box_pack_start (GTK_BOX (vbox2), radiobutton_changes, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_changes), radiobutton_current_group);
  radiobutton_current_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_changes));

  shortcuts.button(radiobutton_changes);
 
  changes_gui = new DisplayChangesGui (vbox2);

  gtk_widget_set_size_request(changes_gui->textview, 450, 500);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(revertdialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(revertdialog, NULL, &shortcuts, "edit/revert");

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(revertdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton1), true);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(revertdialog), okbutton1, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton1), true);

  shortcuts.stockbutton(cancelbutton1);
  shortcuts.stockbutton(okbutton1);
  shortcuts.process();

  g_signal_connect((gpointer) comboboxbook, "changed", G_CALLBACK(on_comboboxbook_changed), gpointer(this));
  g_signal_connect((gpointer) comboboxchapter, "changed", G_CALLBACK(on_comboboxchapter_changed), gpointer(this));
  g_signal_connect((gpointer) treeviewrevisions, "row_activated", G_CALLBACK(on_treeviewrevisions_row_activated), gpointer(this));
  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));
  g_signal_connect ((gpointer) radiobutton_current, "toggled",  G_CALLBACK (on_radiobutton_toggled), gpointer(this));
  g_signal_connect ((gpointer) radiobutton_previous, "toggled",  G_CALLBACK (on_radiobutton_toggled), gpointer(this));
  g_signal_connect ((gpointer) radiobutton_changes, "toggled",  G_CALLBACK (on_radiobutton_toggled), gpointer(this));

  gtk_widget_grab_focus(okbutton1);
  gtk_widget_grab_default(okbutton1);

  // Load books. This also loads the chapters through the callback.
  vector < unsigned int >books = project_get_books(project);
  vector < ustring > localbooks;
  for (unsigned int i = 0; i < books.size(); i++) {
    localbooks.push_back(books_id_to_name(language, books[i]));
  }
  combobox_set_strings(comboboxbook, localbooks);
  combobox_set_string(comboboxbook, books_id_to_name(language, reference->book));
  
  // By default view changes.
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton_changes), true);
}

RevertDialog::~RevertDialog()
{
  delete changes_gui;
  gtk_widget_destroy(revertdialog);
}

int RevertDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(revertdialog));
}

void RevertDialog::on_comboboxbook_changed(GtkComboBox * combobox, gpointer user_data)
{
  ((RevertDialog *) user_data)->on_book_changed();
}

void RevertDialog::on_book_changed()
// Book changed: load chapters.
{
  vector < unsigned int >chapters = project_get_chapters(project, book_get());
  combobox_set_strings(comboboxchapter, chapters);
  if (reference_chapter >= 0) {
    combobox_set_string(comboboxchapter, reference_chapter);
    reference_chapter = -1;
  } else {
    if (!chapters.empty())
      combobox_set_string(comboboxchapter, chapters[0]);
  }
}

void RevertDialog::on_comboboxchapter_changed(GtkComboBox * combobox, gpointer user_data)
{
  ((RevertDialog *) user_data)->on_chapter_changed();
}

void RevertDialog::on_chapter_changed()
{
  // Load the revisions.
  listview_clear_strings(treeviewrevisions, store);
  ustring chapterdirectory = project_data_directory_chapter(project, book_get(), chapter_get());
  ustring path = project_data_filename_chapter(project, book_get(), chapter_get(), false);
  path = gw_path_get_basename(path);
  seconds = snapshots_get_seconds (project, book_get(), chapter_get());
  vector <ustring> lines;
  for (unsigned int i = 0; i < seconds.size(); i++) {
    lines.push_back(date_time_seconds_human_readable(seconds[i], false));
  }
  listview_set_strings(treeviewrevisions, store, lines);

  // Autosize columns.
  gtk_tree_view_columns_autosize(GTK_TREE_VIEW(treeviewrevisions));

  // No revision loaded yet.
  revisionloaded = false;
  
  // Take action defined by radio button.
  on_radiobutton (NULL);
}

void RevertDialog::on_treeviewrevisions_row_activated(GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data)
{
  ((RevertDialog *) user_data)->on_revision_activated();
}

void RevertDialog::on_revision_activated()
{
  // Get the second when the snapshot was taken.
  unsigned int second = 0;
  {
    ustring row = listview_get_active_string(treeviewrevisions);
    for (unsigned int i = 0; i < seconds.size(); i++) {
      if (row == date_time_seconds_human_readable(seconds[i], false)) {
        second = seconds[i];
        break;
      }
    }
  }

  // Load the chapter's data.
  ustring data = snapshots_get_chapter (project, book_get(), chapter_get(), second);
  ParseLine parse (data);
  history_data = parse.lines;

  // Revision loaded.
  revisionloaded = true;
  
  // Display whatever the radiobutton indicates.
  on_radiobutton (NULL);
}

void RevertDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((RevertDialog *) user_data)->on_okbutton();
}

void RevertDialog::on_okbutton()
// On pressing Ok, if the user has selected a change, give warning we're going
// to change the data, and if Yes is responded with, proceed with loading the change.
{
  // If no revision loaded, bail out.
  if (!revisionloaded)
    return;

  // Get current and previous text.
  ustring current_text_filename = project_data_filename_chapter(project, book_get(), chapter_get(), false);
  ReadText rt (current_text_filename, true, false);

  // If no change, bail out.
  if (vector_strings_equal (rt.lines, history_data)) {
    gtkw_dialog_info (NULL, "No changes were applied");
    return;
  }

  // Ask the user if he is sure to load the previous text. If not, bail out.
  int result = gtkw_dialog_question(revertdialog, "Are you sure you wish to revert this chapter to the previous revision?");
  if (result != GTK_RESPONSE_YES)
    return;

  // Go back to previous revision.
  vector < ustring > lines2;
  for (unsigned int i = 0; i < history_data.size(); i++)
    if (!history_data[i].empty())
      lines2.push_back(history_data[i]);
  CategorizeChapterVerse ccv(lines2);
  project_store_chapter(project, book_get(), ccv);
}


unsigned int RevertDialog::book_get()
{
  ustring localbook = combobox_get_active_string(comboboxbook);
  unsigned int book = books_name_to_id(language, localbook);
  return book;
}


unsigned int RevertDialog::chapter_get()
{
  ustring uchapter = combobox_get_active_string(comboboxchapter);
  unsigned int chapter = convert_to_int(uchapter);
  return chapter;
}


void RevertDialog::on_radiobutton_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  ((RevertDialog *) user_data)->on_radiobutton(togglebutton);
}


void RevertDialog::on_radiobutton (GtkToggleButton *togglebutton)
{
  // Bail out if the toggle button gets inactive.
  if (togglebutton)
    if (!gtk_toggle_button_get_active (togglebutton))
      return;
  
  // Clear view.
  changes_gui->clear ();
  
  // Load current text.
  ustring current_text_filename = project_data_filename_chapter(project, book_get(), chapter_get(), false);
  ReadText rt (current_text_filename, true, false);
  
  // View current text?
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_current))) {
    changes_gui->display (rt.lines);
    return;
  }

  // Bail out if no revison has been loaded yet.
  if (!revisionloaded)
    return;
    
  // View history?
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_previous))) {
    changes_gui->display (history_data);
    return;
  }
  
  // View comparison.
  vector <ustring> comparison;
  compare_usfm_text (rt.lines, history_data, comparison, true);
  changes_gui->display (comparison);
}


