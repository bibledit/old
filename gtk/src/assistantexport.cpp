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
#include "assistantexport.h"
#include "help.h"
#include "settings.h"
#include "utilities.h"
#include "gwrappers.h"
#include "tiny_utilities.h"
#include "directories.h"
#include "unixwrappers.h"
#include "git.h"
#include "projectutils.h"
#include "snapshots.h"
#include "gtkwrappers.h"
#include "compress.h"
#include "dialogdate.h"
#include "date_time_utils.h"
#include "keyterms.h"


ExportAssistant::ExportAssistant(WindowReferences * references_window, WindowStyles * styles_window, WindowCheckKeyterms * check_keyterms_window) :
AssistantBase("Export", "export")
// Export assistant.
{
  gtk_assistant_set_forward_page_func (GTK_ASSISTANT (assistant), GtkAssistantPageFunc (assistant_forward_function), gpointer(this), NULL);
  
  g_signal_connect (G_OBJECT (assistant), "apply", G_CALLBACK (on_assistant_apply_signal), gpointer(this));
  g_signal_connect (G_OBJECT (assistant), "prepare", G_CALLBACK (on_assistant_prepare_signal), gpointer(this));

  introduction ("This helps you exporting your data");

  // Configuration and initialization.
  extern Settings *settings;
  ustring project = settings->genconfig.project_get();
  sword_module_created = false;
  my_references_window = references_window;
  my_styles_window = styles_window;
  my_check_keyterms_window = check_keyterms_window;
  date_time = 0;

  // Build the GUI for the task selector.
  vbox_select_type = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_select_type);
  page_number_select_type = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_select_type);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_select_type), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_select_type, "What would you like to export?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_select_type, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_select_type, true);

  GSList *radiobutton_select_type_group = NULL;

  radiobutton_select_type_bible = gtk_radio_button_new_with_mnemonic (NULL, "Bible");
  gtk_widget_show (radiobutton_select_type_bible);
  gtk_box_pack_start (GTK_BOX (vbox_select_type), radiobutton_select_type_bible, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_select_type_bible), radiobutton_select_type_group);
  radiobutton_select_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_select_type_bible));

  radiobutton_select_type_references = gtk_radio_button_new_with_mnemonic (NULL, "References");
  gtk_widget_show (radiobutton_select_type_references);
  gtk_box_pack_start (GTK_BOX (vbox_select_type), radiobutton_select_type_references, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_select_type_references), radiobutton_select_type_group);
  radiobutton_select_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_select_type_references));

  // Exporting references only works when the references window shows.
  gtk_widget_set_sensitive (radiobutton_select_type_references, references_window != NULL);
  
  radiobutton_select_type_stylesheet = gtk_radio_button_new_with_mnemonic (NULL, "Stylesheet");
  gtk_widget_show (radiobutton_select_type_stylesheet);
  gtk_box_pack_start (GTK_BOX (vbox_select_type), radiobutton_select_type_stylesheet, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_select_type_stylesheet), radiobutton_select_type_group);
  radiobutton_select_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_select_type_stylesheet));

  // Exporting references only works when the styles window shows.
  gtk_widget_set_sensitive (radiobutton_select_type_stylesheet, my_styles_window != NULL);

  radiobutton_select_type_notes = gtk_radio_button_new_with_mnemonic (NULL, "Notes");
  gtk_widget_show (radiobutton_select_type_notes);
  gtk_box_pack_start (GTK_BOX (vbox_select_type), radiobutton_select_type_notes, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_select_type_notes), radiobutton_select_type_group);
  radiobutton_select_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_select_type_notes));

  radiobutton_select_type_keyterms = gtk_radio_button_new_with_mnemonic (NULL, "Keyterms");
  gtk_widget_show (radiobutton_select_type_keyterms);
  gtk_box_pack_start (GTK_BOX (vbox_select_type), radiobutton_select_type_keyterms, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_select_type_keyterms), radiobutton_select_type_group);
  radiobutton_select_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_select_type_keyterms));

  // Exporting keyterms only works when the check keyterms window shows.
  gtk_widget_set_sensitive (radiobutton_select_type_keyterms, my_check_keyterms_window != NULL);
  
  Shortcuts shortcuts_select_type (0);
  shortcuts_select_type.button (radiobutton_select_type_bible);
  shortcuts_select_type.button (radiobutton_select_type_references);
  shortcuts_select_type.button (radiobutton_select_type_stylesheet);
  shortcuts_select_type.button (radiobutton_select_type_notes);
  shortcuts_select_type.button (radiobutton_select_type_keyterms);
  shortcuts_select_type.consider_assistant();
  shortcuts_select_type.process();

  // Confirm or change Bible.
  vbox_bible_name = gtk_vbox_new (FALSE, 5);
  gtk_widget_show (vbox_bible_name);
  page_number_bible_name = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_bible_name);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_bible_name), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_bible_name, "Is this the right Bible?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_bible_name, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_bible_name, true);

  label_project_name = gtk_label_new ("Bible name");
  gtk_widget_show (label_project_name);
  gtk_box_pack_start (GTK_BOX (vbox_bible_name), label_project_name, FALSE, FALSE, 0);

  button_bible_name = gtk_button_new ();
  gtk_widget_show (button_bible_name);
  gtk_box_pack_start (GTK_BOX (vbox_bible_name), button_bible_name, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_bible_name, "clicked", G_CALLBACK (on_button_bible_name_clicked), gpointer (this));

  GtkWidget *alignment1;
  GtkWidget *hbox1;
  GtkWidget *image1;
  GtkWidget *label12;

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (button_bible_name), alignment1);

  hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox1);

  image1 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox1), image1, FALSE, FALSE, 0);

  label12 = gtk_label_new_with_mnemonic ("Choose another one");
  gtk_widget_show (label12);
  gtk_box_pack_start (GTK_BOX (hbox1), label12, FALSE, FALSE, 0);

  Shortcuts shortcuts_bible_name (0);
  shortcuts_bible_name.label (label12);
  shortcuts_bible_name.consider_assistant();
  shortcuts_bible_name.process();

  // Select what type to export a Bible to.
  vbox_bible_type = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_bible_type);
  page_number_bible_type = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_bible_type);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_bible_type), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_bible_type, "What would you like to export it to?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_bible_type, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_bible_type, true);

  GSList *radiobutton_bible_type_group = NULL;

  radiobutton_bible_usfm = gtk_radio_button_new_with_mnemonic (NULL, "Unified Standard Format Markers (USFM)");
  gtk_widget_show (radiobutton_bible_usfm);
  gtk_box_pack_start (GTK_BOX (vbox_bible_type), radiobutton_bible_usfm, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_bible_usfm), radiobutton_bible_type_group);
  radiobutton_bible_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_bible_usfm));

  radiobutton_bible_bibleworks = gtk_radio_button_new_with_mnemonic (NULL, "BibleWorks Version Database Compiler");
  gtk_widget_show (radiobutton_bible_bibleworks);
  gtk_box_pack_start (GTK_BOX (vbox_bible_type), radiobutton_bible_bibleworks, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_bible_bibleworks), radiobutton_bible_type_group);
  radiobutton_bible_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_bible_bibleworks));

  radiobutton_bible_osis = gtk_radio_button_new_with_mnemonic (NULL, "Open Scripture Information Standard (OSIS)");
  gtk_widget_show (radiobutton_bible_osis);
  gtk_box_pack_start (GTK_BOX (vbox_bible_type), radiobutton_bible_osis, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_bible_osis), radiobutton_bible_type_group);
  radiobutton_bible_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_bible_osis));

  radiobutton_bible_sword = gtk_radio_button_new_with_mnemonic (NULL, "CrossWire SWORD");
  gtk_widget_show (radiobutton_bible_sword);
  gtk_box_pack_start (GTK_BOX (vbox_bible_type), radiobutton_bible_sword, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_bible_sword), radiobutton_bible_type_group);
  radiobutton_bible_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_bible_sword));

  radiobutton_bible_opendocument = gtk_radio_button_new_with_mnemonic (NULL, "OpenDocument");
  gtk_widget_show (radiobutton_bible_opendocument);
  gtk_box_pack_start (GTK_BOX (vbox_bible_type), radiobutton_bible_opendocument, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_bible_opendocument), radiobutton_bible_type_group);
  radiobutton_bible_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_bible_opendocument));

  radiobutton_bible_gobible = gtk_radio_button_new_with_mnemonic (NULL, "Go Bible");
  gtk_widget_show (radiobutton_bible_gobible);
  gtk_box_pack_start (GTK_BOX (vbox_bible_type), radiobutton_bible_gobible, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_bible_gobible), radiobutton_bible_type_group);
  radiobutton_bible_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_bible_gobible));

  Shortcuts shortcuts_select_bible_type (0);
  shortcuts_select_bible_type.button (radiobutton_bible_usfm);
  shortcuts_select_bible_type.button (radiobutton_bible_bibleworks);
  shortcuts_select_bible_type.button (radiobutton_bible_osis);
  shortcuts_select_bible_type.button (radiobutton_bible_sword);
  shortcuts_select_bible_type.button (radiobutton_bible_opendocument);
  shortcuts_select_bible_type.button (radiobutton_bible_gobible);
  shortcuts_select_bible_type.consider_assistant();
  shortcuts_select_bible_type.process();

  // Select what type of USFM export to make.
  vbox_usfm_type = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_usfm_type);
  page_number_usfm_type = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_usfm_type);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_usfm_type), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_usfm_type, "What type of USFM export will you make?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_usfm_type, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_usfm_type, true);

  GSList *radiobutton_usfm_type_group = NULL;

  radiobutton_usfm_everything = gtk_radio_button_new_with_mnemonic (NULL, "Everything");
  gtk_widget_show (radiobutton_usfm_everything);
  gtk_box_pack_start (GTK_BOX (vbox_usfm_type), radiobutton_usfm_everything, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_usfm_everything), radiobutton_usfm_type_group);
  radiobutton_usfm_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_usfm_everything));

  radiobutton_usfm_changes_only = gtk_radio_button_new_with_mnemonic (NULL, "Changes only");
  gtk_widget_show (radiobutton_usfm_changes_only);
  gtk_box_pack_start (GTK_BOX (vbox_usfm_type), radiobutton_usfm_changes_only, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_usfm_changes_only), radiobutton_usfm_type_group);
  radiobutton_usfm_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_usfm_changes_only));

  Shortcuts shortcuts_select_usfm_type (0);
  shortcuts_select_usfm_type.button (radiobutton_usfm_everything);
  shortcuts_select_usfm_type.button (radiobutton_usfm_changes_only);
  shortcuts_select_usfm_type.consider_assistant();
  shortcuts_select_usfm_type.process();

  // Select what changes of USFM to export.
  vbox_usfm_changes_type = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_usfm_changes_type);
  page_number_usfm_changes_type = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_usfm_changes_type);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_usfm_changes_type), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_usfm_changes_type, "Which changes will you export?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_usfm_changes_type, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_usfm_changes_type, true);

  GSList *radiobutton_usfm_changes_type_group = NULL;

  radiobutton_usfm_changes_since_last = gtk_radio_button_new_with_mnemonic (NULL, "The ones introduced since last time that the changes were exported");
  gtk_widget_show (radiobutton_usfm_changes_since_last);
  gtk_box_pack_start (GTK_BOX (vbox_usfm_changes_type), radiobutton_usfm_changes_since_last, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_usfm_changes_since_last), radiobutton_usfm_changes_type_group);
  radiobutton_usfm_changes_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_usfm_changes_since_last));

  radiobutton_usfm_changes_since_date = gtk_radio_button_new_with_mnemonic (NULL, "The ones introduces since a certain date and time I'll give");
  gtk_widget_show (radiobutton_usfm_changes_since_date);
  gtk_box_pack_start (GTK_BOX (vbox_usfm_changes_type), radiobutton_usfm_changes_since_date, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_usfm_changes_since_date), radiobutton_usfm_changes_type_group);
  radiobutton_usfm_changes_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_usfm_changes_since_date));

  Shortcuts shortcuts_select_usfm_changes_type (0);
  shortcuts_select_usfm_changes_type.button (radiobutton_usfm_changes_since_last);
  shortcuts_select_usfm_changes_type.button (radiobutton_usfm_changes_since_date);
  shortcuts_select_usfm_changes_type.consider_assistant();
  shortcuts_select_usfm_changes_type.process();

  // Select what type of OSIS file to create.
  vbox_osis_type = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_osis_type);
  page_number_osis_type = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_osis_type);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_osis_type), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_osis_type, "What type of OSIS file would you prefer?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_osis_type, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_osis_type, true);
  
  GSList *radiobutton_osis_type_group = NULL;

  radiobutton_osis_recommended = gtk_radio_button_new_with_mnemonic (NULL, "Recommended transformation");
  gtk_widget_show (radiobutton_osis_recommended);
  gtk_box_pack_start (GTK_BOX (vbox_osis_type), radiobutton_osis_recommended, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_osis_recommended), radiobutton_osis_type_group);
  radiobutton_osis_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_osis_recommended));

  radiobutton_osis_go_bible = gtk_radio_button_new_with_mnemonic (NULL, "For Go Bible Creator");
  gtk_widget_show (radiobutton_osis_go_bible);
  gtk_box_pack_start (GTK_BOX (vbox_osis_type), radiobutton_osis_go_bible, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_osis_go_bible), radiobutton_osis_type_group);
  radiobutton_osis_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_osis_go_bible));

  radiobutton_osis_old = gtk_radio_button_new_with_mnemonic (NULL, "Old method");
  gtk_widget_show (radiobutton_osis_old);
  gtk_box_pack_start (GTK_BOX (vbox_osis_type), radiobutton_osis_old, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_osis_old), radiobutton_osis_type_group);
  radiobutton_osis_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_osis_old));

  Shortcuts shortcuts_select_osis_type (0);
  shortcuts_select_osis_type.button (radiobutton_osis_recommended);
  shortcuts_select_osis_type.button (radiobutton_osis_go_bible);
  shortcuts_select_osis_type.button (radiobutton_osis_old);
  shortcuts_select_osis_type.consider_assistant();
  shortcuts_select_osis_type.process();

  // SWORD module variables.
  entry_sword_name = gtk_entry_new ();
  gtk_widget_show (entry_sword_name);
  page_number_sword_name = gtk_assistant_append_page (GTK_ASSISTANT (assistant), entry_sword_name);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), entry_sword_name, "What is the name for the module?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), entry_sword_name, GTK_ASSISTANT_PAGE_CONTENT);

  g_signal_connect((gpointer) entry_sword_name, "changed", G_CALLBACK(on_entry_sword_changed), gpointer(this));
  
  entry_sword_description = gtk_entry_new ();
  gtk_widget_show (entry_sword_description);
  page_number_sword_description = gtk_assistant_append_page (GTK_ASSISTANT (assistant), entry_sword_description);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), entry_sword_description, "What is the description for the module?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), entry_sword_description, GTK_ASSISTANT_PAGE_CONTENT);

  g_signal_connect((gpointer) entry_sword_description, "changed", G_CALLBACK(on_entry_sword_changed), gpointer(this));

  entry_sword_about = gtk_entry_new ();
  gtk_widget_show (entry_sword_about);
  page_number_sword_about = gtk_assistant_append_page (GTK_ASSISTANT (assistant), entry_sword_about);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), entry_sword_about, "What can you say about the module?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), entry_sword_about, GTK_ASSISTANT_PAGE_CONTENT);

  g_signal_connect((gpointer) entry_sword_about, "changed", G_CALLBACK(on_entry_sword_changed), gpointer(this));

  entry_sword_license = gtk_entry_new ();
  gtk_widget_show (entry_sword_license);
  page_number_sword_license = gtk_assistant_append_page (GTK_ASSISTANT (assistant), entry_sword_license);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), entry_sword_license, "What is the license of the module?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), entry_sword_license, GTK_ASSISTANT_PAGE_CONTENT);

  g_signal_connect((gpointer) entry_sword_license, "changed", G_CALLBACK(on_entry_sword_changed), gpointer(this));

  entry_sword_version = gtk_entry_new ();
  gtk_widget_show (entry_sword_version);
  page_number_sword_version = gtk_assistant_append_page (GTK_ASSISTANT (assistant), entry_sword_version);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), entry_sword_version, "What is the version of the module?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), entry_sword_version, GTK_ASSISTANT_PAGE_CONTENT);

  g_signal_connect((gpointer) entry_sword_version, "changed", G_CALLBACK(on_entry_sword_changed), gpointer(this));

  entry_sword_language = gtk_entry_new ();
  gtk_widget_show (entry_sword_language);
  page_number_sword_language = gtk_assistant_append_page (GTK_ASSISTANT (assistant), entry_sword_language);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), entry_sword_language, "What is the language of the module?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), entry_sword_language, GTK_ASSISTANT_PAGE_CONTENT);

  g_signal_connect((gpointer) entry_sword_language, "changed", G_CALLBACK(on_entry_sword_changed), gpointer(this));

  entry_sword_install_path = gtk_entry_new ();
  gtk_widget_show (entry_sword_install_path);
  page_number_sword_install_path = gtk_assistant_append_page (GTK_ASSISTANT (assistant), entry_sword_install_path);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), entry_sword_install_path, "Where is the module to be installed?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), entry_sword_install_path, GTK_ASSISTANT_PAGE_CONTENT);

  g_signal_connect((gpointer) entry_sword_install_path, "changed", G_CALLBACK(on_entry_sword_changed), gpointer(this));

  sword_values_set ();

  // Stylesheet format.
  vbox_stylesheet_format = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_stylesheet_format);
  page_number_stylesheet_format = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_stylesheet_format);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_stylesheet_format), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_stylesheet_format, "What type of export do you chooseS?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_stylesheet_format, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_stylesheet_format, true);

  GSList *radiobutton_stylesheet_export_type_group = NULL;

  radiobutton_stylesheet_format_bibledit = gtk_radio_button_new_with_mnemonic (NULL, "Standard Bibledit-Gtk format");
  gtk_widget_show (radiobutton_stylesheet_format_bibledit);
  gtk_box_pack_start (GTK_BOX (vbox_stylesheet_format), radiobutton_stylesheet_format_bibledit, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_stylesheet_format_bibledit), radiobutton_stylesheet_export_type_group);
  radiobutton_stylesheet_export_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_stylesheet_format_bibledit));

  radiobutton_stylesheet_format_paratext = gtk_radio_button_new_with_mnemonic (NULL, "Paratext format");
  gtk_widget_show (radiobutton_stylesheet_format_paratext);
  gtk_box_pack_start (GTK_BOX (vbox_stylesheet_format), radiobutton_stylesheet_format_paratext, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_stylesheet_format_paratext), radiobutton_stylesheet_export_type_group);
  radiobutton_stylesheet_export_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_stylesheet_format_paratext));

  Shortcuts shortcuts_select_stylesheet_export_type (0);
  shortcuts_select_stylesheet_export_type.button (radiobutton_stylesheet_format_bibledit);
  shortcuts_select_stylesheet_export_type.button (radiobutton_stylesheet_format_paratext);
  shortcuts_select_stylesheet_export_type.consider_assistant();
  shortcuts_select_stylesheet_export_type.process();

  // Include keyterms without a rendering?
  checkbutton_keyterms_without_rendering = gtk_check_button_new_with_mnemonic ("Include keyterms without a rendering");
  gtk_widget_show (checkbutton_keyterms_without_rendering);
  page_number_keyterms_without_rendering = gtk_assistant_append_page (GTK_ASSISTANT (assistant), checkbutton_keyterms_without_rendering);
  gtk_container_set_border_width (GTK_CONTAINER (checkbutton_keyterms_without_rendering), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), checkbutton_keyterms_without_rendering, "Include keyterms without a rendering?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), checkbutton_keyterms_without_rendering, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), checkbutton_keyterms_without_rendering, true);

  Shortcuts shortcuts_keyterms_wo_rendering (0);
  shortcuts_keyterms_wo_rendering.button (checkbutton_keyterms_without_rendering);
  shortcuts_keyterms_wo_rendering.consider_assistant();
  shortcuts_keyterms_wo_rendering.process();
  
  // Compress it?
  checkbutton_zip = gtk_check_button_new_with_mnemonic ("Compress it");
  gtk_widget_show (checkbutton_zip);
  page_number_zip = gtk_assistant_append_page (GTK_ASSISTANT (assistant), checkbutton_zip);
  gtk_container_set_border_width (GTK_CONTAINER (checkbutton_zip), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), checkbutton_zip, "Would you like to compress it?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), checkbutton_zip, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), checkbutton_zip, true);

  Shortcuts shortcuts_compress (0);
  shortcuts_compress.button (checkbutton_zip);
  shortcuts_compress.consider_assistant();
  shortcuts_compress.process();
  
  // Select date and time.
  vbox_date_time = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_date_time);
  page_number_date_time = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_date_time);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_date_time), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_date_time, "Please enter the date and time?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_date_time, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_date_time, true);

  button_date_time = gtk_button_new ();
  gtk_widget_show (button_date_time);
  gtk_box_pack_start (GTK_BOX (vbox_date_time), button_date_time, FALSE, FALSE, 0);

  GtkWidget *alignment2;
  GtkWidget *hbox2;
  GtkWidget *image2;

  alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (button_date_time), alignment2);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox2);

  image2 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_box_pack_start (GTK_BOX (hbox2), image2, FALSE, FALSE, 0);

  label_date_time = gtk_label_new_with_mnemonic ("");
  gtk_widget_show (label_date_time);
  gtk_box_pack_start (GTK_BOX (hbox2), label_date_time, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_date_time, "clicked", G_CALLBACK (on_button_date_time_clicked), gpointer(this));

  // Comment entry.
  entry_comment = gtk_entry_new ();
  gtk_widget_show (entry_comment);
  page_number_comment = gtk_assistant_append_page (GTK_ASSISTANT (assistant), entry_comment);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), entry_comment, "Any comments on this export?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), entry_comment, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), entry_comment, true);
  
  // Select file where to save to.
  vbox_file = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_file);
  page_number_file = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_file);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_file), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_file, "Where would you like to save it?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_file, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_file, false);

  button_file = gtk_button_new ();
  gtk_widget_show (button_file);
  gtk_box_pack_start (GTK_BOX (vbox_file), button_file, FALSE, FALSE, 0);

  alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (button_file), alignment2);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox2);

  image2 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_box_pack_start (GTK_BOX (hbox2), image2, FALSE, FALSE, 0);

  label_file = gtk_label_new_with_mnemonic ("");
  gtk_widget_show (label_file);
  gtk_box_pack_start (GTK_BOX (hbox2), label_file, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_file, "clicked", G_CALLBACK (on_button_file_clicked), gpointer(this));

  // Select folder where to save to.
  vbox_folder = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_folder);
  page_number_folder = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_folder);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_folder), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_folder, "Where would you like to save it?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_folder, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_folder, false);

  button_folder = gtk_button_new ();
  gtk_widget_show (button_folder);
  gtk_box_pack_start (GTK_BOX (vbox_folder), button_folder, FALSE, FALSE, 0);

  alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (button_folder), alignment2);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox2);

  image2 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_box_pack_start (GTK_BOX (hbox2), image2, FALSE, FALSE, 0);

  label_folder = gtk_label_new_with_mnemonic ("");
  gtk_widget_show (label_folder);
  gtk_box_pack_start (GTK_BOX (hbox2), label_folder, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_folder, "clicked", G_CALLBACK (on_button_folder_clicked), gpointer(this));

  // Build the confirmation stuff.
  label_confirm = gtk_label_new ("Export about to be done");
  gtk_widget_show (label_confirm);
  page_number_confirm = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_confirm);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_confirm, "The export is about to be done");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_confirm, GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_confirm, true);
  
  label_progress = gtk_label_new ("Export in progress");
  gtk_widget_show (label_progress);
  page_number_progress = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_progress);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_progress, "");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_progress, GTK_ASSISTANT_PAGE_PROGRESS);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_progress, true);
  
  label_summary = gtk_label_new ("Export done");
  gtk_widget_show (label_summary);
  summary_page_number = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_summary);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_summary, "Ready");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_summary, GTK_ASSISTANT_PAGE_SUMMARY);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_summary, true);
  
  // Finish building assistant.
  gtk_widget_show_all (assistant);
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), 0);
}

ExportAssistant::~ExportAssistant()
{
}

void ExportAssistant::on_assistant_prepare_signal (GtkAssistant *assistant, GtkWidget *page, gpointer user_data)
{
  ((ExportAssistant *) user_data)->on_assistant_prepare(page);
}


void ExportAssistant::on_assistant_prepare (GtkWidget *page)
{
  extern Settings *settings;

  // Page to confirm or change the name of the Bible.
  if (page == vbox_bible_name) {
    if (bible_name.empty()) {
      bible_name = settings->genconfig.project_get();
    }
    gtk_label_set_text (GTK_LABEL (label_project_name), bible_name.c_str());
    if (bible_name.empty()) {
      gtk_label_set_text (GTK_LABEL (label_project_name), "No Bible selected");
    }
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_bible_name, !bible_name.empty());
  }

  // Page for date / time.
  if (page == vbox_date_time) {
    if (date_time == 0) {
      ProjectConfiguration * projectconfig = settings->projectconfig (bible_name);
      date_time = projectconfig->backup_incremental_last_time_get();
    }
    gtk_label_set_text (GTK_LABEL (label_date_time), date_time_seconds_human_readable (date_time, true).c_str());
  }
  
  // Page for comment.
  if (page == entry_comment) {
    ustring comment = gtk_entry_get_text (GTK_ENTRY (entry_comment));
    if (comment.empty()) {
      ProjectConfiguration * projectconfig = settings->projectconfig (bible_name);
      gtk_entry_set_text (GTK_ENTRY (entry_comment), projectconfig->backup_comment_get().c_str());
    }    
  }
  
  // Page for filename to save to.
  if (page == vbox_file) {
    // We may have to retrieve the filename from the configuration under certain circumstances.
    if (filename.empty ()) {
      if (get_type () == etBible) {
        if (get_bible_type () == ebtBibleWorks) {
          filename = settings->genconfig.export_to_bibleworks_filename_get();
        }
      }
      if (get_type () == etReferences) {
        filename = settings->genconfig.references_file_get();
      }
    }
    gtk_label_set_text (GTK_LABEL (label_file), filename.c_str());
    if (filename.empty()) {
      gtk_label_set_text (GTK_LABEL (label_file), "(None)");
    }
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_file, !filename.empty());
  }
  
  // Page for foldername where to save to.
  if (page == vbox_folder) {
    // Optionally retrieve the folder where to save the Sword module to.
    if (foldername.empty ()) {
      if (get_type () == etBible) {
        if (get_bible_type () == ebtSWORD) {
          foldername = settings->genconfig.export_to_sword_module_path_get().c_str();
        }
      }
    }
    // By default save to the home directory.
    if (foldername.empty ()) {
      foldername = g_get_home_dir ();
    }
    gtk_label_set_text (GTK_LABEL (label_folder), foldername.c_str());
    if (foldername.empty()) {
      gtk_label_set_text (GTK_LABEL (label_folder), "(None)");
    }
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_folder, !foldername.empty());
  }
}


void ExportAssistant::on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data)
{
  ((ExportAssistant *) user_data)->on_assistant_apply();
}


void ExportAssistant::on_assistant_apply ()
{
  // Show progress page.
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), page_number_progress);
  // Take action depending on the type of export.
  switch (get_type()) {
    case etBible:
    {
      switch (get_bible_type()) {
        case ebtUSFM:
        {
          switch (get_usfm_type ()) {
            case eutEverything:
            {
              if (get_compressed ()) {
                export_to_usfm(bible_name, filename, true);
              } else {
                export_to_usfm(bible_name, foldername, false);
              }
              break;
            }
            case eutChangesOnly:
            {
              extern Settings * settings;
              ProjectConfiguration * projectconfig = settings->projectconfig (bible_name);
              ustring comment = gtk_entry_get_text (GTK_ENTRY (entry_comment));
              projectconfig->backup_comment_set (comment);
              switch (get_usfm_changes_type ()) {
                case euctSinceLast:
                {
                  guint32 last_time = projectconfig->backup_incremental_last_time_get();
                  export_to_usfm_changes (bible_name, last_time, comment);                  
                  projectconfig->backup_incremental_last_time_set(date_time_seconds_get_current());
                  break;
                }
                case euctSinceDateTime:
                {
                  export_to_usfm_changes (bible_name, date_time, comment);
                  projectconfig->backup_incremental_last_time_set(date_time);
                  break;
                }
              }
              break;
            }
          }
          break;
        }
        case ebtBibleWorks:
        {
          export_to_bibleworks(bible_name, filename);
          break;
        }
        case ebtOSIS:
        {
          switch (get_osis_type ()) {
            case eotRecommended:
            {
              export_to_osis_recommended (bible_name, filename);
              break;
            }
            case eotGoBibleCreator:
            {
              export_to_osis_for_go_bible_creator (bible_name, filename);
              break;
            }
            case eotOld:
            {
              export_to_osis_old (bible_name, filename);
              break;
            }
          }
          break;
        }
        case ebtSWORD:
        {
          extern Settings *settings;
          ProjectConfiguration *projectconfig = settings->projectconfig(bible_name);
          ustring name = gtk_entry_get_text(GTK_ENTRY(entry_sword_name));
          replace_text(name, " ", "_");
          projectconfig->sword_name_set(name);
          projectconfig->sword_description_set(gtk_entry_get_text(GTK_ENTRY(entry_sword_description)));
          projectconfig->sword_about_set(gtk_entry_get_text(GTK_ENTRY(entry_sword_about)));
          projectconfig->sword_license_set(gtk_entry_get_text(GTK_ENTRY(entry_sword_license)));
          projectconfig->sword_version_set(gtk_entry_get_text(GTK_ENTRY(entry_sword_version)));
          projectconfig->sword_language_set(gtk_entry_get_text(GTK_ENTRY(entry_sword_language)));
          settings->genconfig.export_to_sword_install_path_set(gtk_entry_get_text(GTK_ENTRY(entry_sword_install_path)));
          settings->genconfig.export_to_sword_module_path_set(foldername);
          export_to_sword (bible_name, foldername);
          sword_module_created = true;
          break;
        }
        case ebtOpenDocument:
        {
          extern Settings *settings;
          settings->genconfig.references_file_set(filename);
          export_to_opendocument(bible_name, filename);
          break;
        }
        case ebtGoBible:
        {
          if (gw_find_program_in_path("gobiblecreator")) {
            export_to_go_bible (bible_name, foldername);
          } else {
            gtkw_dialog_info (NULL, "The GoBibleCreator was not found. Install it. See the online help.");
          }
          break;
        }
      }
      break;
    }
    case etReferences:
    {
      if (my_references_window) {
        my_references_window->save(filename);
      }
      break;
    }
    case etStylesheet:
    {
      if (my_styles_window) {
        my_styles_window->export_sheet(filename, get_stylesheet_export_format ());
      }
      break;
    }
    case etNotes:
    {
      vector <unsigned int> ids_to_display;
      export_translation_notes(filename, ids_to_display, true);
      break;
    }
    case etKeyterms:
    {
      keyterms_export_renderings (my_check_keyterms_window->collection(), get_include_keyterms_without_rendering ());
      break;
    }
  }
  // Show summary.
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), summary_page_number);
  
  // Save values.
  extern Settings * settings;
  if (get_type () == etBible) {
    if (get_bible_type () == ebtBibleWorks) {
      settings->genconfig.export_to_bibleworks_filename_set(filename);
    }
  }
}


gint ExportAssistant::assistant_forward_function (gint current_page, gpointer user_data)
{
  return ((ExportAssistant *) user_data)->assistant_forward (current_page);
}


gint ExportAssistant::assistant_forward (gint current_page)
{
  // Create forward sequence.
  forward_sequence.clear();
  switch (get_type()) {
    case etBible:
    {
      switch (get_bible_type()) {
        case ebtUSFM:
        {
          switch (get_usfm_type ()) {
            case eutEverything:
            {
              forward_sequence.insert (page_number_select_type);
              forward_sequence.insert (page_number_bible_name);
              forward_sequence.insert (page_number_bible_type);
              forward_sequence.insert (page_number_usfm_type);
              forward_sequence.insert (page_number_zip);
              if (get_compressed ()) {
                forward_sequence.insert (page_number_file);
              } else {
                forward_sequence.insert (page_number_folder);
              }
              break;
            }
            case eutChangesOnly:
            {
              forward_sequence.insert (page_number_select_type);
              forward_sequence.insert (page_number_bible_name);
              forward_sequence.insert (page_number_bible_type);
              forward_sequence.insert (page_number_usfm_type);
              forward_sequence.insert (page_number_usfm_changes_type);
              switch (get_usfm_changes_type ()) {
                case euctSinceLast:
                {
                  break;
                }
                case euctSinceDateTime:
                {
                  forward_sequence.insert (page_number_date_time);
                  break;
                }
              }
              forward_sequence.insert (page_number_comment);
              break;
            }
          }
          break;
        }
        case ebtBibleWorks:
        {
          forward_sequence.insert (page_number_select_type);
          forward_sequence.insert (page_number_bible_name);
          forward_sequence.insert (page_number_bible_type);
          forward_sequence.insert (page_number_file);
          break;
        }
        case ebtOSIS:
        {
          forward_sequence.insert (page_number_select_type);
          forward_sequence.insert (page_number_bible_name);
          forward_sequence.insert (page_number_bible_type);
          forward_sequence.insert (page_number_osis_type);
          forward_sequence.insert (page_number_file);
          switch (get_osis_type ()) {
            case eotRecommended:
            {
              break;
            }
            case eotGoBibleCreator:
            {
              break;
            }
            case eotOld:
            {
              break;
            }
          }
          break;
        }
        case ebtSWORD:
        {
          forward_sequence.insert (page_number_select_type);
          forward_sequence.insert (page_number_bible_name);
          forward_sequence.insert (page_number_bible_type);
          forward_sequence.insert (page_number_sword_name);
          forward_sequence.insert (page_number_sword_description);
          forward_sequence.insert (page_number_sword_about);
          forward_sequence.insert (page_number_sword_license);
          forward_sequence.insert (page_number_sword_version);
          forward_sequence.insert (page_number_sword_language);
          forward_sequence.insert (page_number_sword_install_path);
          forward_sequence.insert (page_number_folder);
          break;
        }
        case ebtOpenDocument:
        {
          forward_sequence.insert (page_number_select_type);
          forward_sequence.insert (page_number_bible_name);
          forward_sequence.insert (page_number_bible_type);
          forward_sequence.insert (page_number_file);
          break;
        }
        case ebtGoBible:
        {
          forward_sequence.insert (page_number_select_type);
          forward_sequence.insert (page_number_bible_name);
          forward_sequence.insert (page_number_bible_type);
          forward_sequence.insert (page_number_folder);
          break;
        }
      }
      break;
    }
    case etReferences:
    {
      forward_sequence.insert (page_number_select_type);
      forward_sequence.insert (page_number_file);
      break;
    }
    case etStylesheet:
    {
      forward_sequence.insert (page_number_select_type);
      forward_sequence.insert (page_number_stylesheet_format);
      forward_sequence.insert (page_number_file);
      break;
    }
    case etNotes:
    {
      forward_sequence.insert (page_number_select_type);
      forward_sequence.insert (page_number_file);
      break;
    }
    case etKeyterms:
    {
      forward_sequence.insert (page_number_keyterms_without_rendering);
      break;
    }
  }

  // Always end up goint to the confirmation and summary pages.
  forward_sequence.insert (page_number_confirm);
  forward_sequence.insert (summary_page_number);
  
  // Take the next page in the forward sequence if there's one available.
  if (current_page < summary_page_number) {
    do {
      current_page++;
    } while (forward_sequence.find (current_page) == forward_sequence.end());
  }
  return current_page;
}


void ExportAssistant::on_button_bible_name_clicked (GtkButton *button, gpointer user_data)
{
  ((ExportAssistant *) user_data)->on_button_bible_name ();
}


void ExportAssistant::on_button_bible_name ()
{
  project_select(bible_name);
  on_assistant_prepare (vbox_bible_name);
  sword_values_set ();
}


void ExportAssistant::on_button_date_time_clicked (GtkButton *button, gpointer user_data)
{
  ((ExportAssistant *) user_data)->on_button_date_time ();
}


void ExportAssistant::on_button_date_time ()
{
  DateDialog dialog(&date_time, true);
  dialog.run();
  on_assistant_prepare (vbox_date_time);
}


void ExportAssistant::on_button_file_clicked (GtkButton *button, gpointer user_data)
{
  ((ExportAssistant *) user_data)->on_button_file ();
}


void ExportAssistant::on_button_file ()
{
  ustring file = gtkw_file_chooser_save (assistant, "", filename);
  if (!file.empty()) {
    filename = file;
    if ((get_type() == etBible) && (get_bible_type() == ebtUSFM)) {
      compress_ensure_zip_suffix (filename);
    }
    on_assistant_prepare (vbox_file);
  }
}


void ExportAssistant::on_button_folder_clicked (GtkButton *button, gpointer user_data)
{
  ((ExportAssistant *) user_data)->on_button_folder ();
}


void ExportAssistant::on_button_folder ()
{
  ustring folder = gtkw_file_chooser_select_folder (assistant, "", foldername);
  if (!folder.empty()) {
    foldername = folder;
    on_assistant_prepare (vbox_folder);
  }
}


ExportType ExportAssistant::get_type ()
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_bible))) {
    return etBible;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_references))) {
    return etReferences;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_stylesheet))) {
    return etStylesheet;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_notes))) {
    return etNotes;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_keyterms))) {
    return etKeyterms;
  }
  return etBible;
}


ExportBibleType ExportAssistant::get_bible_type ()
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_bible_usfm))) {
    return ebtUSFM;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_bible_bibleworks))) {
    return ebtBibleWorks;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_bible_osis))) {
    return ebtOSIS;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_bible_sword))) {
    return ebtSWORD;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_bible_opendocument))) {
    return ebtOpenDocument;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_bible_gobible))) {
    return ebtGoBible;
  }
  return ebtUSFM;
}


ExportUsfmType ExportAssistant::get_usfm_type ()
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_usfm_everything))) {
    return eutEverything;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_usfm_changes_only))) {
    return eutChangesOnly;
  }
  return eutEverything;
}


ExportUsfmChangesType ExportAssistant::get_usfm_changes_type ()
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_usfm_changes_since_last))) {
    return euctSinceLast;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_usfm_changes_since_date))) {
    return euctSinceDateTime;
  }
  return euctSinceLast;
}


bool ExportAssistant::get_compressed ()
{
  return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton_zip));
}


ExportOsisType ExportAssistant::get_osis_type ()
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_osis_recommended))) {
    return eotRecommended;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_osis_go_bible))) {
    return eotGoBibleCreator;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_osis_old))) {
    return eotOld;
  }
  return eotRecommended;
}


void ExportAssistant::on_entry_sword_changed(GtkEditable * editable, gpointer user_data)
{
  ((ExportAssistant *) user_data)->on_entry_sword(editable);
}


void ExportAssistant::on_entry_sword(GtkEditable *editable)
{
  ustring value = gtk_entry_get_text(GTK_ENTRY(editable));
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), GTK_WIDGET (editable), !value.empty());
}


void ExportAssistant::sword_values_set ()
{
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig (bible_name);
  gtk_entry_set_text(GTK_ENTRY(entry_sword_name), projectconfig->sword_name_get().c_str());
  gtk_entry_set_text(GTK_ENTRY(entry_sword_description), projectconfig->sword_description_get().c_str());
  gtk_entry_set_text(GTK_ENTRY(entry_sword_about), projectconfig->sword_about_get().c_str());
  gtk_entry_set_text(GTK_ENTRY(entry_sword_license), projectconfig->sword_license_get().c_str());
  gtk_entry_set_text(GTK_ENTRY(entry_sword_version), projectconfig->sword_version_get().c_str());
  gtk_entry_set_text(GTK_ENTRY(entry_sword_language), projectconfig->sword_language_get().c_str());
  gtk_entry_set_text(GTK_ENTRY(entry_sword_install_path), settings->genconfig.export_to_sword_install_path_get().c_str());
}


bool ExportAssistant::get_include_keyterms_without_rendering ()
{
  return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton_keyterms_without_rendering));
}


StylesheetExportFormatType ExportAssistant::get_stylesheet_export_format ()
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_stylesheet_format_bibledit))) {
    return seftBibledit;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_stylesheet_format_paratext))) {
    return seftParatext;
  }
  return seftBibledit;
}


