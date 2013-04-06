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
#include "import.h"
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


ImportBookRead::ImportBookRead(const ustring & filename, const ustring & encoding)
{
  // Read the file.
  gchar *contents;
  gsize length;
  GError *error = NULL;
  if (!g_file_get_contents(filename.c_str(), &contents, &length, &error)) {
    cerr << error->message << endl;
    g_error_free(error);
    return;
  }
  // Handle files created with Windows Notepad.
  contents = de_windows_notepad(contents);

  // Convert the data.
  if (!encoding.empty()) {
    gchar *output;
    output = unicode_convert(contents, encoding);
    if (output) {
      g_free(contents);
      contents = output;
    }
  }
  // Check whether valid UTF8 data.
  if (!g_utf8_validate(contents, -1, NULL)) {
    cerr << "Data from " << filename << " is not valid UTF8." << endl;
    return;
  }
  // Divide text into separate lines.
  ParseLine parseline(contents);
  g_free(contents);
  rawlines = parseline.lines;
}



gchar *unicode_convert(gchar * data, const ustring & encoding)
{
  gchar *output;
  gssize length = strlen(data);
  GError *error = NULL;
  output = g_convert(data, length, "UTF-8", encoding.c_str(), NULL, NULL, &error);
  if (output == NULL) {
    cerr << error->message << endl;
    g_error_free(error);
  }
  return output;
}



void import_check_usfm_files (vector <ustring>& filenames, vector <unsigned int>& bookids, const ustring& bible, vector <ustring>& messages)
{
  // Check whether all the USFM files have proper \id data.
  if (messages.empty()) {
    vector <ustring> files_names_temp;
    for (unsigned int i = 0; i < filenames.size(); i++) {
      unsigned int book_id = books_paratext_to_id(get_usfm_id_from_file(filenames[i]));
      if (book_id) {
        files_names_temp.push_back (filenames[i]);
        bookids.push_back (book_id);
      } else {
        messages.push_back ("Unknown book in file " + filenames[i]);
      }
    }
    filenames = files_names_temp;
  }
  // Check whether none of the books to be imported is already in the project.
  if (messages.empty()) {
    vector <unsigned int> books_in_project = project_get_books (bible);
    set <unsigned int> books_in_project_set (books_in_project.begin(), books_in_project.end());
    for (unsigned int i = 0; i < bookids.size(); i++) {
      if (books_in_project_set.find (bookids[i]) != books_in_project_set.end()) {
        messages.push_back ("File " + filenames[i] + " has book " + books_id_to_english (bookids[i]) + ", but this one is already in the project");
      }
    }
  }
}


void import_usfm_file (const ustring& file, unsigned int book, const ustring& project, vector <ustring>& messages)
{
  // Read the file.
  gchar *contents;
  gsize length;
  GError *error = NULL;
  if (!g_file_get_contents(file.c_str(), &contents, &length, &error)) {
    messages.push_back (error->message);
    g_error_free(error);
    return;
  }

  // Handle files created with Windows Notepad.
  contents = de_windows_notepad(contents);

  // Divide text into separate lines.
  ParseLine parseline(contents);
  g_free(contents);

  // Clean up according to the USFM standard.
  CleanUsfm cleanusfm(parseline.lines);
  parseline.lines.assign(cleanusfm.lines.begin(), cleanusfm.lines.end());

  // Categorize the lines.
  CategorizeChapterVerse ccv(parseline.lines);

  // Store in project.
  project_store_book(project, book, ccv);
}


