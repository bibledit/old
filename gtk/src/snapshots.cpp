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


#include "snapshots.h"
#include "projectutils.h"
#include "directories.h"
#include "gwrappers.h"
#include <sqlite3.h>
#include "date_time_utils.h"
#include "progresswindow.h"
#include "utilities.h"
#include "tiny_utilities.h"
#include "maintenance.h"
#include "books.h"


ustring old_snapshots_content_database (const ustring& project)
// Gives the old snapshots content database for a given project.
{
  return gw_build_filename(Directories->get_projects(), project, "snapshots.sql");
}


ustring snapshots_directory (const ustring& project)
{
  return gw_build_filename(Directories->get_projects(), project, "snapshots");
}


ustring snapshots_database (const ustring& project, unsigned int book, unsigned int chapter)
{
  ustring filename;
  if (book) {
    filename = gw_build_filename (snapshots_directory (project), books_id_to_english (book) + "." + convert_to_string (chapter) + ".sql");
  }
  return filename;
}


void snapshots_initialize_all ()
// Initializes the snapshots system.
{
  // Initialize contents database for all projects.
  vector <ustring> projects = projects_get_all();
  for (unsigned int i = 0; i < projects.size(); i++) {
    snapshots_initialize_project(projects[i]);
  }
}


void snapshots_initialize_project (const ustring& project)
// Initializes the snapshots system for a given project.
{
  // Ensure that the snapshots directory is there.
  ustring directory = snapshots_directory (project);
  if (g_file_test(directory.c_str(), G_FILE_TEST_IS_DIR))
    return;
  
  // Create directory.
  gw_mkdir_with_parents (directory);

  // If the old snapshots database is there, convert it to the new,
  // and remove it. Otherwise take snapshot of the whole project.
  ustring old_filename = old_snapshots_content_database (project);
  if (g_file_test(old_filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    ProgressWindow progresswindow ("Converting snapshots to new format for project " + project, false);
    sqlite3 *db;
    sqlite3_open(old_snapshots_content_database (project).c_str(), &db);
    SqliteReader reader(0);
    char *sql;
    sql = g_strdup_printf("select * from snapshots;");
    sqlite3_exec(db, sql, reader.callback, &reader, NULL);
    g_free(sql);
    progresswindow.set_iterate (0, 1, reader.ustring0.size());
    for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
      progresswindow.iterate ();
      unsigned int book = convert_to_int (reader.ustring0[i]);
      unsigned int chapter = convert_to_int (reader.ustring1[i]);
      ustring content = reader.ustring2[i];
      unsigned int seconds = convert_to_int (reader.ustring3[i]);
      unsigned int persistent = convert_to_int (reader.ustring4[i]);
      snapshots_shoot_chapter (project, book, chapter, content, seconds, persistent);
    }
    sqlite3_close(db);
    unlink (old_filename.c_str());
  } else {
    snapshots_shoot_project (project);
  }
}


void snapshots_shoot_project (const ustring& project)
// Takes a snapshot of the whole project.
{
  ProgressWindow progresswindow ("Updating Snapshots for Project " + project, false);
  // Store snapshots of all the chapters in the project.
  vector <unsigned int> books = project_get_books(project);
  progresswindow.set_iterate(0, 1, books.size());
  for (unsigned int bk = 0; bk < books.size(); bk++) {
    progresswindow.iterate();
    vector <unsigned int> chapters = project_get_chapters(project, books[bk]);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {
      snapshots_shoot_chapter (project, books[bk], chapters[ch], 0, false);
    }
  }
}


void snapshots_shoot_chapter (const ustring& project, unsigned int book, unsigned int chapter, const ustring& content, unsigned int seconds, bool persistent)
// Stores the data passed as a snapshot.
{
  // Database filename.
  ustring filename = snapshots_database (project, book, chapter);

  // Create the database if it does not exist.
  if (!g_file_test(filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    sqlite3 *db;
    sqlite3_open(filename.c_str(), &db);
    sqlite3_exec(db, "PRAGMA synchronous=OFF;", NULL, NULL, NULL);
    char *sql;
    sql = g_strdup_printf("create table snapshots (content text, seconds integer, persistent integer);");
    sqlite3_exec(db, sql, NULL, NULL, NULL);
    g_free(sql);
    sqlite3_close(db);
  }

  // Store it in the database.
  sqlite3 *db;
  sqlite3_open(snapshots_database (project, book, chapter).c_str(), &db);
  char *sql;
  sql = g_strdup_printf("insert into snapshots values ('%s', %d, %d)", double_apostrophy (content).c_str(), seconds, persistent);
  sqlite3_exec(db, sql, NULL, NULL, NULL);
  g_free(sql);
  sqlite3_close(db);
}


void snapshots_shoot_chapter (const ustring& project, unsigned int book, unsigned int chapter, unsigned int seconds, bool persistent)
// Stores a snapshot of the chapter in the database.
// seconds: the time. If 0, it makes its own time.
// persistent: whether this snapshots is persistent, that is, should never be removed.
{
  // Get the right seconds.
  if (seconds == 0)  
    seconds = date_time_seconds_get_current();

  // Get the chapter data.
  gchar *contents;
  ustring datafile = project_data_filename_chapter (project, book, chapter, false);
  g_file_get_contents(datafile.c_str(), &contents, NULL, NULL);
  ustring content;
  if (contents) {
    content = contents;
    g_free (contents);
  }

  // Store it.
  snapshots_shoot_chapter (project, book, chapter, content, seconds, persistent);

  // Register the database.
  maintenance_register_snapshot (snapshots_database (project, book, chapter));
  maintenance_register_database (snapshots_database (project, book, chapter));
}


vector <unsigned int> snapshots_get_seconds (const ustring& project, unsigned int book, unsigned int chapter)
// Retrieves a list of seconds from the database, given a project, book, and chapter.
{
  vector <unsigned int> seconds;
  sqlite3 *db;
  sqlite3_open(snapshots_database (project, book, chapter).c_str(), &db);
  SqliteReader reader(0);
  char *sql;
  sql = g_strdup_printf("select seconds from snapshots order by seconds desc;");
  sqlite3_exec(db, sql, reader.callback, &reader, NULL);
  g_free(sql);
  for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
    seconds.push_back (convert_to_int (reader.ustring0[i]));
  }
  sqlite3_close(db);
  return seconds;
}


ustring snapshots_get_chapter (const ustring& project, unsigned int book, unsigned int chapter, unsigned int seconds)
// Retrieves a chapter's data from the database, given a project, book, chapter number, and the seconds.
{
  ustring data;
  sqlite3 *db;
  sqlite3_open(snapshots_database (project, book, chapter).c_str(), &db);
  SqliteReader reader(0);
  char *sql;
  sql = g_strdup_printf("select content from snapshots where seconds = %d;", seconds);
  sqlite3_exec(db, sql, reader.callback, &reader, NULL);
  g_free(sql);
  if (!reader.ustring0.empty()) {
    data = reader.ustring0[0];
  }
  sqlite3_close(db);
  return data;
}


unsigned int snapshots_oldest_second (const ustring& project)
{
  unsigned int oldest_second = 0;
  bool first_one_found = false;
  for (unsigned int book = 1; book <= 150; book++) {
    for (unsigned int chapter = 0; chapter <= 150; chapter++) {
      ustring filename = snapshots_database (project, book, chapter);
      if (g_file_test(filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
        sqlite3 *db;
        sqlite3_open(filename.c_str(), &db);
        SqliteReader reader(0);
        char *sql;
        sql = g_strdup_printf("select seconds from snapshots order by seconds asc;");
        sqlite3_exec(db, sql, reader.callback, &reader, NULL);
        g_free(sql);
        if (!reader.ustring0.empty()) {
          unsigned int second = convert_to_int (reader.ustring0[0]);
          if (first_one_found) {
            if (second < oldest_second) 
              oldest_second = second;
          } else {
            oldest_second = second;
          }
          first_one_found = true;
        }
        sqlite3_close(db);
      }
    }
  }
  return oldest_second;
}


void snapshots_get_chapters_changed_since(const ustring & project, unsigned int second, vector <unsigned int>& books, vector <unsigned int>& chapters)
// This gives the books and chapters changed since the second given.
{
  books.clear();
  chapters.clear();
  vector <unsigned int> projectbooks = project_get_books(project);
  for (unsigned int bk = 0; bk < projectbooks.size(); bk++) {
    vector <unsigned int> projectchapters = project_get_chapters(project, projectbooks[bk]);
    for (unsigned int ch = 0; ch < projectchapters.size(); ch++) {
      unsigned int book = projectbooks[bk];
      unsigned int chapter = projectchapters[ch];
      vector <unsigned int> seconds = snapshots_get_seconds (project, book, chapter);
      if (!seconds.empty()) {
        if (seconds[0] > second) {
          books.push_back (book);
          chapters.push_back (chapter);
        }
      }
    }
  }
}


