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


#include "dialogeditstatus.h"
#include "help.h"
#include "settings.h"
#include "books.h"
#include "tiny_utilities.h"
#include "progresswindow.h"
#include "listview.h"
#include "screen.h"


EditStatusDialog::EditStatusDialog(const ustring & project_in, unsigned int book, unsigned int chapter)
{
  // Initialize variables.
  project = project_in;
  setting_status = false;

  // Settings.
  extern Settings *settings;

  // Project status.
  vector < ustring > alltasks = settings->genconfig.project_tasks_names_get();
  alltasks_size = alltasks.size();
  projectstatus = new ProjectStatus(project, alltasks, true);

  // Build dialog.  
  editstatusdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(editstatusdialog), "Edit Status");
  gtk_window_set_position(GTK_WINDOW(editstatusdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(editstatusdialog), TRUE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG (editstatusdialog));
  gtk_widget_show(dialog_vbox1);

  hbox = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(hbox);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox, TRUE, TRUE, 0);

  scrolledwindow_books = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow_books);
  gtk_box_pack_start(GTK_BOX(hbox), scrolledwindow_books, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow_books), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow_books), GTK_SHADOW_IN);

  treeview_books = gtk_tree_view_new();
  gtk_widget_show(treeview_books);
  gtk_container_add(GTK_CONTAINER(scrolledwindow_books), treeview_books);

  liststore_books = gtk_list_store_new(1, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview_books), GTK_TREE_MODEL(liststore_books));
  g_object_unref(liststore_books);
  GtkCellRenderer *renderer_books = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview_books), -1, "Books", renderer_books, "text", 0, NULL);
  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_books)), GTK_SELECTION_MULTIPLE);

  scrolledwindow_chapters = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow_chapters);
  gtk_box_pack_start(GTK_BOX(hbox), scrolledwindow_chapters, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow_chapters), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow_chapters), GTK_SHADOW_IN);

  treeview_chapters = gtk_tree_view_new();
  gtk_widget_show(treeview_chapters);
  gtk_container_add(GTK_CONTAINER(scrolledwindow_chapters), treeview_chapters);

  liststore_chapters = gtk_list_store_new(1, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview_chapters), GTK_TREE_MODEL(liststore_chapters));
  g_object_unref(liststore_chapters);
  GtkCellRenderer *renderer_chapters = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview_chapters), -1, "Chapters", renderer_chapters, "text", 0, NULL);
  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_chapters)), GTK_SELECTION_MULTIPLE);

  scrolledwindow_verses = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow_verses);
  gtk_box_pack_start(GTK_BOX(hbox), scrolledwindow_verses, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow_verses), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow_verses), GTK_SHADOW_IN);

  treeview_verses = gtk_tree_view_new();
  gtk_widget_show(treeview_verses);
  gtk_container_add(GTK_CONTAINER(scrolledwindow_verses), treeview_verses);

  liststore_verses = gtk_list_store_new(1, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview_verses), GTK_TREE_MODEL(liststore_verses));
  g_object_unref(liststore_verses);
  GtkCellRenderer *renderer_verses = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview_verses), -1, "Verses", renderer_verses, "text", 0, NULL);
  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_verses)), GTK_SELECTION_MULTIPLE);

  scrolledwindow_status = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow_status);
  gtk_box_pack_start (GTK_BOX (hbox), scrolledwindow_status, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_status), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_status), GTK_SHADOW_IN);

  viewport_status = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport_status);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_status), viewport_status);

  vbox_status = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_status);
  gtk_container_add (GTK_CONTAINER (viewport_status), vbox_status);

  // Build buttons for each state.
  for (unsigned int i = 0; i < alltasks.size(); i++) {

    GtkWidget *checkbutton_status;
    checkbutton_status = gtk_check_button_new_with_mnemonic(alltasks[i].c_str());
    gtk_widget_show(checkbutton_status);
    gtk_box_pack_start(GTK_BOX(vbox_status), checkbutton_status, FALSE, FALSE, 0);

    g_signal_connect((gpointer) checkbutton_status, "toggled", G_CALLBACK(on_checkbutton_status_toggled), gpointer(this));

    statusbuttons.push_back(checkbutton_status);
  }

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(editstatusdialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(editstatusdialog, NULL, NULL, "edit/status");

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(editstatusdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton), true);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(editstatusdialog), okbutton, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton), true);

  g_signal_connect_after((gpointer) treeview_books, "button_release_event", G_CALLBACK(on_treeview_books_button_event), gpointer(this));
  g_signal_connect_after((gpointer) treeview_books, "key_release_event", G_CALLBACK(on_treeview_books_key_event), gpointer(this));
  g_signal_connect_after((gpointer) treeview_chapters, "button_release_event", G_CALLBACK(on_treeview_chapters_button_event), gpointer(this));
  g_signal_connect_after((gpointer) treeview_chapters, "key_release_event", G_CALLBACK(on_treeview_chapters_key_event), gpointer(this));
  g_signal_connect_after((gpointer) treeview_verses, "button_release_event", G_CALLBACK(on_treeview_verses_button_event), gpointer(this));
  g_signal_connect_after((gpointer) treeview_verses, "key_release_event", G_CALLBACK(on_treeview_verses_key_event), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_focus(okbutton);
  gtk_widget_grab_default(okbutton);

  // Load the books and focus the one that is in the editor.
  vector < ustring > books;
  for (unsigned int i = 0; i < projectstatus->books.size(); i++) {
    ProjectStatusBook *project_status_book = projectstatus->books.at(i);
    books.push_back(books_id_to_english(project_status_book->book));
  }
  listview_set_strings(treeview_books, liststore_books, books);
  listview_focus_string(treeview_books, books_id_to_english(book), false);
  on_treeview_books_cursor();

  // Focus the chapter that is in the editor.
  listview_focus_string(treeview_chapters, convert_to_string(chapter), false);
  on_treeview_chapters_cursor();
  
  new DialogAutoScaler (editstatusdialog, G_MAXINT);
}


EditStatusDialog::~EditStatusDialog()
{
  delete projectstatus;
  gtk_widget_destroy(editstatusdialog);
}


int EditStatusDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(editstatusdialog));
}


gboolean EditStatusDialog::on_treeview_books_button_event(GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_treeview_books_cursor();
  return FALSE;
}


gboolean EditStatusDialog::on_treeview_books_key_event(GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_treeview_books_cursor();
  return FALSE;
}


void EditStatusDialog::on_treeview_books_cursor()
{
  currentbooks = listview_get_active_offsets(treeview_books);
  load_chapters();
}


void EditStatusDialog::load_chapters()
{
  currentchapters.clear();
  vector < ustring > chapters;
  if (currentbooks.size() == 1) {
    ProjectStatusBook *project_status_book = projectstatus->books.at(currentbooks[0]);
    for (unsigned int i = 0; i < project_status_book->chapters.size(); i++) {
      ProjectStatusChapter *project_status_chapter = project_status_book->chapters.at(i);
      chapters.push_back(convert_to_string(project_status_chapter->chapter));
    }
  }
  listview_set_strings(treeview_chapters, liststore_chapters, chapters);
  on_treeview_chapters_cursor();
  show_status();
}


gboolean EditStatusDialog::on_treeview_chapters_button_event(GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_treeview_chapters_cursor();
  return FALSE;
}


gboolean EditStatusDialog::on_treeview_chapters_key_event(GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_treeview_chapters_cursor();
  return FALSE;
}


void EditStatusDialog::on_treeview_chapters_cursor()
{
  currentchapters = listview_get_active_offsets(treeview_chapters);
  load_verses();
}


void EditStatusDialog::load_verses()
{
  currentverses.clear();
  vector < ustring > verses;
  if ((currentbooks.size() == 1) && (currentchapters.size() == 1)) {
    ProjectStatusChapter *project_status_chapter = projectstatus->books.at(currentbooks[0])->chapters.at(currentchapters[0]);
    for (unsigned int i = 0; i < project_status_chapter->status.size(); i++) {
      verses.push_back(convert_to_string(i));
    }
  }
  listview_set_strings(treeview_verses, liststore_verses, verses);
  show_status();
}


gboolean EditStatusDialog::on_treeview_verses_button_event(GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_treeview_verses_cursor();
  return FALSE;
}


gboolean EditStatusDialog::on_treeview_verses_key_event(GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_treeview_verses_cursor();
  return FALSE;
}


void EditStatusDialog::on_treeview_verses_cursor()
{
  currentverses = listview_get_active_offsets(treeview_verses);
  show_status();
}


EditStatusType EditStatusDialog::editstatustype()
// Gets the type of status to be edited.
{
  // First we deal with the books.

  // No books.
  if (currentbooks.empty())
    return estNone;

  // Multiple books.
  if (currentbooks.size() > 1)
    return estBooks;

  // One book and no chapters.
  if ((currentbooks.size() == 1) && (currentchapters.empty()))
    return estBook;

  // Below this only one book is selected, and we deal with the chapters.

  // No chapters.
  if (currentchapters.empty())
    return estNone;

  // Multiple chapters.
  if (currentchapters.size() > 1)
    return estChapters;

  // One chapter and no verses.
  if ((currentchapters.size() == 1) && (currentverses.empty()))
    return estChapter;

  // Below this one book and one chapter have been selected, and we deal with the verses.

  // No verses.
  if (currentverses.empty())
    return estNone;

  // Multiple verses.
  if (currentverses.size() > 1)
    return estVerses;

  // One verse.
  if (currentverses.size() == 1)
    return estVerse;

  // At the end of all: no type.
  return estNone;
}


void EditStatusDialog::show_status()
// Sets the status.
{
  // Get status type.
  EditStatusType statustype = editstatustype();

  // Set sensitivity of the status buttons.
  for (unsigned int i = 0; i < statusbuttons.size(); i++) {
    gtk_widget_set_sensitive(statusbuttons[i], (statustype != estNone));
  }

  // Take action based on what type of status to set.
  switch (statustype) {
  case estNone:
    {
      for (unsigned int i = 0; i < statusbuttons.size(); i++) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(statusbuttons[i]), false);
        gtk_toggle_button_set_inconsistent(GTK_TOGGLE_BUTTON(statusbuttons[i]), false);
      }
      break;
    }
  case estBook:
    {
      ProjectStatusBook *project_status_book = projectstatus->books.at(currentbooks[0]);
      ProjectStatusRecord statusrecord = project_status_book->get();
      set_status(&statusrecord);
      break;
    }
  case estBooks:
    {
      ProjectStatusRecord combinedrecord(alltasks_size);
      for (unsigned int i = 0; i < currentbooks.size(); i++) {
        ProjectStatusBook *project_status_book = projectstatus->books.at(currentbooks[i]);
        ProjectStatusRecord statusrecord = project_status_book->get();
        reporting_merge_child_status_into_parent(alltasks_size, i, combinedrecord, statusrecord);
      }
      set_status(&combinedrecord);
      break;
    }
  case estChapter:
    {
      ProjectStatusBook *project_status_book = projectstatus->books.at(currentbooks[0]);
      ProjectStatusChapter *project_status_chapter = project_status_book->chapters.at(currentchapters[0]);
      ProjectStatusRecord statusrecord = project_status_chapter->get();
      set_status(&statusrecord);
      break;
    }
  case estChapters:
    {
      ProjectStatusRecord combinedrecord(alltasks_size);
      for (unsigned int i = 0; i < currentchapters.size(); i++) {
        ProjectStatusBook *project_status_book = projectstatus->books.at(currentbooks[0]);
        ProjectStatusChapter *project_status_chapter = project_status_book->chapters.at(currentchapters[i]);
        ProjectStatusRecord statusrecord = project_status_chapter->get();
        reporting_merge_child_status_into_parent(alltasks_size, i, combinedrecord, statusrecord);
      }
      set_status(&combinedrecord);
      break;
    }
  case estVerse:
    {
      ProjectStatusBook *project_status_book = projectstatus->books.at(currentbooks[0]);
      ProjectStatusChapter *project_status_chapter = project_status_book->chapters.at(currentchapters[0]);
      int verse = currentverses[0];
      ProjectStatusRecord statusrecord = project_status_chapter->status[verse];
      set_status(&statusrecord);
      break;
    }
  case estVerses:
    {
      ProjectStatusRecord combinedrecord(alltasks_size);
      for (unsigned int i = 0; i < currentverses.size(); i++) {
        ProjectStatusBook *project_status_book = projectstatus->books.at(currentbooks[0]);
        ProjectStatusChapter *project_status_chapter = project_status_book->chapters.at(currentchapters[0]);
        int verse = currentverses[i];
        ProjectStatusRecord statusrecord = project_status_chapter->status[verse];
        reporting_merge_child_status_into_parent(alltasks_size, i, combinedrecord, statusrecord);
      }
      set_status(&combinedrecord);
      break;
    }
  }
}


void EditStatusDialog::set_status(ProjectStatusRecord * statusrecord)
{
  setting_status = true;
  for (unsigned int i = 0; i < statusrecord->tasks_done.size(); i++) {
    gtk_toggle_button_set_inconsistent(GTK_TOGGLE_BUTTON(statusbuttons[i]), false);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(statusbuttons[i]), false);
    if (statusrecord->tasks_done[i] > 0)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(statusbuttons[i]), true);
    if (statusrecord->tasks_done[i] < 0)
      gtk_toggle_button_set_inconsistent(GTK_TOGGLE_BUTTON(statusbuttons[i]), true);
  }
  setting_status = false;
}


void EditStatusDialog::on_checkbutton_status_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_checkbutton_status(togglebutton);
}


void EditStatusDialog::on_checkbutton_status(GtkToggleButton * togglebutton)
{
  // Bail out if status is being set by the program.
  if (setting_status)
    return;

  // Deal with inconsistency.
  if (gtk_toggle_button_get_inconsistent(togglebutton)) {
    gtk_toggle_button_set_inconsistent(togglebutton, false);
  }
  // Get button's offset.
  unsigned int button_offset = 0;
  for (unsigned int i = 0; i < statusbuttons.size(); i++) {
    if (GTK_TOGGLE_BUTTON(statusbuttons[i]) == togglebutton) {
      button_offset = i;
    }
  }

  // Get button's value.
  int button_value = 0;
  if (gtk_toggle_button_get_inconsistent(togglebutton)) {
    button_value = -1;
  } else {
    if (gtk_toggle_button_get_active(togglebutton)) {
      button_value = 1;
    }
  }

  // Get status type.
  EditStatusType statustype = editstatustype();

  // Take action based on what type of status to save.
  switch (statustype) {
  case estNone:
    {
      break;
    }
  case estBook:
    {
      ProjectStatusBook *project_status_book = projectstatus->books.at(currentbooks[0]);
      ProjectStatusRecord statusrecord = project_status_book->get();
      statusrecord.tasks_done[button_offset] = button_value;
      project_status_book->set(statusrecord);
      break;
    }
  case estBooks:
    {
      ProjectStatusRecord combinedrecord(alltasks_size);
      for (unsigned int i = 0; i < currentbooks.size(); i++) {
        ProjectStatusBook *project_status_book = projectstatus->books.at(currentbooks[i]);
        ProjectStatusRecord statusrecord = project_status_book->get();
        reporting_merge_child_status_into_parent(alltasks_size, i, combinedrecord, statusrecord);
      }
      combinedrecord.tasks_done[button_offset] = button_value;
      for (unsigned int i = 0; i < currentbooks.size(); i++) {
        ProjectStatusBook *project_status_book = projectstatus->books.at(currentbooks[i]);
        project_status_book->set(combinedrecord);
      }
      break;
    }
  case estChapter:
    {
      ProjectStatusBook *project_status_book = projectstatus->books.at(currentbooks[0]);
      ProjectStatusChapter *project_status_chapter = project_status_book->chapters.at(currentchapters[0]);
      ProjectStatusRecord statusrecord = project_status_chapter->get();
      statusrecord.tasks_done[button_offset] = button_value;
      project_status_chapter->set(statusrecord);
      break;
    }
  case estChapters:
    {
      ProjectStatusRecord combinedrecord(alltasks_size);
      for (unsigned int i = 0; i < currentchapters.size(); i++) {
        ProjectStatusBook *project_status_book = projectstatus->books.at(currentbooks[0]);
        ProjectStatusChapter *project_status_chapter = project_status_book->chapters.at(currentchapters[i]);
        ProjectStatusRecord statusrecord = project_status_chapter->get();
        reporting_merge_child_status_into_parent(alltasks_size, i, combinedrecord, statusrecord);
      }
      combinedrecord.tasks_done[button_offset] = button_value;
      for (unsigned int i = 0; i < currentchapters.size(); i++) {
        ProjectStatusBook *project_status_book = projectstatus->books.at(currentbooks[0]);
        ProjectStatusChapter *project_status_chapter = project_status_book->chapters.at(currentchapters[i]);
        project_status_chapter->set(combinedrecord);
      }
      break;
    }
  case estVerse:
    {
      ProjectStatusBook *project_status_book = projectstatus->books.at(currentbooks[0]);
      ProjectStatusChapter *project_status_chapter = project_status_book->chapters.at(currentchapters[0]);
      int verse = currentverses[0];
      ProjectStatusRecord statusrecord = project_status_chapter->status[verse];
      statusrecord.tasks_done[button_offset] = button_value;
      project_status_chapter->set_verse(verse, statusrecord);
      break;
    }
  case estVerses:
    {
      ProjectStatusRecord combinedrecord(alltasks_size);
      ProjectStatusBook *project_status_book = projectstatus->books.at(currentbooks[0]);
      ProjectStatusChapter *project_status_chapter = project_status_book->chapters.at(currentchapters[0]);
      for (unsigned int i = 0; i < currentverses.size(); i++) {
        int verse = currentverses[i];
        ProjectStatusRecord statusrecord = project_status_chapter->status[verse];
        reporting_merge_child_status_into_parent(alltasks_size, i, combinedrecord, statusrecord);
      }
      combinedrecord.tasks_done[button_offset] = button_value;
      for (unsigned int i = 0; i < currentverses.size(); i++) {
        int verse = currentverses[i];
        ProjectStatusRecord statusrecord = project_status_chapter->status[verse];
        statusrecord.tasks_done[button_offset] = button_value;
        project_status_chapter->set_verse(verse, statusrecord);
      }
      break;
    }
  }
}


void EditStatusDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_okbutton();
}


void EditStatusDialog::on_okbutton()
// Called on OK.
{
  // Save status.
  projectstatus->save();
}


