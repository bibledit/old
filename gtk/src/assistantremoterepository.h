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


#ifndef INCLUDED_ASSISTANT_REMOTE_REPOSITORY_H
#define INCLUDED_ASSISTANT_REMOTE_REPOSITORY_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "assistants.h"


class RemoteRepositoryAssistant : public AssistantBase
{
public:
  RemoteRepositoryAssistant(int dummy);
  virtual ~RemoteRepositoryAssistant();
private:
  // Our data.
  ustring bible;

  // Assistant page preparation.
  static void on_assistant_prepare_signal (GtkAssistant *assistant, GtkWidget *page, gpointer user_data);
  void on_assistant_prepare (GtkWidget *page);

  // Bible / notes selection.
  int page_number_bible_notes_selector;
  GtkWidget *vbox_bible_notes_selector;
  GtkWidget *radiobutton_bible_notes_selector_bible;
  GtkWidget *radiobutton_bible_notes_selector_notes;
  bool bible_notes_selector_bible ();

  // Use remote repository.
  int page_number_use_repository;
  GtkWidget *checkbutton_use_repository;

  // Git tester.
  int page_number_try_git;
  GtkWidget *label_try_git;
  bool git_tried_and_okay;
  bool try_git ();
  bool check_git_version ();
  ustring git_testing_directory (const ustring& name);
  ustring git_testing_file (const ustring& name);
  bool try_git_create_repository (const ustring& name, bool remote);
  bool try_git_store_data_in_repository (const ustring& repository, const gchar* data);
  bool try_git_check_data_in_repository (const ustring& repository, const gchar* data);
  bool try_git_fetch_repository (const ustring& remote, const ustring& local);
  bool try_git_checkout_repository (const ustring& local, const ustring& remote);
  bool try_git_pull_repository (const ustring& name);
  bool try_git_push_repository (const ustring& name);

  // Repository path and accessibility check.
  int page_number_repository;
  GtkWidget *vbox_repository;
  GtkWidget *hbox_repository;
  GtkWidget *label_repository;
  GtkWidget *entry_repository;
  GtkWidget *label_repository_accessible;
  bool ignore_entry_repository_changed;
  static void on_entry_repository_changed (GtkEditable *editable, gpointer user_data);
  void on_entry_repository ();
  guint event_id_entry_repository;
  static bool on_entry_changed_timeout(gpointer user_data);
  void entry_changed_timeout();
  ustring repository_url_get();

  // Repository cloning.
  int page_number_clone;
  GtkWidget *vbox_clone;
  GtkWidget *label_clone;
  GtkWidget *button_clone;
  static void on_button_clone_clicked (GtkButton *button, gpointer user_data);
  void on_button_clone ();
  ustring persistent_clone_directory;
  ustring previously_cloned_url;
  bool repository_was_cloned();
  void repository_unclone ();
  
  // Write test.
  int page_number_write_test;
  GtkWidget *label_write_test;
  bool write_access_granted;
  void test_write_access();

  // Pushing our data into the remote repository.
  int page_number_push;
  GtkWidget *vbox_push;
  GtkWidget *label_push;
  GtkWidget *button_push;
  static void on_button_push_clicked (GtkButton *button, gpointer user_data);
  void on_button_push ();

  // Confirmation stuff.
  int page_number_confirm;
  GtkWidget *label_confirm;
  GtkWidget *label_progress;
  GtkWidget *label_summary;
  unsigned int summary_page_number;
  static void on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data);
  void on_assistant_apply ();
  static gint assistant_forward_function (gint current_page, gpointer user_data);
  gint assistant_forward (gint current_page);
};


#endif
