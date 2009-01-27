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
#include "dialogviewgit.h"
#include "help.h"
#include "utilities.h"
#include "dialoggitadd.h"
#include "progresswindow.h"
#include "gwrappers.h"
#include "git.h"

enum { COLUMN_TASK, COLUMN_PROJECT, COLUMN_DESCRIPTION, COLUMN_STATUS, NUM_COLUMNS };

ViewGitDialog::ViewGitDialog(int dummy)
{
  viewgitdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(viewgitdialog), "View Git Tasks");
  gtk_window_set_position(GTK_WINDOW(viewgitdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(viewgitdialog), TRUE);

  dialog_vbox1 = GTK_DIALOG(viewgitdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  label1 = gtk_label_new("This list shows the Git tasks that are pending.\nTasks can be deleted or added.");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label1, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_SHADOW_IN);

  treeview1 = gtk_tree_view_new();
  gtk_widget_show(treeview1);
  gtk_container_add(GTK_CONTAINER(scrolledwindow1), treeview1);
  gtk_widget_set_size_request(treeview1, -1, 400);

  hbox1 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox1, TRUE, TRUE, 0);

  buttonadd = gtk_button_new_from_stock("gtk-add");
  gtk_widget_show(buttonadd);
  gtk_box_pack_start(GTK_BOX(hbox1), buttonadd, FALSE, FALSE, 0);

  buttondelete = gtk_button_new_from_stock("gtk-delete");
  gtk_widget_show(buttondelete);
  gtk_box_pack_start(GTK_BOX(hbox1), buttondelete, FALSE, FALSE, 0);

  dialog_action_area1 = GTK_DIALOG(viewgitdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(viewgitdialog, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(viewgitdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(viewgitdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) buttonadd, "clicked", G_CALLBACK(on_buttonadd_clicked), gpointer(this));
  g_signal_connect((gpointer) buttondelete, "clicked", G_CALLBACK(on_buttondelete_clicked), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_focus(treeview1);
  gtk_widget_grab_default(okbutton);

  // Storage, renderer, column and selection.
  store = gtk_list_store_new(NUM_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview1), GTK_TREE_MODEL(store));
  g_object_unref(store);
  GtkCellRenderer *renderer;
  renderer = gtk_cell_renderer_text_new();
  column_task = gtk_tree_view_column_new_with_attributes("Task", renderer, "text", COLUMN_TASK, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview1), column_task);
  renderer = gtk_cell_renderer_text_new();
  column_project = gtk_tree_view_column_new_with_attributes("Project", renderer, "text", COLUMN_PROJECT, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview1), column_project);
  renderer = gtk_cell_renderer_text_new();
  column_description = gtk_tree_view_column_new_with_attributes("Description", renderer, "text", COLUMN_DESCRIPTION, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview1), column_description);
  renderer = gtk_cell_renderer_text_new();
  column_status = gtk_tree_view_column_new_with_attributes("Status", renderer, "text", COLUMN_STATUS, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview1), column_status);
  select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview1));
  gtk_tree_selection_set_mode(select, GTK_SELECTION_MULTIPLE);

  // GUI updater.
  reload();
  event_source_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 500, GSourceFunc(reload_timeout), gpointer(this), NULL);
}

ViewGitDialog::~ViewGitDialog()
{
  gw_destroy_source(event_source_id);
  gtk_widget_destroy(viewgitdialog);
}

int ViewGitDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(viewgitdialog));
}

bool ViewGitDialog::reload_timeout(gpointer data)
{
  ((ViewGitDialog *) data)->reload();
  return true;
}

void ViewGitDialog::reload()
{
  // Retrieve the values to load in the listview.
  vector < unsigned int >tasks;
  vector < ustring > projects;
  vector < unsigned int >books;
  vector < unsigned int >chapters;
  vector < unsigned int >fails;
  git_get_tasks(tasks, projects, books, chapters, fails);

  // Variables for iterating through the liststore. Point to first row.
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview1));
  GtkTreeIter iter;
  bool valid = gtk_tree_model_get_iter_first(model, &iter);

  // Go through all the values in the database.
  for (unsigned int i = 0; i < tasks.size(); i++) {

    // Describe task fully.
    gchar *task = NULL;
    ustring description;
    ustring status;
    git_task_human_readable(tasks[i], projects[i], books[i], chapters[i], fails[i], task, description, status);

    // Calculate the id of the values in the database.
    ustring db_id = make_id(task, projects[i], description, status);

    // If we have a valid row in the liststore, see whether to delete it or leave it in.
    bool deleted = true;
    while (valid && deleted) {
      // If the id of the row is different from the id of the database, delete it.
      ustring row_id = row_get_id(model, &iter);
      if (db_id != row_id) {
        deleted = true;
        valid = gtk_list_store_remove(store, &iter);
      } else {
        deleted = false;
      }
    }

    // If we're at the end of the liststore, add the values to it.
    if (!valid) {
      gtk_list_store_append(store, &iter);
      gchar *task = NULL;
      ustring description;
      ustring status;
      git_task_human_readable(tasks[i], projects[i], books[i], chapters[i], fails[i], task, description, status);
      gtk_list_store_set(store, &iter, COLUMN_TASK, task, COLUMN_PROJECT, projects[i].c_str(), COLUMN_DESCRIPTION, description.c_str(), COLUMN_STATUS, status.c_str(), -1);
    }
    // Get validity of next iterator.
    valid = gtk_tree_model_iter_next(model, &iter);

  }

  // Autosize columns because data may have been changed.
  gtk_tree_view_columns_autosize(GTK_TREE_VIEW(treeview1));

  // If there were no tasks, clear the store.
  if (tasks.empty()) {
    gtk_list_store_clear(store);
  }
}

void ViewGitDialog::collect_iters(GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
  ((vector < GtkTreeIter > *)data)->push_back(*iter);
}

void ViewGitDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ViewGitDialog *) user_data)->on_okbutton();
}

void ViewGitDialog::on_okbutton()
{
}

void ViewGitDialog::on_buttonadd_clicked(GtkButton * button, gpointer user_data)
{
  ((ViewGitDialog *) user_data)->on_addbutton();
}

void ViewGitDialog::on_addbutton()
{
  GitAddDialog dialog(0);
  dialog.run();
  reload();
}

void ViewGitDialog::on_buttondelete_clicked(GtkButton * button, gpointer user_data)
{
  ((ViewGitDialog *) user_data)->on_deletebutton();
}

void ViewGitDialog::on_deletebutton()
{
  // Get focused tasks.
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview1));
  vector < ustring > focused;
  vector < GtkTreeIter > iters;
  gtk_tree_selection_selected_foreach(select, ViewGitDialog::collect_iters, gpointer(&iters));
  for (unsigned int i = 0; i < iters.size(); i++) {
    GtkTreeIter iter = iters[i];
    ustring id = row_get_id(model, &iter);
    focused.push_back(id);
  }
  set < ustring > focused_set(focused.begin(), focused.end());
  // Get all tasks.
  vector < unsigned int >tasks;
  vector < ustring > projects;
  vector < unsigned int >books;
  vector < unsigned int >chapters;
  vector < unsigned int >fails;
  git_get_tasks(tasks, projects, books, chapters, fails);
  // Erase the ones focused.
  ProgressWindow progresswindow("", false);
  progresswindow.set_iterate(0, 1, tasks.size());
  for (unsigned int i = 0; i < tasks.size(); i++) {
    progresswindow.iterate();
    gchar *hrtask;
    ustring hrdesc;
    ustring hrstatus;
    git_task_human_readable(tasks[i], projects[i], books[i], chapters[i], fails[i], hrtask, hrdesc, hrstatus);
    ustring id = make_id(hrtask, projects[i], hrdesc, hrstatus);
    if (focused_set.find(id) != focused_set.end()) {
      git_erase_task((GitTaskType) tasks[i], projects[i], books[i], chapters[i]);
    }
  }
  // Clear store, load new values.  
  gtk_list_store_clear(store);
  reload();
}

ustring ViewGitDialog::row_get_id(GtkTreeModel * model, GtkTreeIter * iter)
// Gets the id of the row pointed at by iter.
{
  gchar *task;
  gchar *project;
  gchar *description;
  gchar *status;
  gtk_tree_model_get(model, iter, COLUMN_TASK, &task, COLUMN_PROJECT, &project, COLUMN_DESCRIPTION, &description, COLUMN_STATUS, &status, -1);
  ustring id = make_id(task, project, description, status);
  g_free(task);
  g_free(project);
  g_free(description);
  g_free(status);
  return id;
}

ustring ViewGitDialog::make_id(const ustring & task, const ustring & project, const ustring & description, const ustring & status)
// Makes an id out of a number of variables.
{
  ustring id(task);
  id.append(project);
  id.append(description);
  id.append(status);
  return id;
}
