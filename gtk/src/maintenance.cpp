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


#include "maintenance.h"
#include "directories.h"
#include "stylesheetutils.h"
#include "projectutils.h"
#include "categorize.h"
#include "settings.h"
#include "notes_utils.h"
#include "git.h"
#include "gwrappers.h"
#include <sqlite3.h>
#include "date_time_utils.h"
#include "referencememory.h"
#include "snapshots.h"
#include "utilities.h"
#include "dialogsystemlog.h"


ustring maintenance_database_filename ()
{
  return gw_build_filename (Directories->get_temp (), "maintenance3.sql");
}


void maintenance_initialize ()
// Initializes the maintenance register.
{
  // Create database if it doesn't exist.
  if (!g_file_test (maintenance_database_filename ().c_str(), G_FILE_TEST_IS_REGULAR)) {
    sqlite3 *db;
    sqlite3_open(maintenance_database_filename().c_str(), &db);
    sqlite3_busy_timeout(db, 2000);
    sqlite3_exec(db, "create table commands  (workingdirectory text, shellcommand text);", NULL, NULL, NULL);
    sqlite3_exec(db, "create table snapshots (filename text);",                            NULL, NULL, NULL);
    sqlite3_exec(db, "create table databases (filename text);",                            NULL, NULL, NULL);
    sqlite3_exec(db, "create table gitrepos  (directory text);",                           NULL, NULL, NULL);
    sqlite3_close(db);
  }
}


void maintenance_register_shell_command (const ustring& working_directory, const ustring& shell_command)
// Registers a shell command to be run as part of the maintenance routine on bibledit shutdown.
{
  sqlite3 *db;
  sqlite3_open(maintenance_database_filename().c_str(), &db);
  sqlite3_busy_timeout(db, 2000);
  char *sql;
  sql = g_strdup_printf("insert into commands values ('%s', '%s');", double_apostrophy (working_directory).c_str(), double_apostrophy (shell_command).c_str());
  sqlite3_exec(db, sql, NULL, NULL, NULL);
  g_free(sql);
  sqlite3_close(db);
}


void maintenance_register_git_repository (const ustring& directory)
// Registers a git repository in the database for the maintenance routine on bibledit shutdown.
// The information is interpreted as: This repository has been changed once more.
{
  sqlite3 *db;
  sqlite3_open(maintenance_database_filename().c_str(), &db);
  sqlite3_busy_timeout(db, 2000);
  char *sql;
  sql = g_strdup_printf("insert into gitrepos values ('%s');", double_apostrophy (directory).c_str());
  sqlite3_exec(db, sql, NULL, NULL, NULL);
  g_free(sql);
  sqlite3_close(db);
}


void maintenance_register_database (const ustring& filename)
{
  sqlite3 *db;
  sqlite3_open(maintenance_database_filename().c_str(), &db);
  sqlite3_busy_timeout(db, 2000);
  char *sql;
  sql = g_strdup_printf("insert into databases values ('%s');", double_apostrophy (filename).c_str());
  sqlite3_exec(db, sql, NULL, NULL, NULL);
  g_free(sql);
  sqlite3_close(db);
}


void maintenance_register_snapshot (const ustring& filename)
{
  sqlite3 *db;
  sqlite3_open(maintenance_database_filename().c_str(), &db);
  sqlite3_busy_timeout(db, 2000);
  char *sql;
  sql = g_strdup_printf("insert into snapshots values ('%s');", double_apostrophy (filename).c_str());
  sqlite3_exec(db, sql, NULL, NULL, NULL);
  g_free(sql);
  sqlite3_close(db);
}


void shutdown_actions()
// Takes certain actions when Bibledit shuts down.
{
  // Start maintenance program.
  GwSpawn spawn ("bibledit-shutdown");
  spawn.arg (maintenance_database_filename());
  spawn.arg (log_file_name(lftShutdown, false));
  spawn.async ();
  spawn.run ();
}


