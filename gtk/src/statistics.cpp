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

#include "statistics.h"
#include "projectutils.h"
#include "directories.h"
#include "gwrappers.h"
#include <sqlite3.h>
#include "date_time_utils.h"
#include "reference.h"
#include "progresswindow.h"
#include "utilities.h"

class StatisticsRecord {
 public:
  StatisticsRecord(const ustring & project);
  ~StatisticsRecord();
  void edit(const Reference & reference);
 private:
   ustring myproject;
   vector < Reference > edits;
};

ustring statistics_database(const ustring & project, const gchar * suffix)
{
  ustring filename;
  filename = gw_build_filename(directories_get_projects(), project, "statistics.");
  if (suffix)
    filename.append(suffix);
  // Current suffix.
  else
    filename.append("sql1");
  return filename;
}

void statistics_initial_check_all()
// Does initial checks on the statistics systems.
{
  // Do all projects.
  vector < ustring > projects = projects_get_all();
  for (unsigned int i = 0; i < projects.size(); i++) {
    statistics_initial_check_project(projects[i], true);
  }
}

void statistics_initial_check_project(const ustring & project, bool gui)
// Does initial checks on the statistics system of a project.
// Upgrades if needed.
{
  // Name of the database.
  ustring filename = statistics_database(project);

  // Since the system is out of order, remove any old database.
  if (g_file_test(filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    gw_message ("Removing Statistics Database for project " + project);
    unlink (filename.c_str());
  }
  return;

  // Skip if database exists.
  if (g_file_test(filename.c_str(), G_FILE_TEST_IS_REGULAR))
    return;

  // Progress.
  cout << "Statistics database for " << project << ":";
  ProgressWindow *progresswindow = NULL;
  if (gui)
    progresswindow = new ProgressWindow(project + ": upgrading statistics", false);

  // Create and initialize the database.
  filename = statistics_database(project, "sql1");
  if (!g_file_test(filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    cout << " creating";
    sqlite3 *db;
    int rc;
    try {
      rc = sqlite3_open(filename.c_str(), &db);
      if (rc)
        throw runtime_error(sqlite3_errmsg(db));
      char *sql;

      // Table stores date/time that changes where made in a reference.
      sql = g_strdup_printf("create table edits (book integer, chapter integer, verse text, seconds integer);");
      rc = sqlite3_exec(db, sql, NULL, NULL, NULL);
      g_free(sql);
      if (rc)
        throw runtime_error(sqlite3_errmsg(db));

      // Fast write.  
      sql = g_strdup_printf("PRAGMA synchronous=OFF;");
      rc = sqlite3_exec(db, sql, NULL, NULL, NULL);
      g_free(sql);
      if (rc)
        throw runtime_error(sqlite3_errmsg(db));

      // Initially fill the table with all verses we now have.
      int seconds = date_time_seconds_get_current();
      vector < unsigned int >books = project_get_books(project);
      if (gui)
        progresswindow->set_iterate(0, 1, books.size());
      for (unsigned int bk = 0; bk < books.size(); bk++) {
        if (gui)
          progresswindow->iterate();
        vector < unsigned int >chapters = project_get_chapters(project, books[bk]);
        for (unsigned int ch = 0; ch < chapters.size(); ch++) {
          vector < ustring > verses = project_get_verses(project, books[bk], chapters[ch]);
          for (unsigned int vs = 0; vs < verses.size(); vs++) {
            sql = g_strdup_printf("insert into edits values (%d, %d, '%s', %d)", books[bk], chapters[ch], verses[vs].c_str(), seconds);
            rc = sqlite3_exec(db, sql, NULL, NULL, NULL);
            g_free(sql);
            if (rc)
              throw runtime_error(sqlite3_errmsg(db));
          }
        }
      }
    }
    catch(exception & ex) {
      gw_critical(ex.what());
    }
    sqlite3_close(db);
  }
  // End progress.
  cout << endl;
  if (gui)
    delete progresswindow;

}

void statistics_record_store_chapter(const ustring & project, unsigned int book, unsigned int chapter, CategorizeChapterVerse & ccv, unsigned int first, unsigned int last)
{
  return;
  // Does the book exist?
  bool book_exists = project_book_exists(project, book);

  // List of verses currently in the chapter.
  vector < ustring > current_verses;
  if (book_exists)
    current_verses = project_get_verses(project, book, chapter);

  // List of verses going to be stored in the chapter.
  set < ustring > new_verses;
  for (unsigned int i = first; i < last; i++) {
    new_verses.insert(ccv.verse[i]);
  }

  // Statistics recorder.
  StatisticsRecord record(project);

  // Check if a verse fell out.
  for (unsigned int i = 0; i < current_verses.size(); i++) {
    if (new_verses.find(current_verses[i]) == new_verses.end()) {
      Reference reference(book, chapter, current_verses[i]);
      record.edit(reference);
    }
  }

  // Compare new verses with old ones.
  for (unsigned int i = first; i < last; i++) {
    ustring old_data;
    if (book_exists)
      old_data = project_retrieve_verse(project, book, chapter, ccv.verse[i]);
    if (ccv.line[i] != old_data) {
      Reference reference(book, chapter, ccv.verse[i]);
      record.edit(reference);
    }
  }
}

void statistics_record_store_verse(const ustring & project, unsigned int book, unsigned int chapter, const ustring & verse, const ustring & data)
{
  return;
  ustring old_data = project_retrieve_verse(project, book, chapter, verse);
  if (data != old_data) {
    StatisticsRecord record(project);
    Reference reference(book, chapter, verse);
    record.edit(reference);
  }
}

void statistics_record_remove_chapter(const ustring & project, unsigned int book, unsigned int chapter)
{
  return;
  StatisticsRecord record(project);
  vector < ustring > verses = project_get_verses(project, book, chapter);
  for (unsigned int vs = 0; vs < verses.size(); vs++) {
    Reference reference(book, chapter, verses[vs]);
    record.edit(reference);
  }
}

void statistics_record_remove_book(const ustring & project, unsigned int book)
{
  return;
  StatisticsRecord record(project);
  vector < unsigned int >chapters = project_get_chapters(project, book);
  for (unsigned int ch = 0; ch < chapters.size(); ch++) {
    vector < ustring > verses = project_get_verses(project, book, chapters[ch]);
    for (unsigned int vs = 0; vs < verses.size(); vs++) {
      Reference reference(book, chapters[ch], verses[vs]);
      record.edit(reference);
    }
  }
}

StatisticsRecord::StatisticsRecord(const ustring & project)
{
  myproject = project;
}

StatisticsRecord::~StatisticsRecord()
{
  // See if we record edits.
  if (!edits.empty()) {
    sqlite3 *db;
    int rc;
    try {
      rc = sqlite3_open(statistics_database(myproject).c_str(), &db);
      if (rc)
        throw runtime_error(sqlite3_errmsg(db));
      char *sql;
      // Fast write.  
      sql = g_strdup_printf("PRAGMA synchronous=OFF;");
      rc = sqlite3_exec(db, sql, NULL, NULL, NULL);
      g_free(sql);
      if (rc)
        throw runtime_error(sqlite3_errmsg(db));
      // Record edits.
      int seconds = date_time_seconds_get_current();
      for (unsigned int i = 0; i < edits.size(); i++) {
        sql = g_strdup_printf("insert into edits values (%d, %d, '%s', %d)", edits[i].book, edits[i].chapter, edits[i].verse.c_str(), seconds);
        rc = sqlite3_exec(db, sql, NULL, NULL, NULL);
        g_free(sql);
        if (rc)
          throw runtime_error(sqlite3_errmsg(db));
      }
    }
    catch(exception & ex) {
      gw_critical(ex.what());
    }
    sqlite3_close(db);
  }
}

void StatisticsRecord::edit(const Reference & reference)
{
  edits.push_back(reference);
}

// Statistics is now out of order because we retrieve these things from
// the Snapshots.
// In the future we could use it to track administrative changes 
// per verse, e.g. in which stage a particular verse is.

