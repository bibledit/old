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
#include "dialogxfernotes2text.h"
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
#include "gui.h"
#include <glib/gi18n.h>

XferNotes2TextDialog::XferNotes2TextDialog(int dummy)
{

  notestransferdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(notestransferdialog), _("Transfer Project Notes To Text"));
  gtk_window_set_position(GTK_WINDOW(notestransferdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(notestransferdialog), TRUE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG (notestransferdialog));
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 5);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, FALSE, FALSE, 0);

  label1 = gtk_label_new(_("This will transfer all the Project Notes that display for each verse into the currently opened project"));
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 0);
  gtk_label_set_line_wrap(GTK_LABEL(label1), TRUE);
  gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

  label3 = gtk_label_new(_("The transfer cannot be undone."));
  gtk_widget_show(label3);
  gtk_box_pack_start(GTK_BOX(vbox1), label3, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label3), 0, 0.5);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(notestransferdialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(notestransferdialog, NULL, NULL, "tools/transfer-project-notes-to-text");

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(notestransferdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton), true);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(notestransferdialog), okbutton, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton), true);

  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton);
  gtk_widget_grab_focus(okbutton);
}


XferNotes2TextDialog::~XferNotes2TextDialog()
{
  gtk_widget_destroy(notestransferdialog);
}


int XferNotes2TextDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(notestransferdialog));
}


void XferNotes2TextDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((XferNotes2TextDialog *) user_data)->on_okbutton();
}


void XferNotes2TextDialog::on_okbutton()
// Do the transfer.
{
  // Double question just to be sure, as the change can have great impact.
  if (!gui_double_question(notestransferdialog, "", ""))
    return;

  // Get the project.
  extern Settings *settings;
  ustring project = settings->genconfig.project_get();

  // Progress.
  ProgressWindow progresswindow(_("Transferring notes to text"), false);

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

        // Transfer the note.
        transfer_note(project, books[bk], chapters[ch], verses[vs]);
      }
    }
  }
}


void XferNotes2TextDialog::transfer_note(const ustring & project, unsigned int book, unsigned int chapter, const ustring & verse)
// Transfer the note.
{
  // Select the set of notes for the current reference.
  ustring reference = books_id_to_english(book) + " " + convert_to_string(chapter) + ":" + verse;
  vector < unsigned int >ids;
  unsigned int id_cursor;
  notes_select(ids, id_cursor, reference);

  // Bail out if there are no notes.
  if (ids.empty())
    return;

  // Retrieve the note(s) and the verse text.
  vector < ustring > notes;
  notes_read(ids, notes);
  ustring text = project_retrieve_verse(project, book, chapter, verse);

  for (unsigned int i = 0; i < notes.size(); i++) {

    if (i)
      text.append("\n\\p ");

    ParseLine parseline(notes[i]);
    for (unsigned int i2 = 0; i2 < parseline.lines.size(); i2++) {
      text.append("\n\\p ");
      text.append(parseline.lines[i2]);
    }

  }

  // Store the modified verse text.
  project_store_verse(project, book, chapter, verse, text);
}


