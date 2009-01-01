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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/

#include "shutdown.h"
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

void shutdown_actions()
// Takes certain actions when Bibledit shuts down.
{
  // Open a configuration.
  extern Settings *settings;

  // The actions are not always taken, but only once in so many days.
  int clear_up_day = settings->genconfig.clear_up_day_get();
  int current_day = date_time_julian_day_get_current();
  if (current_day < (clear_up_day + 30))
    return;
  settings->genconfig.clear_up_day_set(current_day);

  // Project related databases.
  // Delete these, as they are old. This is temporal, 
  // after some versions it can go away. Introduced in version 3.2. todo
  vector < ustring > projects = projects_get_all();
  for (unsigned int i = 0; i < projects.size(); i++) {
    unlink(gw_build_filename(directories_get_projects(), projects[i], "data.sql2").c_str());
  }

  // Stylesheets: vacuum the sqlite databases.
  vector < ustring > stylesheets;
  stylesheet_get_ones_available(stylesheets);
  for (unsigned int i = 0; i < stylesheets.size(); i++) {
    stylesheet_vacuum(stylesheets[i]);
  }

  // Notes: vacuum the sqlite databases.
  notes_vacuum();
}

void vacuum_database(const ustring & filename)
// Vacuums the database given by "filename". 
{
  if (filename.empty())
    return;
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    rc = sqlite3_open(filename.c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 2000);
    rc = sqlite3_exec(db, "vacuum;", NULL, NULL, &error);
    if (rc != SQLITE_OK) {
      throw runtime_error(error);
    }
  }
  catch(exception & ex) {
    ustring message(ex.what());
    g_critical("%s", message.c_str());
  }
  sqlite3_close(db);
}
