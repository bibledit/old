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
#include "utilities.h"
#include <glib.h>
#include "referencememory.h"
#include "directories.h"
#include <sqlite3.h>
#include "sqlite_reader.h"
#include "bible.h"
#include "date_time_utils.h"
#include "gwrappers.h"
#include "shell.h"
#include "progresswindow.h"
#include "settings.h"
#include "projectutils.h"
#include "books.h"
#include "settings.h"
#include "versification.h"
#include "unixwrappers.h"
#include "tiny_utilities.h"
#include "maintenance.h"


ustring references_memory_database_filename()
// Returns the filename of the database.
{
  return gw_build_filename(Directories->get_configuration(), "referencememory.sql1");
}


void references_memory_database_verify()
// Verifies or creates the database.
{
  ustring filename = references_memory_database_filename();
  if (!g_file_test(filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    sqlite3 *db;
    sqlite3_open(filename.c_str(), &db);
    sqlite3_busy_timeout(db, 1000);
    sqlite3_exec(db, "create table chapters (book integer, chapter integer);", NULL, NULL, NULL);
    sqlite3_exec(db, "create table verses (book integer, chapter integer, verse text);", NULL, NULL, NULL);
    sqlite3_close(db);
  }
}


void references_memory_store (const Reference& reference)
// Stores a reference into the database.
{
  // Open database.
  sqlite3 *db;
  sqlite3_open(references_memory_database_filename().c_str(), &db);
  sqlite3_busy_timeout(db, 1000);
  char *sql;

  // Store the most recent chapter per book.
  sql = g_strdup_printf("delete from chapters where book = %d;", reference.book);
  sqlite3_exec(db, sql, NULL, NULL, NULL);
  g_free(sql);
  sql = g_strdup_printf("insert into chapters values (%d, %d);", reference.book, reference.chapter);
  sqlite3_exec(db, sql, NULL, NULL, NULL);
  g_free(sql);

  // Store the most recent verse per chapter.
  sql = g_strdup_printf("delete from verses where book = %d and chapter = %d;", reference.book, reference.chapter);
  sqlite3_exec(db, sql, NULL, NULL, NULL);
  g_free(sql);
  sql = g_strdup_printf("insert into verses values (%d, %d, '%s');", reference.book, reference.chapter, reference.verse.c_str());
  sqlite3_exec(db, sql, NULL, NULL, NULL);
  g_free(sql);
  
  // Close database.
  sqlite3_close(db);
  
  // Register this write in the database maintenance system.
  maintenance_register_database (references_memory_database_filename());
}


bool references_memory_retrieve (Reference& reference, bool chapter_switch)
// If references are memorized, retrieve these, and return true.
// Return false if nothing is memorized, or if nothing is found in the database.
{
  // Bail out if it doesn't remember references.
  extern Settings * settings;
  if (!settings->genconfig.remember_verse_per_chapter_get())
    return false;
  // Result of operation.
  bool result = true;  
  
  // Open database.
  sqlite3 *db;
  sqlite3_open(references_memory_database_filename().c_str(), &db);
  sqlite3_busy_timeout(db, 1000);
  char *sql;

  // Retrieve the chapter number in case the book switched.
  if (!chapter_switch) {
    SqliteReader reader(0);
    sql = g_strdup_printf("select chapter from chapters where book = %d;", reference.book);
    sqlite3_exec(db, sql, reader.callback, &reader, NULL);
    g_free(sql);
    if (reader.ustring0.empty()) {
      result = false;
    }
    if (result) {
      reference.chapter = convert_to_int (reader.ustring0[0]);
    }
  }

  // Retrieve the verse number based on the chapter that is there.
  if (result) {
    SqliteReader reader(0);
    sql = g_strdup_printf("select verse from verses where book = %d and chapter = %d;", reference.book, reference.chapter);
    sqlite3_exec(db, sql, reader.callback, &reader, NULL);
    g_free(sql);
    if (reader.ustring0.empty()) {
      result = false;
    }
    if (result) {
      reference.verse = reader.ustring0[0];
    }
  }
  
  // Close database.
  sqlite3_close(db);
    
  // Indicate whether the operation was successful.
  return result;  
}


