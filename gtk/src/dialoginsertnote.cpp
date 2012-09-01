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
#include "dialoginsertnote.h"
#include "utilities.h"
#include "directories.h"
#include "combobox.h"
#include "dialogentry.h"
#include "gwrappers.h"
#include "notetemplate.h"
#include "usfmtools.h"
#include "stylesheetutils.h"
#include "shell.h"
#include "settings.h"
#include <gdk/gdkkeysyms.h>
#include "help.h"
#include "tidy.h"
#include "screen.h"
#include "tiny_utilities.h"


InsertNoteDialog::InsertNoteDialog(NoteType dialogtype)
{
  extern Settings *settings;
  mydialogtype = dialogtype;
  allow_entry_activate = true;

  insertnotedialog = gtk_dialog_new();
  ustring title = "Insert ";
  switch (dialogtype) {
  case indtFootnote:
    title.append("footnote");
    break;
  case indtEndnote:
    title.append("endnote");
    break;
  case indtCrossreference:
    title.append("crossreference");
    break;
  }
  gtk_window_set_title(GTK_WINDOW(insertnotedialog), title.c_str());
  gtk_window_set_position(GTK_WINDOW(insertnotedialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(insertnotedialog), TRUE);
  gtk_window_set_resizable(GTK_WINDOW(insertnotedialog), FALSE);
  gtk_window_set_destroy_with_parent(GTK_WINDOW(insertnotedialog), TRUE);
  gtk_window_set_type_hint(GTK_WINDOW(insertnotedialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG(insertnotedialog));
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox1, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(hbox1), 1);

  label1 = gtk_label_new_with_mnemonic("N_umbering");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(hbox1), label1, FALSE, FALSE, 0);

  GSList *radiobutton_numbering_automatic_group = NULL;

  radiobutton_numbering_automatic = gtk_radio_button_new_with_mnemonic(NULL, "Automatic");
  gtk_widget_show(radiobutton_numbering_automatic);
  gtk_box_pack_start(GTK_BOX(hbox1), radiobutton_numbering_automatic, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_numbering_automatic), radiobutton_numbering_automatic_group);
  radiobutton_numbering_automatic_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_numbering_automatic));

  radiobutton_numbering_none = gtk_radio_button_new_with_mnemonic(NULL, "None");
  gtk_widget_show(radiobutton_numbering_none);
  gtk_box_pack_start(GTK_BOX(hbox1), radiobutton_numbering_none, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_numbering_none), radiobutton_numbering_automatic_group);
  radiobutton_numbering_automatic_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_numbering_none));

  radiobutton_numbering_character = gtk_radio_button_new_with_mnemonic(NULL, "Character");
  gtk_widget_show(radiobutton_numbering_character);
  gtk_box_pack_start(GTK_BOX(hbox1), radiobutton_numbering_character, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_numbering_character), radiobutton_numbering_automatic_group);
  radiobutton_numbering_automatic_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_numbering_character));

  entry_numbering = gtk_entry_new();
  gtk_widget_show(entry_numbering);
  gtk_box_pack_start(GTK_BOX(hbox1), entry_numbering, false, false, 0);
  gtk_entry_set_max_length(GTK_ENTRY(entry_numbering), 3);
  gtk_entry_set_activates_default(GTK_ENTRY(entry_numbering), TRUE);
  gtk_entry_set_width_chars(GTK_ENTRY(entry_numbering), 3);

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator1, TRUE, TRUE, 6);

  checkbutton_automatic_reference = gtk_check_button_new_with_mnemonic("Automatically insert the current _reference");
  gtk_widget_show(checkbutton_automatic_reference);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_automatic_reference, FALSE, FALSE, 0);

  hbox2 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox2);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox2, FALSE, FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(hbox2), 1);

  checkbutton_chapter = gtk_check_button_new_with_mnemonic("Cha_pter");
  gtk_widget_show(checkbutton_chapter);
  gtk_box_pack_start(GTK_BOX(hbox2), checkbutton_chapter, FALSE, FALSE, 0);

  entry_reference_separator = gtk_entry_new();
  gtk_widget_show(entry_reference_separator);
  gtk_box_pack_start(GTK_BOX(hbox2), entry_reference_separator, FALSE, FALSE, 0);
  gtk_entry_set_max_length(GTK_ENTRY(entry_reference_separator), 3);
  gtk_entry_set_width_chars(GTK_ENTRY(entry_reference_separator), 3);

  checkbutton_verse = gtk_check_button_new_with_mnemonic("_Verse");
  gtk_widget_show(checkbutton_verse);
  gtk_box_pack_start(GTK_BOX(hbox2), checkbutton_verse, FALSE, FALSE, 0);

  entry_reference_suffix = gtk_entry_new();
  gtk_widget_show(entry_reference_suffix);
  gtk_box_pack_start(GTK_BOX(hbox2), entry_reference_suffix, FALSE, FALSE, 0);
  gtk_entry_set_max_length(GTK_ENTRY(entry_reference_suffix), 5);
  gtk_entry_set_width_chars(GTK_ENTRY(entry_reference_suffix), 5);

  vseparator1 = gtk_vseparator_new();
  gtk_widget_show(vseparator1);
  gtk_box_pack_start(GTK_BOX(hbox2), vseparator1, FALSE, FALSE, 0);

  label_automatic_reference = gtk_label_new("");
  gtk_widget_show(label_automatic_reference);
  gtk_box_pack_start(GTK_BOX(hbox2), label_automatic_reference, FALSE, FALSE, 0);

  // Add content button.

  hseparator5 = gtk_hseparator_new();
  gtk_widget_show(hseparator5);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator5, TRUE, TRUE, 6);

  hbox6 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox6);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox6, FALSE, TRUE, 0);

  button_add = gtk_button_new();
  gtk_widget_show(button_add);
  gtk_box_pack_start(GTK_BOX(hbox6), button_add, FALSE, FALSE, 0);

  alignment2 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment2);
  gtk_container_add(GTK_CONTAINER(button_add), alignment2);

  hbox7 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox7);
  gtk_container_add(GTK_CONTAINER(alignment2), hbox7);

  image2 = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image2);
  gtk_box_pack_start(GTK_BOX(hbox7), image2, FALSE, FALSE, 0);

  label4 = gtk_label_new_with_mnemonic("_Add content");
  gtk_widget_show(label4);
  gtk_box_pack_start(GTK_BOX(hbox7), label4, FALSE, FALSE, 0);

  // Templates.

  hseparator3 = gtk_hseparator_new();
  gtk_widget_show(hseparator3);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator3, TRUE, TRUE, 6);

  label2 = gtk_label_new_with_mnemonic("_Templates");
  gtk_widget_show(label2);
  gtk_box_pack_start(GTK_BOX(vbox1), label2, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label2), 0, 0.5);

  hbox3 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox3);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox3, TRUE, TRUE, 0);

  combobox_templates = gtk_combo_box_new_text();
  gtk_widget_show(combobox_templates);
  gtk_box_pack_start(GTK_BOX(hbox3), combobox_templates, TRUE, TRUE, 0);
  gtk_widget_set_can_default (GTK_WIDGET (combobox_templates), true);

  button_template_new = gtk_button_new_from_stock("gtk-new");
  gtk_widget_show(button_template_new);
  gtk_box_pack_start(GTK_BOX(hbox3), button_template_new, FALSE, FALSE, 0);

  button_template_delete = gtk_button_new_from_stock("gtk-delete");
  gtk_widget_show(button_template_delete);
  gtk_box_pack_start(GTK_BOX(hbox3), button_template_delete, FALSE, FALSE, 0);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(insertnotedialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(insertnotedialog, NULL, NULL, "menu-none/insert-footnote-endnote-crossreference");

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(insertnotedialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton1), true);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(insertnotedialog), okbutton1, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton1), true);

  g_signal_connect((gpointer) radiobutton_numbering_automatic, "toggled", G_CALLBACK(on_radiobutton_numbering_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_numbering_none, "toggled", G_CALLBACK(on_radiobutton_numbering_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_numbering_character, "toggled", G_CALLBACK(on_radiobutton_numbering_toggled), gpointer(this));
  g_signal_connect((gpointer) entry_numbering, "activate", G_CALLBACK(on_entry_numbering_activate), gpointer(this));
  g_signal_connect((gpointer) checkbutton_automatic_reference, "toggled", G_CALLBACK(on_checkbutton_automatic_reference_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_chapter, "toggled", G_CALLBACK(on_checkbutton_chapter_toggled), gpointer(this));
  g_signal_connect((gpointer) entry_reference_separator, "changed", G_CALLBACK(on_entry_reference_separator_changed), gpointer(this));
  g_signal_connect((gpointer) entry_reference_separator, "activate", G_CALLBACK(on_entry_reference_separator_activate), gpointer(this));
  g_signal_connect((gpointer) checkbutton_verse, "toggled", G_CALLBACK(on_checkbutton_verse_toggled), gpointer(this));
  g_signal_connect((gpointer) entry_reference_suffix, "changed", G_CALLBACK(on_entry_reference_suffix_changed), gpointer(this));
  g_signal_connect((gpointer) entry_reference_suffix, "activate", G_CALLBACK(on_entry_reference_suffix_activate), gpointer(this));
  g_signal_connect((gpointer) button_add, "clicked", G_CALLBACK(on_button_add_clicked), gpointer(this));
  g_signal_connect((gpointer) combobox_templates, "changed", G_CALLBACK(on_combobox_templates_changed), gpointer(this));
  g_signal_connect((gpointer) button_template_new, "clicked", G_CALLBACK(on_button_template_new_clicked), gpointer(this));
  g_signal_connect((gpointer) button_template_delete, "clicked", G_CALLBACK(on_button_template_delete_clicked), gpointer(this));
  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));

  gtk_label_set_mnemonic_widget(GTK_LABEL(label2), combobox_templates);

  // Load available content markers.
  vector < Style > styles;
  stylesheet_get_styles(settings->genconfig.stylesheet_get(), styles);
  for (unsigned int i = 0; i < styles.size(); i++) {
    switch (dialogtype) {
    case indtFootnote:
    case indtEndnote:
      {
        if (styles[i].type == stFootEndNote) {
          if ((styles[i].subtype == fentStandardContent)
              || (styles[i].subtype == fentContent)
              || (styles[i].subtype == fentContentWithEndmarker)
              || (styles[i].subtype == fentParagraph)) {
            content_markers.push_back(styles[i].marker);
            content_endmarkers.push_back(styles[i].subtype == fentContentWithEndmarker);
            content_names.push_back(styles[i].name);
          }
        }
        break;
      }
    case indtCrossreference:
      {
        if (styles[i].type == stCrossreference) {
          if ((styles[i].subtype == ctStandardContent)
              || (styles[i].subtype == ctContent)
              || (styles[i].subtype == ctContentWithEndmarker)) {
            content_markers.push_back(styles[i].marker);
            content_endmarkers.push_back(styles[i].subtype == ctContentWithEndmarker);
            content_names.push_back(styles[i].name);
          }
        }
        break;
      }
    }
  }

  // Set gui.
  ustring template_name;
  switch (dialogtype) {
  case indtFootnote:
    template_name = settings->genconfig.insert_footnote_template_get();
    break;
  case indtEndnote:
    template_name = settings->genconfig.insert_endnote_template_get();
    break;
  case indtCrossreference:
    template_name = settings->genconfig.insert_xref_template_get();
    break;
  }
  templates_load(template_name, true);
  on_combobox_templates();

  dialog_position_restore(dptInsertNote, insertnotedialog);
}


InsertNoteDialog::~InsertNoteDialog()
{
  dialog_position_save(dptInsertNote, insertnotedialog);
  gtk_widget_destroy(insertnotedialog);
}


int InsertNoteDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(insertnotedialog));
}


void InsertNoteDialog::on_radiobutton_numbering_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((InsertNoteDialog *) user_data)->on_radiobutton_numbering(togglebutton);
}


void InsertNoteDialog::on_radiobutton_numbering(GtkToggleButton * togglebutton)
{
  gtk_label_set_mnemonic_widget(GTK_LABEL(label1), GTK_WIDGET(togglebutton));
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_numbering_character))) {
    gtk_widget_set_sensitive(entry_numbering, true);
  } else {
    gtk_widget_set_sensitive(entry_numbering, false);
  }
}


int InsertNoteDialog::numbering_get()
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_numbering_none)))
    return ntNone;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_numbering_character)))
    return ntCharacter;
  return ntAutomatic;
}


void InsertNoteDialog::numbering_set(int numbering)
{
  NumberingType numberingtype = (NumberingType) numbering;
  switch (numberingtype) {
  case ntAutomatic:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_numbering_automatic), true);
    on_radiobutton_numbering(GTK_TOGGLE_BUTTON(radiobutton_numbering_automatic));
    break;
  case ntNone:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_numbering_none), true);
    on_radiobutton_numbering(GTK_TOGGLE_BUTTON(radiobutton_numbering_none));
    break;
  case ntCharacter:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_numbering_character), true);
    on_radiobutton_numbering(GTK_TOGGLE_BUTTON(radiobutton_numbering_character));
    break;
  }

}


void InsertNoteDialog::on_entry_numbering_activate(GtkEntry * entry, gpointer user_data)
{
  ((InsertNoteDialog *) user_data)->focus_next_widget(GTK_WIDGET(entry));
}


void InsertNoteDialog::on_checkbutton_automatic_reference_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((InsertNoteDialog *) user_data)->on_checkbutton_automatic_reference();
}


void InsertNoteDialog::on_checkbutton_automatic_reference()
{
  bool active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_automatic_reference));
  gtk_widget_set_sensitive(checkbutton_chapter, active);
  gtk_widget_set_sensitive(checkbutton_verse, active);
  gtk_widget_set_sensitive(entry_reference_suffix, active);
  gtk_widget_set_sensitive(label_automatic_reference, active);
  on_checkbutton_chapter();
  on_checkbutton_verse();
}


void InsertNoteDialog::on_checkbutton_chapter_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((InsertNoteDialog *) user_data)->on_checkbutton_chapter();
}


void InsertNoteDialog::on_checkbutton_chapter()
{
  separator_sensitive_set();
  reference_get();
}


void InsertNoteDialog::separator_sensitive_set()
{
  bool sensitive = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_automatic_reference));
  if (sensitive)
    sensitive = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_chapter));
  if (sensitive)
    sensitive = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_verse));
  gtk_widget_set_sensitive(entry_reference_separator, sensitive);
}


void InsertNoteDialog::on_entry_reference_separator_changed(GtkEditable * editable, gpointer user_data)
{
  ((InsertNoteDialog *) user_data)->on_entry_reference_separator_changed2();
}


void InsertNoteDialog::on_entry_reference_separator_changed2()
{
  reference_get();
}


void InsertNoteDialog::on_entry_reference_separator_activate(GtkEntry * entry, gpointer user_data)
{
  ((InsertNoteDialog *) user_data)->focus_next_widget(GTK_WIDGET(entry));
}


void InsertNoteDialog::on_checkbutton_verse_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((InsertNoteDialog *) user_data)->on_checkbutton_verse();
}


void InsertNoteDialog::on_checkbutton_verse()
{
  separator_sensitive_set();
  reference_get();
}


void InsertNoteDialog::on_entry_reference_suffix_changed(GtkEditable * editable, gpointer user_data)
{
  ((InsertNoteDialog *) user_data)->on_entry_reference_suffix_changed2();
}


void InsertNoteDialog::on_entry_reference_suffix_changed2()
{
  reference_get();
}


void InsertNoteDialog::on_entry_reference_suffix_activate(GtkEntry * entry, gpointer user_data)
{
  ((InsertNoteDialog *) user_data)->focus_next_widget(GTK_WIDGET(entry));
}


ustring InsertNoteDialog::reference_get()
// Returns the reference as is going to be inserted in the note.
// Also set the label accordingly.
{
  // Variable for result.
  ustring result;
  // Configuration
  extern Settings *settings;
  // If the separator widget is sensitive, get the chapter and verse.
  if (gtk_widget_get_sensitive (entry_reference_separator)) {
    result = settings->genconfig.chapter_get()
        + gtk_entry_get_text(GTK_ENTRY(entry_reference_separator))
        + settings->genconfig.verse_get();
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_chapter))) {
    result = settings->genconfig.chapter_get();
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_verse))) {
    result = settings->genconfig.verse_get();
  }
  // Add the suffix.
  result.append(gtk_entry_get_text(GTK_ENTRY(entry_reference_suffix)));
  // See whether we to insert it.
  if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_automatic_reference)))
    result.clear();
  // Set the label.
  ustring label;
  if (!result.empty())
    label = "(" + result + ")";
  gtk_label_set_text(GTK_LABEL(label_automatic_reference), label.c_str());
  // Return result.
  return result;
}


void InsertNoteDialog::on_button_content_clicked(GtkButton * button, gpointer user_data)
{
  ((InsertNoteDialog *) user_data)->on_button_content_remove(button);
}


void InsertNoteDialog::on_button_content_remove(GtkButton * button)
{
  // Get the offset of the widget to remove.
  int widget_offset = -1;
  for (unsigned int i = 0; i < buttons_remove.size(); i++) {
    if (GTK_WIDGET(button) == buttons_remove[i])
      widget_offset = i;
  }
  // Widget not found: bail out.
  if (widget_offset < 0)
    return;
  // Destroy the widgets.
  gtk_widget_destroy(vbox2s[widget_offset]);
  // Remove widget's pointers.
  vector_remove_content(vbox2s, widget_offset);
  vector_remove_content(combobox_styles, widget_offset);
  vector_remove_content(spinbutton_sizes, widget_offset);
  vector_remove_content(scrolledwindow_contents, widget_offset);
  vector_remove_content(textview_contents, widget_offset);
  vector_remove_content(checkbutton_contents, widget_offset);
  vector_remove_content(checkbutton_tidies, widget_offset);
  vector_remove_content(buttons_remove, widget_offset);
  vector_remove_content(buttons_remove_label, widget_offset);
  // Update shotcuts.
  set_dynamic_shortcuts();
}


void InsertNoteDialog::vector_remove_content(vector < GtkWidget * >&container, int offset)
{
  vector < GtkWidget * >::iterator iter;
  iter = container.begin();
  for (int i = 0; i < offset; i++)
    iter++;
  container.erase(iter);
}


void InsertNoteDialog::on_button_add_clicked(GtkButton * button, gpointer user_data)
{
  ((InsertNoteDialog *) user_data)->on_button_add();
}


void InsertNoteDialog::on_button_add()
// Adds a new content entry to the gui.
{
  GtkWidget *vbox2;
  GtkWidget *hseparator4;
  GtkWidget *hbox8;
  GtkWidget *combobox_style;
  GtkWidget *label_size;
  GtkObject *spinbutton_size_adj;
  GtkWidget *spinbutton_size;
  GtkWidget *checkbutton_content;
  GtkWidget *vseparator2;
  GtkWidget *checkbutton_tidy;
  GtkWidget *vseparator3;
  GtkWidget *button_remove;
  GtkWidget *alignment1;
  GtkWidget *hbox5;
  GtkWidget *image1;
  GtkWidget *label3;
  GtkWidget *scrolledwindow_content;
  GtkWidget *textview_content;

  vbox2 = gtk_vbox_new(FALSE, 1);
  gtk_widget_show(vbox2);
  gtk_box_pack_start(GTK_BOX(vbox1), vbox2, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(vbox2), 1);

  hseparator4 = gtk_hseparator_new();
  gtk_widget_show(hseparator4);
  gtk_box_pack_start(GTK_BOX(vbox2), hseparator4, TRUE, TRUE, 6);

  hbox8 = gtk_hbox_new(FALSE, 6);
  gtk_widget_show(hbox8);
  gtk_box_pack_start(GTK_BOX(vbox2), hbox8, TRUE, TRUE, 0);

  combobox_style = gtk_combo_box_new_text();
  gtk_widget_show(combobox_style);
  gtk_box_pack_start(GTK_BOX(hbox8), combobox_style, TRUE, TRUE, 0);

  // Store the default size of the combobox for later use.
  gint defaultheight;
  {
    GtkRequisition sizerequisition;
    gtk_widget_size_request(combobox_style, &sizerequisition);
    defaultheight = (int)(sizerequisition.height * 0.8);
  }

  label_size = gtk_label_new("Size");
  gtk_widget_show(label_size);
  gtk_box_pack_start(GTK_BOX(hbox8), label_size, FALSE, FALSE, 0);

  spinbutton_size_adj = gtk_adjustment_new(defaultheight, defaultheight, 30 * defaultheight, defaultheight, 3 * defaultheight, 0);
  spinbutton_size = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_size_adj), 1, 0);
  gtk_widget_show(spinbutton_size);
  gtk_widget_set_size_request(spinbutton_size, defaultheight / 2, -1);
  gtk_box_pack_start(GTK_BOX(hbox8), spinbutton_size, FALSE, FALSE, 0);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton_size), TRUE);

  checkbutton_content = gtk_check_button_new_with_mnemonic("Remember content");
  gtk_widget_show(checkbutton_content);
  gtk_box_pack_start(GTK_BOX(hbox8), checkbutton_content, FALSE, FALSE, 0);

  vseparator2 = gtk_vseparator_new();
  gtk_widget_show(vseparator2);
  gtk_box_pack_start(GTK_BOX(hbox8), vseparator2, TRUE, TRUE, 0);

  checkbutton_tidy = gtk_check_button_new_with_mnemonic("Tidy text");
  gtk_widget_show(checkbutton_tidy);
  gtk_box_pack_start(GTK_BOX(hbox8), checkbutton_tidy, FALSE, FALSE, 0);

  vseparator3 = gtk_vseparator_new();
  gtk_widget_show(vseparator3);
  gtk_box_pack_start(GTK_BOX(hbox8), vseparator3, TRUE, TRUE, 0);

  button_remove = gtk_button_new();
  gtk_widget_show(button_remove);
  gtk_box_pack_start(GTK_BOX(hbox8), button_remove, FALSE, FALSE, 0);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(button_remove), alignment1);

  hbox5 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox5);
  gtk_container_add(GTK_CONTAINER(alignment1), hbox5);

  image1 = gtk_image_new_from_stock("gtk-remove", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX(hbox5), image1, FALSE, FALSE, 0);

  label3 = gtk_label_new_with_mnemonic("");
  gtk_widget_show(label3);
  gtk_box_pack_start(GTK_BOX(hbox5), label3, FALSE, FALSE, 0);

  scrolledwindow_content = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow_content);
  gtk_box_pack_start(GTK_BOX(vbox2), scrolledwindow_content, FALSE, FALSE, 0);
  gtk_widget_set_size_request(scrolledwindow_content, -1, defaultheight);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow_content), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow_content), GTK_SHADOW_IN);

  textview_content = gtk_text_view_new();
  gtk_widget_show(textview_content);
  gtk_container_add(GTK_CONTAINER(scrolledwindow_content), textview_content);
  gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(textview_content), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview_content), GTK_WRAP_WORD);

  g_signal_connect((gpointer) button_remove, "clicked", G_CALLBACK(on_button_content_clicked), gpointer(this));
  g_signal_connect((gpointer) spinbutton_size, "value_changed", G_CALLBACK(on_spinbutton_size_value_changed), gpointer(this));
  g_signal_connect((gpointer) textview_content, "key_press_event", G_CALLBACK(on_textview_content_key_press_event), gpointer(this));

  gtk_box_reorder_child(GTK_BOX(vbox1), vbox2, vbox2s.size() + 4);

  vector < ustring > lines;
  for (unsigned int i = 0; i < content_markers.size(); i++) {
    ustring line;
    line.append(content_names[i] + " (" + usfm_get_full_opening_marker(content_markers[i]));
    if (content_endmarkers[i])
      line.append("..." + usfm_get_full_closing_marker(content_markers[i]));
    line.append(")");
    lines.push_back(line);
  }
  combobox_set_strings(combobox_style, lines);
  if (lines.size() > 0)
    combobox_set_string(combobox_style, lines[0]);

  vbox2s.push_back(vbox2);
  combobox_styles.push_back(combobox_style);
  spinbutton_sizes.push_back(spinbutton_size);
  scrolledwindow_contents.push_back(scrolledwindow_content);
  textview_contents.push_back(textview_content);
  checkbutton_contents.push_back(checkbutton_content);
  checkbutton_tidies.push_back(checkbutton_tidy);
  buttons_remove.push_back(button_remove);
  buttons_remove_label.push_back(label3);

  set_dynamic_shortcuts();
}


ustring InsertNoteDialog::template_filename_get(const ustring & template_name)
{
  ustring filename;
  switch (mydialogtype) {
  case indtFootnote:
    filename = "template-footnote-";
    break;
  case indtEndnote:
    filename = "template-endnote-";
    break;
  case indtCrossreference:
    filename = "template-crossreference-";
    break;
  }
  filename.append(template_name);
  return filename;
}


void InsertNoteDialog::templates_load(const ustring & template_name, bool update_gui)
{
  updategui = update_gui;
  ReadFiles rd(directories_get_configuration(), template_filename_get(""), "");
  for (unsigned int i = 0; i < rd.files.size(); i++)
    rd.files[i].erase(0, template_filename_get("").length());
  if (rd.files.size() == 0)
    rd.files.push_back("");
  combobox_set_strings(combobox_templates, rd.files);
  for (unsigned int i = 0; i < rd.files.size(); i++) {
    if (rd.files[i] == template_name) {
      combobox_set_string(combobox_templates, template_name);
    }
  }
  updategui = true;
}


void InsertNoteDialog::on_combobox_templates_changed(GtkComboBox * combobox, gpointer user_data)
{
  ((InsertNoteDialog *) user_data)->on_combobox_templates();
}


void InsertNoteDialog::on_combobox_templates()
// On a change of template, reload the values in the gui.
{
  // Do nothing if we wish the gui not to be updated.
  if (!updategui)
    return;
  // Load the template.
  ustring template_name = combobox_get_active_string(combobox_templates);
  NoteTemplate notetemplate(gw_build_filename(directories_get_configuration(), template_filename_get(template_name)), mydialogtype, false);
  // Set values in the fixed gui.
  gtk_entry_set_text(GTK_ENTRY(entry_numbering), notetemplate.anchor.c_str());
  gtk_entry_set_text(GTK_ENTRY(entry_reference_separator), notetemplate.reference_division.c_str());
  gtk_entry_set_text(GTK_ENTRY(entry_reference_suffix), notetemplate.reference_suffix.c_str());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_chapter), notetemplate.reference_chapter);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_verse), notetemplate.reference_verse);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_automatic_reference), notetemplate.automatic_reference);
  // Clear away the dynamic gui.
  while (buttons_remove.size() > 0)
    on_button_content_remove(GTK_BUTTON(buttons_remove[0]));
  // Rebuild the dynamic gui.
  for (unsigned int i = 0; i < notetemplate.content_markers.size(); i++) {
    on_button_add();
    combobox_set_string(combobox_styles[i], notetemplate.content_markers[i]);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton_sizes[i]), notetemplate.content_sizes[i]);
    on_spinbutton_size(GTK_SPIN_BUTTON(spinbutton_sizes[i]));
    if (notetemplate.content_remembers[i]) {
      GtkTextBuffer *textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_contents[i]));
      gtk_text_buffer_set_text(textbuffer, notetemplate.content_texts[i].c_str(), -1);
    }
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_contents[i]), notetemplate.content_remembers[i]);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_tidies[i]), notetemplate.content_tidies[i]);
  }
  // Set remainder of gui.
  numbering_set(notetemplate.numbering);
  on_checkbutton_automatic_reference();
  // Focus first content entry widget, if it is there.
  allow_entry_activate = true;
  focus_next_widget(entry_reference_suffix);
}


void InsertNoteDialog::on_button_template_new_clicked(GtkButton * button, gpointer user_data)
{
  ((InsertNoteDialog *) user_data)->on_button_template_new();
}


void InsertNoteDialog::on_button_template_new()
{
  EntryDialog dialog("Template", "Enter a name for a new template", "");
  dialog.run();
  GwSpawn spawn("touch");
  spawn.arg(gw_build_filename(directories_get_configuration(), template_filename_get(dialog.entered_value)));
  spawn.run();
  templates_load(dialog.entered_value, false);
}


void InsertNoteDialog::on_button_template_delete_clicked(GtkButton * button, gpointer user_data)
{
  ((InsertNoteDialog *) user_data)->on_button_template_delete();
}


void InsertNoteDialog::on_button_template_delete()
{
  ustring filename = gw_build_filename(directories_get_configuration(), template_filename_get(combobox_get_active_string(combobox_templates)));
  unlink(filename.c_str());
  templates_load("", true);
}


void InsertNoteDialog::focus_next_widget(GtkWidget * widget)
/*
The point of this function is that, when an entry is focused and it gets 
activated (by Enter), then the next entry should be focused.
This enables the user to quickly navigate through the gui and enter values.
At the end the Ok button should be focused.
*/
{
  // There is a mechanism in place that any entry that activates within
  // a millisecond after the previos one, will not be handles.
  // This is because for some reason if an entry gets focus, it immediately
  // give the activated signal, so activating the next, and the next, until the
  // end of the chain.
  if (!allow_entry_activate)
    return;
  allow_entry_activate = false;
  g_timeout_add(1, GSourceFunc(on_timeout), gpointer(this));
  int current_entry = 0;
  if (widget == entry_numbering)
    current_entry = 1;
  else if (widget == entry_reference_separator)
    current_entry = 2;
  else if (widget == entry_reference_suffix)
    current_entry = 3;
  else {
    for (unsigned int i = 0; i < textview_contents.size(); i++)
      if (gtk_widget_has_focus (textview_contents[i]))
        current_entry = 4;
  }
  switch (current_entry) {
  case 0:
    {
      if (gtk_widget_get_sensitive (entry_numbering)) {
        gtk_widget_grab_focus(entry_numbering);
        break;
      }
    }
  case 1:
    {
      if (gtk_widget_get_sensitive (entry_reference_separator)) {
        gtk_widget_grab_focus(entry_reference_separator);
        break;
      }
    }
  case 2:
    {
      if (gtk_widget_get_sensitive (entry_reference_suffix)) {
        gtk_widget_grab_focus(entry_reference_suffix);
        break;
      }
    }
  case 3:
    {
      if (textview_contents.empty())
        break;
      gtk_widget_grab_focus(textview_contents[0]);
      break;
    }
  case 4:
    {
      unsigned int focused_entry = 0;
      for (unsigned int i = 0; i < textview_contents.size(); i++) {
        if (gtk_widget_has_focus (textview_contents[i]))
          focused_entry = i;
      }
      focused_entry++;
      if (focused_entry < textview_contents.size()) {
        gtk_widget_grab_focus(textview_contents[focused_entry]);
        break;
      }
    }
  case 5:
    {
      gtk_widget_grab_focus(okbutton1);
    }
  }
}


bool InsertNoteDialog::on_timeout(gpointer data)
{
  ((InsertNoteDialog *) data)->timeout();
  return false;
}


void InsertNoteDialog::timeout()
{
  allow_entry_activate = true;
}


void InsertNoteDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((InsertNoteDialog *) user_data)->on_okbutton();
}


void InsertNoteDialog::on_okbutton()
{
  // Save the current gui.
  extern Settings *settings;
  NoteTemplate notetemplate(gw_build_filename(directories_get_configuration(), template_filename_get(combobox_get_active_string(combobox_templates))), mydialogtype, true);
  notetemplate.numbering = numbering_get();
  notetemplate.anchor = gtk_entry_get_text(GTK_ENTRY(entry_numbering));
  notetemplate.automatic_reference = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_automatic_reference));
  notetemplate.reference_chapter = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_chapter));
  notetemplate.reference_division = gtk_entry_get_text(GTK_ENTRY(entry_reference_separator));
  notetemplate.reference_verse = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_verse));
  notetemplate.reference_suffix = gtk_entry_get_text(GTK_ENTRY(entry_reference_suffix));
  notetemplate.content_markers.clear();
  notetemplate.content_texts.clear();
  notetemplate.content_remembers.clear();
  notetemplate.content_tidies.clear();
  notetemplate.content_sizes.clear();
  for (unsigned int i = 0; i < vbox2s.size(); i++) {
    notetemplate.content_markers.push_back(combobox_get_active_string(combobox_styles[i]));
    // Get text, and convert new lines to spaces, as notes don't have newlines.
    GtkTextBuffer *textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_contents[i]));
    GtkTextIter start;
    gtk_text_buffer_get_start_iter(textbuffer, &start);
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(textbuffer, &end);
    ustring text = gtk_text_buffer_get_text(textbuffer, &start, &end, false);
    replace_text(text, "\n", " ");
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_tidies[i]))) {
      tidy_text(text);
    }
    notetemplate.content_texts.push_back(text);
    notetemplate.content_remembers.push_back(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_contents[i])));
    notetemplate.content_tidies.push_back(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_tidies[i])));
    gtk_spin_button_update(GTK_SPIN_BUTTON(spinbutton_sizes[i]));
    gint size = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinbutton_sizes[i]));
    notetemplate.content_sizes.push_back(size);
  }
  switch (mydialogtype) {
  case indtFootnote:
    {
      settings->genconfig.insert_footnote_template_set(combobox_get_active_string(combobox_templates));
      break;
    }
  case indtEndnote:
    {
      settings->genconfig.insert_endnote_template_set(combobox_get_active_string(combobox_templates));
      break;
    }
  case indtCrossreference:
    {
      settings->genconfig.insert_xref_template_set(combobox_get_active_string(combobox_templates));
      break;
    }
  }
  // Get some markers.
  ustring reference_marker;
  switch (mydialogtype) {
  case indtFootnote:
    {
      reference_marker = "fr";
      break;
    }
  case indtEndnote:
    {
      reference_marker = "fr";
      break;
    }
  case indtCrossreference:
    {
      reference_marker = "xo";
      break;
    }
  }
  // Add numbering.
  if (notetemplate.numbering == 0)
    rawtext.append("+");
  else if (notetemplate.numbering == 1)
    rawtext.append("-");
  else
    rawtext.append(notetemplate.anchor);
  rawtext.append(" ");
  // Deal with the reference.
  ustring reference = reference_get();
  if (!reference.empty()) {
    rawtext.append(usfm_get_full_opening_marker(reference_marker));
    rawtext.append(reference);
  }
  // Add possible content.
  for (unsigned int i = 0; i < notetemplate.content_markers.size(); i++) {
    ustring marker;
    size_t position, length;
    bool b;
    usfm_search_marker(notetemplate.content_markers[i], marker, position, length, b);
    if (!notetemplate.content_texts[i].empty()) {
      rawtext.append(usfm_get_full_opening_marker(marker));
      rawtext.append(trim(notetemplate.content_texts[i]));
      for (unsigned int i2 = 0; i2 < content_markers.size(); i2++) {
        if (marker == content_markers[i2]) {
          if (content_endmarkers[i2]) {
            rawtext.append(usfm_get_full_closing_marker(marker));
          }
        }
      }
      // Automatically add a space.
      rawtext = trim(rawtext);
      rawtext.append(" ");
    }
  }
  // Remove last space.
  rawtext = trim(rawtext);
}


void InsertNoteDialog::on_spinbutton_size_value_changed(GtkSpinButton * spinbutton, gpointer user_data)
{
  ((InsertNoteDialog *) user_data)->on_spinbutton_size(spinbutton);
}


void InsertNoteDialog::on_spinbutton_size(GtkSpinButton * spinbutton)
{
  // Get the offset of the widget to resize. Bail out if not found.
  int widget_offset = -1;
  for (unsigned int i = 0; i < spinbutton_sizes.size(); i++) {
    if (GTK_WIDGET(spinbutton) == spinbutton_sizes[i])
      widget_offset = i;
  }
  if (widget_offset < 0)
    return;
  // Resize the widget.
  gtk_widget_set_size_request(scrolledwindow_contents[widget_offset], -1, gtk_spin_button_get_value_as_int(spinbutton));
}


gboolean InsertNoteDialog::on_textview_content_key_press_event(GtkWidget * widget, GdkEventKey * event, gpointer user_data)
// Handles pressing keys in the content textviews.
{
  // Do not allow newlines in the editor, but when pressing Enter, go to the next widget.
  switch (event->keyval) {
  case GDK_KEY_Return:
  case GDK_KEY_KP_Enter:
    ((InsertNoteDialog *) user_data)->focus_next_widget(widget);
    return true;
  }
  // Handle other keypresses as usual. 
  return false;
}


void InsertNoteDialog::set_dynamic_shortcuts()
{

  for (unsigned int i = 0; i < buttons_remove_label.size(); i++) {
    ustring label = "_" + convert_to_string(i) + " Remove";
    gtk_label_set_text_with_mnemonic(GTK_LABEL(buttons_remove_label[i]), label.c_str());
  }
}


