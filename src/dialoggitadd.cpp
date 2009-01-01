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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/

#include "libraries.h"
#include <glib.h>
#include "dialoggitadd.h"
#include "help.h"
#include "settings.h"
#include "projectutils.h"
#include "progresswindow.h"
#include "git.h"

GitAddDialog::GitAddDialog(int dummy)
{
  gitadddialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(gitadddialog), "Add task");
  gtk_window_set_position(GTK_WINDOW(gitadddialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(gitadddialog), TRUE);

  dialog_vbox1 = GTK_DIALOG(gitadddialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  checkbutton_commit = gtk_check_button_new_with_mnemonic("Commit all local data");
  gtk_widget_show(checkbutton_commit);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_commit, FALSE, FALSE, 0);

  checkbutton_sync = NULL;
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  if (projectconfig->git_use_remote_repository_get()) {

    checkbutton_sync = gtk_check_button_new_with_mnemonic("Synchronize local data with remote repository");
    gtk_widget_show(checkbutton_sync);
    gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_sync, FALSE, FALSE, 0);

  }

  dialog_action_area1 = GTK_DIALOG(gitadddialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(gitadddialog, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(gitadddialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(gitadddialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton);
}

GitAddDialog::~GitAddDialog()
{
  gtk_widget_destroy(gitadddialog);
}

int GitAddDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(gitadddialog));
}

void GitAddDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((GitAddDialog *) user_data)->on_okbutton();
}

void GitAddDialog::on_okbutton()
{
  // Project.
  extern Settings *settings;
  ustring project = settings->genconfig.project_get();

  // Whether to commit the local data.
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_commit))) {
    git_schedule(gttCommitProject, project, 0, 0, "");
  }
  // Whether to synchronize the local data with the remote repository.
  if (checkbutton_sync && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_sync))) {
    ProjectConfiguration *projectconfig = settings->projectconfig(project);
    git_schedule(gttUpdateProject, project, 0, 0, projectconfig->git_remote_repository_url_get());
  }
}
