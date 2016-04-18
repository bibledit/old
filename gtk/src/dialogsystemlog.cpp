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


#include "utilities.h"
#include "dialogsystemlog.h"
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
#include "unixwrappers.h"
#include <glib/gi18n.h>
#include "debug.h"

ustring log_file_name(LogFileType type, bool previous)
{
  ustring filename;
  switch (type) {
    case lftMain:     filename = "bibledit.log"; break;
    case lftDbus:     filename = "dbus.log";     break;
    case lftVCS:      filename = "vcs.log";      break;
    case lftShutdown: filename = "shutdown.log"; break;
	case lftSettings: filename = "settings.log"; break;
  }
  if (previous) {
    filename.append (_(".old"));
  }
  return gw_build_filename(Directories->get_temp(), filename);
}


void move_log_file (LogFileType type)
{
  if (g_file_test (log_file_name(type, false).c_str(), G_FILE_TEST_IS_REGULAR)) {
     unix_mv(log_file_name(type, false), log_file_name(type, true),true);
  }
}


SystemlogDialog::SystemlogDialog(int dummy)
{
  gtkbuilder = gtk_builder_new ();
  gtk_builder_add_from_file (gtkbuilder, gw_build_filename (Directories->get_package_data(), "gtkbuilder.systemlogdialog.xml").c_str(), NULL);

  Shortcuts shortcuts(0);

  dialog = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "dialog"));
  
  // Experimental - trying to make this window not modal, so it can float always to the side.
  gtk_window_set_modal(GTK_WINDOW(dialog), false);
  
  textview = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "textview"));
  //gtk_widget_grab_focus(textview); // omitting this doesn't seem to change the modality of the window

  checkbutton_session = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_session"));
  shortcuts.button (checkbutton_session);
  g_signal_connect((gpointer) checkbutton_session, "toggled", G_CALLBACK(on_checkbutton1_toggled), gpointer(this));

  GSList *radiobuttongroup = NULL;

  radiobutton_main = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_main"));
  shortcuts.button (radiobutton_main);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_main), radiobuttongroup);
  radiobuttongroup = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_main));
  g_signal_connect((gpointer) radiobutton_main, "toggled", G_CALLBACK(on_radiobutton_toggled), gpointer(this));

  radiobutton_shutdown = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_shutdown"));
  shortcuts.button (radiobutton_shutdown);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_shutdown), radiobuttongroup);
  radiobuttongroup = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_shutdown));
  g_signal_connect((gpointer) radiobutton_shutdown, "toggled", G_CALLBACK(on_radiobutton_toggled), gpointer(this));

  radiobutton_settings = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_settings"));
  shortcuts.button (radiobutton_settings);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_settings), radiobuttongroup);
  radiobuttongroup = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_settings));
  g_signal_connect((gpointer) radiobutton_settings, "toggled", G_CALLBACK(on_radiobutton_toggled), gpointer(this));
  
  /* button_diag = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "button_diag"));
  g_signal_connect((gpointer) button_diag, "clicked", G_CALLBACK(on_button_diagnostics_clicked), gpointer(this));
  shortcuts.button (button_diag);

  InDialogHelp * indialoghelp = new InDialogHelp(dialog, gtkbuilder, &shortcuts, NULL);

  GtkWidget *cancelbutton;
  cancelbutton = indialoghelp->cancelbutton;
  gtk_widget_grab_default(cancelbutton);
  shortcuts.stockbutton(cancelbutton);

  GtkWidget * okbutton = indialoghelp->okbutton;
  shortcuts.stockbutton(okbutton);
  gtk_widget_hide (okbutton); */

  shortcuts.process();

  // Load the text.
  load(true);

  // Keep loading the text repeatedly so as to show recent changes also.
  event_source_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 500, GSourceFunc(show_script_dialog_load), gpointer(this), NULL);
}


SystemlogDialog::~SystemlogDialog()
{
  // Destroy the source of the timeout (bibledit crashes if this is not done).
  gw_destroy_source(event_source_id);
  // Get rid of the dialog.
  g_object_unref (gtkbuilder);
  gtk_widget_destroy(dialog);
}


int SystemlogDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dialog));
}


bool SystemlogDialog::show_script_dialog_load(gpointer data)
{
  ((SystemlogDialog *) data)->load(false);
  // Keep going.
  return true;
}


void SystemlogDialog::load(bool force)
{
  // Text buffer. 
  GtkTextBuffer *textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));

  // Write contents of settings file (yes, I know...inefficient) (was called "diagnostics")
  if (currentLogFileType() == lftSettings) {
	  // For the main logfile, it is updated by every gw_message() and so on
	  // For the shutdown log, that is updated by itself as well
	  // Only the settings may change from time to time and I want to make sure
	  // to write those to disk and not present the user with stale info.
	  writeSettings();
  }
  
  // In cases that message keep streaming in, it may happen that the user tries to copy 
  // the messages. He selects the text, but before he can copy it to the clipboard,
  // the new text being loaded erases his selection. The result is that he fails
  // to copy the text to the clipboard. This problem is resolved by introducing the
  // "force" parameter. If force-loading, it loads regardless of any selection.
  // But if the "force" parameter is false, then it won't load if a selection
  // is present.
  if (!force) {
    if (gtk_text_buffer_get_has_selection (textbuffer))
      return;
  }

  // Read the text from the file.
  gchar *contents;
  g_file_get_contents(logfilename().c_str(), &contents, NULL, NULL);

  // If the text isn't there, clear the textbuffer, and bail out.
  if (!contents) {
    gtk_text_buffer_set_text (textbuffer, "", -1);
    return;
  }

  // Only insert the text if new text is available on disk.
  GtkTextIter begin;
  gtk_text_buffer_get_start_iter(textbuffer, &begin);
  GtkTextIter end;
  gtk_text_buffer_get_end_iter(textbuffer, &end);
  if (g_ascii_strcasecmp(contents, gtk_text_buffer_get_text(textbuffer, &begin, &end, false)) != 0) {

    // Loading a huge chunk of text would take a long time.
    // Temporally removing the view from the buffer speeds it up a huge lot.
    g_object_ref(textbuffer);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(textview), NULL);
    gtk_text_buffer_set_text(textbuffer, contents, -1);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(textview), textbuffer);
    g_object_unref(textbuffer);

    // Scroll to end;
    while (gtk_events_pending())
      gtk_main_iteration();
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)), &end);
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(textview), &end, 0, true, 0, 0);
  }
  g_free(contents);
}


void SystemlogDialog::on_checkbutton1_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((SystemlogDialog *) user_data)->load(true);
}


ustring SystemlogDialog::logfilename()
{
  LogFileType type = currentLogFileType();
  return log_file_name (type, gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton_session)));
}

LogFileType SystemlogDialog::currentLogFileType()
{
  LogFileType type = lftMain;
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_shutdown))) {
    type = lftShutdown;
  }
  else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_settings))) {
    type = lftSettings;
  }
  return type;
}

void SystemlogDialog::writeSettings()
{
	// Container to hold output text.
	vector < ustring > lines;
  
	lines.push_back(_("\nGeneral settings\n"));
    ReadText rt(general_configuration_filename(), true, false);
    for (unsigned int i = 0; i < rt.lines.size(); i++) {
      lines.push_back(rt.lines[i]);
    }
	
	vector < ustring > projects = projects_get_all();
    for (unsigned int i = 0; i < projects.size(); i++) {
      lines.push_back(_("\nProject settings for ") + projects[i] + _("\n"));
      ReadText rt(project_configuration_filename(projects[i]), true, false);
      for (unsigned int i = 0; i < rt.lines.size(); i++) {
        lines.push_back(rt.lines[i]);
      }
    }
	
	// Add the settings info to the temp directory.
	ustring settingsfile = gw_build_filename(Directories->get_temp(), _("settings.log")); // should pull settings.log out and use log_file_name, and then fix that so that it is not a stand-alone function
	write_lines(settingsfile, lines); // This closes the file when done.
	return;
}
	
#ifdef OLDSTUFF
void SystemlogDialog::on_button_diagnostics_clicked(GtkButton * button, gpointer user_data)
{
  ((SystemlogDialog *) user_data)->on_button_diagnostics();
}


void SystemlogDialog::on_button_diagnostics()
{
  // Show selection dialog.
  vector < ustring > labels;
  labels.push_back(_("General settings"));
  labels.push_back(_("Project settings"));
  CheckbuttonDialog dialog(_("Diagnostics"), _("Check the items to include in the diagnostics report"), labels, "11");
  if (dialog.run() != GTK_RESPONSE_OK)
    return;

  // Container to hold output text.
  vector < ustring > lines;

  // General settings.
  if (bitpattern_take(dialog.bitpattern)) {
    lines.push_back(_("\nGeneral settings\n"));
    ReadText rt(general_configuration_filename(), true, false);
    for (unsigned int i = 0; i < rt.lines.size(); i++) {
      lines.push_back(rt.lines[i]);
    }
  }
  // Project settings.
  if (bitpattern_take(dialog.bitpattern)) {
    vector < ustring > projects = projects_get_all();
    for (unsigned int i = 0; i < projects.size(); i++) {
      lines.push_back(_("\nProject ") + projects[i] + _(" settings\n"));
      ReadText rt(project_configuration_filename(projects[i]), true, false);
      for (unsigned int i = 0; i < rt.lines.size(); i++) {
        lines.push_back(rt.lines[i]);
      }
    }
  }
  // Add the diagnostics info to the logfile.
  ustring diagnosticsfile = gw_build_filename(Directories->get_temp(), _("diagnostics"));
  write_lines(diagnosticsfile, lines); // This closes the file when done.
  // If the logfile open is lftMain, the problem in Windows is that since fd=1 (stdout) 
  // and fd=2 (stderr) are both open to bibledit.log, Windows says
  // it cannot write to an open file. Linux and msys2 can do it just fine for some reason.
  // So, we close it, write, and re-open. Hassle, but clean way to do it.
  /* if (currentLogFileType() == lftMain) {
	  DEBUG("Temporarily closing main log file")
	  close(1); // stdout ==> bibledit.log
	  close(2); // have to close stderr also, because it points to same logfile!
  } */
  // Now do the "appending" of the diagnostics file to the log file
  shell_pipe_file_append(diagnosticsfile, logfilename());
  // Thus far works, but the log file doesn't seem to reopen
/*   if (currentLogFileType() == lftMain) {
	  int fd = open(log_file_name(lftMain, false).c_str(), O_APPEND);
	  // This should open it back up to fd=1
	  assert(fd == 1);
	  close(2);
      dup(1); // duplicate stderr(2) onto stdout(1), as in bibledit.cpp
	  DEBUG("Reopened main log file")
  } */
  unix_unlink(diagnosticsfile.c_str());
}
#endif

void SystemlogDialog::on_radiobutton_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  ((SystemlogDialog *) user_data)->on_radiobutton(togglebutton);
}


void SystemlogDialog::on_radiobutton (GtkToggleButton *togglebutton)
{
  if (gtk_toggle_button_get_active (togglebutton)) {
    load(true);
  }
}

