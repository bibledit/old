/*
** Copyright (©) 2003-2008 Teus Benschop.
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
#include "git.h"
#include "directories.h"
#include "unixwrappers.h"
#include "progresswindow.h"

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

  // Progress dialog
  ProgressWindow progresswindow("Working...", false);
  progresswindow.set_fraction(0.5);

  // Build dialog.
  Shortcuts shortcuts(0);

  revertdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(revertdialog), "Revert");
  gtk_window_set_position(GTK_WINDOW(revertdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_type_hint(GTK_WINDOW(revertdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG(revertdialog)->vbox;
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

  labelcurrent = gtk_label_new("Current text");
  gtk_widget_show(labelcurrent);
  gtk_box_pack_start(GTK_BOX(vbox2), labelcurrent, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(labelcurrent), 0, 0.5);

  shortcuts.label(labelcurrent);

  scrolledwindowcurrent = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindowcurrent);
  gtk_box_pack_start(GTK_BOX(vbox2), scrolledwindowcurrent, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindowcurrent), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindowcurrent), GTK_SHADOW_IN);

  textviewcurrent = gtk_text_view_new();
  gtk_widget_show(textviewcurrent);
  gtk_container_add(GTK_CONTAINER(scrolledwindowcurrent), textviewcurrent);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textviewcurrent), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textviewcurrent), GTK_WRAP_WORD);

  gtk_label_set_mnemonic_widget(GTK_LABEL(labelcurrent), textviewcurrent);

  gtk_widget_set_size_request(textviewcurrent, 450, 150);

  labelprevious = gtk_label_new("Previous text");
  gtk_widget_show(labelprevious);
  gtk_box_pack_start(GTK_BOX(vbox2), labelprevious, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(labelprevious), 0, 0.5);

  shortcuts.label(labelprevious);

  scrolledwindowprevious = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindowprevious);
  gtk_box_pack_start(GTK_BOX(vbox2), scrolledwindowprevious, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindowprevious), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindowprevious), GTK_SHADOW_IN);

  textviewprevious = gtk_text_view_new();
  gtk_widget_show(textviewprevious);
  gtk_container_add(GTK_CONTAINER(scrolledwindowprevious), textviewprevious);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textviewprevious), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textviewprevious), GTK_WRAP_WORD);

  gtk_label_set_mnemonic_widget(GTK_LABEL(labelprevious), textviewprevious);

  gtk_widget_set_size_request(textviewprevious, 450, 150);

  labelchanges = gtk_label_new("Changes");
  gtk_widget_show(labelchanges);
  gtk_box_pack_start(GTK_BOX(vbox2), labelchanges, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(labelchanges), 0, 0.5);

  shortcuts.label(labelchanges);

  scrolledwindowchanges = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindowchanges);
  gtk_box_pack_start(GTK_BOX(vbox2), scrolledwindowchanges, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindowchanges), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindowchanges), GTK_SHADOW_IN);

  textviewchanges = gtk_text_view_new();
  gtk_widget_show(textviewchanges);
  gtk_container_add(GTK_CONTAINER(scrolledwindowchanges), textviewchanges);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textviewchanges), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textviewchanges), GTK_WRAP_WORD);

  gtk_label_set_mnemonic_widget(GTK_LABEL(labelchanges), textviewchanges);

  gtk_widget_set_size_request(textviewchanges, 450, 150);

  dialog_action_area1 = GTK_DIALOG(revertdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(revertdialog, &shortcuts, "revert");

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(revertdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(revertdialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton1);
  shortcuts.stockbutton(okbutton1);
  shortcuts.process();

  g_signal_connect((gpointer) comboboxbook, "changed", G_CALLBACK(on_comboboxbook_changed), gpointer(this));
  g_signal_connect((gpointer) comboboxchapter, "changed", G_CALLBACK(on_comboboxchapter_changed), gpointer(this));
  g_signal_connect((gpointer) treeviewrevisions, "row_activated", G_CALLBACK(on_treeviewrevisions_row_activated), gpointer(this));
  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));

  gtk_widget_grab_focus(okbutton1);
  gtk_widget_grab_default(okbutton1);

  // Create text tags for additions in bold and deletions in strikethrough.
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textviewchanges));
  addedtag = gtk_text_buffer_create_tag(buffer, NULL, "weight", PANGO_WEIGHT_HEAVY, NULL);
  deletedtag = gtk_text_buffer_create_tag(buffer, NULL, "strikethrough", TRUE, NULL);

  // Copy the project to the temporal one.
  git_clone_project = "__git__revert__poject";
  project_copy(project, git_clone_project);

  // Load books. This also loads the chapters through the callback.
  vector < unsigned int >books = project_get_books(project);
  vector < ustring > localbooks;
  for (unsigned int i = 0; i < books.size(); i++) {
    localbooks.push_back(books_id_to_name(language, books[i]));
  }
  combobox_set_strings(comboboxbook, localbooks);
  combobox_set_string(comboboxbook, books_id_to_name(language, reference->book));
}

RevertDialog::~RevertDialog()
{
  gtk_widget_destroy(revertdialog);
  if (!git_clone_project.empty())
    project_delete(git_clone_project);
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
  commits.clear();
  seconds.clear();
  git_log_read(chapterdirectory, commits, seconds, path);
  vector < ustring > lines;
  for (unsigned int i = 0; i < seconds.size(); i++) {
    lines.push_back(date_time_seconds_human_readable(seconds[i], false));
  }
  listview_set_strings(treeviewrevisions, store, lines);

  // Autosize columns.
  gtk_tree_view_columns_autosize(GTK_TREE_VIEW(treeviewrevisions));

  // Load current text in the textview.
  current_text_filename = project_data_filename_chapter(project, book_get(), chapter_get(), false);
  gchar *contents;
  g_file_get_contents(current_text_filename.c_str(), &contents, NULL, NULL);
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textviewcurrent));
  if (contents) {
    gtk_text_buffer_set_text(buffer, contents, -1);
    g_free(contents);
  }
  // No revision loaded yet.
  revisionloaded = false;
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textviewprevious));
  gtk_text_buffer_set_text(buffer, "", -1);
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textviewchanges));
  gtk_text_buffer_set_text(buffer, "", -1);
}

void RevertDialog::on_treeviewrevisions_row_activated(GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data)
{
  ((RevertDialog *) user_data)->on_revision_activated();
}

void RevertDialog::on_revision_activated()
{
  // Get the commit's name.
  ustring commit;
  {
    ustring row = listview_get_active_string(treeviewrevisions);
    for (unsigned int i = 0; i < seconds.size(); i++) {
      if (row == date_time_seconds_human_readable(seconds[i], false)) {
        commit = commits[i];
        break;
      }
    }
  }

  // Get the data directory.
  ustring datadirectory = project_data_directory_project(git_clone_project);

  // Check the revision out.
  {
    GwSpawn spawn("git-checkout");
    spawn.workingdirectory(datadirectory);
    spawn.arg("-b");
    branch++;
    spawn.arg("branch" + convert_to_string(branch));
    spawn.arg(commit);
    spawn.run();
  }

  // Load the chapter in the view.
  ustring filename_history = project_data_filename_chapter(git_clone_project, book_get(), chapter_get(), false);
  gchar *contents;
  g_file_get_contents(filename_history.c_str(), &contents, NULL, NULL);
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textviewprevious));
  if (contents) {
    gtk_text_buffer_set_text(buffer, contents, -1);
    g_free(contents);
  }
  // Get differences.
  {
    GwSpawn spawn("diff");
    spawn.arg(current_text_filename);
    spawn.arg(filename_history);
    spawn.read();
    spawn.run();
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textviewchanges));
    gtk_text_buffer_set_text(buffer, "", -1);
    for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
      ustring line = spawn.standardout[i];
      if (line == "---")
        continue;
      bool added = false;
      bool deleted = false;
      if (!line.empty()) {
        ustring mark = line.substr(0, 1);
        if (mark == ">")
          added = true;
        if (mark == "<")
          deleted = true;
        if (added || deleted)
          line.erase(0, 2);
        else
          continue;
      }
      line.append("\n");
      GtkTextIter iter;
      gtk_text_buffer_get_end_iter(buffer, &iter);
      if (added) {
        gtk_text_buffer_insert_with_tags(buffer, &iter, line.c_str(), -1, addedtag, NULL);
      } else if (deleted) {
        gtk_text_buffer_insert_with_tags(buffer, &iter, line.c_str(), -1, deletedtag, NULL);
      } else {
        gtk_text_buffer_insert(buffer, &iter, line.c_str(), -1);
      }
    }
  }

  // Revision loaded.
  revisionloaded = true;
}

void RevertDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((RevertDialog *) user_data)->on_okbutton();
}

void RevertDialog::on_okbutton()
// On pressing Ok, if the user has selected a change, give warning we're going
// to change the data, and if Yes is respondedn, proceed with loading the change.
{
  // If no revision loaded, bail out.
  if (!revisionloaded)
    return;

  // Get current and previous text.
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textviewcurrent));
  GtkTextIter startiter;
  GtkTextIter enditer;
  gtk_text_buffer_get_start_iter(buffer, &startiter);
  gtk_text_buffer_get_end_iter(buffer, &enditer);
  ustring currenttext = gtk_text_buffer_get_text(buffer, &startiter, &enditer, false);
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textviewprevious));
  gtk_text_buffer_get_start_iter(buffer, &startiter);
  gtk_text_buffer_get_end_iter(buffer, &enditer);
  ustring previoustext = gtk_text_buffer_get_text(buffer, &startiter, &enditer, false);

  // If no change, bail out.
  if (currenttext == previoustext)
    return;

  // Ask the user if he is sure to load the previous text. If not, bail out.
  int result = gtkw_dialog_question(revertdialog, "Are you sure you wish to go back to the previous revision?");
  if (result != GTK_RESPONSE_YES)
    return;

  // Go back to previous revision.
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textviewprevious));
  vector < ustring > lines;
  textbuffer_get_lines(buffer, lines);
  vector < ustring > lines2;
  for (unsigned int i = 0; i < lines.size(); i++)
    if (!lines[i].empty())
      lines2.push_back(lines[i]);
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
