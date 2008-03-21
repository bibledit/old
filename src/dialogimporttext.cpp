/*
** Copyright (©) 2003-2008 Teus Benschop.
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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/


#include "libraries.h"
#include <glib.h>
#include "dialogimporttext.h"
#include "usfmtools.h"
#include "bible.h"
#include "utilities.h"
#include "dialogwait.h"
#include "utilities.h"
#include "projectutils.h"
#include "usfmtools.h"
#include "dialogselectbooks.h"
#include "gwrappers.h"
#include "dialogunicode.h"
#include "shell.h"
#include "import.h"
#include "progresswindow.h"
#include "books.h"
#include "settings.h"
#include "gui.h"
#include "combobox.h"
#include "dialogassignbooks.h"
#include "help.h"
#include "shortcuts.h"
#include "tiny_utilities.h"


ImportTextDialog::ImportTextDialog (int dummy)
{
  Shortcuts shortcuts (0); 
  
  importsfmdialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (importsfmdialog), "Import text");
  gtk_window_set_position (GTK_WINDOW (importsfmdialog), GTK_WIN_POS_CENTER_ON_PARENT);

  dialog_vbox1 = GTK_DIALOG (importsfmdialog)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 5);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  table1 = gtk_table_new (10, 4, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, TRUE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table1), 4);
  gtk_table_set_col_spacings (GTK_TABLE (table1), 4);

  image_directory = gtk_image_new_from_stock ("gtk-no", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_directory);
  gtk_table_attach (GTK_TABLE (table1), image_directory, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label_directory_gui = gtk_label_new ("");
  gtk_widget_show (label_directory_gui);
  gtk_table_attach (GTK_TABLE (table1), label_directory_gui, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_directory_gui), 0, 0.5);

  label24 = gtk_label_new ("Select directory");
  gtk_widget_show (label24);
  gtk_table_attach (GTK_TABLE (table1), label24, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label24), 0, 0.5);

  button_directory = gtk_button_new ();
  gtk_widget_show (button_directory);
  gtk_table_attach (GTK_TABLE (table1), button_directory, 3, 4, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  alignment3 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment3);
  gtk_container_add (GTK_CONTAINER (button_directory), alignment3);

  hbox6 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox6);
  gtk_container_add (GTK_CONTAINER (alignment3), hbox6);

  image3 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image3);
  gtk_box_pack_start (GTK_BOX (hbox6), image3, FALSE, FALSE, 0);

  label25 = gtk_label_new_with_mnemonic ("Directory");
  gtk_widget_show (label25);
  gtk_box_pack_start (GTK_BOX (hbox6), label25, FALSE, FALSE, 0);

  shortcuts.label (label25);

  // Set the type to USFM by default.
  importtype = itUsfm;

  label_unicode = gtk_label_new ("");
  gtk_widget_show (label_unicode);
  gtk_table_attach (GTK_TABLE (table1), label_unicode, 0, 4, 8, 9,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_unicode), 0, 0.5);

  image_unicode = gtk_image_new_from_stock ("gtk-no", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_unicode);
  gtk_table_attach (GTK_TABLE (table1), image_unicode, 0, 1, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label_unicode_gui = gtk_label_new ("");
  gtk_widget_show (label_unicode_gui);
  gtk_table_attach (GTK_TABLE (table1), label_unicode_gui, 1, 2, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_unicode_gui), 0, 0.5);

  label29 = gtk_label_new ("Convert to Unicode");
  gtk_widget_show (label29);
  gtk_table_attach (GTK_TABLE (table1), label29, 2, 3, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label29), 0, 0.5);

  button_unicode = gtk_button_new ();
  gtk_widget_show (button_unicode);
  gtk_table_attach (GTK_TABLE (table1), button_unicode, 3, 4, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  alignment5 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment5);
  gtk_container_add (GTK_CONTAINER (button_unicode), alignment5);

  hbox10 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox10);
  gtk_container_add (GTK_CONTAINER (alignment5), hbox10);

  image8 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image8);
  gtk_box_pack_start (GTK_BOX (hbox10), image8, FALSE, FALSE, 0);

  label30 = gtk_label_new_with_mnemonic ("Unicode");
  gtk_widget_show (label30);
  gtk_box_pack_start (GTK_BOX (hbox10), label30, FALSE, FALSE, 0);

  shortcuts.label (label30);
  
  label_books = gtk_label_new ("");
  gtk_widget_show (label_books);
  gtk_table_attach (GTK_TABLE (table1), label_books, 0, 4, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_books), 0, 0.5);

  image_books = gtk_image_new_from_stock ("gtk-no", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_books);
  gtk_table_attach (GTK_TABLE (table1), image_books, 0, 1, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label_books_gui = gtk_label_new ("");
  gtk_widget_show (label_books_gui);
  gtk_table_attach (GTK_TABLE (table1), label_books_gui, 1, 2, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_books_gui), 0, 0.5);

  label27 = gtk_label_new ("Select books");
  gtk_widget_show (label27);
  gtk_table_attach (GTK_TABLE (table1), label27, 2, 3, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label27), 0, 0.5);

  button_books = gtk_button_new ();
  gtk_widget_show (button_books);
  gtk_table_attach (GTK_TABLE (table1), button_books, 3, 4, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  alignment4 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment4);
  gtk_container_add (GTK_CONTAINER (button_books), alignment4);

  hbox8 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox8);
  gtk_container_add (GTK_CONTAINER (alignment4), hbox8);

  image6 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image6);
  gtk_box_pack_start (GTK_BOX (hbox8), image6, FALSE, FALSE, 0);

  label28 = gtk_label_new_with_mnemonic ("Books");
  gtk_widget_show (label28);
  gtk_box_pack_start (GTK_BOX (hbox8), label28, FALSE, FALSE, 0);

  shortcuts.label (label28);

  label_directory = gtk_label_new ("");
  gtk_widget_show (label_directory);
  gtk_table_attach (GTK_TABLE (table1), label_directory, 0, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_directory), 0, 0.5);

  label_import = gtk_label_new ("");
  gtk_widget_show (label_import);
  gtk_table_attach (GTK_TABLE (table1), label_import, 0, 4, 11, 12,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_import), 0, 0.5);

  image_assign_ok = gtk_image_new_from_stock ("gtk-about", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_assign_ok);
  gtk_table_attach (GTK_TABLE (table1), image_assign_ok, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  label_assign_ok = gtk_label_new ("Done");
  gtk_widget_show (label_assign_ok);
  gtk_table_attach (GTK_TABLE (table1), label_assign_ok, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_assign_ok), 0, 0.5);

  label_assign_question = gtk_label_new ("Assign names to unknown books");
  gtk_widget_show (label_assign_question);
  gtk_table_attach (GTK_TABLE (table1), label_assign_question, 2, 3, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_assign_question), 0, 0.5);

  button_assign = gtk_button_new ();
  gtk_widget_show (button_assign);
  gtk_table_attach (GTK_TABLE (table1), button_assign, 3, 4, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  alignment6 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment6);
  gtk_container_add (GTK_CONTAINER (button_assign), alignment6);

  hbox11 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox11);
  gtk_container_add (GTK_CONTAINER (alignment6), hbox11);

  image10 = gtk_image_new_from_stock ("gtk-zoom-fit", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image10);
  gtk_box_pack_start (GTK_BOX (hbox11), image10, FALSE, FALSE, 0);

  label38 = gtk_label_new_with_mnemonic ("Assign");
  gtk_widget_show (label38);
  gtk_box_pack_start (GTK_BOX (hbox11), label38, FALSE, FALSE, 0);

  shortcuts.label (label38);

  label_assign_info = gtk_label_new ("Information");
  gtk_widget_show (label_assign_info);
  gtk_table_attach (GTK_TABLE (table1), label_assign_info, 0, 4, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_assign_info), 0, 0.5);

  expander1 = gtk_expander_new (NULL);
  gtk_widget_show (expander1);
  gtk_table_attach (GTK_TABLE (table1), expander1, 0, 4, 9, 10,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_expander_set_expanded (GTK_EXPANDER (expander1), false);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (expander1), vbox2);

  checkbutton_overwrite = gtk_check_button_new_with_mnemonic ("Overwrite");
  gtk_widget_show (checkbutton_overwrite);
  gtk_box_pack_start (GTK_BOX (vbox2), checkbutton_overwrite, FALSE, FALSE, 0);

  shortcuts.button (checkbutton_overwrite);

  label42 = gtk_label_new ("Options");
  gtk_widget_show (label42);
  gtk_expander_set_label_widget (GTK_EXPANDER (expander1), label42);

  shortcuts.label (label42);

  dialog_action_area1 = GTK_DIALOG (importsfmdialog)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (importsfmdialog, &shortcuts, "import_text");

  cancelbutton = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (importsfmdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton, GTK_CAN_DEFAULT);

  shortcuts.stockbutton (cancelbutton);

  okbutton = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (importsfmdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton, GTK_CAN_DEFAULT);

  shortcuts.stockbutton (okbutton);
  
  shortcuts.process ();

  g_signal_connect ((gpointer) button_directory, "clicked",
                    G_CALLBACK (on_button_directory_clicked),
                    gpointer(this));
  g_signal_connect ((gpointer) button_assign, "clicked",
                    G_CALLBACK (on_button_assign_clicked),
                    gpointer(this));
  g_signal_connect ((gpointer) button_unicode, "clicked",
                    G_CALLBACK (on_button_unicode_clicked),
                    gpointer(this));
  g_signal_connect ((gpointer) button_books, "clicked",
                    G_CALLBACK (on_button_books_clicked),
                    gpointer(this));
  g_signal_connect ((gpointer) checkbutton_overwrite, "toggled",
                    G_CALLBACK (on_checkbutton_overwrite_toggled),
                    gpointer(this));
  g_signal_connect ((gpointer) okbutton, "clicked",
                    G_CALLBACK (on_okbutton_clicked),
                    gpointer(this));

  gtk_widget_grab_default (okbutton);

  // Set gui.
  select_all_books = true;
  assigning_done = false;
  set_gui ();
}


ImportTextDialog::~ImportTextDialog ()
{
  gtk_widget_destroy (importsfmdialog);
}


int ImportTextDialog::run ()
{
  return gtk_dialog_run (GTK_DIALOG (importsfmdialog));
}


ustring ImportTextDialog::pick_newest_filename (const vector <ustring>& filenames)
// This function returns the newest filename of the vector of filenames given.
{
  // Pick newest.
  unsigned int modificationtime = 0;
  ustring returnvalue;
  // Solve bug that when the modification times were equal, nothing was taken.
  if (filenames.size() > 0)
    returnvalue = filenames[0];
  // Get newest (= youngest) file.
  for (unsigned int i = 0; i < filenames.size (); i++) {
    unsigned int modtime = file_get_modification_time (filenames[i]);
    if (modtime > modificationtime) {
      modificationtime = modtime;
      returnvalue = filenames[i];
    }
  }
  return returnvalue;
}


void ImportTextDialog::on_button_directory_clicked (GtkButton *button, gpointer user_data)
{
  ((ImportTextDialog *) user_data)->on_button_directory();
}


void ImportTextDialog::on_button_directory ()
{
  GtkWidget *dialog;
  dialog = gtk_file_chooser_dialog_new ("Select a folder", GTK_WINDOW (importsfmdialog),
    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
    GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
  gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), directory.c_str());
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
    directory = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
    select_all_books = true;
  }
  gtk_widget_destroy (dialog);
  assigning_done = false;
  set_gui ();
}



void ImportTextDialog::on_button_assign_clicked (GtkButton *button, gpointer user_data)
{
  ((ImportTextDialog *) user_data)->on_button_assign();
}


void ImportTextDialog::on_button_assign ()
// Assigns names to filenames containing unknown books.
/*
From Paratext's helpfile:
  XXA to XXG Books
  At the request of previous Paratext users a set of non-biblical books have been 
  added to this version. The XXA to XXG books appear at the end of the list of 
  books and may be used for any kind of additional material desired, e.g., 
  glossary, concordance, etc.
From USFM 2.x standard:
  XXA used for Front Matter (will become \id FRT in Paratext 6.1)
  XXB used for Back Matter (will become \id BAK in Paratext 6.1)
  XXC used for Other Material (will become \id OTH in Paratext 6.1)
*/
{
  AssignBooksDialog dialog (unknown_filenames);
  if (dialog.run() == GTK_RESPONSE_OK) {
    for (unsigned int i = 0; i < dialog.assigned_files.size(); i++) {
      filename_bookname_map [dialog.assigned_files[i]] = dialog.assigned_books[i];
      if (dialog.assigned (dialog.assigned_books[i]))
        selected_books.insert (books_english_to_id (dialog.assigned_books[i]));
    }
    assigning_done = true;
    set_gui ();
  }
}


void ImportTextDialog::on_button_books_clicked (GtkButton *button, gpointer user_data)
{
  ((ImportTextDialog *) user_data)->on_button_books();
}


void ImportTextDialog::on_button_books ()
{
  SelectBooksDialog dialog (false);
  dialog.selectables (selectable_booknames);
  dialog.selection_set (selected_books);
  if (dialog.run () == GTK_RESPONSE_OK) {
    selected_books = dialog.selectionset;
    set_gui ();  
  }
}


void ImportTextDialog::on_button_unicode_clicked (GtkButton *button, gpointer user_data)
{
  ((ImportTextDialog *) user_data)->on_button_unicode();
}


void ImportTextDialog::on_button_unicode ()
{
  for (unsigned int i = 0; i < filenames_not_in_unicode.size(); i++) {
    UnicodeDialog unicodedialog (filenames_not_in_unicode[i]);
    if (unicodedialog.run () == GTK_RESPONSE_OK) {
      encoding_convert_from = unicodedialog.encoding;
      break;
    }
  }
  set_gui ();
}


void ImportTextDialog::on_checkbutton_overwrite_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  ((ImportTextDialog *) user_data)->set_gui();
}


void ImportTextDialog::on_okbutton_clicked (GtkButton *button, gpointer user_data)
{
  ((ImportTextDialog *) user_data)->on_okbutton();
}


void ImportTextDialog::on_okbutton ()
// Does the actual import
{
  extern Settings * settings;
  // Progress.
  ProgressWindow progress ("Importing ...", false);
  progress.set_iterate (0, 1, selectable_booknames.size());
  // Make container with all files to be converted.
  set <ustring> to_be_converted (filenames_not_in_unicode.begin(), filenames_not_in_unicode.end());
  // Go through all selectable books.
  for (unsigned int i = 0; i < selectable_booknames.size(); i++) {
    progress.iterate ();
    // Is book selected for import?
    if (selected_books.find (selectable_booknames[i]) != selected_books.end()) {
      // Get book's filename.
      ustring source = selectable_filenames[i];
      // Handle possible special encoding, and read the book.
      ustring encoding;
      if (to_be_converted.find (selectable_filenames[i]) != to_be_converted.end()) {
        encoding = encoding_convert_from;
      }
      ImportBookRead ibr (source, encoding);
      // Depending on type of import, convert the book.
      switch (importtype) {
        case itUsfm:
        {
          ibr.usfm ();
          break;
        }
        case itBibleWorks:
        {
          ibr.bibleworks ();
          break;
        }
        case itMechonMamre:
        {
          ibr.mechonmamre ();
          break;
        }          
      }
      // Rewrite the \id line, mainly for unknown books, but also just to be sure.
      for (unsigned int ln = 0; ln < ibr.lines.size(); ln++) {
        if (ibr.lines[ln].find ("\\id ") == 0) {
          ustring id = books_id_to_paratext (selectable_booknames[i]);
          if (!id.empty()) {
            ibr.lines[ln].erase (4, 3);
            ibr.lines[ln].insert (4, id);
          }
          break;
        }
      }
      // Categorize the lines.
      CategorizeChapterVerse ccv (ibr.lines);
      // Store in project.
      project_store_book (settings->genconfig.project_get(), selectable_booknames[i], ccv);
    }
  }
}


void ImportTextDialog::set_gui()
{
  // Handle progress.
  ProgressWindow progresswindow ("Processing import...", false);
  
  // Configuration
  extern Settings * settings;
 
  // Directory part.
  
  // Initialize variable directory.
  if (directory.empty ()) 
    directory = g_get_home_dir ();
  // Read the directory, without the subdirectories.
  vector <ustring> all_files;
  ReadFiles rd (directory, "", "");
  for (unsigned int i = 0; i < rd.files.size (); i++) {
    ustring filename = gw_build_filename (directory, rd.files[i]);
    /* Only check on files with a size smaller than so many bytes. This solves a
       problem of importing files, when a huge file of hundreds of megabytes is 
       found. It would try to read it all. This would take a very long time. 
       Therefore only files that are relatively small are taken into account.
       A file exported from BibleWorks can have sizes up to about 5 Mbyte.
     */
    if (file_get_size (filename) < 10000000) {
      all_files.push_back (filename);
    }
  }
  // See which files seem proper text files. Try all import types.
  bool tried_everything = false;
  vector <ustring> proper_text_files;
  importtype = itUsfm;
  while (!tried_everything && proper_text_files.empty()) {
    progresswindow.set_iterate (0, 0.5, all_files.size());
    for (unsigned int i = 0; i < all_files.size (); i++) {
      progresswindow.iterate ();
      switch (importtype) {
        case itUsfm:
        {
          if (!get_usfm_id_from_file (all_files[i]).empty ()) {
            proper_text_files.push_back (all_files[i]);
          }
          break;
        }
        case itBibleWorks:
        {
          if (!bibleworks_file_get_bookname (all_files[i]).empty()) {
            proper_text_files.push_back (all_files[i]);
          }
          break;
        }
        case itMechonMamre:
        {
          if (mechon_mamre_copyright (all_files[i])) {
            unsigned int digitcount = digit_count_in_string (all_files[i]);
            if (digitcount == 3 || digitcount == 4) {
              proper_text_files.push_back (all_files[i]);
            }
          }
          break;
        }          
        default: 
        {
          // We're through looking for importable files.
          tried_everything = true;
        }
      }
    }
    // If no proper files were found, try the next type.
    if (proper_text_files.empty()) {
      unsigned int i = importtype;
      i++;
      importtype = (ImportType) i;
    }
  }
  // Info for user.
  ustring info = "Directory: " + directory;
  if (all_files.empty())
    info.append ("\nThis directory contains no files");
  else {
    info.append ("\nNumber of files in this directory: " + convert_to_string (int (all_files.size ())));
    info.append ("\nNumber of files that seem to be proper texts: " + convert_to_string (int (proper_text_files.size ())));
    if (!proper_text_files.empty()) {
      info.append ("\nType of texts found: " + import_type_human_readable (importtype));
    }
  }
  gtk_label_set_text (GTK_LABEL (label_directory), info.c_str());
  // Set Warning/Okay.
  gui_okay (image_directory, label_directory_gui, proper_text_files.size () > 0);

  // Handle book type.

  switch (importtype) {
    case itUsfm:
    {
      // Do nothing.
      break;
    }
    case itBibleWorks:
    {
      // Importing BibleWorks files: divide the files per book.
      vector <ustring> bibleworks_text_files (proper_text_files);
      proper_text_files.clear();
      for (unsigned int i = 0; i < bibleworks_text_files.size(); i++) {
        vector<ustring> filenames = bibleworks_file_divide (bibleworks_text_files[i]);
        for (unsigned int i2 = 0; i2 < filenames.size(); i2++) {
          proper_text_files.push_back (filenames[i2]);
        }
      }
      break;
    }
    case itMechonMamre:
    {
      // Importing Mechon Mamre Hebrew files: make one master file per book.
      proper_text_files = mechon_mamre_produce_master_files (proper_text_files);
      break;
    }
  }
  
  // Handle Book Assignment.

  // Vector contains all English names of the proper text files.
  vector <ustring> textfiles_names;
  for (unsigned int i = 0; i < proper_text_files.size (); i++) {
    ustring english_name;
    switch (importtype) {
      case itUsfm:
      {
        ustring id_string = get_usfm_id_from_file (proper_text_files[i]);
        unsigned int id = books_paratext_to_id (id_string);
        english_name = books_id_to_english (id);
        break;
      }
      case itBibleWorks:
      {
        english_name = bibleworks_file_get_bookname (proper_text_files[i]);
        break;
      }
      case itMechonMamre:
      {
        english_name = gw_path_get_basename (proper_text_files[i]);
        break;
      }          
    }
    textfiles_names.push_back (english_name);
  }
  // See if we have booknames for all our textfiles. If not try to get them.
  unknown_filenames.clear();
  for (unsigned int i = 0; i < textfiles_names.size(); i++) {
    if (textfiles_names[i].empty()) {
      textfiles_names[i] = filename_bookname_map [proper_text_files[i]];
      if (textfiles_names[i].empty())
        unknown_filenames.push_back (proper_text_files[i]);
    }
  }
  // If the user has assigned names to unknwon books, even if some books were 
  // left unassigned, it still will be regarded as done.
  if (assigning_done) unknown_filenames.clear();
  // Set gui and button depending on whether booknames are fine.  
  gui_okay (image_assign_ok, label_assign_ok, unknown_filenames.empty());
  gtk_widget_set_sensitive (button_assign, !unknown_filenames.empty());
  ustring assign_information;
  if (unknown_filenames.empty()) {
    assign_information = "The names of all books are known";
  } else {
    assign_information = "There are unknown books: " + convert_to_string (unknown_filenames.size());
  }
  gtk_label_set_text (GTK_LABEL (label_assign_info), assign_information.c_str());
  
  // Handle Book Selection.
  
  // Set button's sensitivity.
  gtk_widget_set_sensitive (button_books, proper_text_files.size() > 0);
  // Handle selection mechanism.
  selectable_booknames.clear();
  selectable_filenames.clear();
  if (select_all_books)
    selected_books.clear();
  // See which ones can be imported.
  // If we overwrite existing text, we take it as if there are no books in the
  // project.
  vector <unsigned int> books_already_in_project;
  vector <unsigned int> scripture_books;
  if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton_overwrite)))
    scripture_books = project_get_books (settings->genconfig.project_get());  
  vector <unsigned int> ids = books_type_to_ids (btUnknown);
  for (unsigned int i = 0; i < ids.size(); i++) {
    unsigned int book_id = ids[i];
    bool inproject = false;
    for (unsigned int i2 = 0; i2 < scripture_books.size (); i2++) {
      if (scripture_books[i2] == book_id) {
        inproject = true;
        books_already_in_project.push_back (book_id);
        break;
      }
    }
    if (!inproject) {
      // Search if the book is among the files.
      vector <ustring> filenames_particular_book;
      for (unsigned int i2 = 0; i2 < proper_text_files.size (); i2++) {
        if (textfiles_names[i2] == books_id_to_english (book_id)) {
          filenames_particular_book.push_back (proper_text_files[i2]);
        }
      }
      // Sort out what we found.
      ustring booktocopy;
      if (filenames_particular_book.empty ());
      else if (filenames_particular_book.size () == 1) {
        booktocopy = filenames_particular_book[0];
      } else {
        booktocopy = pick_newest_filename (filenames_particular_book);
      }
      // If we've a book, store it.
      if (!booktocopy.empty ()) {
        selectable_filenames.push_back (booktocopy);
        selectable_booknames.push_back (book_id);
        if (select_all_books)
          selected_books.insert (book_id);
      }
    }
  }
  // Clear initialization flag.
  select_all_books = false;
  // Set whether anything was selected.
  gui_okay (image_books, label_books_gui, selected_books.size () > 0);
  // Information for user.
  info = "Books selected: " + convert_to_string ((unsigned int) selected_books.size()) + " out of " + convert_to_string ((unsigned int) selectable_booknames.size());
  if (!books_already_in_project.empty()) {
    info.append ("\nBooks that could not be imported\nbecause they are already in the project: " + convert_to_string ((unsigned int) books_already_in_project.size()));
  }
  gtk_label_set_text (GTK_LABEL (label_books), info.c_str());
  
  // Unicode part.
  
  // Progress
  progresswindow.set_iterate (0.5, 1, selectable_filenames.size());
  // Sensitivity of button.
  gtk_widget_set_sensitive (button_unicode, selected_books.size () > 0);
  // Check all text on Unicode.
  filenames_not_in_unicode.clear();
  for (unsigned int i = 0; i < selectable_filenames.size(); i++) {
    // Progress
    progresswindow.iterate ();
    // Is this book selected?
    if (selected_books.find (selectable_booknames[i]) != selected_books.end()) {
      ustring file_contents;
      bool unicode_valid = true;
      {
        gchar *s;
        g_file_get_contents (selectable_filenames[i].c_str(), &s, NULL, NULL);
        file_contents = s;
        g_free (s);
      }
      if (!file_contents.validate ()) {
        unicode_valid = false;
        filenames_not_in_unicode.push_back (selectable_filenames[i]);
      }
    }    
  }
  // User information. Widgets sensitivity.
  bool unicode_ok = filenames_not_in_unicode.size() == 0;
  info = "Books in already Unicode format: " + convert_to_string ((unsigned int) (selected_books.size() - filenames_not_in_unicode.size()));
  if (!encoding_convert_from.empty()) {
    info.append ("\nBooks to be converted from encoding " + encoding_convert_from + " to Unicode: " + convert_to_string ((unsigned int) (filenames_not_in_unicode.size())));
    unicode_ok = true;
  }
  gtk_label_set_text (GTK_LABEL (label_unicode), info.c_str());
  gui_okay (image_unicode, label_unicode_gui, unicode_ok);
  gtk_widget_set_sensitive (button_unicode, filenames_not_in_unicode.size() != 0);

  // Set whether linking and importing per chapter is possible.
  switch (importtype) {
    // Usfm: possible.
    case itUsfm:
    {
      gtk_widget_set_sensitive (checkbutton_overwrite, true);
      break;
    }
    // BibleWorks, etc.: not possible.
    case itBibleWorks:
    case itMechonMamre:
    {
      gtk_widget_set_sensitive (checkbutton_overwrite, false);
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_overwrite), false);      
      break;
    }          
  }
  
  // Final information.
  bool import_possible = (proper_text_files.size () > 0) && (selected_books.size () > 0) && (unicode_ok);
  info.clear();
  if (import_possible)
    info = "Press OK to import";
  gtk_label_set_text (GTK_LABEL (label_import), info.c_str());
  // Set sensitivity of OK button.
  gtk_widget_set_sensitive (okbutton, import_possible);
}
