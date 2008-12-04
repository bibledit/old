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

#include "utilities.h"
#include "dialogshowscript.h"
#include <glib.h>
#include "constants.h"
#include "directories.h"
#include "gwrappers.h"
#include "help.h"
#include "shortcuts.h"
#include "dialogcheckbutton.h"
#include "shell.h"
#include "projectutils.h"
#include "generalconfig.h"
#include "settings.h"

ShowScriptDialog::ShowScriptDialog(int dummy) {
  Shortcuts shortcuts(0);

  showscriptdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW (showscriptdialog), "Logfile");
  gtk_window_set_position(GTK_WINDOW (showscriptdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW (showscriptdialog), TRUE);
  gtk_window_set_default_size(GTK_WINDOW (showscriptdialog), 640, 480);
  gtk_window_set_type_hint(GTK_WINDOW (showscriptdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox = GTK_DIALOG (showscriptdialog)->vbox;
  gtk_widget_show(dialog_vbox);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX (dialog_vbox), vbox1, TRUE, TRUE, 0);

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_box_pack_start(GTK_BOX (vbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  textview1 = gtk_text_view_new();
  gtk_widget_show(textview1);
  gtk_container_add(GTK_CONTAINER (scrolledwindow1), textview1);
  gtk_text_view_set_editable(GTK_TEXT_VIEW (textview1), FALSE);

  checkbutton1 = gtk_check_button_new_with_mnemonic("Previous session");
  gtk_widget_show(checkbutton1);
  gtk_box_pack_start(GTK_BOX (vbox1), checkbutton1, FALSE, FALSE, 0);

  shortcuts.button(checkbutton1);

  hbox1 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX (vbox1), hbox1, FALSE, FALSE, 0);

  GSList *radiobutton_bibledit_group= NULL;

  radiobutton_bibledit = gtk_radio_button_new_with_mnemonic(NULL, "General");
  gtk_widget_show(radiobutton_bibledit);
  gtk_box_pack_start(GTK_BOX (hbox1), radiobutton_bibledit, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON (radiobutton_bibledit), radiobutton_bibledit_group);
  radiobutton_bibledit_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON (radiobutton_bibledit));

  shortcuts.button(radiobutton_bibledit);

  radiobutton_bibletime = gtk_radio_button_new_with_mnemonic(NULL, "BibleTime");
  gtk_widget_show(radiobutton_bibletime);
  gtk_box_pack_start(GTK_BOX (hbox1), radiobutton_bibletime, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON (radiobutton_bibletime), radiobutton_bibledit_group);
  radiobutton_bibledit_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON (radiobutton_bibletime));

  shortcuts.button(radiobutton_bibletime);

  radiobutton_git = gtk_radio_button_new_with_mnemonic(NULL, "Git");
  gtk_widget_show(radiobutton_git);
  gtk_box_pack_start(GTK_BOX (hbox1), radiobutton_git, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON (radiobutton_git), radiobutton_bibledit_group);
  radiobutton_bibledit_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON (radiobutton_git));

  shortcuts.button(radiobutton_git);

  button_diagnostics = gtk_button_new();
  gtk_widget_show(button_diagnostics);
  gtk_box_pack_start(GTK_BOX (hbox1), button_diagnostics, FALSE, FALSE, 0);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER (button_diagnostics), alignment1);

  hbox2 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox2);
  gtk_container_add(GTK_CONTAINER (alignment1), hbox2);

  image1 = gtk_image_new_from_stock("gtk-preferences", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX (hbox2), image1, FALSE, FALSE, 0);

  label1 = gtk_label_new_with_mnemonic("Diagnostics");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX (hbox2), label1, FALSE, FALSE, 0);

  shortcuts.label(label1);

  dialog_action_area1 = GTK_DIALOG (showscriptdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (showscriptdialog, &shortcuts, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG (showscriptdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton);

  shortcuts.process();

  g_signal_connect ((gpointer) checkbutton1, "toggled", G_CALLBACK (on_checkbutton1_toggled), gpointer(this));
  g_signal_connect ((gpointer) button_diagnostics, "clicked", G_CALLBACK (on_button_diagnostics_clicked), gpointer(this));

  gtk_widget_grab_focus(textview1);
  gtk_widget_grab_default(cancelbutton);

  // Load the text.
  load();

  // Keep loading the text repeatedly so as to show recent changes also.
  event_source_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 500, GSourceFunc (show_script_dialog_load), gpointer(this), NULL);
}

ShowScriptDialog::~ShowScriptDialog() {
  // Destroy the source of the timeout (bibledit crashes if this is not done).
  gw_destroy_source(event_source_id);
  // Get rid of the dialog.
  gtk_widget_destroy(showscriptdialog);
}

int ShowScriptDialog::run() {
  return gtk_dialog_run(GTK_DIALOG (showscriptdialog));
}

bool ShowScriptDialog::show_script_dialog_load(gpointer data) {
  return ((ShowScriptDialog *) data)->load();
}

bool ShowScriptDialog::load() {
  // Read the text from the file.
  gchar *contents;
  g_file_get_contents(logfilename().c_str(), &contents, NULL, NULL);

  // Text buffer. 
  GtkTextBuffer * textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (textview1));

  // Only insert the text if new text is available on disk.
  GtkTextIter begin;
  gtk_text_buffer_get_start_iter(textbuffer, &begin);
  GtkTextIter end;
  gtk_text_buffer_get_end_iter(textbuffer, &end);
  if (g_ascii_strcasecmp (contents, gtk_text_buffer_get_text (textbuffer, &begin, &end, false)) != 0) {

    // Loading a huge chunk of text would take a long time.
    // Temporally removing the buffer from the view speeds it up a huge lot.
    g_object_ref(textbuffer);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW (textview1), NULL);
    gtk_text_buffer_set_text (textbuffer, contents, -1);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW (textview1), textbuffer);
    g_object_unref(textbuffer);

    // Scroll to end;
    while (gtk_events_pending()) gtk_main_iteration ();
    GtkTextIter end;
    gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview1)), &end);
    gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW (textview1), &end, 0, true, 0, 0);
  }
  g_free(contents);
  // Do it again next time.
  return true;
}

void ShowScriptDialog::on_checkbutton1_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
  ((ShowScriptDialog *) user_data)->load();
}

void ShowScriptDialog::on_radiobutton_bibledit_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
  ((ShowScriptDialog *) user_data)->load();
}

void ShowScriptDialog::on_radiobutton_bibletime_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
  ((ShowScriptDialog *) user_data)->load();
}

void ShowScriptDialog::on_radiobutton_git_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
  ((ShowScriptDialog *) user_data)->load();
}

ustring ShowScriptDialog::logfilename() {
  ustring filename;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (radiobutton_bibledit))) {
    filename = "bibledit.log";
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (radiobutton_bibletime))) {
    filename = "bibletime.log";
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (radiobutton_git))) {
    filename = "git.log";
  }
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (checkbutton1))) {
    filename.append(".old");
  }
  return gw_build_filename(directories_get_temp(), filename);
}

void ShowScriptDialog::on_button_diagnostics_clicked(GtkButton *button, gpointer user_data) {
  ((ShowScriptDialog *) user_data)->on_button_diagnostics();
}

void ShowScriptDialog::on_button_diagnostics() {
  // Show selection dialog.
  vector <ustring> labels;
  labels.push_back("General settings");
  labels.push_back("Project settings");
  CheckbuttonDialog dialog("Diagnostics", "Tick the items to include in the diagnostics report", labels, "11");
  if (dialog.run() != GTK_RESPONSE_OK)
    return;

  // Container to hold output text.
  vector <ustring> lines;

  // General settings.
  if (bitpattern_take(dialog.bitpattern)) {
    lines.push_back("\nGeneral settings\n");
    ReadText rt(general_configuration_filename(), true, false);
    for (unsigned int i = 0; i < rt.lines.size(); i++) {
      lines.push_back(rt.lines[i]);
    }
  }

  // Project settings.
  if (bitpattern_take(dialog.bitpattern)) {
    vector <ustring> projects = projects_get_all();
    for (unsigned int i = 0; i < projects.size(); i++) {
      lines.push_back("\nProject " + projects[i] + " settings\n");
      ReadText rt(project_configuration_filename(projects[i]), true, false);
      for (unsigned int i = 0; i < rt.lines.size(); i++) {
        lines.push_back(rt.lines[i]);
      }
    }
  }

  // Add the diagnostics info to the logfile.
  ustring diagnosticsfile = gw_build_filename(directories_get_temp(), "diagnostics");
  write_lines(diagnosticsfile, lines);
  shell_pipe_file_append(diagnosticsfile, logfilename());
  unlink(diagnosticsfile.c_str());
}
