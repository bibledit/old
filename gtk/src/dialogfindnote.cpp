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


#include "libraries.h"
#include <glib.h>
#include "dialogfindnote.h"
#include "utilities.h"
#include <sqlite3.h>
#include "sqlite_reader.h"
#include "notes_utils.h"
#include "completion.h"
#include "gwrappers.h"
#include "help.h"
#include "settings.h"
#include "tiny_utilities.h"


FindNoteDialog::FindNoteDialog(int dummy)
{
  extern Settings *settings;

  findnotedialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(findnotedialog), "Find in project notes");
  gtk_window_set_position(GTK_WINDOW(findnotedialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(findnotedialog), TRUE);
  // Next one added to Glade's code.
  gtk_window_set_destroy_with_parent(GTK_WINDOW(findnotedialog), TRUE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG(findnotedialog));
  gtk_widget_show(dialog_vbox1);

  hbox2 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox2);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox2, TRUE, TRUE, 0);

  label5 = gtk_label_new("Search for");
  gtk_widget_show(label5);
  gtk_box_pack_start(GTK_BOX(hbox2), label5, FALSE, FALSE, 4);

  entry1 = gtk_entry_new();
  // Next one has been added to Glade's code.
  gtk_entry_set_text(GTK_ENTRY(entry1), settings->session.searchword.c_str());
  gtk_widget_show(entry1);
  gtk_box_pack_start(GTK_BOX(hbox2), entry1, TRUE, TRUE, 0);
  gtk_entry_set_activates_default(GTK_ENTRY(entry1), TRUE);

  checkbutton_case = gtk_check_button_new_with_mnemonic("Case _sensitive");
  gtk_widget_show(checkbutton_case);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_case, FALSE, FALSE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_case), settings->session.search_case_sensitive);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(findnotedialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(findnotedialog, NULL, NULL, NULL);

  buttonfind = gtk_button_new_from_stock("gtk-find");
  gtk_widget_show(buttonfind);
  gtk_dialog_add_action_widget(GTK_DIALOG(findnotedialog), buttonfind, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (buttonfind), true);

  buttoncancel = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(buttoncancel);
  gtk_dialog_add_action_widget(GTK_DIALOG(findnotedialog), buttoncancel, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (buttoncancel), true);

  // The next one modified from Glade's code.
  g_signal_connect((gpointer) buttonfind, "clicked", G_CALLBACK(findnotedialog_on_buttonfind_clicked), gpointer(this));
  g_signal_connect((gpointer) entry1, "changed", G_CALLBACK(on_word_entry_changed), gpointer(this));

  gtk_widget_grab_focus(entry1);
  gtk_widget_grab_default(buttonfind);

  // Entry completion
  completion_setup(entry1, cpSearch);

  set_gui();
}


FindNoteDialog::~FindNoteDialog()
{
  gtk_widget_destroy(findnotedialog);
}


int FindNoteDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(findnotedialog));
}


void FindNoteDialog::findnotedialog_on_buttonfind_clicked(GtkButton * button, gpointer user_data)
{
  ((FindNoteDialog *) user_data)->on_buttonfind_clicked();
}


void FindNoteDialog::on_buttonfind_clicked()
{
  extern Settings *settings;

  // Connect to database.
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    rc = sqlite3_open(notes_index_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);

    // Get the word to search for.
    settings->session.searchword = gtk_entry_get_text(GTK_ENTRY(entry1));
    settings->session.search_case_sensitive = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_case));

    // Get the string to search for.
    // We need to normalize the search expression, as prescribed, when comparing strings.
    ustring searchword_verbatim = settings->session.searchword.normalize();
    ustring searchword_casefold = searchword_verbatim.casefold();

    // Go through the whole index database.
    SqliteReader reader(0);
    char *sql;
    sql = g_strdup_printf("select id, casefolded from notes;");
    rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
    g_free(sql);
    if (rc != SQLITE_OK) {
      throw runtime_error(error);
    }
    for (unsigned int i = 0; i < reader.ustring0.size(); i++) {

      // See whether the case-insensitive word to search for is in this note.
      ustring casefolded_note = reader.ustring1[i];
      if (casefolded_note.find(searchword_casefold) != string::npos) {

        // Read the note from disk.
        gint32 id = convert_to_int(reader.ustring0[i]);
        ustring note;
        ustring project;
        ustring fullreference;
        ustring category;
        int date_created;
        ustring user_created;
        int date_modified;
        ustring logbook;
        notes_read_one_from_file (id, note, project, fullreference, category, date_created, user_created, date_modified, logbook);

        // If case sensitive search, do an additional test.
        if (settings->session.search_case_sensitive) {
          if (note.find (searchword_verbatim) == string::npos) {
            continue;
          }
        }

        // Get the numerical equivalent of the reference.
        ustring reference = fullreference;
        {
          // Parse the string into its possible several references.
          Parse parse(reference);
          // Take the first refence available.
          if (parse.words.size() > 0)
            reference = parse.words[0];
        }

        // Store data.
        ids.push_back(id);
        references.push_back(reference);
        allreferences.push_back(fullreference);
        dates.push_back(date_modified);
      }
    }

    // Sort the notes.
    notes_sort(ids, references, allreferences, dates);
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }

  // Close connection.  
  sqlite3_close(db);

  // Entry completion
  completion_finish(entry1, cpSearch);
}


void FindNoteDialog::on_word_entry_changed(GtkEditable * editable, gpointer user_data)
{
  ((FindNoteDialog *) user_data)->set_gui();
}


void FindNoteDialog::set_gui()
{
  string searchword = gtk_entry_get_text(GTK_ENTRY(entry1));
  gtk_widget_set_sensitive(buttonfind, searchword.size() > 0);
}


