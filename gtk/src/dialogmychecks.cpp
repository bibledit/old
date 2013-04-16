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
#include "dialogmychecks.h"
#include "help.h"
#include "utilities.h"
#include "settings.h"
#include "scripturechecks.h"
#include "shortcuts.h"


MyChecksDialog::MyChecksDialog(WindowReferences * references_window)
{
  // Save variables.
  my_references_window = references_window;

  // Shortcuts.
  Shortcuts shortcuts(0);

  // Build gui.
  mychecksdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(mychecksdialog), "My checks");
  gtk_window_set_position(GTK_WINDOW(mychecksdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(mychecksdialog), TRUE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG(mychecksdialog));
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new("Chapters and verses");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

  checkbutton_chapters_verses = gtk_check_button_new_with_mnemonic("Chapters and verses");
  gtk_widget_show(checkbutton_chapters_verses);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_chapters_verses, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_chapters_verses);

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator1, TRUE, TRUE, 0);

  label2 = gtk_label_new("Markers");
  gtk_widget_show(label2);
  gtk_box_pack_start(GTK_BOX(vbox1), label2, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label2), 0, 0.5);

  checkbutton_markers_validate = gtk_check_button_new_with_mnemonic("Validate");
  gtk_widget_show(checkbutton_markers_validate);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_markers_validate, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_markers_validate);

  checkbutton_markers_count = gtk_check_button_new_with_mnemonic("Count");
  gtk_widget_show(checkbutton_markers_count);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_markers_count, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_markers_count);

  checkbutton_markers_compare = gtk_check_button_new_with_mnemonic("Compare");
  gtk_widget_show(checkbutton_markers_compare);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_markers_compare, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_markers_compare);

  checkbutton_markers_spacing = gtk_check_button_new_with_mnemonic("Spacing");
  gtk_widget_show(checkbutton_markers_spacing);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_markers_spacing, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_markers_spacing);

  hseparator2 = gtk_hseparator_new();
  gtk_widget_show(hseparator2);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator2, TRUE, TRUE, 0);

  label3 = gtk_label_new("Characters");
  gtk_widget_show(label3);
  gtk_box_pack_start(GTK_BOX(vbox1), label3, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label3), 0, 0.5);

  checkbutton_characters_inventory = gtk_check_button_new_with_mnemonic("Inventory");
  gtk_widget_show(checkbutton_characters_inventory);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_characters_inventory, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_characters_inventory);

  checkbutton_characters_unwanted_patterns = gtk_check_button_new_with_mnemonic("Unwanted patterns");
  gtk_widget_show(checkbutton_characters_unwanted_patterns);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_characters_unwanted_patterns, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_characters_unwanted_patterns);

  hseparator3 = gtk_hseparator_new();
  gtk_widget_show(hseparator3);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator3, TRUE, TRUE, 0);

  label4 = gtk_label_new("Words");
  gtk_widget_show(label4);
  gtk_box_pack_start(GTK_BOX(vbox1), label4, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label4), 0, 0.5);

  checkbutton_words_capitalization = gtk_check_button_new_with_mnemonic("Capitalization");
  gtk_widget_show(checkbutton_words_capitalization);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_words_capitalization, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_words_capitalization);

  checkbutton_words_repetition = gtk_check_button_new_with_mnemonic("Repetition");
  gtk_widget_show(checkbutton_words_repetition);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_words_repetition, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_words_repetition);

  checkbutton_words_unwanted = gtk_check_button_new_with_mnemonic("Unwanted");
  gtk_widget_show(checkbutton_words_unwanted);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_words_unwanted, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_words_unwanted);

  checkbutton_words_inventory = gtk_check_button_new_with_mnemonic("Inventory");
  gtk_widget_show(checkbutton_words_inventory);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_words_inventory, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_words_inventory);

  hseparator4 = gtk_hseparator_new();
  gtk_widget_show(hseparator4);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator4, TRUE, TRUE, 0);

  label5 = gtk_label_new("Punctuation");
  gtk_widget_show(label5);
  gtk_box_pack_start(GTK_BOX(vbox1), label5, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label5), 0, 0.5);

  checkbutton_punctuation_matching_pairs = gtk_check_button_new_with_mnemonic("Matching pairs");
  gtk_widget_show(checkbutton_punctuation_matching_pairs);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_punctuation_matching_pairs, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_punctuation_matching_pairs);

  hseparator5 = gtk_hseparator_new();
  gtk_widget_show(hseparator5);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator5, TRUE, TRUE, 0);

  label6 = gtk_label_new("References");
  gtk_widget_show(label6);
  gtk_box_pack_start(GTK_BOX(vbox1), label6, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label6), 0, 0.5);

  checkbutton_references_inventory = gtk_check_button_new_with_mnemonic("Inventory");
  gtk_widget_show(checkbutton_references_inventory);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_references_inventory, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_references_inventory);

  checkbutton_references_validate = gtk_check_button_new_with_mnemonic("Validate");
  gtk_widget_show(checkbutton_references_validate);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_references_validate, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_references_validate);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(mychecksdialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(mychecksdialog, NULL, &shortcuts, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(mychecksdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton), true);

  shortcuts.stockbutton(cancelbutton);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(mychecksdialog), okbutton, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton), true);

  shortcuts.stockbutton(okbutton);

  shortcuts.process();

  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton);
  gtk_widget_grab_focus(okbutton);

  // Set checkbuttons.
  extern Settings *settings;
  ustring pattern = settings->genconfig.mychecks_get();
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_chapters_verses), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_markers_validate), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_markers_count), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_markers_compare), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_characters_inventory), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_characters_unwanted_patterns), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_words_capitalization), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_words_repetition), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_words_unwanted), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_words_inventory), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_punctuation_matching_pairs), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_markers_spacing), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_references_inventory), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_references_validate), bitpattern_take(pattern));
}


MyChecksDialog::~MyChecksDialog()
{
  gtk_widget_destroy(mychecksdialog);
}


int MyChecksDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(mychecksdialog));
}


void MyChecksDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((MyChecksDialog *) user_data)->on_okbutton();
}


void MyChecksDialog::on_okbutton()
{
  // Hide widget. Necessary because else the cancelling of the various checks does not work.
  gtk_widget_hide(mychecksdialog);
  // Save the state of the checkbuttons.
  ustring pattern;
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_chapters_verses)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_markers_validate)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_markers_count)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_markers_compare)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_characters_inventory)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_characters_unwanted_patterns)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_words_capitalization)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_words_repetition)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_words_unwanted)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_words_inventory)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_punctuation_matching_pairs)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_markers_spacing)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_references_inventory)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_references_validate)));
  extern Settings *settings;
  settings->genconfig.mychecks_set(pattern);
  // Collect all checking results.
  CollectCheckingResults results(0);
  // Do all the checks.
  bool keep_going = true;
  if (bitpattern_take(pattern))
    if (keep_going)
      keep_going = scripture_checks_chapters_verses(NULL, &results);
  if (bitpattern_take(pattern))
    if (keep_going)
      keep_going = scripture_checks_validate_usfms(NULL, &results);
  if (bitpattern_take(pattern))
    if (keep_going)
      keep_going = scripture_checks_count_usfms(false);
  if (bitpattern_take(pattern))
    if (keep_going)
      keep_going = scripture_checks_compare_usfms(NULL, &results);
  if (bitpattern_take(pattern))
    if (keep_going)
      keep_going = scripture_checks_count_characters(false);
  if (bitpattern_take(pattern))
    if (keep_going)
      keep_going = scripture_checks_unwanted_patterns(NULL, &results);
  if (bitpattern_take(pattern))
    if (keep_going)
      keep_going = scripture_checks_capitalization(NULL, &results);
  if (bitpattern_take(pattern))
    if (keep_going)
      keep_going = scripture_checks_repetition(NULL, &results);
  if (bitpattern_take(pattern))
    if (keep_going)
      keep_going = scripture_checks_unwanted_words(NULL, &results);
  if (bitpattern_take(pattern))
    if (keep_going)
      keep_going = scripture_checks_word_inventory(false);
  if (bitpattern_take(pattern))
    if (keep_going)
      keep_going = scripture_checks_matching_pairs(NULL, &results);
  if (bitpattern_take(pattern))
    if (keep_going)
      keep_going = scripture_checks_usfm_spacing(NULL, &results);
  if (bitpattern_take(pattern))
    if (keep_going)
      keep_going = scripture_checks_references_inventory(false);
  if (bitpattern_take(pattern))
    if (keep_going)
      keep_going = scripture_checks_validate_references(NULL, &results);
  // Display checking results.
  if (keep_going)
    checks_display_references_comments(results.references, results.comments, my_references_window);
}



