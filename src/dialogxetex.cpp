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


#include "dialogxetex.h"
#include "utilities.h"
#include "bible.h"
#include "usfm.h"
#include "usfmtools.h"
#include "xmlutils.h"
#include <time.h>
#include "settings.h"
#include "scriptureportions.h"
#include "help.h"
#include "dialogselectbooks.h"
#include "shortcuts.h"
#include "gwrappers.h"
#include "directories.h"


XeTeXDialog::XeTeXDialog(int dummy)
{
  extern Settings *settings;

  gtkbuilder = gtk_builder_new ();
  gtk_builder_add_from_file (gtkbuilder, gw_build_filename (directories_get_package_data(), "gtkbuilder.xetexdialog.xml").c_str(), NULL);

  dialog = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "dialog"));

  label_portion = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_portion"));

  button_portion = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "button_portion"));
  g_signal_connect((gpointer) button_portion, "clicked", G_CALLBACK(on_button_portion_clicked), gpointer(this));

  expander = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "expander"));
  gtk_expander_set_expanded(GTK_EXPANDER(expander), settings->session.print_dialog_options_expanded);

  label_expander = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_expander"));

  notebook = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "notebook"));
  g_signal_connect_after((gpointer) notebook, "switch_page", G_CALLBACK(on_notebook_switch_page), gpointer(this));

  label_tab_notes = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_tab_notes"));
  checkbutton_full_references = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_full_references"));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_full_references), settings->session.print_references_in_notes_in_full);
  gtk_widget_set_sensitive (checkbutton_full_references, false);

  label_tab_page = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_tab_page"));
  checkbutton_cropmarks = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_cropmarks"));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_cropmarks), settings->session.print_crop_marks);

  InDialogHelp * indialoghelp = new InDialogHelp(dialog, gtkbuilder, NULL, "file/print/project");
  cancelbutton = indialoghelp->cancelbutton;
  okbutton = indialoghelp->okbutton;
  gtk_widget_grab_focus(okbutton);
  gtk_widget_grab_default(okbutton);
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  set_gui();
}


XeTeXDialog::~XeTeXDialog()
{
  gtk_widget_destroy(dialog);
}


int XeTeXDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dialog));
}


void XeTeXDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((XeTeXDialog *) user_data)->on_okbutton();
}


void XeTeXDialog::on_okbutton()
{
  extern Settings *settings;
  settings->session.print_dialog_options_expanded = gtk_expander_get_expanded(GTK_EXPANDER(expander));
  settings->session.print_references_in_notes_in_full = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_full_references));
  settings->session.print_crop_marks = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_cropmarks));
}


void XeTeXDialog::on_button_portion_clicked(GtkButton * button, gpointer user_data)
{
  ((XeTeXDialog *) user_data)->on_button_portion();
}


void XeTeXDialog::on_button_portion()
{
  SelectBooksDialog dialog(true);
  if (dialog.run() == GTK_RESPONSE_OK) {
    set_gui();
  }
}


void XeTeXDialog::set_gui()
{
  // Portion indicator.
  extern Settings *settings;
  ScripturePortions scriptureportions(settings->genconfig.project_get());
  if (scriptureportions.reordering_portions_all())
    gtk_label_set_text(GTK_LABEL(label_portion), CHAPTER_VERSE_SELECTION_ALL);
  else
    gtk_label_set_text(GTK_LABEL(label_portion), "part");
  // Keyboard accelerators.
  Shortcuts shortcuts(0);
  shortcuts.button (button_portion);
  shortcuts.label(label_expander);
  shortcuts.label (label_tab_notes);
  shortcuts.label (label_tab_page);
  shortcuts.stockbutton(cancelbutton);
  shortcuts.stockbutton(okbutton);
  GtkWidget * helpbutton = gtk_button_new_from_stock (GTK_STOCK_HELP);
  shortcuts.stockbutton(helpbutton);
  int page = gtk_notebook_get_current_page (GTK_NOTEBOOK (notebook));
  switch (page) {
    case 0:
    {
      // Notes page.
      shortcuts.button(checkbutton_full_references);
      break;
    }
    case 1:
    {
      // Page settings.
      shortcuts.button(checkbutton_cropmarks);
      break;
    }
  }
  shortcuts.process();
  gtk_widget_destroy (helpbutton);
}


void XeTeXDialog::on_notebook_switch_page (GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data)
{
  ((XeTeXDialog *) user_data)->on_notebook(page_num);
}


void XeTeXDialog::on_notebook (guint page_num)
{
  set_gui();
}

