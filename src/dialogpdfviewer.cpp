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
#include "dialogpdfviewer.h"
#include "help.h"
#include "shortcuts.h"
#include "settings.h"
#include "pdfviewer.h"


PDFViewerDialog::PDFViewerDialog(int dummy)
{
  Shortcuts shortcuts(0);

  pdfviewerdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(pdfviewerdialog), "PDF Viewer");
  gtk_window_set_position(GTK_WINDOW(pdfviewerdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(pdfviewerdialog), TRUE);

  dialog_vbox1 = GTK_DIALOG(pdfviewerdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  GSList *radiobutton_automatic_group = NULL;

  radiobutton_automatic = gtk_radio_button_new_with_mnemonic(NULL, "Automatic");
  gtk_widget_show(radiobutton_automatic);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), radiobutton_automatic, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_automatic), radiobutton_automatic_group);
  radiobutton_automatic_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_automatic));

  shortcuts.button(radiobutton_automatic);

  radiobutton_manual = gtk_radio_button_new_with_mnemonic(NULL, "Manual");
  gtk_widget_show(radiobutton_manual);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), radiobutton_manual, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_manual), radiobutton_automatic_group);
  radiobutton_automatic_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_manual));

  shortcuts.button(radiobutton_manual);

  label_path = gtk_label_new("Path to PDF Viewer");
  gtk_widget_show(label_path);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label_path, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_path), 0, 0.5);

  shortcuts.label(label_path);

  entry_path = gtk_entry_new();
  gtk_widget_show(entry_path);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), entry_path, FALSE, FALSE, 0);

  gtk_label_set_mnemonic_widget(GTK_LABEL(label_path), entry_path);

  label_arguments = gtk_label_new("Optional arguments");
  gtk_widget_show(label_arguments);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label_arguments, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_arguments), 0, 0.5);

  shortcuts.label(label_arguments);

  entry_arguments = gtk_entry_new();
  gtk_widget_show(entry_arguments);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), entry_arguments, FALSE, FALSE, 0);

  gtk_label_set_mnemonic_widget(GTK_LABEL(label_arguments), entry_arguments);

  dialog_action_area1 = GTK_DIALOG(pdfviewerdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(pdfviewerdialog, NULL, &shortcuts, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(pdfviewerdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(pdfviewerdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton);
  shortcuts.stockbutton(okbutton);
  shortcuts.process();

  g_signal_connect((gpointer) radiobutton_automatic, "toggled", G_CALLBACK(on_radiobutton_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_manual, "toggled", G_CALLBACK(on_radiobutton_toggled), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_focus(okbutton);
  gtk_widget_grab_default(okbutton);

  // Initialize the GUI.
  extern Settings *settings;

  if (settings->genconfig.pdf_viewer_automatic_get())
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_automatic), true);
  else
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_manual), true);
  gtk_entry_set_text(GTK_ENTRY(entry_path), settings->genconfig.pdf_viewer_path_get().c_str());
  gtk_entry_set_text(GTK_ENTRY(entry_arguments), settings->genconfig.pdf_viewer_arguments_get().c_str());
  gui();
}


PDFViewerDialog::~PDFViewerDialog()
{
  gtk_widget_destroy(pdfviewerdialog);
}


int PDFViewerDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(pdfviewerdialog));
}


void PDFViewerDialog::on_radiobutton_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((PDFViewerDialog *) user_data)->on_radiobutton();
}


void PDFViewerDialog::on_radiobutton()
{
  gui();
}


void PDFViewerDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((PDFViewerDialog *) user_data)->on_okbutton();
}


void PDFViewerDialog::on_okbutton()
{
  extern Settings *settings;
  settings->genconfig.pdf_viewer_automatic_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_automatic)));
  settings->genconfig.pdf_viewer_path_set(gtk_entry_get_text(GTK_ENTRY(entry_path)));
  settings->genconfig.pdf_viewer_arguments_set(gtk_entry_get_text(GTK_ENTRY(entry_arguments)));
}


void PDFViewerDialog::gui()
{
  bool manual = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_manual));
  gtk_widget_set_sensitive(label_path, manual);
  gtk_widget_set_sensitive(entry_path, manual);
  gtk_widget_set_sensitive(label_arguments, manual);
  gtk_widget_set_sensitive(entry_arguments, manual);
}


