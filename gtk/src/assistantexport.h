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


#ifndef INCLUDED_ASSISTANT_EXPORT_H
#define INCLUDED_ASSISTANT_EXPORT_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "assistants.h"
#include "backup.h"
#include "export_utils.h"
#include "windowreferences.h"
#include "windowstyles.h"
#include "windowcheckkeyterms.h"
#include "stylesheetutils.h"


class ExportAssistant : public AssistantBase
{
public:
  ExportAssistant(WindowReferences * references_window, WindowStyles * styles_window, WindowCheckKeyterms * check_keyterms_window);
  virtual ~ExportAssistant();
  bool sword_module_created;
private:
  // Local variables.
  WindowReferences * my_references_window;
  WindowStyles * my_styles_window;
  WindowCheckKeyterms * my_check_keyterms_window;

  // Assistant page preparation.
  static void on_assistant_prepare_signal (GtkAssistant *assistant, GtkWidget *page, gpointer user_data);
  void on_assistant_prepare (GtkWidget *page);

  // Backup type selection.
  int page_number_select_type;
  GtkWidget *vbox_select_type;
  GtkWidget *radiobutton_select_type_bible;
  GtkWidget *radiobutton_select_type_references;
  GtkWidget *radiobutton_select_type_stylesheet;
  GtkWidget *radiobutton_select_type_notes;
  GtkWidget *radiobutton_select_type_keyterms;
  ExportType get_type ();

  // Confirm or change Bible.
  int page_number_bible_name;
  GtkWidget *vbox_bible_name;
  GtkWidget *label_project_name;
  GtkWidget *button_bible_name;
  GtkWidget *alignment1;
  GtkWidget *hbox1;
  GtkWidget *image1;
  GtkWidget *label12;
  static void on_button_bible_name_clicked (GtkButton *button, gpointer user_data);
  void on_button_bible_name ();
  ustring bible_name;

  // Bible export type.
  int page_number_bible_type;
  GtkWidget *vbox_bible_type;
  GtkWidget *radiobutton_bible_usfm;
  GtkWidget *radiobutton_bible_bibleworks;
  GtkWidget *radiobutton_bible_osis;
  GtkWidget *radiobutton_bible_sword;
  GtkWidget *radiobutton_bible_opendocument;
  //GtkWidget *radiobutton_bible_gobible;
  ExportBibleType get_bible_type ();

  // USFM export type.
  int page_number_usfm_type;
  GtkWidget *vbox_usfm_type;
  GtkWidget *radiobutton_usfm_everything;
  GtkWidget *radiobutton_usfm_changes_only;
  ExportUsfmType get_usfm_type ();
  
  // Changed USFM export type.
  int page_number_usfm_changes_type;
  GtkWidget *vbox_usfm_changes_type;
  GtkWidget *radiobutton_usfm_changes_since_last;
  GtkWidget *radiobutton_usfm_changes_since_date;
  ExportUsfmChangesType get_usfm_changes_type ();
  
  // OSIS export type.
  int page_number_osis_type;
  GtkWidget *vbox_osis_type;
  GtkWidget *radiobutton_osis_recommended;
  GtkWidget *radiobutton_osis_go_bible;
  GtkWidget *radiobutton_osis_old;
  ExportOsisType get_osis_type ();

  // Sword module variables.
  int page_number_sword_name;
  GtkWidget * entry_sword_name;
  int page_number_sword_description;
  GtkWidget * entry_sword_description;
  int page_number_sword_about;
  GtkWidget * entry_sword_about;
  int page_number_sword_license;
  GtkWidget * entry_sword_license;
  int page_number_sword_version;
  GtkWidget * entry_sword_version;
  int page_number_sword_language;
  GtkWidget * entry_sword_language;
  int page_number_sword_install_path;
  GtkWidget * entry_sword_install_path;
  static void on_entry_sword_changed (GtkEditable *editable, gpointer user_data);
  void on_entry_sword (GtkEditable *editable);
  void sword_values_set ();

  // Stylesheet format.
  int page_number_stylesheet_format;
  GtkWidget *vbox_stylesheet_format;
  GtkWidget *radiobutton_stylesheet_format_bibledit;
  GtkWidget *radiobutton_stylesheet_format_paratext;
  StylesheetExportFormatType get_stylesheet_export_format ();
    
  // Include keyterms without rendering?
  int page_number_keyterms_without_rendering;
  GtkWidget *checkbutton_keyterms_without_rendering;
  bool get_include_keyterms_without_rendering ();
  
  // Compress it?
  int page_number_zip;
  GtkWidget *checkbutton_zip;
  bool get_compressed ();
  
  // Select date and time.
  int page_number_date_time;
  GtkWidget *vbox_date_time;
  GtkWidget *button_date_time;
  GtkWidget *label_date_time;
  static void on_button_date_time_clicked (GtkButton *button, gpointer user_data);
  void on_button_date_time ();
  unsigned int date_time;

  // Comment entry.  
  int page_number_comment;
  GtkWidget * entry_comment;

  // Select file.
  int page_number_file;
  GtkWidget *vbox_file;
  GtkWidget *button_file;
  GtkWidget *label_file;
  static void on_button_file_clicked (GtkButton *button, gpointer user_data);
  void on_button_file ();
  ustring filename;
  
  // Select folder.
  int page_number_folder;
  GtkWidget *vbox_folder;
  GtkWidget *button_folder;
  GtkWidget *label_folder;
  static void on_button_folder_clicked (GtkButton *button, gpointer user_data);
  void on_button_folder ();
  ustring foldername;
  
  // Confirmation stuff.
  int page_number_confirm;
  GtkWidget *label_confirm;
  int page_number_progress;
  GtkWidget *label_progress;
  GtkWidget *label_summary;
  int summary_page_number;
  static void on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data);
  void on_assistant_apply ();
  static gint assistant_forward_function (gint current_page, gpointer user_data);
  gint assistant_forward (gint current_page);
  set <int> forward_sequence;
};


#endif
