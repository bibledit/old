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
#include "onlinebible.h"
#include "combobox.h"


ImportAssistant::ImportAssistant(WindowReferences * references_window, WindowStyles * styles_window, WindowCheckKeyterms * check_keyterms_window, WindowsOutpost * windows_outpost) :
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
  my_windows_outpost = windows_outpost;
  import_notes = false;
  import_keyterms = false;

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

  radiobutton_bible_online_bible = gtk_radio_button_new_with_mnemonic (NULL, "Online Bible Text");
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

  // Online Bible connection?
  label_online_bible_running = gtk_label_new ("");
  gtk_widget_show (label_online_bible_running);
  page_number_online_bible_running = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_online_bible_running);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_online_bible_running, "Connected to the Online Bible?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_online_bible_running, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_online_bible_running, false);

  // In case there's no data in the project, the Online Bible won't get connected.
  // Therefore connect manually.
  if (online_bible_is_running ()) {
    my_windows_outpost->OnlineBibleReferenceGet ();
  }

  // Online Bible to import.
  vbox_online_bible_bible = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_online_bible_bible);
  page_number_online_bible_bible = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_online_bible_bible);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_online_bible_bible, "Which Bible would you like to import?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_online_bible_bible, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_online_bible_bible, false);

  combobox_online_bible_bible = gtk_combo_box_new_text ();
  gtk_widget_show (combobox_online_bible_bible);
  gtk_box_pack_start (GTK_BOX (vbox_online_bible_bible), combobox_online_bible_bible, false, false, 0);

  g_signal_connect ((gpointer) combobox_online_bible_bible, "changed", G_CALLBACK (on_combobox_online_bible_bible_changed), gpointer (this));

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

  GtkWidget * alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (button_files), alignment2);

  GtkWidget * hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox2);

  GtkWidget * image2 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_box_pack_start (GTK_BOX (hbox2), image2, FALSE, FALSE, 0);

  GtkWidget * label = gtk_label_new_with_mnemonic ("Select files");
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_files, "clicked", G_CALLBACK (on_button_files_clicked), gpointer(this));

  label_files = gtk_label_new ("");
  gtk_widget_show (label_files);
  gtk_box_pack_start (GTK_BOX (vbox_files), label_files, FALSE, FALSE, 0);

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
  my_windows_outpost->OnlineBibleDirectMode = false;
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

  // Online Bible connected?
  if (page == label_online_bible_running) {
    if (my_windows_outpost->online_bible_server_connected) {
      gtk_label_set_text (GTK_LABEL (label_online_bible_running), "Yes, connected to the Online Bible");
    } else {
      gtk_label_set_text (GTK_LABEL (label_online_bible_running), "No, could not connect to the Online Bible\nIs the Online Bible running?");
    }
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_online_bible_running, my_windows_outpost->online_bible_server_connected);
  }

  // Online Bible to import. Set the available Bibles.
  if (page == vbox_online_bible_bible) {
    my_windows_outpost->OnlineBibleDirectMode = true;
    vector <ustring> bibles = combobox_get_strings (combobox_online_bible_bible);
    if (bibles.empty()) {
      ustring response = my_windows_outpost->OnlineBibleDirectCommandResponseGet ("GetVersionList");
      if (online_bible_ok_reply_validate (response)) {
        ParseLine parseline (response);
        if (!parseline.lines.empty()) {
          combobox_set_strings (combobox_online_bible_bible, parseline.lines);
          combobox_set_index (combobox_online_bible_bible, 0);
          gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_online_bible_bible, true);
        }
      }
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
  
  // Page for summary.
  if (page == label_summary) {
    ustring label;
    for (unsigned int i = 0; i < summary_messages.size(); i++) {
      if (i)
        label.append ("\n");
      label.append (summary_messages[i]);
    }
    if (!label.empty()) {
      gtk_label_set_text (GTK_LABEL (label_summary), label.c_str());
    }
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_summary, summary_messages.empty());
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
    case itBible:
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
          import_bibleworks_text_file (files_names[0], bible_name, summary_messages);
          break;
        }
        case ibtOnlineBible:
        {
          import_online_bible (my_windows_outpost, combobox_get_active_string (combobox_online_bible_bible), bible_name, summary_messages);
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
    case itReferences:
    {
      summary_messages.push_back ("Importing references is done in the references window.");
      summary_messages.push_back ("In that window, click on [actions], then click on \"Import a list of references\".");
      break;
    }
    case itStylesheet:
    {
      if (my_styles_window) {
        my_styles_window->on_stylesheet_import();
      }
      break;
    }
    case itNotes:
    {
      summary_messages.push_back ("After this window closes an opportunity will be offered to import notes.");
      import_notes = true;
      break;
    }
    case itKeyterms:
    {
      summary_messages.push_back ("After this window closes an opportunity will be offered to import keyterms.");
      import_keyterms = true;
      break;
    }
  }
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
    case itBible:
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
          forward_sequence.insert (page_number_online_bible_running);
          forward_sequence.insert (page_number_online_bible_bible);
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

  // Always end up going to the confirmation and summary pages.
  forward_sequence.insert (page_number_confirm);
  forward_sequence.insert (summary_page_number);
  
  // Take the next page in the forward sequence.
  if (current_page < summary_page_number) {
    do {
      current_page++;
    } while (forward_sequence.find (current_page) == forward_sequence.end());
  }
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
  if (files_messages.empty()) {
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
  }

  // Gui update.
  on_assistant_prepare (vbox_files);
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


void ImportAssistant::on_combobox_online_bible_bible_changed (GtkComboBox *combobox, gpointer user_data)
{
  ((ImportAssistant *) user_data)->on_combobox_online_bible_bible ();
}


void ImportAssistant::on_combobox_online_bible_bible ()
{
}

