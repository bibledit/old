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

#ifndef INCLUDED_DIALOGIMPORT_NOTES_H
#define INCLUDED_DIALOGIMPORT_NOTES_H

#include <gtk/gtk.h>
#include "ustring.h"


enum ImportFormat {ifUnknown, ifBibleditNotesVersion3, ifScriptureNotesVersion20};


class ImportNotesDialog
{
public:
  ImportNotesDialog (int dummy);
  ~ ImportNotesDialog ();
  int run ();
protected:
  GtkWidget *importnotesdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *notebook;
  GtkWidget *vbox4;
  GtkWidget *label13;
  GtkWidget *label24;
  GtkWidget *vbox2;
  GtkWidget *label6;
  GtkWidget *select_directory_button;
  GtkWidget *alignment1;
  GtkWidget *hbox2;
  GtkWidget *image1;
  GtkWidget *label8;
  GtkWidget *xml_file_label;
  GtkWidget *label26;
  GtkWidget *vbox5;
  GtkWidget *label18;
  GtkWidget *label28;
  GtkWidget *vbox6;
  GtkWidget *label19;
  GtkWidget *label30;
  GtkWidget *vbox8;
  GtkWidget *label23;
  GtkWidget *label32;
  GtkWidget *hbox1;
  GtkWidget *backbutton;
  GtkWidget *forwardbutton;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *applybutton;
private:
  ustring file_to_import;
  ImportFormat import_format;
  void set_gui ();
  void select_file_page ();
  void unicode_page ();
  void make_known_corrections_to_xml_file ();
  static void on_backbutton_clicked (GtkButton * button, gpointer user_data);
  void on_backbutton ();
  static void on_forwardbutton_clicked (GtkButton * button, gpointer user_data);
  void on_forwardbutton ();
  static void on_applybutton_clicked (GtkButton * button, gpointer user_data);
  static void on_select_file_button_clicked (GtkButton * button, gpointer user_data);
  void on_select_file_button ();
  void on_apply ();
  static void start_element_handler  (GMarkupParseContext *context,
                                      const gchar         *element_name,
                                      const gchar        **attribute_names,
                                      const gchar        **attribute_values,
                                      gpointer             user_data,
                                      GError             **error);
  void start_element_handler (const ustring& element_name);
  static void end_element_handler    (GMarkupParseContext *context,
                                      const gchar         *element_name,
                                      gpointer             user_data,
                                      GError             **error);
  void end_element_handler   (const ustring& element_name);
  static void text_handler           (GMarkupParseContext *context,
                                      const gchar         *text,
                                      gsize                text_len,
                                      gpointer             user_data,
                                      GError             **error);
  void text_handler          (const ustring& text);
  static void passthrough_handler    (GMarkupParseContext *context,
                                      const gchar         *passthrough_text,
                                      gsize                text_len,
                                      gpointer             user_data,
                                      GError             **error);
  static void error_handler          (GMarkupParseContext *context,
                                      GError              *error,
                                      gpointer             user_data);
  // List of which elements are for what.
  ustring note_element;
  ustring reference_element;
  ustring notetext_element;
  ustring category_element;
  ustring project_element;
  ustring datemodified_element;
  ustring datecreated_element;
  ustring createdby_element;
  // Places to store data for gather together data for importing a new note.
  ustring reference_content;
  ustring notetext_content;
  ustring status_content;
  ustring category_content;
  ustring project_content;
  ustring datemodified_content;
  ustring datecreated_content;
  ustring createdby_content;
  // Some household data.
  ustring current_element;
  int depth;
  int depth_of_notetext;
};


#endif
