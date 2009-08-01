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
#include "bibleworks.h"
#include "utilities.h"
#include "clean.h"
#include "usfmtools.h"
#include "bible.h"
#include "books.h"
#include "gwrappers.h"
#include "directories.h"
#include "unixwrappers.h"
#include "tiny_utilities.h"
#include "dialogradiobutton.h"
#include "localizedbooks.h"
#include "categorize.h"
#include "projectutils.h"
#include "progresswindow.h"


ustring bibleworks_file_get_bookname(const ustring & filename)
// Retrieves the bookname from a file exported by BibleWorks. Though there can
// be several books in such a file, this functions retrieves only the name
// of the first book.
{
#define MYMAX 10
  ReadText rt(filename, true, false);
  unsigned int maximum = MYMAX;
  maximum = CLAMP(maximum, 0, rt.lines.size());
  unsigned int goodline = 0;
  ustring bookname;
  try {
    for (unsigned int i = 0; i < maximum; i++) {
      ustring bookabbreviation = rt.lines[i].substr(0, 3);
      unsigned int id = books_bibleworks_to_id(bookabbreviation);
      if (id) {
        goodline++;
        bookname = books_id_to_english(id);
      }
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  if (goodline != MYMAX)
    bookname.clear();
  return bookname;
#undef MYMAX
}


void import_check_bibleworks_file (vector <ustring>& filenames, vector <unsigned int>& bookids, const ustring& bible, vector <ustring>& messages)
// Checks the file exported from BibleWorks and meant to be imported as a Bible.
{
  // Check whether there's only one file selected.
  if (messages.empty()) {
    if (filenames.size() > 1) {
      messages.push_back ("You have selected more than one file");
    }
  }

  // Check that the BibleWorks file is a valid one.
  if (messages.empty()) {
    ustring english_name = bibleworks_file_get_bookname(filenames[0]);
    unsigned int id = books_english_to_id (english_name);
    if (id == 0) {
      messages.push_back ("The file cannot be recognized as coming from BibleWorks");
    }
  }

  // Check that the Bible to import the file into is empty.
  if (messages.empty()) {
    vector <unsigned int> books = project_get_books (bible);
    if (!books.empty()) {
      messages.push_back ("You try to import data into a Bible that is not empty");
    }
  }
}


void import_bibleworks_file (const ustring& file, const ustring& bible, vector <ustring>& messages)
// Imports a bibleworks file.
{
  // Read the file.
  ReadText rt (file, true, false);

  // If there's nothing to import, bail out.
  if (rt.lines.empty()) {
    messages.push_back ("The file is empty");
  }

  // Divide the input into separate bits for each book.
  vector <VectorUstring> bookdata;
  if (messages.empty()) {
    try {
      ustring previousbook;
      vector < ustring > booklines;
      for (unsigned int i = 0; i < rt.lines.size(); i++) {
        ustring currentbook = rt.lines[i].substr(0, 3);
        if (i == 0) {
          previousbook = currentbook;
        }
        if (currentbook != previousbook) {
          bookdata.push_back (booklines);
          booklines.clear();
          previousbook = currentbook;
        }
        booklines.push_back(rt.lines[i]);
      }
      bookdata.push_back (booklines);
    }
    catch(exception & ex) {
      messages.push_back(ex.what());
    }
  }

  // Import each book.
  if (messages.empty ()) {
    ProgressWindow progresswindow ("Importing", false);
    progresswindow.set_iterate (0, 1, bookdata.size());
    for (unsigned int i = 0; i < bookdata.size(); i++) {
      progresswindow.iterate ();
      try {

        // Input and output data.
        vector <ustring> rawlines = bookdata[i];
        vector <ustring> usfmlines;

        // Get the name of the book.
        unsigned int book_id = books_bibleworks_to_id(rawlines[0].substr(0, 3));
        if (book_id == 0) {
          messages.push_back ("Unknown book: " + rawlines[0]);
          return;
        }

        // Store USFM id.
        ustring usfmid = books_id_to_paratext (book_id);
        usfmlines.push_back("\\id " + usfmid);

        // Convert the BibleWorks lines to USFM code.
        ustring previouschapter = "0";
        for (unsigned int i = 0; i < rawlines.size(); i++) {
          // Convert chapter information.
          ustring line = rawlines[i];
          line.erase(0, 4);
          ustring currentchapter = number_in_string(line);
          line.erase(0, currentchapter.length() + 1);
          if (currentchapter != previouschapter) {
            usfmlines.push_back("\\c " + currentchapter);
            usfmlines.push_back("\\p");
            previouschapter = currentchapter;
          }
          // Convert verse data.
          usfmlines.push_back("\\v " + line);
        }

        // Store into the Bible.
        CategorizeChapterVerse ccv(usfmlines);
        project_store_book(bible, book_id, ccv);
        
      }
      catch(exception & ex) {
        messages.push_back(ex.what());
      }
    }
  }

}

