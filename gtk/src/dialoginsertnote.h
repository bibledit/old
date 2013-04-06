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


#ifndef INCLUDED_DIALOG_INSERT_NOTE_H
#define INCLUDED_DIALOG_INSERT_NOTE_H


#include <gtk/gtk.h>
#include "types.h"
#include "editor_aids.h"


class InsertNoteDialog
{
public:
  InsertNoteDialog (NoteType dialogtype);
  ~InsertNoteDialog ();
  int run ();
  ustring rawtext;
protected:
private:
  // Dialog.
  GtkWidget *insertnotedialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  // Numbering.
  GtkWidget *hbox1;
  GtkWidget *label1;
  GtkWidget *radiobutton_numbering_automatic;
  GtkWidget *radiobutton_numbering_none;
  GtkWidget *radiobutton_numbering_character;
  GtkWidget *entry_numbering;
  // Automatic reference.
  GtkWidget *hseparator1;
  GtkWidget *checkbutton_automatic_reference;
  GtkWidget *hbox2;
  GtkWidget *checkbutton_chapter;
  GtkWidget *entry_reference_separator;
  GtkWidget *checkbutton_verse;
  GtkWidget *entry_reference_suffix;
  GtkWidget *vseparator1;
  GtkWidget *label_automatic_reference;
  // Content.
  vector<GtkWidget *> vbox2s;
  vector<GtkWidget *> combobox_styles;
  vector<GtkWidget *> spinbutton_sizes;
  vector<GtkWidget *> scrolledwindow_contents;
  vector<GtkWidget *> textview_contents;
  vector<GtkWidget *> checkbutton_contents;
  vector<GtkWidget *> checkbutton_tidies;
  vector<GtkWidget *> buttons_remove;
  vector<GtkWidget *> buttons_remove_label;
  // Add content.
  GtkWidget *hseparator5;
  GtkWidget *hbox6;
  GtkWidget *button_add;
  GtkWidget *alignment2;
  GtkWidget *hbox7;
  GtkWidget *image2;
  GtkWidget *label4;
  // Templates.
  GtkWidget *hseparator3;
  GtkWidget *label2;
  GtkWidget *hbox3;
  GtkWidget *combobox_templates;
  GtkWidget *button_template_new;
  GtkWidget *button_template_delete;
  // Dialog.
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  NoteType mydialogtype;
  vector<ustring> content_markers;
  vector<bool> content_endmarkers;
  vector<ustring> content_names;
  bool allow_entry_activate;
  static void on_radiobutton_numbering_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_radiobutton_numbering (GtkToggleButton *togglebutton);
  int numbering_get ();
  void numbering_set (int numbering);
  static void on_entry_numbering_activate (GtkEntry *entry, gpointer user_data);
  static void on_checkbutton_automatic_reference_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_automatic_reference ();
  static void on_checkbutton_chapter_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_chapter ();
  void separator_sensitive_set ();
  static void on_entry_reference_separator_changed (GtkEditable *editable, gpointer user_data);
  void on_entry_reference_separator_changed2 ();
  static void on_entry_reference_separator_activate (GtkEntry *entry, gpointer user_data);
  static void on_checkbutton_verse_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_verse ();
  static void on_entry_reference_suffix_changed (GtkEditable *editable, gpointer user_data);
  void on_entry_reference_suffix_changed2 ();
  static void on_entry_reference_suffix_activate (GtkEntry *entry, gpointer user_data);
  ustring reference_get ();
  static void on_button_content_clicked (GtkButton *button, gpointer user_data);
  void on_button_content_remove (GtkButton *button);
  void vector_remove_content (vector<GtkWidget *>& container, int offset);
  static void on_button_add_clicked (GtkButton *button, gpointer user_data);
  void on_button_add ();
  ustring template_filename_get (const ustring& template_name);
  void templates_load (const ustring& template_name, bool update_gui);
  bool updategui;
  static void on_combobox_templates_changed (GtkComboBox *combobox, gpointer user_data);
  void on_combobox_templates ();  
  static void on_button_template_new_clicked (GtkButton *button, gpointer user_data);
  void on_button_template_new ();
  static void on_button_template_delete_clicked (GtkButton *button, gpointer user_data);
  void on_button_template_delete ();
  void focus_next_widget (GtkWidget * widget);
  static bool on_timeout (gpointer data);
  void timeout ();
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  static void on_spinbutton_size_value_changed (GtkSpinButton *spinbutton, gpointer user_data);
  void on_spinbutton_size (GtkSpinButton *spinbutton);
  static gboolean on_textview_content_key_press_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  void set_dynamic_shortcuts ();
};


#endif
