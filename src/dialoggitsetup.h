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

#ifndef INCLUDED_DIALOGGITSETUP_H
#define INCLUDED_DIALOGGITSETUP_H

#include <gtk/gtk.h>

class GitSetupDialog
{
public:
  GitSetupDialog(int dummy);
  ~GitSetupDialog();
  int run();
protected:
  GtkWidget *gitsetupdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *checkbutton_remote;
  GtkWidget *hbox20;
  GtkWidget *label_path;
  GtkWidget *entry_path;
  GtkWidget *hseparator1;
  GtkWidget *hbox25;
  GtkWidget *image_version;
  GtkWidget *label_version_done;
  GtkWidget *label_version_info;
  GtkWidget *hseparator9;
  GtkWidget *hbox21;
  GtkWidget *image_tasks_running_ok;
  GtkWidget *label_tasks_running_ok;
  GtkWidget *label_tasks_running_info;
  GtkWidget *hseparator5;
  GtkWidget *hbox22;
  GtkWidget *image_repository_accessible_ok;
  GtkWidget *label_repository_accessible_done;
  GtkWidget *label_repository_accessible_info;
  GtkWidget *label_repository_accessible_output;
  GtkWidget *hseparator6;
  GtkWidget *hbox23;
  GtkWidget *image_repository_writable;
  GtkWidget *label_repository_writable_done;
  GtkWidget *label_repository_writable_info;
  GtkWidget *label_repository_writable_output;
  GtkWidget *hseparator7;
  GtkWidget *hbox28;
  GtkWidget *image_action;
  GtkWidget *label_action_done;
  GtkWidget *label_action_info;
  GtkWidget *vbox5;
  GtkWidget *button_copy;
  GtkWidget *button_synchronize;
  GtkWidget *hseparator11;
  GtkWidget *hbox24;
  GtkWidget *image_pull;
  GtkWidget *label_pull_done;
  GtkWidget *label_pull_info;
  GtkWidget *label_pull_output;
  GtkWidget *hseparator8;
  GtkWidget *hbox27;
  GtkWidget *image_push;
  GtkWidget *label_push_done;
  GtkWidget *label_push_info;
  GtkWidget *label_push_output;
  GtkWidget *hseparator10;
  GtkWidget *hbox19;
  GtkWidget *label_pull_push;
  GtkObject *spinbutton_update_adj;
  GtkWidget *spinbutton_update;
  GtkWidget *label55;
  GtkWidget *vbox4;
  GtkWidget *label53;
  GtkWidget *radiobutton_conflict_me;
  GtkWidget *radiobutton_conflict_server;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  static void on_checkbutton_remote_toggled(GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_remote();
  static void on_entry_changed(GtkEditable *editable, gpointer user_data);
  void on_entry();
  guint event_id_tasks_ready;
  guint event_id_gui;
  static bool delayer(gpointer user_data);
  void delayed();
  void copy_repository_to_project();
  void set_gui();
  static bool tasks_counter_timeout(gpointer user_data);
  void tasks_counter();
  int project_tasks_count;
  int previous_project_tasks_count;
  static void on_okbutton_clicked(GtkButton *button, gpointer user_data);
  void on_okbutton();
  bool version_okay;
  ustring url_get();
  bool test_read_access(ustring& error);
  bool test_write_access(ustring& error);
  bool test_pull_changes(ustring& error);
  bool test_push_changes(ustring& error);
  bool test_clone_repository(ustring& error);
  static void on_button_action_clicked(GtkButton *button, gpointer user_data);
  void button_action_clicked(GtkButton *button);
  bool action_copy;
  bool action_sync;
  
};

#endif
