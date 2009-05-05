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
#include "referencememory.h"
#include "snapshots.h"
#include "utilities.h"


ustring shutdown_command_file ()
{
  return gw_build_filename (directories_get_temp (), "shutdown-actions");
}

void shutdown_actions()
// Takes certain actions when Bibledit shuts down.
{
  // Open a configuration.
  extern Settings *settings;

  // The actions are not always taken, but only once a day.
  int clear_up_day = settings->genconfig.clear_up_day_get();
  int current_day = date_time_julian_day_get_current();
  if (current_day < (clear_up_day + 1)) {
    return;
  }
  settings->genconfig.clear_up_day_set(current_day);

  // Stylesheets: vacuum the sqlite database.
  stylesheet_vacuum();

  // Notes: vacuum the sqlite databases.
  notes_vacuum();
  
  // References memory: vacuum the sqlite database.
  vacuum_database (references_memory_database_filename());
  
  // Clean up Snapshots.
  snapshots_clean_up ();

  // Write shutdown actions.
  unlink (shutdown_command_file().c_str());
  extern Settings *settings;
  write_lines (shutdown_command_file(), settings->session.vacuum_files);
  GwSpawn spawn ("bibledit-shutdown");
  spawn.arg (shutdown_command_file());
  spawn.async ();
  spawn.run ();
}


void vacuum_database(const ustring & filename)
// Schedules the database given by "filename" for vacuuming.
{
  if (filename.empty())
    return;
  extern Settings *settings;
  settings->session.vacuum_files.push_back (filename);
}
