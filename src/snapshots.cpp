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


ustring snapshots_content_database (const ustring& project)
// Gives the snapshots content database for a given project.
{
  return gw_build_filename(directories_get_projects(), project, "snapshots.sql");
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
  // Skip if database exists.
  ustring filename = snapshots_content_database (project);
  if (g_file_test(filename.c_str(), G_FILE_TEST_IS_REGULAR))
    return;

  // Feedback.
  ProgressWindow progresswindow ("Creating Snapshots Database for Project " + project, false);

  // Create database.
  sqlite3 *db;
  sqlite3_open(filename.c_str(), &db);
  char *sql;
  sql = g_strdup_printf("create table snapshots (book integer, chapter integer, content text, seconds integer, persistent integer);");
  sqlite3_exec(db, sql, NULL, NULL, NULL);
  g_free(sql);
  
  // We're going to write a lot, therefore write fast.  
  sql = g_strdup_printf("PRAGMA synchronous=OFF;");
  sqlite3_exec(db, sql, NULL, NULL, NULL);
  g_free(sql);

  // Current time.
  int seconds = date_time_seconds_get_current();

  // Store snapshots of all the chapters in the project.
  vector <unsigned int> books = project_get_books(project);
  progresswindow.set_iterate(0, 1, books.size());
  for (unsigned int bk = 0; bk < books.size(); bk++) {
    progresswindow.iterate();
    vector <unsigned int> chapters = project_get_chapters(project, books[bk]);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {
      vector <ustring> content1 = project_retrieve_chapter (project, books[bk], chapters[ch]);
      ustring content2;
      for (unsigned int i = 0; i < content1.size(); i++) {
        if (i)
          content2.append ("\n");
        content2.append (content1[i]);
      }
      sql = g_strdup_printf("insert into snapshots values (%d, %d, '%s', %d, 0)", books[bk], chapters[ch], double_apostrophy (content2).c_str(), seconds);
      sqlite3_exec(db, sql, NULL, NULL, NULL);
      g_free(sql);
    }
  }

  // Close the database.
  sqlite3_close(db);
}


void snapshots_shoot_chapter (const ustring& project, unsigned int book, unsigned int chapter, unsigned int seconds, bool persistent)
// Stores a snapshot of the chapter in the database.
// seconds: the time. If 0, it makes its own time.
// persistent: whether this snapshots is persistent, that is, should never be removed.
{
  // Store snapshot content.
  {
    if (seconds == 0)  
      seconds = date_time_seconds_get_current();
    gchar *contents;
    ustring datafile = project_data_filename_chapter (project, book, chapter, false);
    g_file_get_contents(datafile.c_str(), &contents, NULL, NULL);
    sqlite3 *db;
    sqlite3_open(snapshots_content_database (project).c_str(), &db);
    char *sql;
    if (contents)
      sql = g_strdup_printf("insert into snapshots values (%d, %d, '%s', %d, %d)", book, chapter, double_apostrophy (contents).c_str(), seconds, persistent);
    else
      sql = g_strdup_printf("insert into snapshots values (%d, %d, '""', %d, %d)", book, chapter, seconds, persistent);
    sqlite3_exec(db, sql, NULL, NULL, NULL);
    g_free(sql);
    sqlite3_close(db);
    g_free (contents);
  }
  // Register project for maintenance on shutdown.
  maintenance_register_database (project, snapshots_content_database (project).c_str());
}


vector <unsigned int> snapshots_get_seconds (const ustring& project, unsigned int book, unsigned int chapter)
// Retrieves a list of seconds from the database, given a project, book, and chapter.
{
  vector <unsigned int> seconds;
  sqlite3 *db;
  sqlite3_open(snapshots_content_database (project).c_str(), &db);
  SqliteReader reader(0);
  char *sql;
  sql = g_strdup_printf("select seconds from snapshots where book = %d and chapter = %d order by seconds desc;", book, chapter);
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
  sqlite3_open(snapshots_content_database (project).c_str(), &db);
  SqliteReader reader(0);
  char *sql;
  sql = g_strdup_printf("select content from snapshots where book = %d and chapter = %d and seconds = %d;", book, chapter, seconds);
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
  sqlite3 *db;
  sqlite3_open(snapshots_content_database (project).c_str(), &db);
  SqliteReader reader(0);
  char *sql;
  sql = g_strdup_printf("select seconds from snapshots order by seconds asc;");
  sqlite3_exec(db, sql, reader.callback, &reader, NULL);
  g_free(sql);
  if (!reader.ustring0.empty()) {
    oldest_second = convert_to_int (reader.ustring0[0]);
  }
  sqlite3_close(db);
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


