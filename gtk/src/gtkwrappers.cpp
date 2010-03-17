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
#include "gtkwrappers.h"
#include "gwrappers.h"
#include <glib.h>


void gtkw_dialog_info(GtkWidget * parent, const ustring & info)
{
  // Create the basic dialog.
  GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent), GtkDialogFlags(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT), GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", info.c_str());
  // Add cancel button so that pressing the Escape button will also close the window,
  // but hide this button so it's not visible, but Escape remains working.
  GtkWidget *cancel = gtk_dialog_add_button(GTK_DIALOG(dialog), "", GTK_RESPONSE_CANCEL);
  gtk_widget_hide(cancel);
  // Run dialog.
  gtk_dialog_run(GTK_DIALOG(dialog));
  // After through, destroy it.
  gtk_widget_destroy(dialog);
}


int gtkw_dialog_question(GtkWidget * parent, const ustring & question, int default_response)
{
  int result;
  GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "%s", question.c_str());
  if (default_response != GTK_RESPONSE_NO)
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), default_response);
  result = gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
  return result;
}


void gtkw_dialog_error(GtkWidget * parent, const ustring & error)
{
  GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", error.c_str());
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}


void gtkw_dialog_warning(GtkWidget * parent, const ustring & warning)
{
  GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "%s", warning.c_str());
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}


ustring gtkw_file_chooser_select_folder(GtkWidget * parent, const ustring & title, const ustring & directory)
// Allows user to select a folder.
// parent: window will be centered on parent, or NULL.
// title: If given, will be title for the dialog.
// directory: If given, will be directory selected by default.
// Returns the folder selected, or is empty if no selection was made.
{
  // Initialize variables.
  ustring selection;
  ustring mytitle(title);
  if (mytitle.empty())
    mytitle = "Select a folder";
  ustring mydirectory(directory);
  if (mydirectory.empty())
    mydirectory = g_get_home_dir();
  mydirectory.append(G_DIR_SEPARATOR_S);
  // Create dialog.
  GtkWidget *dialog;
  dialog = gtk_file_chooser_dialog_new(mytitle.c_str(), GTK_WINDOW(parent), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
  gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), mydirectory.c_str());
  if (directory.empty()) {
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), g_get_home_dir());
  }
  // Run dialog.
  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
    selection = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
  }
  // Destroy dialog
  gtk_widget_destroy(dialog);
  // Return selection.
  return selection;
}


ustring gtkw_file_chooser_open(GtkWidget * parent, const ustring & title, const ustring & file)
// Allows the user to select a file.
// parent: window will be centered on parent, or NULL.
// title: If given, will be title for the dialog.
// file: If given, will be file selected by default.
// Returns the file that was selected, or is empty if no selection was made.
{
  // Initialize variables.
  ustring selection;
  ustring mytitle(title);
  if (mytitle.empty())
    mytitle = "Select a file";
  ustring myfile(file);
  if (myfile.empty())
    myfile = g_get_home_dir();
  // Create dialog.
  GtkWidget *dialog;
  dialog = gtk_file_chooser_dialog_new(mytitle.c_str(), GTK_WINDOW(parent), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
  gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), myfile.c_str());
  if (file.empty()) {
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), g_get_home_dir());
  }
  // Run dialog.
  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
    selection = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
  }
  // Destroy dialog
  gtk_widget_destroy(dialog);
  // Return selection.
  return selection;
}


vector < ustring > gtkw_file_chooser_open_multiple(GtkWidget * parent, const ustring & title, ustring directory)
// Allows the user to select multiple files.
// parent: window will be centered on parent, or NULL.
// title: If given, will be title for the dialog.
// directory: If given, will be the directory to look in.
// Returns the files that were selected, or is empty if no selection was made.
{
  // Initialize variables.
  vector < ustring > selection;
  ustring mytitle(title);
  if (mytitle.empty())
    mytitle = "Select a file";
  // Create dialog.
  GtkWidget *dialog;
  dialog = gtk_file_chooser_dialog_new(mytitle.c_str(), GTK_WINDOW(parent), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
  if (directory.empty())
    directory = g_get_home_dir();
  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), directory.c_str());
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), true);
  // Run dialog.
  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
    GSList *filenames1 = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
    GSList *filenames2 = filenames1;
    while (filenames1) {
      selection.push_back((gchar *) filenames1->data);
      g_free(filenames1->data);
      filenames1 = filenames1->next;
    }
    g_slist_free(filenames2);
  }
  // Destroy dialog
  gtk_widget_destroy(dialog);
  // Return selection.
  return selection;
}

ustring gtkw_file_chooser_save(GtkWidget * parent, const ustring & title, const ustring & file)
// Allows the user to save to file.
// parent: window will be centered on parent, or NULL.
// title: If given, will be title for the dialog.
// file: If given, will be file saved to by default.
// Returns the file that was selected, or is empty if no selection was made.
{
  ustring selection;
  ustring mytitle(title);
  if (mytitle.empty())
    mytitle = "Save to file";
  ustring myfile(file);
  GtkWidget *dialog;
  dialog = gtk_file_chooser_dialog_new(mytitle.c_str(), GTK_WINDOW(parent), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
  gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), myfile.c_str());
  if (myfile.empty()) {
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), g_get_home_dir());
  }
  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
    selection = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
  }
  gtk_widget_destroy(dialog);
  return selection;
}
