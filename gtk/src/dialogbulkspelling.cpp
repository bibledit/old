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
#include "dialogbulkspelling.h"
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
#include <glib/gi18n.h>

BulkSpellingDialog::BulkSpellingDialog(const vector <ustring> words)
{
  gtkbuilder = gtk_builder_new ();
  gtk_builder_add_from_file (gtkbuilder, gw_build_filename (Directories->get_package_data(), "gtkbuilder.bulkspellingdialog.xml").c_str(), NULL);

  Shortcuts shortcuts(0);

  dialog = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "dialog"));

  label = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label"));
  gtk_label_set_text (GTK_LABEL (label), _("All the words that are misspelled are visible below.\nClick on a word to approve the spelling."));

  vbox = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "vbox"));

  InDialogHelp * indialoghelp = new InDialogHelp(dialog, gtkbuilder, &shortcuts, NULL);
  cancelbutton = indialoghelp->cancelbutton;
  okbutton = indialoghelp->okbutton;
  gtk_widget_grab_default(okbutton);
  gtk_widget_grab_focus(okbutton);
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  // Create a button for each misspelled word.
  GtkWidget * spellbutton;
  for (unsigned int i = 0; i < words.size(); i++) {
    spellbutton = gtk_button_new_with_label(words[i].c_str());
    gtk_widget_show(spellbutton);
    gtk_box_pack_start(GTK_BOX(vbox), spellbutton, false, false, 0);
    g_signal_connect((gpointer) spellbutton, "clicked", G_CALLBACK(on_spellbutton_clicked), gpointer(this));
  }  

  shortcuts.process();
  gtk_widget_show(dialog);
  
  new DialogAutoScaler (dialog, G_MAXINT);
}


BulkSpellingDialog::~BulkSpellingDialog()
{
  g_object_unref (gtkbuilder);
  gtk_widget_destroy(dialog);
}


int BulkSpellingDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dialog));
}


void BulkSpellingDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((BulkSpellingDialog *) user_data)->on_okbutton();
}


void BulkSpellingDialog::on_okbutton()
{
}


void BulkSpellingDialog::on_spellbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((BulkSpellingDialog *) user_data)->on_spellbutton(button);
}


void BulkSpellingDialog::on_spellbutton(GtkButton *button)
{
  approved.push_back (gtk_button_get_label (button));
  gtk_widget_destroy (GTK_WIDGET (button));
}

