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
#include "dialogoriginreferences.h"
#include "gtkwrappers.h"
#include "help.h"
#include "originreferences.h"
#include "settings.h"
#include "shortcuts.h"


OriginReferencesDialog::OriginReferencesDialog(int dummy)
{
  extern Settings *settings;

  Shortcuts shortcuts(0);

  originreferencesdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(originreferencesdialog), "Bible notes mass update");
  gtk_window_set_position(GTK_WINDOW(originreferencesdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(originreferencesdialog), TRUE);

  dialog_vbox1 = GTK_DIALOG(originreferencesdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  label3 = gtk_label_new("This tool can update all the footnotes, endnotes and crossreferences in the project.");
  gtk_widget_show(label3);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label3, FALSE, FALSE, 0);
  gtk_label_set_line_wrap(GTK_LABEL(label3), TRUE);
  gtk_misc_set_alignment(GTK_MISC(label3), 0, 0.5);

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hseparator1, TRUE, TRUE, 0);

  label4 = gtk_label_new("Types of note to consider:");
  gtk_widget_show(label4);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label4, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label4), 0, 0.5);

  checkbutton_footnotes = gtk_check_button_new_with_mnemonic("Footnotes");
  gtk_widget_show(checkbutton_footnotes);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_footnotes, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_footnotes);

  checkbutton_endnotes = gtk_check_button_new_with_mnemonic("Endnotes");
  gtk_widget_show(checkbutton_endnotes);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_endnotes, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_endnotes);

  checkbutton_xrefs = gtk_check_button_new_with_mnemonic("Crossreferences");
  gtk_widget_show(checkbutton_xrefs);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_xrefs, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_xrefs);

  hseparator2 = gtk_hseparator_new();
  gtk_widget_show(hseparator2);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hseparator2, FALSE, FALSE, 0);

  label5 = gtk_label_new("Action to take:");
  gtk_widget_show(label5);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label5, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label5), 0, 0.5);

  GSList *radiobutton_remove_group = NULL;

  radiobutton_nothing = gtk_radio_button_new_with_mnemonic(NULL, "_Make no changes but give statistics only");
  gtk_widget_show(radiobutton_nothing);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), radiobutton_nothing, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_nothing), radiobutton_remove_group);
  radiobutton_remove_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_nothing));

  shortcuts.button(checkbutton_xrefs);

  radiobutton_remove = gtk_radio_button_new_with_mnemonic(NULL, "_Remove all the references");
  gtk_widget_show(radiobutton_remove);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), radiobutton_remove, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_remove), radiobutton_remove_group);
  radiobutton_remove_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_remove));

  shortcuts.button(checkbutton_xrefs);

  hbox1 = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox1, FALSE, FALSE, 0);

  radiobutton_add = gtk_radio_button_new_with_mnemonic(NULL, "Add references");
  gtk_widget_show(radiobutton_add);
  gtk_box_pack_start(GTK_BOX(hbox1), radiobutton_add, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_add), radiobutton_remove_group);
  radiobutton_remove_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_add));

  shortcuts.button(radiobutton_add);

  checkbutton_book = gtk_check_button_new_with_mnemonic("Book");
  gtk_widget_show(checkbutton_book);
  gtk_box_pack_start(GTK_BOX(hbox1), checkbutton_book, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_book);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_book), settings->session.bnmu_add_book);

  checkbutton_chapter = gtk_check_button_new_with_mnemonic("Chapter");
  gtk_widget_show(checkbutton_chapter);
  gtk_box_pack_start(GTK_BOX(hbox1), checkbutton_chapter, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_chapter);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_chapter), settings->session.bnmu_add_chapter);

  entry_dot = gtk_entry_new();
  gtk_widget_show(entry_dot);
  gtk_box_pack_start(GTK_BOX(hbox1), entry_dot, FALSE, FALSE, 0);
  gtk_entry_set_max_length(GTK_ENTRY(entry_dot), 1);
  gtk_entry_set_invisible_char(GTK_ENTRY(entry_dot), 8226);
  gtk_entry_set_width_chars(GTK_ENTRY(entry_dot), 2);

  gtk_entry_set_text(GTK_ENTRY(entry_dot), settings->session.bnmu_verse_prefix.c_str());

  checkbutton_verse = gtk_check_button_new_with_mnemonic("Verse");
  gtk_widget_show(checkbutton_verse);
  gtk_box_pack_start(GTK_BOX(hbox1), checkbutton_verse, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_verse);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_verse), settings->session.bnmu_add_verse);

  entry2 = gtk_entry_new();
  gtk_widget_show(entry2);
  gtk_box_pack_start(GTK_BOX(hbox1), entry2, FALSE, FALSE, 0);
  gtk_entry_set_max_length(GTK_ENTRY(entry2), 3);
  gtk_entry_set_invisible_char(GTK_ENTRY(entry2), 8226);
  gtk_entry_set_width_chars(GTK_ENTRY(entry2), 2);

  gtk_entry_set_text(GTK_ENTRY(entry2), settings->session.bnmu_verse_suffix.c_str());

  vseparator1 = gtk_vseparator_new();
  gtk_widget_show(vseparator1);
  gtk_box_pack_start(GTK_BOX(hbox1), vseparator1, FALSE, FALSE, 0);

  label_example = gtk_label_new("");
  gtk_widget_show(label_example);
  gtk_box_pack_start(GTK_BOX(hbox1), label_example, FALSE, FALSE, 0);

  radiobutton_text_label = gtk_radio_button_new_with_mnemonic(NULL, "Add missing _text labels");
  gtk_widget_show(radiobutton_text_label);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), radiobutton_text_label, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_text_label), radiobutton_remove_group);
  radiobutton_remove_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_text_label));

  dialog_action_area1 = GTK_DIALOG(originreferencesdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(originreferencesdialog, NULL, &shortcuts, "tools/bible-notes-mass-update");

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(originreferencesdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(originreferencesdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton);
  shortcuts.stockbutton(okbutton);
  shortcuts.process();

  g_signal_connect((gpointer) radiobutton_nothing, "toggled", G_CALLBACK(on_radiobutton_action_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_remove, "toggled", G_CALLBACK(on_radiobutton_action_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_add, "toggled", G_CALLBACK(on_radiobutton_action_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_text_label, "toggled", G_CALLBACK(on_radiobutton_action_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_book, "toggled", G_CALLBACK(on_checkbutton_reference_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_chapter, "toggled", G_CALLBACK(on_checkbutton_reference_toggled), gpointer(this));
  g_signal_connect((gpointer) entry_dot, "changed", G_CALLBACK(on_entry_reference_changed), gpointer(this));
  g_signal_connect((gpointer) entry2, "changed", G_CALLBACK(on_entry_reference_changed), gpointer(this));
  g_signal_connect((gpointer) checkbutton_verse, "toggled", G_CALLBACK(on_checkbutton_reference_toggled), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton);

  on_radiobutton_action();
  on_reference();
}


OriginReferencesDialog::~OriginReferencesDialog()
{
  gtk_widget_destroy(originreferencesdialog);
}


int OriginReferencesDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(originreferencesdialog));
}


void OriginReferencesDialog::on_radiobutton_action_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((OriginReferencesDialog *) user_data)->on_radiobutton_action();
}


void OriginReferencesDialog::on_radiobutton_action()
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_nothing)))
    action = oratNothing;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_remove)))
    action = oratRemoveReferences;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_add)))
    action = oratAddReference;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_text_label)))
    action = oratTextLabels;
  bool sensitive = (action == oratAddReference);
  gtk_widget_set_sensitive(checkbutton_book, sensitive);
  gtk_widget_set_sensitive(checkbutton_chapter, sensitive);
  gtk_widget_set_sensitive(entry_dot, sensitive);
  gtk_widget_set_sensitive(checkbutton_verse, sensitive);
  gtk_widget_set_sensitive(entry2, sensitive);
  gtk_widget_set_sensitive(label_example, sensitive);
}


void OriginReferencesDialog::on_checkbutton_reference_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((OriginReferencesDialog *) user_data)->on_reference();
}


void OriginReferencesDialog::on_entry_reference_changed(GtkEditable * editable, gpointer user_data)
{
  ((OriginReferencesDialog *) user_data)->on_reference();
}


void OriginReferencesDialog::on_reference()
{
  ustring exampletext = origin_reference_produce(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_book)),
                                                 1,
                                                 "English",
                                                 gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_chapter)),
                                                 1,
                                                 gtk_entry_get_text(GTK_ENTRY(entry_dot)),
                                                 gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_verse)),
                                                 "1",
                                                 gtk_entry_get_text(GTK_ENTRY(entry2)),
                                                 true);
  gtk_label_set_text(GTK_LABEL(label_example), exampletext.c_str());
}


void OriginReferencesDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((OriginReferencesDialog *) user_data)->on_okbutton();
}


void OriginReferencesDialog::on_okbutton()
{
  // Double warning in case of an edit.  
  if (action != oratNothing) {
    if (gtkw_dialog_question(originreferencesdialog, "Are you sure you wish to edit the notes?") == GTK_RESPONSE_NO)
      return;
    if (gtkw_dialog_question(originreferencesdialog, "This will permanently modify your project.\nThe changes cannot be undone.\nAre you sure?") == GTK_RESPONSE_NO)
      return;
  }
  // Store a few important settings in the session.
  extern Settings *settings;
  settings->session.bnmu_add_book = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_book));
  settings->session.bnmu_add_chapter = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_chapter));
  settings->session.bnmu_add_verse = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_verse));
  settings->session.bnmu_verse_prefix = gtk_entry_get_text(GTK_ENTRY(entry_dot));
  settings->session.bnmu_verse_suffix = gtk_entry_get_text(GTK_ENTRY(entry2));

  // Do the thing.
  OriginReferences originreferences(settings->genconfig.project_get(),
                                    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_footnotes)),
                                    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_endnotes)), gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_xrefs)), action, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_book)), gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_chapter)), gtk_entry_get_text(GTK_ENTRY(entry_dot)), gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_verse)), gtk_entry_get_text(GTK_ENTRY(entry2)), true);
}



