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
#include "assistantresource.h"
#include "help.h"
#include "settings.h"
#include "utilities.h"
#include "gwrappers.h"
#include "gtkwrappers.h"
#include "tiny_utilities.h"
#include "directories.h"
#include "resource_utils.h"
#include "unixwrappers.h"
#include "progresswindow.h"
#include "dialogbooknames.h"
#include "books.h"
#include "combobox.h"
#include "shell.h"
#include "dialoglistview.h"
#include "dialogresourceconverter.h"
#include "resource_conversion_utils.h"
#include <glib/gi18n.h>

ResourceAssistant::ResourceAssistant(const ustring& resource_template) :
  AssistantBase(_("Resource editor"), _("resource_editor"))
// Assistant for editing or creating a resource.
{
  // Since this Assistant contains browsers, it needs a bigger size.
  gtk_window_resize (GTK_WINDOW (assistant), 800, 600);

  gtk_assistant_set_forward_page_func (GTK_ASSISTANT (assistant), GtkAssistantPageFunc (assistant_forward_function), gpointer(this), NULL);
  
  g_signal_connect (G_OBJECT (assistant), "apply", G_CALLBACK (on_assistant_apply_signal), gpointer(this));
  g_signal_connect (G_OBJECT (assistant), "prepare", G_CALLBACK (on_assistant_prepare_signal), gpointer(this));

  // Configuration and initialization.
  edited_resource_template = resource_template;

  // Working directory.
  unix_rmdir(working_directory());
  gw_mkdir_with_parents(working_directory());

  // If a Resource is being edited, copy it into the working directory. 
  {
    if (!edited_resource_template.empty()) {
      ustring resource_directory = gw_path_get_dirname (edited_resource_template);
      if (resource_directory == Directories->get_package_data()) {
        // It's a template, copy the template only.
        unix_cp (edited_resource_template, working_configuration_file ());
      } else {
        // It's an existing editable resource, copy the whole thing.
        ustring command = "cd" + shell_quote_space(resource_directory);
        command.append("; cp -r *" + shell_quote_space(working_directory()));
        if (system(command.c_str())); // This one does not work with GwSpawn.
      }
    }
  }

  // Introduction.
  ustring intro_message = _("Step by step you will be ");
  if (edited_resource_template.empty()) 
    intro_message.append (_("creating a new Resource"));
  else
    intro_message.append (_("editing the Resource"));
  intro_message.append (_(".\nAny changes you make show up immediately.\n\nThe online help provides more information for each step.")
                       );
  introduction (intro_message);

  // Title.
  vbox_title = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_title);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_title);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_title, _("What is the title of the Resource?"));
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_title, GTK_ASSISTANT_PAGE_CONTENT);

  entry_title = gtk_entry_new ();
  gtk_widget_show (entry_title);
  gtk_box_pack_start (GTK_BOX (vbox_title), entry_title, FALSE, FALSE, 0);

  gtk_entry_set_text (GTK_ENTRY (entry_title), resource_get_title(working_configuration_file ()).c_str());

  g_signal_connect((gpointer) entry_title, "changed", G_CALLBACK(on_entry_title_changed), gpointer(this));

  label_title = gtk_label_new ("");
  gtk_widget_show (label_title);
  gtk_box_pack_start (GTK_BOX (vbox_title), label_title, FALSE, FALSE, 0);

  // Files and folders.
  table_files = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (table_files);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), table_files);
  gtk_container_set_border_width (GTK_CONTAINER (table_files), 10);
  gtk_table_set_row_spacings (GTK_TABLE (table_files), 10);
  gtk_table_set_col_spacings (GTK_TABLE (table_files), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), table_files, _("Would you like to add local files to the Resource?"));
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), table_files, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), table_files, true);

  button_add_files = gtk_button_new ();
  gtk_widget_show (button_add_files);
  gtk_table_attach (GTK_TABLE (table_files), button_add_files, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (button_add_files), alignment1);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox2);

  image1 = gtk_image_new_from_stock ("gtk-add", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox2), image1, FALSE, FALSE, 0);

  label6 = gtk_label_new_with_mnemonic (_("_Add files"));
  gtk_widget_show (label6);
  gtk_box_pack_start (GTK_BOX (hbox2), label6, FALSE, FALSE, 0);

  button_add_folders = gtk_button_new ();
  gtk_widget_show (button_add_folders);
  gtk_table_attach (GTK_TABLE (table_files), button_add_folders, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (button_add_folders), alignment2);

  hbox3 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox3);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox3);

  image2 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_box_pack_start (GTK_BOX (hbox3), image2, FALSE, FALSE, 0);

  label7 = gtk_label_new_with_mnemonic (_("Add f_olders"));
  gtk_widget_show (label7);
  gtk_box_pack_start (GTK_BOX (hbox3), label7, FALSE, FALSE, 0);

  label_files = gtk_label_new ("");
  gtk_widget_show (label_files);
  gtk_table_attach (GTK_TABLE (table_files), label_files, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_files), 0, 0.5);

  label_folders = gtk_label_new ("");
  gtk_widget_show (label_folders);
  gtk_table_attach (GTK_TABLE (table_files), label_folders, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_folders), 0, 0.5);

  g_signal_connect ((gpointer) button_add_files, "clicked",
                    G_CALLBACK (on_button_add_files_clicked),
                    gpointer (this));
  g_signal_connect ((gpointer) button_add_folders, "clicked",
                    G_CALLBACK (on_button_add_folders_clicked),
                    gpointer (this));

  on_button_add_files (true);
  on_button_add_folders (true);

  // Home page.
  vbox_home_page = gtk_vbox_new (FALSE, 20);
  gtk_widget_show (vbox_home_page);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_home_page);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_home_page, _("What is the home page of the Resource?"));
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_home_page, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_home_page, true);

  entry_home_page = gtk_entry_new ();
  gtk_widget_show (entry_home_page);
  gtk_box_pack_start (GTK_BOX (vbox_home_page), entry_home_page, FALSE, FALSE, 0);

  gtk_entry_set_text (GTK_ENTRY (entry_home_page), resource_get_home_page(working_configuration_file ()).c_str());
  
  vbox_home_page_resource = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_home_page_resource);
  gtk_box_pack_start (GTK_BOX (vbox_home_page), vbox_home_page_resource, TRUE, TRUE, 0);

  browser_home_page = new WebkitBrowser (vbox_home_page_resource);
  
  g_signal_connect ((gpointer) entry_home_page, "changed", G_CALLBACK (on_entry_home_page_changed), gpointer (this));

  on_entry_home_page ();

  // Book set.
  vbox_bookset = gtk_vbox_new (FALSE, 10);
  gtk_widget_show (vbox_bookset);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_bookset), 10);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_bookset);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_bookset, _("What are the abbreviations for the books of the Resource?"));
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_bookset, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_bookset, true);

  hbox_bookset = gtk_hbox_new (FALSE, 10);
  gtk_widget_show (hbox_bookset);
  gtk_box_pack_start (GTK_BOX (vbox_bookset), hbox_bookset, FALSE, FALSE, 0);

  button_bookset = gtk_button_new ();
  gtk_widget_show (button_bookset);
  gtk_box_pack_start (GTK_BOX (hbox_bookset), button_bookset, FALSE, FALSE, 0);

  GtkWidget *alignment3;
  GtkWidget *hbox5;
  GtkWidget *image3;
  GtkWidget *label12;

  alignment3 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment3);
  gtk_container_add (GTK_CONTAINER (button_bookset), alignment3);

  hbox5 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox5);
  gtk_container_add (GTK_CONTAINER (alignment3), hbox5);

  image3 = gtk_image_new_from_stock ("gtk-info", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image3);
  gtk_box_pack_start (GTK_BOX (hbox5), image3, FALSE, FALSE, 0);

  label12 = gtk_label_new_with_mnemonic (_("_Abbreviations"));
  gtk_widget_show (label12);
  gtk_box_pack_start (GTK_BOX (hbox5), label12, FALSE, FALSE, 0);

  label_bookset = gtk_label_new ("");
  gtk_widget_show (label_bookset);
  gtk_box_pack_start (GTK_BOX (hbox_bookset), label_bookset, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_bookset, "clicked", G_CALLBACK (on_button_bookset_clicked), gpointer (this));

  hbox_books_import = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_books_import);
  gtk_box_pack_start (GTK_BOX (vbox_bookset), hbox_books_import, FALSE, FALSE, 0);

  button_books_import = gtk_button_new ();
  gtk_widget_show (button_books_import);
  gtk_box_pack_start (GTK_BOX (hbox_books_import), button_books_import, FALSE, FALSE, 0);

  GtkWidget *alignment4;
  GtkWidget *hbox7;
  GtkWidget *image4;
  GtkWidget *label23;

  alignment4 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment4);
  gtk_container_add (GTK_CONTAINER (button_books_import), alignment4);

  hbox7 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox7);
  gtk_container_add (GTK_CONTAINER (alignment4), hbox7);

  image4 = gtk_image_new_from_stock ("gtk-add", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image4);
  gtk_box_pack_start (GTK_BOX (hbox7), image4, FALSE, FALSE, 0);

  label23 = gtk_label_new_with_mnemonic (_("_Import"));
  gtk_widget_show (label23);
  gtk_box_pack_start (GTK_BOX (hbox7), label23, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_books_import, "clicked", G_CALLBACK (on_button_books_import_clicked), gpointer (this));

  books = resource_get_books(working_configuration_file());

  on_button_bookset (true);
  
  // Book set 2.
  vbox_bookset2 = gtk_vbox_new (FALSE, 10);
  gtk_widget_show (vbox_bookset2);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_bookset2), 10);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_bookset2);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_bookset2, _("What are the second abbreviations for the books of the Resource?"));
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_bookset2, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_bookset2, true);

  hbox_bookset2 = gtk_hbox_new (FALSE, 10);
  gtk_widget_show (hbox_bookset2);
  gtk_box_pack_start (GTK_BOX (vbox_bookset2), hbox_bookset2, FALSE, FALSE, 0);

  button_bookset2 = gtk_button_new ();
  gtk_widget_show (button_bookset2);
  gtk_box_pack_start (GTK_BOX (hbox_bookset2), button_bookset2, FALSE, FALSE, 0);

  alignment3 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment3);
  gtk_container_add (GTK_CONTAINER (button_bookset2), alignment3);

  hbox5 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox5);
  gtk_container_add (GTK_CONTAINER (alignment3), hbox5);

  image3 = gtk_image_new_from_stock ("gtk-info", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image3);
  gtk_box_pack_start (GTK_BOX (hbox5), image3, FALSE, FALSE, 0);

  label12 = gtk_label_new_with_mnemonic (_("_Abbreviations"));
  gtk_widget_show (label12);
  gtk_box_pack_start (GTK_BOX (hbox5), label12, FALSE, FALSE, 0);

  label_bookset2 = gtk_label_new ("");
  gtk_widget_show (label_bookset2);
  gtk_box_pack_start (GTK_BOX (hbox_bookset2), label_bookset2, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_bookset2, "clicked", G_CALLBACK (on_button_bookset2_clicked), gpointer (this));

  hbox_books2_import = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_books2_import);
  gtk_box_pack_start (GTK_BOX (vbox_bookset2), hbox_books2_import, FALSE, FALSE, 0);

  button_books2_import = gtk_button_new ();
  gtk_widget_show (button_books2_import);
  gtk_box_pack_start (GTK_BOX (hbox_books2_import), button_books2_import, FALSE, FALSE, 0);

  alignment4 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment4);
  gtk_container_add (GTK_CONTAINER (button_books2_import), alignment4);

  hbox7 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox7);
  gtk_container_add (GTK_CONTAINER (alignment4), hbox7);

  image4 = gtk_image_new_from_stock ("gtk-add", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image4);
  gtk_box_pack_start (GTK_BOX (hbox7), image4, FALSE, FALSE, 0);

  label23 = gtk_label_new_with_mnemonic (_("_Import"));
  gtk_widget_show (label23);
  gtk_box_pack_start (GTK_BOX (hbox7), label23, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_books2_import, "clicked", G_CALLBACK (on_button_books2_import_clicked), gpointer (this));

  books2 = resource_get_books2(working_configuration_file());

  on_button_bookset2 (true);

  // Anchors.
  vbox_anchors = gtk_vbox_new (FALSE, 10);
  gtk_widget_show (vbox_anchors);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_anchors), 10);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_anchors);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_anchors, _("Would you like to write the anchors in the pages of the Resource?"));
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_anchors, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_anchors, true);

  hbox_anchors = gtk_hbox_new (FALSE, 10);
  gtk_widget_show (hbox_anchors);
  gtk_box_pack_start (GTK_BOX (vbox_anchors), hbox_anchors, FALSE, FALSE, 0);

  button_anchors = gtk_button_new ();
  gtk_widget_show (button_anchors);
  gtk_box_pack_start (GTK_BOX (hbox_anchors), button_anchors, FALSE, FALSE, 0);

  alignment3 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment3);
  gtk_container_add (GTK_CONTAINER (button_anchors), alignment3);

  hbox5 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox5);
  gtk_container_add (GTK_CONTAINER (alignment3), hbox5);

  image3 = gtk_image_new_from_stock ("gtk-info", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image3);
  gtk_box_pack_start (GTK_BOX (hbox5), image3, FALSE, FALSE, 0);

  label12 = gtk_label_new_with_mnemonic (_("_Anchors"));
  gtk_widget_show (label12);
  gtk_box_pack_start (GTK_BOX (hbox5), label12, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_anchors, "clicked", G_CALLBACK (on_button_anchors_clicked), gpointer (this));

  // URL.
  vbox_url = gtk_vbox_new (FALSE, 10);
  gtk_widget_show (vbox_url);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_url);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_url, _("What is the URL constructor for the Resource?"));
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_url, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_url, true);

  entry_url = gtk_entry_new ();
  gtk_widget_show (entry_url);
  gtk_box_pack_start (GTK_BOX (vbox_url), entry_url, FALSE, FALSE, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry_url), 9679);

  gtk_entry_set_text (GTK_ENTRY (entry_url), resource_get_url_constructor(working_configuration_file()).c_str());

  hbox_tester = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_tester);
  gtk_box_pack_start (GTK_BOX (vbox_url), hbox_tester, FALSE, FALSE, 0);

  combobox_test_book = gtk_combo_box_new_text ();
  gtk_widget_show (combobox_test_book);
  gtk_box_pack_start (GTK_BOX (hbox_tester), combobox_test_book, FALSE, FALSE, 0);

  vector <unsigned int> bookids = books_type_to_ids(btUnknown);
  vector <ustring> booknames;
  for (unsigned int i = 0; i < bookids.size(); i++) {
    booknames.push_back (books_id_to_english (bookids[bookids[i]]));
  }
  combobox_set_strings (combobox_test_book, booknames);
  vector <unsigned int> nt_ids = books_type_to_ids (btNewTestament);
  combobox_set_string (combobox_test_book, books_id_to_english (nt_ids[0]));

  combobox_test_chapter = gtk_combo_box_new_text ();
  gtk_widget_show (combobox_test_chapter);
  gtk_box_pack_start (GTK_BOX (hbox_tester), combobox_test_chapter, FALSE, FALSE, 0);

  vector <unsigned int> chapters;
  for (unsigned int i = 1; i <= 150; i++) {
    chapters.push_back (i);
  }
  combobox_set_strings (combobox_test_chapter, chapters);
  combobox_set_index (combobox_test_chapter, 0);
  
  combobox_test_verse = gtk_combo_box_new_text ();
  gtk_widget_show (combobox_test_verse);
  gtk_box_pack_start (GTK_BOX (hbox_tester), combobox_test_verse, FALSE, FALSE, 0);

  vector <unsigned int> verses;
  for (unsigned int i = 1; i <= 186; i++) {
    verses.push_back (i);
  }
  combobox_set_strings (combobox_test_verse, verses);
  combobox_set_index (combobox_test_verse, 0);

  g_signal_connect ((gpointer) entry_url, "changed", G_CALLBACK (on_entry_url_changed), gpointer (this));
  g_signal_connect ((gpointer) combobox_test_book, "changed", G_CALLBACK (on_combobox_test_changed), gpointer (this));
  g_signal_connect ((gpointer) combobox_test_chapter, "changed", G_CALLBACK (on_combobox_test_changed), gpointer (this));
  g_signal_connect ((gpointer) combobox_test_verse, "changed", G_CALLBACK (on_combobox_test_changed), gpointer (this));

  browser_tester = new WebkitBrowser (vbox_url);

  on_url ();

  // Build the confirmation stuff.
  label_confirm = gtk_label_new (_("Settings are ready to be applied"));
  gtk_widget_show (label_confirm);
  page_number_confirm = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_confirm);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_confirm, _("Settings are ready to be applied"));
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_confirm, GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_confirm, true);
  
  label_progress = gtk_label_new ("");
  gtk_widget_show (label_progress);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_progress);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_progress, "");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_progress, GTK_ASSISTANT_PAGE_PROGRESS);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_progress, true);
  
  label_summary = gtk_label_new (_("Settings have been applied"));
  gtk_widget_show (label_summary);
  summary_page_number = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_summary);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_summary, "Ready");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_summary, GTK_ASSISTANT_PAGE_SUMMARY);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_summary, true);
  
  // Finish building assistant.
  gtk_widget_show_all (assistant);
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), 0);
}

ResourceAssistant::~ResourceAssistant()
{
  delete browser_home_page;
  delete browser_tester;
}

void ResourceAssistant::on_assistant_prepare_signal (GtkAssistant *assistant, GtkWidget *page, gpointer user_data)
{
  ((ResourceAssistant *) user_data)->on_assistant_prepare(page);
}


void ResourceAssistant::on_assistant_prepare (GtkWidget *page)
{
  if (page == vbox_title) {
    on_entry_title();
  }
}


void ResourceAssistant::on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data)
{
  ((ResourceAssistant *) user_data)->on_assistant_apply();
}


void ResourceAssistant::on_assistant_apply ()
{
  // Assemble the template.
  GKeyFile *keyfile = g_key_file_new();
  vector <ustring> templatedata;

  // Title.
  g_key_file_set_string(keyfile, resource_template_general_group(), resource_template_title_key(), title().c_str());
  
  // Home page.
  g_key_file_set_string(keyfile, resource_template_general_group(), resource_template_home_page_key(), home_page().c_str());

  // Book set.
  {
    vector <unsigned int> ids = books_type_to_ids(btUnknown);
    for (unsigned int i = 0; i < ids.size(); i++) {
      ustring book = books[ids[i]];
      if (!book.empty()) {
        g_key_file_set_string(keyfile, resource_template_books_group(), books_id_to_english(ids[i]).c_str(), book.c_str());
      }
    }
  }

  // Book set 2.
  {
    vector <unsigned int> ids = books_type_to_ids(btUnknown);
    for (unsigned int i = 0; i < ids.size(); i++) {
      ustring book = books2[ids[i]];
      if (!book.empty()) {
        g_key_file_set_string(keyfile, resource_template_books2_group(), books_id_to_english(ids[i]).c_str(), book.c_str());
      }
    }
  }

  // URL constructor.
  g_key_file_set_string(keyfile, resource_template_general_group(), resource_template_url_constructor_key(), url_constructor ().c_str());
  
  // Write the template.
  gchar *data = g_key_file_to_data(keyfile, NULL, NULL);
  if (data) {
    g_file_set_contents(working_configuration_file ().c_str(), data, -1, NULL);
    g_free(data);
  }
  g_key_file_free(keyfile);

  // Move the Resource into place.
  ustring directory = shell_clean_filename(title());
  directory = gw_build_filename(Directories->get_resources(), directory);
  unix_rmdir (directory);
  unix_mv (working_directory (), directory);

  // Store where the resource was put.
  created_resource_template = gw_build_filename (directory, resource_template_ini());

  // Show summary.
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), summary_page_number);
}


gint ResourceAssistant::assistant_forward_function (gint current_page, gpointer user_data)
{
  return ((ResourceAssistant *) user_data)->assistant_forward (current_page);
}

gint ResourceAssistant::assistant_forward (gint current_page)
{
  // Default behaviour is to go to the next page.
  gint new_page_number = current_page + 1;

  /*
  if (current_page == page_number_task_selector) {
    // Go to the right page depending on which task is selected.
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_task_selector_settings))) {
      new_page_number = page_number_interval;
    }
  }
  */

  // Return the new page.
  return new_page_number;
}


void ResourceAssistant::on_entry_title_changed (GtkEditable *editable, gpointer user_data)
{
  ((ResourceAssistant *) user_data)->on_entry_title();
}


void ResourceAssistant::on_entry_title ()
{
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_title, false);
  // Handle situation that no title is given.
  if (title().empty()) {
    gtk_label_set_text (GTK_LABEL (label_title), _("Enter the title"));
    return;
  }
  // Check that the title is available.
  // If the title is the same as the one of the Resource being edited, that's fine.
  vector < ustring > resources;
  vector < ustring > filenames;
  resources = resource_get_resources(filenames, false);
  set <ustring> resource_set(resources.begin(), resources.end());
  bool title_is_available = (resource_set.find(title()) == resource_set.end());
  if (title() == resource_get_title(working_configuration_file ()))
    title_is_available = true;
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_title, title_is_available);
  ustring message;
  if (!title_is_available)
    message = _("This title already exists");
  gtk_label_set_text (GTK_LABEL (label_title), message.c_str());
}


ustring ResourceAssistant::title ()
// Get the title
{
  ustring title = gtk_entry_get_text(GTK_ENTRY(entry_title));
  title = trim(title);
  return title;
}


ustring ResourceAssistant::working_directory ()
{
  return gw_build_filename(Directories->get_temp(), "resource-editor");
}


ustring ResourceAssistant::working_configuration_file ()
{
  return gw_build_filename (working_directory (), resource_template_ini());
}


void ResourceAssistant::on_button_add_files_clicked (GtkButton *button, gpointer user_data)
{
  ((ResourceAssistant *) user_data)->on_button_add_files(false);
}


void ResourceAssistant::on_button_add_files (bool gui_only)
{
  if (!gui_only) {
    vector <ustring> files;
    files = gtkw_file_chooser_open_multiple(NULL, "", "");
    if (!files.empty()) {
      ProgressWindow progresswindow(_("Adding files"), false);
      progresswindow.set_iterate(0, 1, files.size());
      for (unsigned int i = 0; i < files.size(); i++) {
        progresswindow.iterate();
        unix_cp(files[i], working_directory());
      }
    }
  }
  ReadFiles rf(working_directory(), "", "");
  ustring text;
  text.append(_("Files in resource: "));
  text.append(convert_to_string(rf.files.size()));
  gtk_label_set_text(GTK_LABEL(label_files), text.c_str());
}


void ResourceAssistant::on_button_add_folders_clicked (GtkButton *button, gpointer user_data)
{
  ((ResourceAssistant *) user_data)->on_button_add_folders(false);
}


void ResourceAssistant::on_button_add_folders (bool gui_only)
{
  if (!gui_only) {
    ustring directory = gtkw_file_chooser_select_folder(NULL, "", "");
    if (!directory.empty()) {
      ProgressWindow progresswindow(_("Adding directory"), false);
      unix_cp_r(directory, working_directory());
    }
  }
  ReadDirectories rd(working_directory(), "", "");
  ustring text;
  text.append(_("Directories in resource: "));
  text.append(convert_to_string(rd.directories.size()));
  gtk_label_set_text(GTK_LABEL(label_folders), text.c_str());
}


void ResourceAssistant::on_entry_home_page_changed (GtkEditable *editable, gpointer user_data)
{
  ((ResourceAssistant *) user_data)->on_entry_home_page();
}


void ResourceAssistant::on_entry_home_page ()
{
  ustring homepage = resource_url_get(home_page (), working_configuration_file ());
  browser_home_page->set_home_page (homepage);
  browser_home_page->go_to(homepage);
}


ustring ResourceAssistant::home_page ()
// Retrieves the home page from the gui.
{
  ustring homepage = gtk_entry_get_text (GTK_ENTRY (entry_home_page));
  homepage = trim (homepage);
  return homepage;
}


void ResourceAssistant::on_button_bookset_clicked (GtkButton *button, gpointer user_data)
{
  ((ResourceAssistant *) user_data)->on_button_bookset(false);
}


void ResourceAssistant::on_button_bookset (bool update_gui_only)
{
  if (!update_gui_only) {
    BooknamesDialog dialog(books, _("Enter the abbreviations for the books"), _("Abbreviations"));
    if (dialog.run() == GTK_RESPONSE_OK) {
      books = dialog.newbooks;
    }
  }  
  ustring text = _("Abbreviations entered: ");
  text.append(convert_to_string(books.size()));
  gtk_label_set_text(GTK_LABEL(label_bookset), text.c_str());
}


void ResourceAssistant::on_button_books_import_clicked (GtkButton *button, gpointer user_data)
{
  ((ResourceAssistant *) user_data)->on_button_books_import();
}


void ResourceAssistant::on_button_books_import ()
// Import the books from another resource.
{
  vector <ustring> filenames;
  vector <ustring> resources = resource_get_resources(filenames, false);
  quick_sort(resources, filenames, 0, resources.size());
  ListviewDialog dialog(_("Import from resource"), resources, "", false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    ustring resource = dialog.focus;
    ustring filename;
    for (unsigned int i = 0; i < resources.size(); i++) {
      if (resource == resources[i]) {
        filename = filenames[i];
      }
    }
    if (!filename.empty()) {
      books = resource_get_books (filename);
      on_button_bookset (true);
    }
  }
}


void ResourceAssistant::on_button_bookset2_clicked (GtkButton *button, gpointer user_data)
{
  ((ResourceAssistant *) user_data)->on_button_bookset2(false);
}


void ResourceAssistant::on_button_bookset2 (bool update_gui_only)
{
  if (!update_gui_only) {
    BooknamesDialog dialog(books2, _("Enter the second abbreviations for the books"), _("Abbreviations"));
    if (dialog.run() == GTK_RESPONSE_OK) {
      books2 = dialog.newbooks;
    }
  }  
  ustring text = _("Abbreviations entered: ");
  text.append(convert_to_string(books2.size()));
  gtk_label_set_text(GTK_LABEL(label_bookset2), text.c_str());
}


void ResourceAssistant::on_button_books2_import_clicked (GtkButton *button, gpointer user_data)
{
  ((ResourceAssistant *) user_data)->on_button_books2_import();
}


void ResourceAssistant::on_button_books2_import ()
// Import the second lot of books from another resource.
{
  vector < ustring > filenames;
  vector < ustring > resources = resource_get_resources(filenames, false);
  quick_sort(resources, filenames, 0, resources.size());
  ListviewDialog dialog(_("Import from resource"), resources, "", false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    ustring resource = dialog.focus;
    ustring filename;
    for (unsigned int i = 0; i < resources.size(); i++) {
      if (resource == resources[i]) {
        filename = filenames[i];
      }
    }
    if (!filename.empty()) {
      books2 = resource_get_books2 (filename);
      on_button_bookset2 (true);
    }
  }
}


void ResourceAssistant::on_button_anchors_clicked (GtkButton *button, gpointer user_data)
{
  ((ResourceAssistant *) user_data)->on_button_anchors();
}


void ResourceAssistant::on_button_anchors ()
{
  ResourceConverterDialog dialog(working_directory ());
  if (dialog.run() == GTK_RESPONSE_OK) {
    // Modify the url constructor by adding the anchor to it, except if one is already there.
    ustring constructor = url_constructor ();
    if (constructor.find ("#") == string::npos) {
      constructor.append ("#");
      constructor.append (resource_conversion_anchor_prefix ());
      constructor.append (resource_url_constructor_chapter());
      constructor.append ("_");
      constructor.append (resource_url_constructor_verse());
      gtk_entry_set_text (GTK_ENTRY (entry_url), constructor.c_str());
    }
  }
}


ustring ResourceAssistant::url_constructor ()
// Retrieves the URL constructor from the gui.
{
  ustring url = gtk_entry_get_text (GTK_ENTRY (entry_url));
  url = trim (url);
  return url;
}


void ResourceAssistant::on_entry_url_changed (GtkEditable *editable, gpointer user_data)
{
  ((ResourceAssistant *) user_data)->on_url();
}


void ResourceAssistant::on_combobox_test_changed (GtkComboBox *combobox, gpointer user_data)
{
  ((ResourceAssistant *) user_data)->on_url();
}


void ResourceAssistant::on_url ()
{
  ustring url = resource_url_get(url_constructor (), working_configuration_file ());
  unsigned int book = books_english_to_id (combobox_get_active_string (combobox_test_book));
  unsigned int chapter = convert_to_int (combobox_get_active_string (combobox_test_chapter));
  ustring verse = combobox_get_active_string (combobox_test_verse);
  Reference reference (book, chapter, verse);
  url = resource_url_enter_reference(url, books, books2, reference);
  browser_tester->go_to (url);
}


ustring ResourceAssistant::new_resource_get ()
// If there's a new resource, returns its name.
{
  if (edited_resource_template.empty()) {
    return title ();
  }
  return "";
}


ustring ResourceAssistant::edited_resource_get ()
// If there's an edited resource, return its name.
{
  if (!edited_resource_template.empty()) {
    return title ();
  }
  return "";
}



/*

todo resources.

We may need an option to download all verses and store these locally.
So Bibledit goes to a website, downloads all verses, stores them under the URL's, so these can be kept for later use offline.
Bibledit may use httrack to download sites to local files for offline viewing. But httrack may produce huge files, so a warning may have to be given
to the user. Else Bibledit can manually download the page for each verse, using htmltrack 2 levels deep and store that info, then 
retrieve it later for each verse.
To make an engine in bibledit that downloads the online version. 

This requires the Versification for the Resource to be set.

We need predictable locations for this resource, so that if this location is found, the URL is not used.
When found, the local copy is used.

The page for this needs a button that downloads all verses,
and another one that removes all locally cached information.

As wget is more common, we better use that utility.

The maximum depth to which the retrieval may descend is specified with the ‘-l’ option. The default maximum depth is five layers. 
When downloading from Internet servers, consider using the ‘-w’ option to introduce a delay between accesses to the server.
If you want to download only one page, use ‘--page-requisites’ without any additional recursion.

wget --recursive --no-clobber --page-requisites --convert-links --domains net.bible.org --user-agent=Mozilla --level=1 "http://net.bible.org/bible.php?book=Mat&chapter=1"

--quiet 
--wait=10 

http://net.bible.org/bible.php?book=Mat&chapter=1
http://net.bible.org/verse.php?book=Mat&chapter=1&verse=1

We may need to have an option to --wait some time, as optional, so by default it speeds on.

If using the --no-clobber option, and downloading everything to the same directory, we st
are going to do a lot of saving the more we download, because no file will be downloaded twice.
E.g. the strong's numbers are downloaded only once, and be re-used by all pages.

Some recommend httrack as if that beats wget.

httrack 'http://net.bible.org/bible.php?book=Mat&chapter=1' -O "." -%v -r2 --can-go-down -c8


*/


