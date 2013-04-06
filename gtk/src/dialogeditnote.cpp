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
#include "dialogeditnote.h"
#include "utilities.h"
#include "gwrappers.h"
#include "usfmtools.h"
#include "stylesheetutils.h"
#include <gdk/gdkkeysyms.h>
#include "help.h"
#include "shortcuts.h"
#include "biblenotes.h"
#include "settings.h"
#include "styles.h"
#include "combobox.h"


EditNoteDialog::EditNoteDialog(Editor2 * editor)
{
  // Save variables.
  myeditor = editor;

  // Build dialog.
  Shortcuts shortcuts(0);

  editnotedialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(editnotedialog), "Edit note");
  gtk_window_set_position(GTK_WINDOW(editnotedialog), GTK_WIN_POS_CENTER_ON_PARENT);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG(editnotedialog));
  gtk_widget_show(dialog_vbox1);

  hbox1 = gtk_hbox_new(FALSE, 10);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox1, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(hbox1), 1);

  label1 = gtk_label_new_with_mnemonic("Numbering");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(hbox1), label1, FALSE, FALSE, 0);

  GSList *radiobutton_numbering_automatic_group = NULL;

  radiobutton_numbering_automatic = gtk_radio_button_new_with_mnemonic(NULL, "Automatic");
  gtk_widget_show(radiobutton_numbering_automatic);
  gtk_box_pack_start(GTK_BOX(hbox1), radiobutton_numbering_automatic, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_numbering_automatic), radiobutton_numbering_automatic_group);
  radiobutton_numbering_automatic_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_numbering_automatic));

  shortcuts.button(radiobutton_numbering_automatic);

  radiobutton_numbering_none = gtk_radio_button_new_with_mnemonic(NULL, "None");
  gtk_widget_show(radiobutton_numbering_none);
  gtk_box_pack_start(GTK_BOX(hbox1), radiobutton_numbering_none, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_numbering_none), radiobutton_numbering_automatic_group);
  radiobutton_numbering_automatic_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_numbering_none));

  shortcuts.button(radiobutton_numbering_none);

  radiobutton_numbering_character = gtk_radio_button_new_with_mnemonic(NULL, "Character");
  gtk_widget_show(radiobutton_numbering_character);
  gtk_box_pack_start(GTK_BOX(hbox1), radiobutton_numbering_character, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_numbering_character), radiobutton_numbering_automatic_group);
  radiobutton_numbering_automatic_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_numbering_character));

  shortcuts.button(radiobutton_numbering_character);

  entry_numbering = gtk_entry_new();
  gtk_widget_show(entry_numbering);
  gtk_box_pack_start(GTK_BOX(hbox1), entry_numbering, FALSE, FALSE, 0);
  gtk_entry_set_max_length(GTK_ENTRY(entry_numbering), 3);
  gtk_entry_set_width_chars(GTK_ENTRY(entry_numbering), 3);

  hseparator5 = gtk_hseparator_new();
  gtk_widget_show(hseparator5);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hseparator5, TRUE, TRUE, 0);

  hbox7 = gtk_hbox_new(FALSE, 10);
  gtk_widget_show(hbox7);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox7, FALSE, FALSE, 0);

  label4 = gtk_label_new_with_mnemonic("Type");
  gtk_widget_show(label4);
  gtk_box_pack_start(GTK_BOX(hbox7), label4, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label4), 0, 0.5);

  combobox1 = gtk_combo_box_new_text();
  gtk_widget_show(combobox1);
  gtk_box_pack_start(GTK_BOX(hbox7), combobox1, TRUE, TRUE, 0);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(editnotedialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(editnotedialog, NULL, &shortcuts, NULL);

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(editnotedialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton1), true);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(editnotedialog), okbutton1, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton1), true);

  shortcuts.stockbutton(cancelbutton1);
  shortcuts.stockbutton(okbutton1);
  shortcuts.process();

  g_signal_connect((gpointer) radiobutton_numbering_automatic, "toggled", G_CALLBACK(on_radiobutton_numbering_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_numbering_none, "toggled", G_CALLBACK(on_radiobutton_numbering_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_numbering_character, "toggled", G_CALLBACK(on_radiobutton_numbering_toggled), gpointer(this));
  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));

  // Get a pointer to the focused note paragraph.
  note_paragraph = static_cast <EditorActionCreateNoteParagraph *> (myeditor->focused_paragraph);

  // If no EditorNote found, disable the OK button.
  gtk_widget_set_sensitive(okbutton1, note_paragraph != NULL);

  // Load the available note markers in the combobox.
  vector < ustring > names;
  ustring stylesheet = stylesheet_get_actual ();
  extern Styles *styles;
  Usfm *usfm = styles->usfm(stylesheet);
  for (unsigned int i = 0; i < usfm->styles.size(); i++) {
    bool record = false;
    if (usfm->styles[i].type == stFootEndNote) {
      if (usfm->styles[i].subtype == fentFootnote)
        record = true;
      if (usfm->styles[i].subtype == fentEndnote)
        record = true;
    }
    if (usfm->styles[i].type == stCrossreference) {
      if (usfm->styles[i].subtype == ctCrossreference)
        record = true;
    }
    if (record) {
      names.push_back(usfm->styles[i].marker + " " + usfm->styles[i].name);
    }
  }
  combobox_set_strings(combobox1, names);

  // Set GUI.
  if (note_paragraph) {
    for (unsigned int i = 0; i < names.size(); i++) {
      if (note_paragraph->opening_closing_marker == get_marker(names[i])) {
        combobox_set_string(combobox1, names[i]);
      }
    }
    NumberingType numberingtype;
    ustring character = "*";
    biblenotes_get_note_numbering(note_paragraph->caller_usfm, numberingtype, character);
    numbering_set(numberingtype);
    gtk_entry_set_text(GTK_ENTRY(entry_numbering), character.c_str());
  }
}


EditNoteDialog::~EditNoteDialog()
{
  gtk_widget_destroy(editnotedialog);
}


int EditNoteDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(editnotedialog));
}


void EditNoteDialog::on_radiobutton_numbering_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((EditNoteDialog *) user_data)->on_radiobutton_numbering(togglebutton);
}


void EditNoteDialog::on_radiobutton_numbering(GtkToggleButton * togglebutton)
{
  gtk_label_set_mnemonic_widget(GTK_LABEL(label1), GTK_WIDGET(togglebutton));
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_numbering_character))) {
    gtk_widget_set_sensitive(entry_numbering, true);
    gtk_widget_grab_focus(entry_numbering);
  } else {
    gtk_widget_set_sensitive(entry_numbering, false);
  }
}


int EditNoteDialog::numbering_get()
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_numbering_none)))
    return ntNone;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_numbering_character)))
    return ntCharacter;
  return ntAutomatic;
}


void EditNoteDialog::numbering_set(int numbering)
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


void EditNoteDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((EditNoteDialog *) user_data)->on_okbutton();
}


void EditNoteDialog::on_okbutton()
{
  // Get the old note type.
  EditorNoteType oldnotetype = note_type_get(myeditor->project, note_paragraph->opening_closing_marker);

  // Get and store the new note marker and type.
  note_paragraph->opening_closing_marker = get_marker(combobox_get_active_string(combobox1));
  EditorNoteType newnotetype = note_type_get(myeditor->project, note_paragraph->opening_closing_marker);

  // Change the caller, if needed.
  NumberingType numberingtype = (NumberingType) numbering_get();
  gchar *character = (gchar *) gtk_entry_get_text(GTK_ENTRY(entry_numbering));
  note_paragraph->caller_usfm = biblenotes_get_note_caller(numberingtype, character);

  /*
     Change the main body of the footnote if the note type changed sufficiently.
     There are some markers that only belong to foot/endnotes,
     and other ones that only belong to crossreferences.
     Switch between these markers, if that is needed.
   */

  // Find out if there was a switch, and if so, what switch was made.
  bool switch_at_all = false;
  bool switch_to_xref = false;
  if ((oldnotetype == entCrossreference) && (newnotetype != oldnotetype)) {
    switch_at_all = true;
  }
  if ((newnotetype == entCrossreference) && (newnotetype != oldnotetype)) {
    switch_at_all = true;
    switch_to_xref = true;
  }

  if (switch_at_all) {

    // Go through the whole note, finding tags and replacing them with new ones.
    GtkTextIter startiter, enditer;
    gtk_text_buffer_get_start_iter(note_paragraph->textbuffer, &startiter);
    gtk_text_buffer_get_end_iter(note_paragraph->textbuffer, &enditer);

    GtkTextIter iter = startiter;
    do {
      ustring paragraph_style;
      ustring character_style;
      GSList *tags = NULL, *tagp = NULL;
      tags = gtk_text_iter_get_tags(&iter);
      for (tagp = tags; tagp != NULL; tagp = tagp->next) {
        GtkTextTag *tag = (GtkTextTag *) tagp->data;
        gchar *strval;
        g_object_get(G_OBJECT(tag), "name", &strval, NULL);
        if (strlen(strval)) {
          if (paragraph_style.empty())
            paragraph_style = strval;
          else
            character_style = strval;
        }
        g_free(strval);
      }
      if (tags) {
        g_slist_free(tags);
      }

      GtkTextIter iter2 = iter;
      gtk_text_iter_forward_char(&iter2);
      if (!paragraph_style.empty()) {
        gtk_text_buffer_remove_tag_by_name(note_paragraph->textbuffer, paragraph_style.c_str(), &iter, &iter2);
      }
      if (!character_style.empty()) {
        gtk_text_buffer_remove_tag_by_name(note_paragraph->textbuffer, character_style.c_str(), &iter, &iter2);
      }
      if (!paragraph_style.empty()) {
        textbuffer_apply_named_tag(note_paragraph->textbuffer, switch_marker(paragraph_style, switch_to_xref), &iter, &iter2);
      }
      if (!character_style.empty()) {
        textbuffer_apply_named_tag(note_paragraph->textbuffer, switch_marker(character_style, switch_to_xref), &iter, &iter2);
      }

      gtk_text_iter_forward_char(&iter);
    } while (gtk_text_iter_in_range(&iter, &startiter, &enditer));

  }
}


ustring EditNoteDialog::switch_marker(const ustring & marker, bool switch_to_xref)
/*
In case a footnote or endnote is changed to a crossreference, or vice versa, 
then the styles of the note's body need to be updated.
This function takes a "marker", and returns the changed marker that corresponds 
to it.
If switch_to_xref" is true, then the switch is made from foot/endnote to a
crossreference. If this flag is false, then the change is from a crossreference
to a footnote or endnote.
*/
{
  /*
     Markers for foot- and endnotes:
     fr Reference
     fk Keyword
     fq  Quotation
     fqa Alternative translation.
     fl  Labelled Text
     fp  Additional Paragraph
     fv  Verse in Text
     ft  Text
     fdc Apocrypha

     Markers for crossreferences:
     xo  Origin Reference
     xk  Keyword
     xq  Quotation
     xt  Targets
     xdc Apocrypha    
   */

  ustring newmarker(marker);

  if (switch_to_xref) {
    if (marker == "fr")
      newmarker = "xo";
    if (marker == "fk")
      newmarker = "xk";
    if (marker == "fq")
      newmarker = "xq";
    if (marker == "fqa")
      newmarker = "xt";
    if (marker == "fl")
      newmarker = "xt";
    if (marker == "fp")
      newmarker = "xt";
    if (marker == "fv")
      newmarker = "xt";
    if (marker == "ft")
      newmarker = "xt";
    if (marker == "fdc")
      newmarker = "xdc";
  } else {
    if (marker == "xo")
      newmarker = "fr";
    if (marker == "xk")
      newmarker = "fk";
    if (marker == "xq")
      newmarker = "fq";
    if (marker == "xt")
      newmarker = "ft";
    if (marker == "xdc")
      newmarker = "fdc";
  }

  return newmarker;
}


ustring EditNoteDialog::get_marker(const ustring & text)
// Given a string starting with a marker, then a space, then something else,
// this returns the marker.
// E.g. "f * Footnote" would yield "f".
{
  ustring marker(text);
  size_t pos = text.find(" ");
  if (pos != string::npos) {
    marker.erase(pos, marker.length() - pos);
  }
  return marker;
}


