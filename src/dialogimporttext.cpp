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
#include "help.h"
#include "shortcuts.h"
#include "tiny_utilities.h"
#include "gtkwrappers.h"
#include "compress.h"
#include "directories.h"
#include "unixwrappers.h"
#include "localizedbooks.h"
#include "dialogbooknames.h"


void on_okbutton()
// Does the actual import
{
/*
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
      case ibtMechonMamre:
        {
          ibr.mechonmamre();
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
  */
}

void set_gui()
{
/*

      switch (importtype) {
      case ibtBibleWorks:
        {
          if (!bibleworks_file_get_bookname(all_files[i]).empty()) {
            proper_text_files.push_back(all_files[i]);
          }
          break;
        }
      case ibtMechonMamre:
        {
          if (mechon_mamre_copyright(all_files[i])) {
            unsigned int digitcount = digit_count_in_string(all_files[i]);
            if (digitcount == 3 || digitcount == 4) {
              proper_text_files.push_back(all_files[i]);
            }
          }
          break;
        }
      case ibtOnlineBible:
      {
        if (online_bible_file(all_files[i])) {
          proper_text_files.push_back (all_files[i]);
        }
        break;
      }
      }
    }

  }


  // When importing files from the Online Bible, we can use unusual book names.
  gtk_widget_set_sensitive (hbox_book_names, (importtype == ibtOnlineBible));


  // Handle book type.

  switch (importtype) {
  case ibtBibleWorks:
    {
      // Importing BibleWorks files: divide the files per book.
      vector < ustring > bibleworks_text_files(proper_text_files);
      proper_text_files.clear();
      for (unsigned int i = 0; i < bibleworks_text_files.size(); i++) {
        vector < ustring > filenames = bibleworks_file_divide(bibleworks_text_files[i]);
        for (unsigned int i2 = 0; i2 < filenames.size(); i2++) {
          proper_text_files.push_back(filenames[i2]);
        }
      }
      break;
    }
  case ibtMechonMamre:
    {
      // Importing Mechon Mamre Hebrew files: make one master file per book.
      proper_text_files = mechon_mamre_produce_master_files(proper_text_files);
      break;
    }
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
    case ibtBibleWorks:
      {
        english_name = bibleworks_file_get_bookname(proper_text_files[i]);
        break;
      }
    case ibtMechonMamre:
      {
        english_name = gw_path_get_basename(proper_text_files[i]);
        break;
      }
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



  // Set whether linking and importing per chapter is possible.
  switch (importtype) {
    // Usfm: possible.
  case ibtUsfm:
    {
      gtk_widget_set_sensitive(checkbutton_overwrite, true);
      break;
    }
    // BibleWorks, etc.: not possible.
  case ibtBibleWorks:
  case ibtMechonMamre:
  case ibtOnlineBible:
  case ibtRawText:
    {
      gtk_widget_set_sensitive(checkbutton_overwrite, false);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_overwrite), false);
      break;
    }
  }
  */
}


