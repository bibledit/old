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

#include "libraries.h"
#include <glib.h>
#include "dialognotestransfer.h"
#include "notes_utils.h"
#include "utilities.h"
#include "combobox.h"
#include "projectutils.h"
#include "settings.h"
#include "progresswindow.h"
#include "gtkwrappers.h"
#include "date_time_utils.h"
#include "usfmtools.h"
#include "help.h"
#include "books.h"
#include "tiny_utilities.h"


NotesTransferDialog::NotesTransferDialog(int dummy)
{

  notestransferdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(notestransferdialog), "Transfer To Project Notes");
  gtk_window_set_position(GTK_WINDOW(notestransferdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(notestransferdialog), TRUE);

  dialog_vbox1 = GTK_DIALOG(notestransferdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 5);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, FALSE, FALSE, 0);

  label1 = gtk_label_new("This will transfer all text from the currently opened project into the Project notes.");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 0);
  gtk_label_set_line_wrap(GTK_LABEL(label1), TRUE);
  gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

  hbox1 = gtk_hbox_new(FALSE, 5);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox1, TRUE, TRUE, 0);

  label2 = gtk_label_new("The notes will be put into category");
  gtk_widget_show(label2);
  gtk_box_pack_start(GTK_BOX(hbox1), label2, FALSE, FALSE, 0);

  combobox1 = gtk_combo_box_new_text();
  gtk_widget_show(combobox1);
  gtk_box_pack_start(GTK_BOX(hbox1), combobox1, TRUE, TRUE, 0);

  label3 = gtk_label_new("The transfer cannot be undone.");
  gtk_widget_show(label3);
  gtk_box_pack_start(GTK_BOX(vbox1), label3, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label3), 0, 0.5);

  dialog_action_area1 = GTK_DIALOG(notestransferdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(notestransferdialog, NULL, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(notestransferdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(notestransferdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  // Read all the categories and add amy new ones from the database.
  ReadText rt(notes_categories_filename());
  notes_categories_add_from_database(rt.lines);
  combobox_set_strings(combobox1, rt.lines);
  if (!rt.lines.empty())
    combobox_set_string(combobox1, rt.lines[0]);
}


NotesTransferDialog::~NotesTransferDialog()
{
  gtk_widget_destroy(notestransferdialog);
}


int NotesTransferDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(notestransferdialog));
}


void NotesTransferDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((NotesTransferDialog *) user_data)->on_okbutton();
}


void NotesTransferDialog::on_okbutton()
{
  // Get the project.
  extern Settings *settings;
  ustring project = settings->genconfig.project_get();

  // Progress.
  ProgressWindow progresswindow("Transferring text to notes", false);

  // Get the category into which to insert notes.
  ustring category = combobox_get_active_string(combobox1);

  // Go through the books in the project.
  vector < unsigned int >books = project_get_books(project);
  for (unsigned int bk = 0; bk < books.size(); bk++) {

    // Progress.
    progresswindow.set_text(books_id_to_english(books[bk]));

    // Go through the chapters in this book. Progress.
    vector < unsigned int >chapters = project_get_chapters(project, books[bk]);
    progresswindow.set_iterate(0, 1, chapters.size());
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {
      progresswindow.iterate();

      // Go through the verses in this chapter.
      vector < ustring > verses = project_get_verses(project, books[bk], chapters[ch]);
      for (unsigned int vs = 0; vs < verses.size(); vs++) {

        // Retrieve each verse and insert it into the notes.
        ustring text = project_retrieve_verse(project, books[bk], chapters[ch], verses[vs]);
        clean_note(text);
        if (!text.empty()) {
          transfer_note(project, books[bk], chapters[ch], verses[vs], text, category);
        }

      }
    }
  }
}


void NotesTransferDialog::clean_note(ustring & note)
// "Cleans" the note: removes unnecessary stuff.
{
  ustring working_copy(note);
  ustring marker = usfm_extract_marker(working_copy);
  if (marker == "v") {
    usfm_extract_marker(note);
    note = trim(note);
    ustring verse = number_in_string(note);
    note.erase(0, verse.length());
    note = trim(note);
  }
}


void NotesTransferDialog::transfer_note(const ustring & project, unsigned int book, unsigned int chapter, const ustring & verse, ustring & text, const ustring & category)
{
  int id = notes_database_get_unique_id();
  ustring references = books_id_to_osis (book) + "." + convert_to_string (chapter) + "." + verse;
  int date_created = date_time_julian_day_get_current();
  ustring user = g_get_real_name();
  ustring logbook;
  notes_store_one_in_file(id, text, project, references, category, date_created, user, date_time_julian_day_get_current(), logbook);
}
