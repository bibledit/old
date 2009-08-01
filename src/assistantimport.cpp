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
#include "assistantimport.h"
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
#include "usfmtools.h"
#include "books.h"
#include "bibleworks.h"
#include "onlinebible.h"


ImportAssistant::ImportAssistant(WindowReferences * references_window, WindowStyles * styles_window, WindowCheckKeyterms * check_keyterms_window) :
AssistantBase("Import", "import")
// Import assistant.
{
  gtk_assistant_set_forward_page_func (GTK_ASSISTANT (assistant), GtkAssistantPageFunc (assistant_forward_function), gpointer(this), NULL);
  
  g_signal_connect (G_OBJECT (assistant), "apply", G_CALLBACK (on_assistant_apply_signal), gpointer(this));
  g_signal_connect (G_OBJECT (assistant), "prepare", G_CALLBACK (on_assistant_prepare_signal), gpointer(this));

  introduction ("This helps you importing data");

  // Configuration and initialization.
  extern Settings *settings;
  ustring project = settings->genconfig.project_get();
  my_references_window = references_window;
  my_styles_window = styles_window;
  my_check_keyterms_window = check_keyterms_window;
  date_time = 0;
  unicode_okay = false;

  // Build the GUI for the task selector.
  vbox_select_type = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_select_type);
  page_number_select_type = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_select_type);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_select_type), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_select_type, "What would you like to import?");
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

  // Importing references only works when the references window shows.
  gtk_widget_set_sensitive (radiobutton_select_type_references, references_window != NULL);
  
  radiobutton_select_type_stylesheet = gtk_radio_button_new_with_mnemonic (NULL, "Stylesheet");
  gtk_widget_show (radiobutton_select_type_stylesheet);
  gtk_box_pack_start (GTK_BOX (vbox_select_type), radiobutton_select_type_stylesheet, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_select_type_stylesheet), radiobutton_select_type_group);
  radiobutton_select_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_select_type_stylesheet));

  // Importing styles only works when the styles window shows.
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

  // Importing keyterms only works when the check keyterms window shows.
  gtk_widget_set_sensitive (radiobutton_select_type_keyterms, my_check_keyterms_window != NULL);
  
  Shortcuts shortcuts_select_type (0);
  shortcuts_select_type.button (radiobutton_select_type_bible);
  shortcuts_select_type.button (radiobutton_select_type_references);
  shortcuts_select_type.button (radiobutton_select_type_stylesheet);
  shortcuts_select_type.button (radiobutton_select_type_notes);
  shortcuts_select_type.button (radiobutton_select_type_keyterms);
  shortcuts_select_type.consider_assistant();
  shortcuts_select_type.process();

  // Confirm or change Bible to import into.
  vbox_bible_name = gtk_vbox_new (FALSE, 5);
  gtk_widget_show (vbox_bible_name);
  page_number_bible_name = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_bible_name);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_bible_name), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_bible_name, "Is this the right Bible to import into?");
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

  // Select what type of Bible data to import.
  vbox_bible_type = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_bible_type);
  page_number_bible_type = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_bible_type);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_bible_type), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_bible_type, "What type of data would you like to import?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_bible_type, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_bible_type, true);

  GSList *radiobutton_bible_type_group = NULL;

  radiobutton_bible_usfm = gtk_radio_button_new_with_mnemonic (NULL, "Unified Standard Format Markers (USFM)");
  gtk_widget_show (radiobutton_bible_usfm);
  gtk_box_pack_start (GTK_BOX (vbox_bible_type), radiobutton_bible_usfm, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_bible_usfm), radiobutton_bible_type_group);
  radiobutton_bible_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_bible_usfm));

  radiobutton_bible_bibleworks = gtk_radio_button_new_with_mnemonic (NULL, "BibleWorks Exported Database");
  gtk_widget_show (radiobutton_bible_bibleworks);
  gtk_box_pack_start (GTK_BOX (vbox_bible_type), radiobutton_bible_bibleworks, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_bible_bibleworks), radiobutton_bible_type_group);
  radiobutton_bible_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_bible_bibleworks));

  radiobutton_bible_online_bible = gtk_radio_button_new_with_mnemonic (NULL, "Online Bible Exported Text - out of order");
  gtk_widget_show (radiobutton_bible_online_bible);
  gtk_box_pack_start (GTK_BOX (vbox_bible_type), radiobutton_bible_online_bible, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_bible_online_bible), radiobutton_bible_type_group);
  radiobutton_bible_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_bible_online_bible));

  radiobutton_bible_raw_text = gtk_radio_button_new_with_mnemonic (NULL, "Raw Text");
  gtk_widget_show (radiobutton_bible_raw_text);
  gtk_box_pack_start (GTK_BOX (vbox_bible_type), radiobutton_bible_raw_text, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_bible_raw_text), radiobutton_bible_type_group);
  radiobutton_bible_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_bible_raw_text));

  Shortcuts shortcuts_select_bible_type (0);
  shortcuts_select_bible_type.button (radiobutton_bible_usfm);
  shortcuts_select_bible_type.button (radiobutton_bible_bibleworks);
  shortcuts_select_bible_type.button (radiobutton_bible_online_bible);
  shortcuts_select_bible_type.button (radiobutton_bible_raw_text);
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
  
  // Select files to import from.
  vbox_files = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_files);
  page_number_files = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_files);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_files), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_files, "Which file or files would you like to import?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_files, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_files, false);

  button_files = gtk_button_new ();
  gtk_widget_show (button_files);
  gtk_box_pack_start (GTK_BOX (vbox_files), button_files, FALSE, FALSE, 0);

  alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (button_files), alignment2);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox2);

  image2 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_box_pack_start (GTK_BOX (hbox2), image2, FALSE, FALSE, 0);

  GtkWidget * label = gtk_label_new_with_mnemonic ("Select files");
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_files, "clicked", G_CALLBACK (on_button_files_clicked), gpointer(this));

  label_files = gtk_label_new ("");
  gtk_widget_show (label_files);
  gtk_box_pack_start (GTK_BOX (vbox_files), label_files, FALSE, FALSE, 0);

  // Unicode check.
  vbox_unicode = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_unicode);
  page_number_unicode = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_unicode);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_unicode), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_unicode, "Unicode check");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_unicode, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_unicode, false);

  button_unicode = gtk_button_new ();
  gtk_widget_show (button_unicode);
  gtk_box_pack_start (GTK_BOX (vbox_unicode), button_unicode, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_unicode, "clicked", G_CALLBACK (on_button_unicode_clicked), gpointer(this));

  alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (button_unicode), alignment2);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox2);

  image2 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_box_pack_start (GTK_BOX (hbox2), image2, FALSE, FALSE, 0);

  label_unicode = gtk_label_new_with_mnemonic ("");
  gtk_widget_show (label_unicode);
  gtk_box_pack_start (GTK_BOX (hbox2), label_unicode, FALSE, FALSE, 0);

  // Build the confirmation stuff.
  label_confirm = gtk_label_new ("Import about to be done");
  gtk_widget_show (label_confirm);
  page_number_confirm = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_confirm);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_confirm, "The import is about to be done");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_confirm, GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_confirm, true);
  
  label_progress = gtk_label_new ("");
  gtk_widget_show (label_progress);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_progress);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_progress, "");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_progress, GTK_ASSISTANT_PAGE_PROGRESS);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_progress, true);
  
  label_summary = gtk_label_new ("Import done");
  gtk_widget_show (label_summary);
  summary_page_number = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_summary);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_summary, "Ready");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_summary, GTK_ASSISTANT_PAGE_SUMMARY);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_summary, true);
  
  // Finish building assistant.
  gtk_widget_show_all (assistant);
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), 0);
}

ImportAssistant::~ImportAssistant()
{
}

void ImportAssistant::on_assistant_prepare_signal (GtkAssistant *assistant, GtkWidget *page, gpointer user_data)
{
  ((ImportAssistant *) user_data)->on_assistant_prepare(page);
}


void ImportAssistant::on_assistant_prepare (GtkWidget *page)
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
  
  // Page for filenames to import.
  if (page == vbox_files) {
    if (files_names.empty()) {
      files_messages.push_back ("No files selected");
    }
    ustring label;
    label.append ("Files count: ");
    label.append (convert_to_string (files_names.size()));
    label.append ("\n");
    for (unsigned int i = 0; i < files_messages.size(); i++) {
      label.append ("\n");
      label.append (files_messages[i]);
    }
    gtk_label_set_text (GTK_LABEL (label_files), label.c_str());
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_files, files_messages.empty());
  }
  
  // Page for the unicode check.
  if (page == vbox_unicode) {
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_unicode, unicode_okay);
  }
  
  // Page for summary.
  if (page == label_summary) {
    ustring label;
    for (unsigned int i = 0; i < summary_messages.size(); i++) {
      if (i)
        label.append ("\n");
      label.append (summary_messages[i]);
    }
    if (!label.empty())
      gtk_label_set_text (GTK_LABEL (label_summary), label.c_str());
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_files, summary_messages.empty());
  }
}


void ImportAssistant::on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data)
{
  ((ImportAssistant *) user_data)->on_assistant_apply();
}


void ImportAssistant::on_assistant_apply ()
{
  // Take action depending on the type of import.
  switch (get_type()) {
    case etBible:
    {
      switch (get_bible_type()) {
        case ibtUsfm:
        {
          for (unsigned int i = 0; i < files_names.size(); i++) {
            import_usfm_file (files_names[i], files_book_ids[i], bible_name, summary_messages);
          }
          break;
        }
        case ibtBibleWorks:
        {
          import_bibleworks_file (files_names[0], bible_name, summary_messages);
          break;
        }
        case ibtOnlineBible:
        {
          summary_messages.push_back ("This is out of order right now");
          break;
        }
        case ibtRawText:
        {
          summary_messages.push_back ("Importing raw text is a manual process. The online help provides more information on that.");
          break;
        }
      }
      break;
    }
    case etReferences:
    {
      summary_messages.push_back ("Importing references is done in the references window.");
      summary_messages.push_back ("In that window, click on [actions], then click on \"Import a list of references\".");
      break;
    }
    case etStylesheet:
    {
      if (my_styles_window) {
        my_styles_window->on_stylesheet_import();
      }
      break;
    }
    case etNotes:
    {
      summary_messages.push_back ("After this window closes an opportunity will be offered to import notes.");
      import_notes = true;
      break;
    }
    case etKeyterms:
    {
      summary_messages.push_back ("After this window closes an opportunity will be offered to import keyterms.");
      import_keyterms = true;
      break;
    }
  }
  // Show summary.
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), summary_page_number);
  on_assistant_prepare (label_summary);
}


gint ImportAssistant::assistant_forward_function (gint current_page, gpointer user_data)
{
  return ((ImportAssistant *) user_data)->assistant_forward (current_page);
}


gint ImportAssistant::assistant_forward (gint current_page)
{
  // Create forward sequence.
  forward_sequence.clear();
  forward_sequence.insert (page_number_select_type);
  switch (get_type()) {
    case etBible:
    {
      switch (get_bible_type()) {
        case ibtUsfm:
        {
          forward_sequence.insert (page_number_bible_name);
          forward_sequence.insert (page_number_bible_type);
          forward_sequence.insert (page_number_files);
          break;
        }
        case ibtBibleWorks:
        {
          forward_sequence.insert (page_number_bible_name);
          forward_sequence.insert (page_number_bible_type);
          forward_sequence.insert (page_number_files);
          break;
        }
        case ibtOnlineBible:
        {
          forward_sequence.insert (page_number_bible_name);
          forward_sequence.insert (page_number_bible_type);
          break;
        }
        case ibtRawText:
        {
          forward_sequence.insert (page_number_bible_name);
          forward_sequence.insert (page_number_bible_type);
          break;
        }
      }
      break;
    }
    case etReferences:
    {
      break;
    }
    case etStylesheet:
    {
      break;
    }
    case etNotes:
    {
      break;
    }
    case etKeyterms:
    {
      break;
    }
  }

  // Always end up going to the confirmation and summary pages.
  forward_sequence.insert (page_number_confirm);
  forward_sequence.insert (summary_page_number);
  
  // Take the next page in the forward sequence.
  do {
    current_page++;
  } while (forward_sequence.find (current_page) == forward_sequence.end());
  return current_page;
}


void ImportAssistant::on_button_bible_name_clicked (GtkButton *button, gpointer user_data)
{
  ((ImportAssistant *) user_data)->on_button_bible_name ();
}


void ImportAssistant::on_button_bible_name ()
{
  project_select(bible_name);
  on_assistant_prepare (vbox_bible_name);
  sword_values_set ();
}


void ImportAssistant::on_button_date_time_clicked (GtkButton *button, gpointer user_data)
{
  ((ImportAssistant *) user_data)->on_button_date_time ();
}


void ImportAssistant::on_button_date_time ()
{
  DateDialog dialog(&date_time, true);
  dialog.run();
  on_assistant_prepare (vbox_date_time);
}


void ImportAssistant::on_button_files_clicked (GtkButton *button, gpointer user_data)
{
  ((ImportAssistant *) user_data)->on_button_files ();
}


void ImportAssistant::on_button_files ()
// Selection and processing of the files to import.
{
  // Set directory.
  ustring directory;
  if (!files_names.empty()) {
    directory = gw_path_get_dirname (files_names[0]);
  }

  // Processing variables.
  files_messages.clear();
  files_book_ids.clear();
  
  // Select files.
  {
    vector <ustring> files = gtkw_file_chooser_open_multiple (assistant, "", directory);
    if (!files.empty()) {
      files_names = files;
    }
  }

  // Ensure that there are only uncompressed files, or only one compressed file.
  vector <ustring> compressed_files;
  for (unsigned int i = 0; i < files_names.size(); i++) {
    if (compressed_archive_recognized (files_names[i]))
      compressed_files.push_back (files_names[i]);
  }
  if (!compressed_files.empty()) {
    if (compressed_files.size() > 1) {
      files_messages.push_back ("You have selected more than one compressed file");
    }
    if (compressed_files.size() == 1) {
      if (files_names.size() != 1) {
        files_messages.push_back ("You have selected a mixture of normal and compressed files");
      }
    }
  }

  // Optionally uncompress the archive and let the user select files from within it.
  if (files_messages.empty()) {
    if (compressed_files.size() == 1) {
      ustring unpack_directory = gw_build_filename (directories_get_temp (), "uncompress");
      unix_rmdir (unpack_directory);
      uncompress (compressed_files[0], unpack_directory);
      gtkw_dialog_info (assistant, "You will now be asked to select files from within the compressed archive");
      files_names = gtkw_file_chooser_open_multiple (assistant, "", unpack_directory);
    }
  }

  // Check that all files are in Unicode.
  if (files_messages.empty()) {
    vector <ustring> unicode_files;
    vector <ustring> non_unicode_files;
    for (unsigned int i = 0; i < files_names.size(); i++) {
      ustring contents;
      gchar *s;
      g_file_get_contents(files_names[i].c_str(), &s, NULL, NULL);
      contents = s;
      g_free(s);
      if (contents.validate()) {
        unicode_files.push_back (files_names[i]);
      } else {
        non_unicode_files.push_back (files_names[i]);
      }
    }
    files_names = unicode_files;
    if (!non_unicode_files.empty()) {
      files_messages.push_back ("The following files are not in the right Unicode format and are therefore not fit for import:");
      for (unsigned int i = 0; i < non_unicode_files.size(); i++) {
        files_messages.push_back (non_unicode_files[i]);
      }
      files_messages.push_back ("The online help provides more information about how to make these fit for use.");
    }
  }

  // Check that at least one file was selected.
  if (files_messages.empty()) {
    if (files_names.empty()) {
      files_messages.push_back ("No files were selected");
    }
  }

  // Specific checks for each import type. 
  switch (get_type ()) {
  case itBible:
    {
      switch (get_bible_type()) {
      case ibtUsfm:
        {
          import_check_usfm_files (files_names, files_book_ids, bible_name, files_messages);
          break;
        }
      case ibtBibleWorks:
        {
          import_check_bibleworks_file (files_names, files_book_ids, bible_name, files_messages);
          break;
        }
      case ibtOnlineBible:
        {
          //online_bible_check_file (files_names, files_book_ids, bible_name, files_messages);
          break;
        }
      case ibtRawText:
        {
          break;
        }
      }
      break;
    }
  case itReferences:
    {
      break;
    }
  case itStylesheet:
    {
      break;
    }
  case itNotes:
    {
      break;
    }
  case itKeyterms:
    {
      break;
    }
  }

  // Gui update.
  on_assistant_prepare (vbox_files);
}


void ImportAssistant::on_button_unicode_clicked (GtkButton *button, gpointer user_data)
{
  ((ImportAssistant *) user_data)->on_button_unicode ();
}


void ImportAssistant::on_button_unicode ()
{
  on_assistant_prepare (vbox_unicode);
}


ImportType ImportAssistant::get_type ()
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_bible))) {
    return itBible;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_references))) {
    return itReferences;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_stylesheet))) {
    return itStylesheet;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_notes))) {
    return itNotes;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_keyterms))) {
    return itKeyterms;
  }
  return itBible;
}


ImportBibleType ImportAssistant::get_bible_type ()
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_bible_usfm))) {
    return ibtUsfm;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_bible_bibleworks))) {
    return ibtBibleWorks;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_bible_online_bible))) {
    return ibtOnlineBible;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_bible_raw_text))) {
    return ibtRawText;
  }
  return ibtUsfm;
}


ExportUsfmType ImportAssistant::get_usfm_type ()
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_usfm_everything))) {
    return eutEverything;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_usfm_changes_only))) {
    return eutChangesOnly;
  }
  return eutEverything;
}


ExportUsfmChangesType ImportAssistant::get_usfm_changes_type ()
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_usfm_changes_since_last))) {
    return euctSinceLast;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_usfm_changes_since_date))) {
    return euctSinceDateTime;
  }
  return euctSinceLast;
}


bool ImportAssistant::get_compressed ()
{
  return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton_zip));
}


ExportOsisType ImportAssistant::get_osis_type ()
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


void ImportAssistant::on_entry_sword_changed(GtkEditable * editable, gpointer user_data)
{
  ((ImportAssistant *) user_data)->on_entry_sword(editable);
}


void ImportAssistant::on_entry_sword(GtkEditable *editable)
{
  ustring value = gtk_entry_get_text(GTK_ENTRY(editable));
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), GTK_WIDGET (editable), !value.empty());
}


void ImportAssistant::sword_values_set ()
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


bool ImportAssistant::get_include_keyterms_without_rendering ()
{
  return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton_keyterms_without_rendering));
}


/*


Todo Import Assistant


The following import paths are to be implemented: Bible / OnlineBible







Complete the helpfile, consolidating all imports and exports into one file, and to work from there.
The help file needs to make a very clear distinction between a backup and an export.
A backup backups everything that belongs to some item, e.g. a whole project or everything.
An export exports part of the data we wish to see, and in a certain format.
A restore is the opposite of a backup.
An import is the opposite of an export.
Write a section about how to convert files into Unicode, using gedit. Refer to this section from the dialog where it checks on Unicode.
  new InDialogHelp(importsfmdialog, NULL, &shortcuts, "import_text");



From dialogproject, the import button goes out.









void set_gui()
{



  // When importing files from the Online Bible, we can use unusual book names.
  // But we should know the book names that are used in e.g. the Dutch books, so these are known by Bibledit.



  case ibtOnlineBible:
    {
      // Divide the files per book as we don't know how many books the user put in one file.
      vector < ustring > olb_text_files(proper_text_files);
      proper_text_files.clear();
      for (unsigned int i = 0; i < olb_text_files.size(); i++) {
        vector < ustring > filenames = online_bible_file_divide(olb_text_files[i], general_adapted_booknames_fill_up(unusual_book_names));
        for (unsigned int i2 = 0; i2 < filenames.size(); i2++) {
          proper_text_files.push_back(filenames[i2]);
        }
      }
      break;
    }
  }

  // Handle Book Assignment.

  // Vector contains all English names of the proper text files.
  vector < ustring > textfiles_names;
  for (unsigned int i = 0; i < proper_text_files.size(); i++) {
    ustring english_name;
    switch (importtype) {
    case ibtOnlineBible:
      {
        unsigned int book, chapter, verse;
        ReadText rt (proper_text_files[i], true, false);
        map <ustring, unsigned int> bookmap = general_adapted_booknames_fill_up(unusual_book_names);
        online_bible_parse_reference (rt.lines[0], book, chapter, verse, bookmap);
        english_name = books_id_to_english(book);
        break;
      }
    case ibtRawText:
      {
        break;
      }
    }
    textfiles_names.push_back(english_name);
  }
  // See if we have booknames for all our textfiles. If not try to get them.
  unknown_filenames.clear();
  for (unsigned int i = 0; i < textfiles_names.size(); i++) {
    if (textfiles_names[i].empty()) {
      textfiles_names[i] = filename_bookname_map[proper_text_files[i]];
      if (textfiles_names[i].empty())
        unknown_filenames.push_back(proper_text_files[i]);
    }
  }
  // If the user has assigned names to unknwon books, even if some books were 
  // left unassigned, it still will be regarded as done.
  if (assigning_done)
    unknown_filenames.clear();
  // Set gui and button depending on whether booknames are fine.  
  gui_okay(image_assign_ok, label_assign_ok, unknown_filenames.empty());
  gtk_widget_set_sensitive(button_assign, !unknown_filenames.empty());
  ustring assign_information;
  if (unknown_filenames.empty()) {
    assign_information = "The names of all books are known";
  } else {
    assign_information = "There are unknown books: " + convert_to_string(unknown_filenames.size());
  }
  gtk_label_set_text(GTK_LABEL(label_assign_info), assign_information.c_str());

  // Clear initialization flag.
  select_all_books = false;
  // Set whether anything was selected.
  gui_okay(image_books, label_books_gui, selected_books.size() > 0);
  // Information for user.
  info = "Books selected: " + convert_to_string((unsigned int)selected_books.size()) + " out of " + convert_to_string((unsigned int)selectable_booknames.size());
  if (!books_already_in_project.empty()) {
    info.append("\nBooks that could not be imported\nbecause they are already in the project: " + convert_to_string((unsigned int)books_already_in_project.size()));
  }
  gtk_label_set_text(GTK_LABEL(label_books), info.c_str());




}


void on_okbutton()
// Does the actual import
{
  extern Settings *settings;

  // Progress.
  ProgressWindow progress("Importing ...", false);
  progress.set_iterate(0, 1, selectable_booknames.size());

  // Go through all selectable books.
  for (unsigned int i = 0; i < selectable_booknames.size(); i++) {
    progress.iterate();
    // Is book selected for import?
    if (selected_books.find(selectable_booknames[i]) != selected_books.end()) {
      // Get book's filename.
      ustring source = selectable_filenames[i];

      ImportBookRead ibr(source, encoding);
      // Depending on type of import, convert the book.
      switch (importtype) {
      case ibtBibleWorks:
        {
          ibr.bibleworks();
          break;
        }
      case ibtOnlineBible:
        {
          ibr.onlinebible(general_adapted_booknames_fill_up(unusual_book_names));
          break;
        }
      case ibtRawText:
        {
          ibr.usfm();
          break;
        }
      }
      // Rewrite the \id line, mainly for unknown books, but also just to be sure.
      for (unsigned int ln = 0; ln < ibr.lines.size(); ln++) {
        if (ibr.lines[ln].find("\\id ") == 0) {
          ustring id = books_id_to_paratext(selectable_booknames[i]);
          if (!id.empty()) {
            ibr.lines[ln].erase(4, 3);
            ibr.lines[ln].insert(4, id);
          }
          break;
        }
      }
      // Categorize the lines.
      CategorizeChapterVerse ccv(ibr.lines);
      // Store in project.
      project_store_book(settings->genconfig.project_get(), selectable_booknames[i], ccv);
    }
  }
}
*/

