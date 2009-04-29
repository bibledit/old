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


ResourceAssistant::ResourceAssistant(bool new_resource) :
AssistantBase("Resource editor", "resource_editor")
// Assistant for editing or creating a resource.
{
  // Since this Assistant contains browsers, it needs a bigger size.
  gtk_window_resize (GTK_WINDOW (assistant), 800, 600);

  gtk_assistant_set_forward_page_func (GTK_ASSISTANT (assistant), GtkAssistantPageFunc (assistant_forward_function), gpointer(this), NULL);
  
  g_signal_connect (G_OBJECT (assistant), "apply", G_CALLBACK (on_assistant_apply_signal), gpointer(this));
  g_signal_connect (G_OBJECT (assistant), "prepare", G_CALLBACK (on_assistant_prepare_signal), gpointer(this));

  // Working directory;
  unix_rmdir(working_directory());
  gw_mkdir_with_parents(working_directory());

  ustring intro_message = "Step by step you will be ";
  if (new_resource) 
    intro_message.append ("creating a new Resource");
  else
    intro_message.append ("editing the existing Resource");
  intro_message.append (".\nAny changes you make show up immediately.");
  introduction (intro_message);

  // Configuration and initialization.
  is_new_resource = new_resource;

  // Title.
  vbox_title = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_title);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_title);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_title, "What is the title of the Resource?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_title, GTK_ASSISTANT_PAGE_CONTENT);

  entry_title = gtk_entry_new ();
  gtk_widget_show (entry_title);
  gtk_box_pack_start (GTK_BOX (vbox_title), entry_title, FALSE, FALSE, 0);

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

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), table_files, "Would you like to add local files to the Resource?");
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

  label6 = gtk_label_new_with_mnemonic ("_Add files");
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

  label7 = gtk_label_new_with_mnemonic ("Add f_olders");
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

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_home_page, "What is the home page of the Resource?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_home_page, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_home_page, true);

  entry_home_page = gtk_entry_new ();
  gtk_widget_show (entry_home_page);
  gtk_box_pack_start (GTK_BOX (vbox_home_page), entry_home_page, FALSE, FALSE, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry_home_page), 9679);

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

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_bookset, "What are the abbreviations for the books of the Resource?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_bookset, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_bookset, true);

  label_bookset_info = gtk_label_new (
    "The Resource browser retrieves pages to display the resource. See the following example:\n"
    "\n"
    "http://www.bible.org/mat2.html\n"
    "\n"
    "The example retrieves Matthew chapter 2 into the browser. The abbreviation for the book of Matthew is \"mat\". Each book has its own abbreviation. You have the opportunity to enter these abbrevitions here."
    );
  gtk_widget_show (label_bookset_info);
  gtk_box_pack_start (GTK_BOX (vbox_bookset), label_bookset_info, FALSE, FALSE, 0);
  gtk_label_set_line_wrap (GTK_LABEL (label_bookset_info), TRUE);

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

  label12 = gtk_label_new_with_mnemonic ("_Abbreviations");
  gtk_widget_show (label12);
  gtk_box_pack_start (GTK_BOX (hbox5), label12, FALSE, FALSE, 0);

  label_bookset_count = gtk_label_new ("");
  gtk_widget_show (label_bookset_count);
  gtk_box_pack_start (GTK_BOX (hbox_bookset), label_bookset_count, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_bookset, "clicked", G_CALLBACK (on_button_bookset_clicked), gpointer (this));
  
  on_button_bookset (true);
  
  // URL.
  vbox_url = gtk_vbox_new (FALSE, 10);
  gtk_widget_show (vbox_url);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_url);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_url, "What is the URL construction for the Resource?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_url, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_url, true);

  label_url = gtk_label_new (
    "A Resource uses a URL (Uniform Resource Locator) to retrieve the information for each verse.\n"
    "On this page you will be constructing a URL that enables the Resource to retrieve the right information.\n"
    "See the following URL:\n"
    "\n"
    "http://www.bible.org/mat5.html#vs6\n"
    "\n"
    "The right URL construction for this is:\n"
    "\n"
    "http://www.bible.org/<book><chapter>.html#vs<verse>\n"
    "\n"
    "Where it says <book>, the Resource will put the abbreviation for that book.\n"
    "You entered this information in one of the preceding pages.\n"
    "And where it says <chapter>, the Resource will put the chapter number.\n"
    "The same applies to <verse>.\n"
    "\n"
    "You can test the URL construction on the next page."    
    );
  gtk_widget_show (label_url);
  gtk_box_pack_start (GTK_BOX (vbox_url), label_url, FALSE, FALSE, 0);

  entry_url = gtk_entry_new ();
  gtk_widget_show (entry_url);
  gtk_box_pack_start (GTK_BOX (vbox_url), entry_url, FALSE, FALSE, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry_url), 9679);

  // Tester.
  vbox_tester = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_tester);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_tester);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_tester, "Would you like to try the URL construction?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_tester, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_tester, true);

  hbox_tester = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_tester);
  gtk_box_pack_start (GTK_BOX (vbox_tester), hbox_tester, FALSE, FALSE, 0);

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

  g_signal_connect ((gpointer) combobox_test_book, "changed", G_CALLBACK (on_combobox_test_changed), gpointer (this));
  g_signal_connect ((gpointer) combobox_test_chapter, "changed", G_CALLBACK (on_combobox_test_changed), gpointer (this));
  g_signal_connect ((gpointer) combobox_test_verse, "changed", G_CALLBACK (on_combobox_test_changed), gpointer (this));

  browser_tester = new WebkitBrowser (vbox_tester);
  
  
  



  // Todo


  // Build the confirmation stuff.
  label_confirm = gtk_label_new ("Settings are ready to be applied");
  gtk_widget_show (label_confirm);
  page_number_confirm = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_confirm);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_confirm, "Settings are ready to be applied");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_confirm, GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_confirm, true);
  
  label_progress = gtk_label_new ("");
  gtk_widget_show (label_progress);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_progress);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_progress, "");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_progress, GTK_ASSISTANT_PAGE_PROGRESS);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_progress, true);
  
  label_summary = gtk_label_new ("Settings have been applied");
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
  if (page == vbox_tester) {
    on_combobox_test ();
  }
}


void ResourceAssistant::on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data)
{
  ((ResourceAssistant *) user_data)->on_assistant_apply();
}


void ResourceAssistant::on_assistant_apply () // Todo
{
  // Assemble the template.
  GKeyFile *keyfile = g_key_file_new();
  vector <ustring> templatedata;

  // Title.
  g_key_file_set_string(keyfile, resource_template_general_group(), resource_template_title_key(), title().c_str());
  
  // Home page.
  g_key_file_set_string(keyfile, resource_template_general_group(), resource_template_home_page_key(), home_page().c_str());

  // Book set.
  vector < unsigned int >ids = books_type_to_ids(btUnknown);
  for (unsigned int i = 0; i < ids.size(); i++) {
    ustring book = books[ids[i]];
    if (!book.empty()) {
      g_key_file_set_string(keyfile, resource_template_books_group(), books_id_to_english(ids[i]).c_str(), book.c_str());
    }
  }

  // URL construction.
  g_key_file_set_string(keyfile, resource_template_general_group(), resource_template_url_constructor_key(), url_construction ().c_str());
  
  // Write the template.
  gchar *data = g_key_file_to_data(keyfile, NULL, NULL);
  if (data) {
    g_file_set_contents(working_configuration_file ().c_str(), data, -1, NULL);
    g_free(data);
  }
  g_key_file_free(keyfile);

  


  /*
  // Get a non-existing directory, or if editing an editable resource, take that directory.
  ustring directory = shell_clean_filename(title);
  directory = gw_build_filename(directories_get_resources(), directory);
  if (mytemplatefile.empty()) {
    while (g_file_test(directory.c_str(), G_FILE_TEST_EXISTS)) {
      directory.append("1");
    }
  }
  gw_mkdir_with_parents(directory);

  // Copy all files from the temporal directory to the resource-directory.
  if (directory != workingdirectory) {
    ustring command = "cd" + shell_quote_space(workingdirectory);
#ifdef WIN32
    command.append("&& move *" + shell_quote_space(directory));
#else
    command.append("; mv *" + shell_quote_space(directory));
#endif
    if (system(command.c_str())) ;
  }

  */



  
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
  if (title().empty()) {
    gtk_label_set_text (GTK_LABEL (label_title), "Enter the title");
    return;
  }
  vector < ustring > resources;
  vector < ustring > filenames;
  resources = resource_get_resources(filenames, false);
  set <ustring> resource_set(resources.begin(), resources.end());
  bool title_is_available = (resource_set.find(title()) == resource_set.end());
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_title, title_is_available);
  ustring message;
  if (!title_is_available)
    message = "This title already exists";
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
  return gw_build_filename(directories_get_temp(), "resource-editor");
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
      ProgressWindow progresswindow("Adding files", false);
      progresswindow.set_iterate(0, 1, files.size());
      for (unsigned int i = 0; i < files.size(); i++) {
        progresswindow.iterate();
        unix_cp(files[i], working_directory());
      }
    }
  }
  ReadFiles rf(working_directory(), "", "");
  ustring text;
  text.append("Files in resource: ");
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
      ProgressWindow progresswindow("Adding directory", false);
      unix_cp_r(directory, working_directory());
    }
  }
  ReadDirectories rd(working_directory(), "", "");
  ustring text;
  text.append("Directories in resource: ");
  text.append(convert_to_string(rd.directories.size()));
  gtk_label_set_text(GTK_LABEL(label_folders), text.c_str());
}


void ResourceAssistant::on_entry_home_page_changed (GtkEditable *editable, gpointer user_data)
{
  ((ResourceAssistant *) user_data)->on_entry_home_page();
}


void ResourceAssistant::on_entry_home_page ()
{
  browser_home_page->set_home_page (home_page ());
  browser_home_page->go_to(home_page ());
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
    BooknamesDialog dialog(books, "Enter the abbreviations for the books", "Abbreviations");
    if (dialog.run() == GTK_RESPONSE_OK) {
      books = dialog.newbooks;
    }
  }  
  ustring text = "Abbreviations entered: ";
  text.append(convert_to_string(books.size()));
  gtk_label_set_text(GTK_LABEL(label_bookset_count), text.c_str());
}


ustring ResourceAssistant::url_construction ()
// Retrieves the URL construction from the gui.
{
  ustring url = gtk_entry_get_text (GTK_ENTRY (entry_url));
  url = trim (url);
  return url;
}


void ResourceAssistant::on_combobox_test_changed (GtkComboBox *combobox, gpointer user_data)
{
  ((ResourceAssistant *) user_data)->on_combobox_test();
}


void ResourceAssistant::on_combobox_test ()
{
  
}



/*






Todo redo resources.

We need to make an Assistant that does the generation of resources.
Each page of the assistant also has a browser object that immediately shows the effect of any change.
Home page: Any change in url reflects in the browser.
Verse retrieval: Any change in url reflects in browser, and the user can set the book, chapter, and verse.
Anything else: Immediate showing.
Each page should have a "Copy from" option, so it copies the things from any other resource, to be selected upon copy.


Please make two sample resources with verse anchors

Try all current resources, whether they work well.

The NextBible site, so please create a similar sample resource for
http://net.bible.org/bible.php?book=Pro&chapter=1

Related to this, BE will need to be able to log in to GoogleDocs if I am going to be able to access pages we have created like the Sabda pages. 
I tried basing a new resource on the Sabda.org resource, using the GoogleDoc address. GoogleDocs that are "published" do show up, 
but private pages are password protected.

Imaging editing the GoogleDoc from Bibledit's browser!

We may need an easier Resource Editor, because the current one seems to be too difficult.
Probably something with tabs, and the editor at the left, and direct testing of the effects right in the editor.

We may need an option to download all verses and store these locally.
So Bibledit goes to a website, downloads all verses, stores them under the URL's, so these can be kept for later use offline.

rtURLForEachVerseAboveURLFilterBelowWithDifferentAnchors This option no longer valid, because there's no second browser.
 
Bibledit may use httrack to download sites to local files for offline viewing. But httrack may produce huge files, so a warning may have to be given
to the user. Else Bibledit can manually download the page for each verse, using htmltrack one level deep and store that info, then 
retrieve it later for each verse.

We may have to use the WebKitFrame (or so) to control the various frames in the NET Bible.

The users has to manually edit a text file, and there are controls that automatically test the result of any change.
Whether to test the home page, various verses, and so on.
Or an automated test of all possible chapters. Or verses per chapter, or indeed, everything.
Or we have a dialog that does the testing, then applies this test to the currently open Resource,
so the user can press the hOme button, can go to certain places in the text, or indeed let it automatically run all things.

Entry of the home url or verse url goes so that it is general, and the user can for local things, can enter "<local>".

Files and directories can be added to the local storage.
This is regardless of the resource type.

  new InDialogHelp(newresourcedialog, NULL, &shortcuts, "resource_new"); This one should move to the Assistant's help.

If no books are given as abbreviations, we take the full book in English.

*/
