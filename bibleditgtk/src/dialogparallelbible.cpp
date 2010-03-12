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
#include "dialogparallelbible.h"
#include "utilities.h"
#include "projectutils.h"
#include "directories.h"
#include "combobox.h"
#include "dialogselectchapters.h"
#include "portion_utils.h"
#include "settings.h"
#include "help.h"
#include "books.h"
#include "shortcuts.h"
#include "tiny_utilities.h"
#include "gwrappers.h"
#include "screen.h"


ParallelBibleDialog::ParallelBibleDialog(int dummy)
{
  gtkbuilder = gtk_builder_new ();
  gtk_builder_add_from_file (gtkbuilder, gw_build_filename (directories_get_package_data(), "gtkbuilder.parallelbibledialog.xml").c_str(), NULL);

  Shortcuts shortcuts(0);
  extern Settings *settings;

  dialog = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "dialog"));

  label_main_project = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_main_project"));
  gtk_label_set_text (GTK_LABEL (label_main_project), settings->genconfig.project_get().c_str());

  label_book = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_book"));
  gtk_label_set_text (GTK_LABEL (label_book), books_id_to_english(settings->genconfig.book_get()).c_str());

  label_chapters = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_chapters"));
  vector <unsigned int> chapters_from, chapters_to;
  vector <ustring> verses_from, verses_to;
  select_portion_get_values(settings->genconfig.project_get(), settings->genconfig.book_get(), settings->genconfig.parallel_bible_chapters_verses_get(), chapters_from, verses_from, chapters_to, verses_to);
  ustring label;
  for (unsigned int i = 0; i < chapters_from.size(); i++) {
    if (i)
      label.append(" | ");
    label.append(select_portion_get_label(settings->genconfig.project_get(), settings->genconfig.book_get(), chapters_from[i], verses_from[i], chapters_to[i], verses_to[i]));
  }
  gtk_label_set_text(GTK_LABEL(label_chapters), label.c_str());

  button_chapters = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "button_chapters"));
  g_signal_connect((gpointer) button_chapters, "clicked", G_CALLBACK(on_button_chapters_clicked), gpointer(this));

  label_button_chapters = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_button_chapters"));
  shortcuts.label(label_button_chapters);

  hbox_select_bibles_gui = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "hbox_select_bibles_gui"));
  select_bibles_gui = new SelectBiblesGui (hbox_select_bibles_gui, shortcuts);
  vector <ustring> bibles = settings->genconfig.parallel_bible_projects_get();
  vector <bool> enabled = settings->genconfig.parallel_bible_enabled_get();
  select_bibles_gui->set (enabled, bibles);
  
  checkbutton_keep_together = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_keep_together"));
  shortcuts.button(checkbutton_keep_together);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_keep_together), settings->genconfig.parallel_bible_keep_verses_together_get());

  checkbutton_include_verse0 = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_include_verse0"));
  shortcuts.button(checkbutton_include_verse0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_include_verse0), settings->genconfig.parallel_bible_include_verse_zero_get());

  InDialogHelp * indialoghelp = new InDialogHelp(dialog, gtkbuilder, &shortcuts, NULL);
  cancelbutton = indialoghelp->cancelbutton;
  okbutton = indialoghelp->okbutton;
  gtk_widget_grab_default(okbutton);
  gtk_widget_grab_focus(okbutton);
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  shortcuts.process();
  gtk_widget_show(dialog);
  
  new DialogAutoScaler (dialog, G_MAXINT);
}


ParallelBibleDialog::~ParallelBibleDialog()
{
  delete select_bibles_gui;
  g_object_unref (gtkbuilder);
  gtk_widget_destroy(dialog);
}


int ParallelBibleDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dialog));
}


void ParallelBibleDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ParallelBibleDialog *) user_data)->on_okbutton();
}

void ParallelBibleDialog::on_okbutton()
{
  // Save values.
  extern Settings *settings;
  vector <bool> enabled;
  vector <ustring> bibles;
  select_bibles_gui->get (enabled, bibles);
  settings->genconfig.parallel_bible_projects_set(bibles);
  settings->genconfig.parallel_bible_enabled_set(enabled);
  settings->genconfig.parallel_bible_keep_verses_together_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_keep_together)));
  settings->genconfig.parallel_bible_chapters_verses_set(gtk_label_get_text(GTK_LABEL(label_chapters)));
  settings->genconfig.parallel_bible_include_verse_zero_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_include_verse0)));
}


void ParallelBibleDialog::on_button_chapters_clicked(GtkButton * button, gpointer user_data)
{
  ((ParallelBibleDialog *) user_data)->on_button_chapters();
}


void ParallelBibleDialog::on_button_chapters()
{
  extern Settings *settings;
  SelectChaptersDialog dialog(settings->genconfig.project_get(), settings->genconfig.book_get(), gtk_label_get_text(GTK_LABEL(label_chapters)));
  if (dialog.run() == GTK_RESPONSE_OK) {
    gtk_label_set_text(GTK_LABEL(label_chapters), dialog.new_selection.c_str());
  }
}


