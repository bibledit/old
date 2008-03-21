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
#include "utilities.h"
#include <glib.h>
#include "constants.h"
#include "projectutilsold.h"
#include "directories.h"
#include "gwrappers.h"
#include "shell.h"
#include "sqlite3.h"
#include "books.h"
#include "vacuum.h"
#include "bible.h"
#include "progresswindow.h"
#include "import.h"
#include "unixwrappers.h"
#include "settings.h"
#include "categorize.h"
#include "sqlite_reader.h"
#include "tiny_utilities.h"


vector<ustring> projects_get_all_old ();
ustring project_book_filename_old (const ustring& project, unsigned int book, bool check_exists = true);
vector<ustring> project_retrieve_chapter_old (const ustring& project, unsigned int book, unsigned int chapter);

vector<unsigned int> project_get_books_old (const ustring& project);
vector<unsigned int> project_get_chapters_old (const ustring& project, unsigned int book);


#define BOOK_SUFFIX ".book.sql3"


void projects_initial_check_old (bool gui)
// Does some initial checks on the projects we have.
// Upgrades projects of an older version to the current version.
{
  // Upgrade to the newer system where the data is stored in flat files again,
  // one chapter per file.
  vector<ustring> raw_projects = projects_get_all_old ();
  vector <ustring> projects;
  for (unsigned int i = 0; i < raw_projects.size(); i++) {
    ustring directory = gw_build_filename (directories_get_projects(), raw_projects[i], "data");
    if (!g_file_test (directory.c_str(), G_FILE_TEST_IS_DIR))
      projects.push_back (raw_projects[i]);
  }
  if (projects.size() > 0) {
    ProgressWindow * progresswindow = NULL;
    if (gui) {
      g_usleep (500000);
      progresswindow = new ProgressWindow ("Upgrading projects", false);
      progresswindow->set_iterate (0, 1, projects.size());
    }
    for (unsigned int p = 0; p < projects.size(); p++) {
      ustring project = projects[p];
      if (gui) {
        progresswindow->iterate ();
        progresswindow->set_text (project);
      }
      ustring datadirectory = gw_build_filename (directories_get_projects(), project, "data");
      gw_mkdir_with_parents (datadirectory);
      vector <unsigned int> books = project_get_books_old (project);
      for (unsigned int bk = 0; bk < books.size(); bk++) {
        unsigned int book = books[bk];
        ustring bookdirectory = gw_build_filename (datadirectory, books_id_to_english (book));
        gw_mkdir_with_parents (bookdirectory);
        vector <unsigned int> chapters = project_get_chapters_old (project, book);
        for (unsigned int ch = 0; ch < chapters.size(); ch++) {
          unsigned int chapter = chapters[ch];
          ustring chapterdirectory = gw_build_filename (bookdirectory, convert_to_string (chapter));
          gw_mkdir_with_parents (chapterdirectory);
          ustring filename = gw_build_filename (chapterdirectory, "data");
          vector <ustring> data = project_retrieve_chapter_old (project, book, chapter);
          write_lines (filename, data);
        }
        unlink (project_book_filename_old (project, book, false).c_str());
      }
      cout << "Project " << project << " was upgraded" << endl;
    }
    if (gui) delete progresswindow;
  }
}


vector<ustring> projects_get_all_old ()
// Gets sorted list of all projects.
{
  ReadDirectories rd (directories_get_projects (), "", "");
  sort (rd.directories.begin(), rd.directories.end());
  return rd.directories;
}


ustring project_book_filename_old (const ustring& project, unsigned int book, bool check_exists)
{
  ustring filename;
  filename = gw_build_filename (directories_get_projects (), project, convert_to_string (book) + BOOK_SUFFIX);
  if (check_exists) {
    if (!g_file_test (filename.c_str(), G_FILE_TEST_IS_REGULAR))
      throw runtime_error ("Book " + books_id_to_english (book) + " does not exist");
  }
  return filename;
}


vector<ustring> project_retrieve_chapter_old (const ustring& project, unsigned int book, unsigned int chapter)
{
  // Holds the chapter.
  vector<ustring> result;
  // Database variables.  
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try
  {
    // Open database.
    rc = sqlite3_open(project_book_filename_old (project, book).c_str (), &db);
    if (rc) throw runtime_error (sqlite3_errmsg(db));
    sqlite3_busy_timeout (db, 1000);
    // Read the chapter including the sequence of the verses.
    SqliteReader sqlitereader (0);
    char * sql;
    sql = g_strdup_printf ("select usfm, sequence from '%d';", chapter);
    rc = sqlite3_exec(db, sql, sqlitereader.callback, &sqlitereader, &error);
    g_free (sql);
    if (rc != SQLITE_OK) {
      throw runtime_error (error);
    }
    vector<ustring> lines (sqlitereader.ustring0.begin(), sqlitereader.ustring0.end());
    // Verses have been stored in the database before. 
    // We cannot be sure however that the verses will be retrieved in the order
    // they were stored. Some factors cause that.
    // For example, while replacing it is possible that only one verse gets 
    // stored. 
    // A sequence variable is used. The verses are sorted on that sequence,
    // so they will be put in the right order.
    vector <unsigned int> sequence;
    for (unsigned int i = 0; i < sqlitereader.ustring1.size(); i++) {
      sequence.push_back (convert_to_int (sqlitereader.ustring1[i]));
    }
    quick_sort (sequence, lines, 0, sequence.size());
    // Make separate lines from it.
    for (unsigned int i = 0; i < lines.size(); i++) {
      ParseLine parseline (lines[i]);
      for (unsigned int i2 = 0; i2 < parseline.lines.size(); i2++) {
        result.push_back (parseline.lines[i2]);
      }
    }
  }
  catch (exception & ex)
  {
    gw_critical (ex.what ());
  }
  // Close connection.  
  sqlite3_close (db);
  // Return the chapter.
  return result;
}


vector<unsigned int> project_get_books_old (const ustring& project)
// Gives all books in this project.
// Reorders the book according to settings made by user.
{
  vector <unsigned int> books;
  {
    ReadFiles rf (gw_build_filename (directories_get_projects (), project), "", BOOK_SUFFIX);
    for (unsigned int i = 0; i < rf.files.size(); i++) {
      ustring book (rf.files[i]);
      book = book.substr (0, book.length() - strlen (BOOK_SUFFIX));
      books.push_back (convert_to_int (book));
    }
  }
  books_order (project, books);
  return books;
}


vector<unsigned int> project_get_chapters_old (const ustring& project, unsigned int book)
// Gives all chapters in a book of a project.
{
  vector <unsigned int> chapters;
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try
  {
    rc = sqlite3_open(project_book_filename_old (project, book).c_str (), &db);
    if (rc) throw runtime_error (sqlite3_errmsg(db));
    sqlite3_busy_timeout (db, 1000);
    SqliteReader sqlitereader (0);
    char * sql;
    sql = g_strdup_printf ("select name from sqlite_master;");
    rc = sqlite3_exec(db, sql, sqlitereader.callback, &sqlitereader, &error);
    g_free (sql);
    if (rc != SQLITE_OK) {
      throw runtime_error (error);
    }
    for (unsigned int i = 0; i < sqlitereader.ustring0.size(); i++) {
      chapters.push_back (convert_to_int (sqlitereader.ustring0[i]));
    }
  }
  catch (exception & ex)
  {
    gw_critical (ex.what ());
  }
  sqlite3_close (db);
  sort (chapters.begin(), chapters.end());
  return chapters;
}
