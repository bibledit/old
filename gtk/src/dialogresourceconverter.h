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


#ifndef INCLUDED_DIALOGRESOURCECONVERTER_H
#define INCLUDED_DIALOGRESOURCECONVERTER_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "types.h"
#include "shortcuts.h"


class ResourceConverterDialog
{
public:
  ResourceConverterDialog (const ustring& working_directory);
  ~ResourceConverterDialog ();
  int run ();
  ResourceConversionType resource_conversion_type;
  ustring chapter_pattern_prefix;
  unsigned int chapter_pattern_index;
  ustring chapter_pattern_suffix;
  ustring verse_pattern_prefix;
  unsigned int verse_pattern_index;
  ustring verse_pattern_suffix;
private:

  // Dialog.
  GtkWidget *resourceconverterdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *label1;
  GtkWidget *table1;
  void build_table_and_type (Shortcuts& shortcuts);
  unsigned int table_attachment_offset;

  // Entry.
  void build_entry (GtkWidget *& image_ok, GtkWidget *& label_ok, GtkWidget *& label, const gchar * label_text, GtkWidget *& entry, const ustring& entry_text, GCallback handler);
  //static void on_entry_changed (GtkEditable *editable, gpointer user_data);
  //void on_entry ();

  // Button.
  void build_button (GtkWidget *& image_ok, GtkWidget *& label_ok, GtkWidget *& label_short, GtkWidget *& button, const gchar * button_text, Shortcuts& shortcuts, GCallback handler, GtkWidget *& label_long);

  // Checkbutton and button.
  void build_checkbutton_button (GtkWidget *& image_ok, GtkWidget *& label_ok, 
                                 GtkWidget *& checkbutton, const gchar * checkbutton_text, GCallback checkbutton_handler, 
                                 GtkWidget *& button, const gchar * button_text, GCallback button_handler, 
                                 Shortcuts& shortcuts, 
                                 GtkWidget *& label);
  
  // Textview.
  void build_textview (GtkWidget *& image_ok, GtkWidget *& label_ok, GtkWidget *& label,
                       GtkWidget *& textview, gchar * text, GCallback handler);
  
  // Entry, combo, entry.
  void build_entry_combo_entry (GtkWidget *& image_ok, GtkWidget *& label_ok,
                                GtkWidget *& label,
                                GtkWidget *& label_entry_1, GtkWidget *& entry_1,
                                GtkWidget *& label_combo, GtkWidget *& combo,
                                GtkWidget *& label_entry_2, GtkWidget *& entry_2,
                                GCallback entry_handler, Shortcuts& shortcuts);

  // Dialog action.
  GtkWidget *dialog_action_area1;
  GtkWidget *helpbutton;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;

  // Working directory and data.
  ustring workingdirectory;
  ustring filename;
  vector <ustring> lines;
  
  // Resource conversion type.
  GtkWidget *image_type_ok;
  GtkWidget *label_type_ok;
  GtkWidget *label_type_short;
  GtkWidget *button_type;
  GtkWidget *label_type_long;
  static void on_type_button_clicked (GtkButton *button, gpointer user_data);
  void on_type_button (bool no_ask);
  bool type_gui ();
  
  // Open file.
  GtkWidget *image_open_file_ok;
  GtkWidget *label_open_file_ok;
  GtkWidget *label_open_file_short;
  GtkWidget *button_open_file;
  GtkWidget *label_open_file_long;
  static void on_open_file_button_clicked (GtkButton *button, gpointer user_data);
  void on_open_file_button ();
  bool open_file_gui ();

  // View file.
  GtkWidget *image_view_file_ok;
  GtkWidget *label_view_file_ok;
  GtkWidget *label_view_file_short;
  GtkWidget *button_view_file_html;
  GtkWidget *label_view_file_long;
  static void on_view_file_button_clicked (GtkButton *button, gpointer user_data);
  void on_view_file_button ();
  bool view_file_gui ();

  // Chapter detection pattern.
  GtkWidget *image_chapter_pattern_ok;
  GtkWidget *label_chapter_pattern_ok;
  GtkWidget *label_chapter_pattern;
  GtkWidget *label_chapter_pattern_1;
  GtkWidget *entry_chapter_pattern_1;
  GtkWidget *label_chapter_pattern_combo;
  GtkWidget *combo_chapter_pattern;
  GtkWidget *label_chapter_pattern_2;
  GtkWidget *entry_chapter_pattern_2;
  static void on_chapter_pattern_entry_changed (GtkEditable *editable, gpointer user_data);
  void on_chapter_pattern_entry ();
  bool chapter_pattern_gui ();

  // Verse detection pattern.
  GtkWidget *image_verse_pattern_ok;
  GtkWidget *label_verse_pattern_ok;
  GtkWidget *label_verse_pattern;
  GtkWidget *label_verse_pattern_1;
  GtkWidget *entry_verse_pattern_1;
  GtkWidget *label_verse_pattern_combo;
  GtkWidget *combo_verse_pattern;
  GtkWidget *label_verse_pattern_2;
  GtkWidget *entry_verse_pattern_2;
  static void on_verse_pattern_entry_changed (GtkEditable *editable, gpointer user_data);
  void on_verse_pattern_entry ();
  bool verse_pattern_gui ();

  // Write anchors.
  GtkWidget *image_write_anchors_ok;
  GtkWidget *label_write_anchors_ok;
  GtkWidget *label_write_anchors_short;
  GtkWidget *button_write_anchors;
  GtkWidget *label_write_anchors_long;
  static void on_write_anchors_button_clicked (GtkButton *button, gpointer user_data);
  void on_write_anchors_button ();
  bool write_anchors_gui ();
  bool anchors_written;

  // General gui.
  void gui ();
  void build_gui (Shortcuts& shortcuts);
  
  // Ok button.
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
};


#endif
