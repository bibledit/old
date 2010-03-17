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
#include "assistantimportkeyterms.h"
#include "help.h"
#include "floatingwindow.h"
#include "keyterms.h"
#include "tiny_utilities.h"
#include "projectutils.h"
#include "settings.h"
#include "gtkwrappers.h"

ImportKeytermsAssistant::ImportKeytermsAssistant(int dummy) :
AssistantBase("Keyterms", "importkeyterms")

// Assistant for adding keyterms.
{
  gtk_assistant_set_forward_page_func (GTK_ASSISTANT (assistant), GtkAssistantPageFunc (assistant_forward_function), gpointer(this), NULL);
  
  g_signal_connect (G_OBJECT (assistant), "apply", G_CALLBACK (on_assistant_apply_signal), gpointer(this));

  introduction ("Text files that contain keyterms can be imported into the keyterms database.");

  // File to import.

  vbox_select_file = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_select_file);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_select_file);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_select_file, "Which file would you like to import?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_select_file, GTK_ASSISTANT_PAGE_CONTENT);

  label5 = gtk_label_new ("The file that will be imported should satisfy certain conditions. The online help provides more information.");
  gtk_widget_show (label5);
  gtk_box_pack_start (GTK_BOX (vbox_select_file), label5, FALSE, FALSE, 0);
  gtk_label_set_line_wrap (GTK_LABEL (label5), TRUE);

  button_open =  gtk_file_chooser_button_new ("", GTK_FILE_CHOOSER_ACTION_OPEN);
  gtk_widget_show (button_open);
  gtk_box_pack_start (GTK_BOX (vbox_select_file), button_open, FALSE, FALSE, 0);
  
  g_signal_connect ((gpointer) button_open, "selection-changed", G_CALLBACK (on_file_chooser_open_file_activated), gpointer (this));

  // Type to import.

  vbox_type = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_type);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_type);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_type, "What type of file are you importing?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_type, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_type, true);

  GSList *radiobutton_type_standard_group = NULL;

  radiobutton_type_standard = gtk_radio_button_new_with_mnemonic (NULL, "Standard textfile");
  gtk_widget_show (radiobutton_type_standard);
  gtk_box_pack_start (GTK_BOX (vbox_type), radiobutton_type_standard, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_type_standard), radiobutton_type_standard_group);
  radiobutton_type_standard_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_type_standard));

  radiobutton_type_otkey_db = gtk_radio_button_new_with_mnemonic (NULL, "OTKEY.DB");
  gtk_widget_show (radiobutton_type_otkey_db);
  gtk_box_pack_start (GTK_BOX (vbox_type), radiobutton_type_otkey_db, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_type_otkey_db), radiobutton_type_standard_group);
  radiobutton_type_standard_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_type_otkey_db));

  radiobutton_type_ktref_db = gtk_radio_button_new_with_mnemonic (NULL, "KTREF.DB");
  gtk_widget_show (radiobutton_type_ktref_db);
  gtk_box_pack_start (GTK_BOX (vbox_type), radiobutton_type_ktref_db, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_type_ktref_db), radiobutton_type_standard_group);
  radiobutton_type_standard_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_type_ktref_db));

  radiobutton_type_ktbh = gtk_radio_button_new_with_mnemonic (NULL, "Key Terms in Biblical Hebrew Project");
  gtk_widget_show (radiobutton_type_ktbh);
  gtk_box_pack_start (GTK_BOX (vbox_type), radiobutton_type_ktbh, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_type_ktbh), radiobutton_type_standard_group);
  radiobutton_type_standard_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_type_ktbh));

  // Collection.
  
  vbox_category = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_category);

  gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_category);
  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_category, "Into which collection will you import?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_category, GTK_ASSISTANT_PAGE_CONTENT);

  entry_category = gtk_entry_new ();
  gtk_widget_show (entry_category);
  gtk_box_pack_start (GTK_BOX (vbox_category), entry_category, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) entry_category, "changed", G_CALLBACK (on_entry_category_changed), gpointer (this));

  label_category = gtk_label_new ("");
  gtk_widget_show (label_category);
  gtk_box_pack_start (GTK_BOX (vbox_category), label_category, FALSE, FALSE, 0);
  gtk_label_set_line_wrap (GTK_LABEL (label_category), TRUE);
  
  on_entry_category();
  
  // Importing.
  
  label_confirm = gtk_label_new ("Ready for import");
  gtk_widget_show (label_confirm);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_confirm);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_confirm, "Ready for import");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_confirm, GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_confirm, true);
  
  label_progress = gtk_label_new ("Importing...");
  gtk_widget_show (label_progress);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_progress);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_progress, "Importing");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_progress, GTK_ASSISTANT_PAGE_PROGRESS);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_progress, true);
  
  label_summary = gtk_label_new ("Import done.");
  gtk_widget_show (label_summary);
  summary_page_number = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_summary);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_summary, "Ready");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_summary, GTK_ASSISTANT_PAGE_SUMMARY);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_summary, true);
  
  
  // Finish assistant.
  
  gtk_widget_show_all (assistant);
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), 0);
}


ImportKeytermsAssistant::~ImportKeytermsAssistant()
{
}


void ImportKeytermsAssistant::on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data)
{
  ((ImportKeytermsAssistant *) user_data)->on_assistant_apply();
}


void ImportKeytermsAssistant::on_assistant_apply ()
{
  ustring filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (button_open));
  ustring category = gtk_entry_get_text (GTK_ENTRY (entry_category));
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_type_standard))) {
    keyterms_import_textfile (filename, category);
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_type_otkey_db))) {
    keyterms_import_otkey_db (filename, category);
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_type_ktref_db))) {
    keyterms_import_ktref_db (filename, category);
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_type_ktbh))) {
    keyterms_import_ktbh_txt (filename, category);
  }
  // Show summary.
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), summary_page_number);
}


gint ImportKeytermsAssistant::assistant_forward_function (gint current_page, gpointer user_data)
{
  return ((ImportKeytermsAssistant *) user_data)->assistant_forward (current_page);
}

gint ImportKeytermsAssistant::assistant_forward (gint current_page)
{
  // Default behaviour is to go to the next page.
  return ++current_page;
}


void ImportKeytermsAssistant::on_file_chooser_open_file_activated (GtkFileChooser *chooser, gpointer user_data)
{
  ((ImportKeytermsAssistant *) user_data)->on_file_chooser_open ();
}


void ImportKeytermsAssistant::on_file_chooser_open ()
{
  ustring filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (button_open));
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_select_file, !filename.empty ());
}


void ImportKeytermsAssistant::on_entry_category_changed (GtkEditable *editable, gpointer user_data)
{
  ((ImportKeytermsAssistant *) user_data)->on_entry_category ();
}


void ImportKeytermsAssistant::on_entry_category ()
// Give appropriate information about entering the category.
{
  ustring category = gtk_entry_get_text (GTK_ENTRY (entry_category));
  
  ustring information;
  bool exists = false;
  if (category.empty ()) {
    information = "Please enter a collection.";  
  } else {
    vector <ustring> categories = keyterms_get_categories();
    for (unsigned int i = 0; i < categories.size(); i++) {
      if (category == categories[i]) {
        exists = true;
      }
    }
    if (exists) {
      information = "This collection already exists.";
    }
  }
  gtk_label_set_text (GTK_LABEL (label_category), information.c_str());
  
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_category, (!category.empty()) && (!exists)); 
}

