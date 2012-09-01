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
#include "dialogwordlist.h"
#include "help.h"
#include "shortcuts.h"
#include "settings.h"
#include "wordlist.h"
#include "gtkwrappers.h"


WordlistDialog::WordlistDialog(int dummy)
{
  extern Settings *settings;

  Shortcuts shortcuts(0);

  wordlistdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(wordlistdialog), "Wordlists");
  gtk_window_set_position(GTK_WINDOW(wordlistdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(wordlistdialog), TRUE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG (wordlistdialog));
  gtk_widget_show(dialog_vbox1);

  label1 = gtk_label_new("This updates the word list entries in the text and generates lists of them");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label1, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hseparator1, FALSE, FALSE, 0);

  vbox2 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox2);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox2, TRUE, TRUE, 0);

  checkbutton_wordlist = gtk_check_button_new_with_mnemonic("Process general word list");
  gtk_widget_show(checkbutton_wordlist);
  gtk_box_pack_start(GTK_BOX(vbox2), checkbutton_wordlist, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_wordlist);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_wordlist), settings->genconfig.wordlist_process_general_get());

  label3 = gtk_label_new("Insert list between markers \\zopenwordlist and \\zclosewordlist");
  gtk_widget_show(label3);
  gtk_box_pack_start(GTK_BOX(vbox2), label3, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label3), 0, 0.5);

  hbox1 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(vbox2), hbox1, FALSE, FALSE, 0);

  checkbutton_asterisk_general = gtk_check_button_new_with_mnemonic("Add an asterisk to each word entry");
  gtk_widget_show(checkbutton_asterisk_general);
  gtk_box_pack_start(GTK_BOX(hbox1), checkbutton_asterisk_general, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_asterisk_general);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_asterisk_general), settings->genconfig.wordlist_general_asterisk_get());

  checkbutton_general_first_time = gtk_check_button_new_with_mnemonic("the first time it occurs in a section");
  gtk_widget_show(checkbutton_general_first_time);
  gtk_box_pack_start(GTK_BOX(hbox1), checkbutton_general_first_time, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_general_first_time);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_general_first_time), settings->genconfig.wordlist_general_asterisk_first_get());

  hseparator3 = gtk_hseparator_new();
  gtk_widget_show(hseparator3);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hseparator3, TRUE, TRUE, 0);

  vbox3 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox3);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox3, TRUE, TRUE, 0);

  checkbutton_hebrew_wordlist = gtk_check_button_new_with_mnemonic("Process Hebrew word list");
  gtk_widget_show(checkbutton_hebrew_wordlist);
  gtk_box_pack_start(GTK_BOX(vbox3), checkbutton_hebrew_wordlist, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_hebrew_wordlist);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_hebrew_wordlist), settings->genconfig.wordlist_process_hebrew_get());

  label4 = gtk_label_new("Insert the list between markers \\zopenhebrewwordlist and \\zclosehebrewwordlist");
  gtk_widget_show(label4);
  gtk_box_pack_start(GTK_BOX(vbox3), label4, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label4), 0, 0.5);

  hbox2 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox2);
  gtk_box_pack_start(GTK_BOX(vbox3), hbox2, TRUE, TRUE, 0);

  checkbutton_asterisk_hebrew = gtk_check_button_new_with_mnemonic("Add an asterisk to each word entry");
  gtk_widget_show(checkbutton_asterisk_hebrew);
  gtk_box_pack_start(GTK_BOX(hbox2), checkbutton_asterisk_hebrew, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_asterisk_hebrew);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_asterisk_hebrew), settings->genconfig.wordlist_hebrew_asterisk_get());

  checkbutton_hebrew_firsttime = gtk_check_button_new_with_mnemonic("the first time it occurs in a section");
  gtk_widget_show(checkbutton_hebrew_firsttime);
  gtk_box_pack_start(GTK_BOX(hbox2), checkbutton_hebrew_firsttime, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_hebrew_firsttime);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_hebrew_firsttime), settings->genconfig.wordlist_hebrew_asterisk_first_get());

  hseparator4 = gtk_hseparator_new();
  gtk_widget_show(hseparator4);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hseparator4, TRUE, TRUE, 0);

  vbox4 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox4);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox4, FALSE, FALSE, 0);

  checkbutton_greek_wordlist = gtk_check_button_new_with_mnemonic("Process Greek word list");
  gtk_widget_show(checkbutton_greek_wordlist);
  gtk_box_pack_start(GTK_BOX(vbox4), checkbutton_greek_wordlist, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_greek_wordlist);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_greek_wordlist), settings->genconfig.wordlist_process_greek_get());

  label5 = gtk_label_new("Insert the list between markers \\zopengreekwordlist and \\zclosegreekwordlist");
  gtk_widget_show(label5);
  gtk_box_pack_start(GTK_BOX(vbox4), label5, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label5), 0, 0.5);

  hbox3 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox3);
  gtk_box_pack_start(GTK_BOX(vbox4), hbox3, TRUE, TRUE, 0);

  checkbutton_asterisk_greek = gtk_check_button_new_with_mnemonic("Add an asterisk to each word entry");
  gtk_widget_show(checkbutton_asterisk_greek);
  gtk_box_pack_start(GTK_BOX(hbox3), checkbutton_asterisk_greek, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_asterisk_greek);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_asterisk_greek), settings->genconfig.wordlist_greek_asterisk_get());

  checkbutton_greek_first_time = gtk_check_button_new_with_mnemonic("the first time it occurs in a section");
  gtk_widget_show(checkbutton_greek_first_time);
  gtk_box_pack_start(GTK_BOX(hbox3), checkbutton_greek_first_time, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_greek_first_time);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_greek_first_time), settings->genconfig.wordlist_greek_asterisk_first_get());

  hseparator6 = gtk_hseparator_new();
  gtk_widget_show(hseparator6);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hseparator6, TRUE, TRUE, 0);

  vbox6 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox6);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox6, FALSE, FALSE, 0);

  checkbutton_index = gtk_check_button_new_with_mnemonic("Process index");
  gtk_widget_show(checkbutton_index);
  gtk_box_pack_start(GTK_BOX(vbox6), checkbutton_index, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_index);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_index), settings->genconfig.wordlist_process_index_get());

  label6 = gtk_label_new("Insert the list between markers \\zopenindex and \\zcloseindex");
  gtk_widget_show(label6);
  gtk_box_pack_start(GTK_BOX(vbox6), label6, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label6), 0, 0.5);

  hbox6 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox6);
  gtk_box_pack_start(GTK_BOX(vbox6), hbox6, TRUE, TRUE, 0);

  checkbutton_asterisk_index = gtk_check_button_new_with_mnemonic("Add an asterisk to each word entry");
  gtk_widget_show(checkbutton_asterisk_index);
  gtk_box_pack_start(GTK_BOX(hbox6), checkbutton_asterisk_index, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_asterisk_index);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_asterisk_index), settings->genconfig.wordlist_index_asterisk_get());

  checkbutton_index_first_time = gtk_check_button_new_with_mnemonic("the first time it occurs in a chapter");
  gtk_widget_show(checkbutton_index_first_time);
  gtk_box_pack_start(GTK_BOX(hbox6), checkbutton_index_first_time, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_index_first_time);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_index_first_time), settings->genconfig.wordlist_index_asterisk_first_get());

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(wordlistdialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(wordlistdialog, NULL, &shortcuts, "tools/generate-word-lists");

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(wordlistdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton), true);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(wordlistdialog), okbutton, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton), true);

  shortcuts.stockbutton(cancelbutton);
  shortcuts.stockbutton(okbutton);
  shortcuts.process();

  g_signal_connect((gpointer) checkbutton_wordlist, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_asterisk_general, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_hebrew_wordlist, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_asterisk_hebrew, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_greek_wordlist, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_asterisk_greek, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_index, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_asterisk_index, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_focus(okbutton);
  gtk_widget_grab_default(okbutton);

  gui();
}


WordlistDialog::~WordlistDialog()
{
  gtk_widget_destroy(wordlistdialog);
}


int WordlistDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(wordlistdialog));
}


void WordlistDialog::on_checkbutton_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((WordlistDialog *) user_data)->gui();
}


void WordlistDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((WordlistDialog *) user_data)->on_okbutton();
}


void WordlistDialog::on_okbutton()
{
  // Save settings in the configuration.
  extern Settings *settings;
  settings->genconfig.wordlist_process_general_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_wordlist)));
  settings->genconfig.wordlist_general_asterisk_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_asterisk_general)));
  settings->genconfig.wordlist_general_asterisk_first_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_general_first_time)));
  settings->genconfig.wordlist_process_hebrew_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_hebrew_wordlist)));
  settings->genconfig.wordlist_hebrew_asterisk_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_asterisk_hebrew)));
  settings->genconfig.wordlist_hebrew_asterisk_first_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_hebrew_firsttime)));
  settings->genconfig.wordlist_process_greek_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_greek_wordlist)));
  settings->genconfig.wordlist_greek_asterisk_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_asterisk_greek)));
  settings->genconfig.wordlist_greek_asterisk_first_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_greek_first_time)));
  settings->genconfig.wordlist_process_index_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_index)));
  settings->genconfig.wordlist_index_asterisk_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_asterisk_index)));
  settings->genconfig.wordlist_index_asterisk_first_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_index_first_time)));
  // Run the three word lists, and collect their messages.
  vector < ustring > messages;
  if (settings->genconfig.wordlist_process_general_get()) {
    Wordlist wordlist(wltGeneral);
    wordlist.run(messages);
  }
  if (settings->genconfig.wordlist_process_hebrew_get()) {
    Wordlist wordlist(wltHebrew);
    wordlist.run(messages);
  }
  if (settings->genconfig.wordlist_process_greek_get()) {
    Wordlist wordlist(wltGreek);
    wordlist.run(messages);
  }
  if (settings->genconfig.wordlist_process_index_get()) {
    Wordlist wordlist(wltIndex);
    wordlist.run(messages);
  }
  // Display messages.
  if (!messages.empty()) {
    ustring message;
    for (unsigned int i = 0; i < messages.size(); i++) {
      message.append(messages[i] + "\n");
    }
    gtkw_dialog_info(wordlistdialog, message.c_str());
  }
}


void WordlistDialog::gui()
{
  bool general = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_wordlist));
  bool hebrew = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_hebrew_wordlist));
  bool greek = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_greek_wordlist));
  bool index = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_index));

  gtk_widget_set_sensitive(label3, general);
  gtk_widget_set_sensitive(label4, hebrew);
  gtk_widget_set_sensitive(label5, greek);
  gtk_widget_set_sensitive(label6, index);

  gtk_widget_set_sensitive(checkbutton_asterisk_general, general);
  gtk_widget_set_sensitive(checkbutton_asterisk_hebrew, hebrew);
  gtk_widget_set_sensitive(checkbutton_asterisk_greek, greek);
  gtk_widget_set_sensitive(checkbutton_asterisk_index, index);

  bool general_asterisk = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_asterisk_general)) && general;
  bool hebrew_asterisk = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_asterisk_hebrew)) && hebrew;
  bool greek_asterisk = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_asterisk_greek)) && greek;
  bool index_asterisk = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_asterisk_index)) && index;

  gtk_widget_set_sensitive(checkbutton_general_first_time, general_asterisk);
  gtk_widget_set_sensitive(checkbutton_hebrew_firsttime, hebrew_asterisk);
  gtk_widget_set_sensitive(checkbutton_greek_first_time, greek_asterisk);
  gtk_widget_set_sensitive(checkbutton_index_first_time, index_asterisk);
}


