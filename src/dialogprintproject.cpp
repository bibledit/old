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


#include "libraries.h"
#include <glib.h>
#include "dialogprintproject.h"
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


PrintProjectDialog::PrintProjectDialog (int dummy)
{
  printprojectdialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (printprojectdialog), "Print project");
  gtk_window_set_position (GTK_WINDOW (printprojectdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal (GTK_WINDOW (printprojectdialog), TRUE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (printprojectdialog), TRUE);
  gtk_window_set_type_hint (GTK_WINDOW (printprojectdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (printprojectdialog)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 2);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new (FALSE, 4);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new ("Portion:");
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (hbox1), label1, FALSE, FALSE, 0);

  label_portion = gtk_label_new ("");
  gtk_widget_show (label_portion);
  gtk_box_pack_start (GTK_BOX (hbox1), label_portion, FALSE, FALSE, 0);

  button_portion = gtk_button_new ();
  gtk_widget_show (button_portion);
  gtk_box_pack_start (GTK_BOX (hbox1), button_portion, FALSE, FALSE, 0);

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (button_portion), alignment1);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox2);

  image1 = gtk_image_new_from_stock ("gtk-properties", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox2), image1, FALSE, FALSE, 0);

  label5 = gtk_label_new_with_mnemonic ("Ch_ange");
  gtk_widget_show (label5);
  gtk_box_pack_start (GTK_BOX (hbox2), label5, FALSE, FALSE, 0);

  dialog_action_area1 = GTK_DIALOG (printprojectdialog)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (printprojectdialog, NULL, NULL);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (printprojectdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (printprojectdialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  g_signal_connect ((gpointer) button_portion, "clicked",
                    G_CALLBACK (on_button_portion_clicked),
                    gpointer(this));
  g_signal_connect ((gpointer) okbutton1, "clicked",
                    G_CALLBACK (on_okbutton1_clicked),
                    gpointer(this));

  gtk_widget_grab_focus (okbutton1);
  gtk_widget_grab_default (okbutton1);
  
  // Set gui.
  gui_reorder_include ();
}


PrintProjectDialog::~PrintProjectDialog ()
{
  gtk_widget_destroy (printprojectdialog);
}


int PrintProjectDialog::run ()
{
  return gtk_dialog_run (GTK_DIALOG (printprojectdialog));
}


void PrintProjectDialog::on_okbutton1_clicked (GtkButton *button, gpointer user_data)
{
  ((PrintProjectDialog *) user_data)->on_okbutton ();
}


void PrintProjectDialog::on_okbutton ()
{
}


void PrintProjectDialog::on_button_portion_clicked (GtkButton *button, gpointer user_data)
{
  ((PrintProjectDialog *) user_data)->on_button_portion ();
}


void PrintProjectDialog::on_button_portion ()
{
  SelectBooksDialog dialog (true);
  if (dialog.run () == GTK_RESPONSE_OK) {
    gui_reorder_include ();
  }
}


void PrintProjectDialog::gui_reorder_include ()
// Set labels whether the order of the books is standard and everything is included.
{
  extern Settings * settings;
  ScripturePortions scriptureportions (settings->genconfig.project_get());
  if (scriptureportions.reordering_portions_all ())
    gtk_label_set_text (GTK_LABEL (label_portion), CHAPTER_VERSE_SELECTION_ALL);
  else
    gtk_label_set_text (GTK_LABEL (label_portion), "part");
}
