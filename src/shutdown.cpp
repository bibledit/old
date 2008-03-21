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


#include "shutdown.h"
#include "directories.h"
#include "stylesheetutils.h"
#include "projectutils.h"
#include "categorize.h"
#include "progresswindow.h"
#include "settings.h"
#include "notes_utils.h"
#include "git.h"


void shutdown_actions ()
// Takes certain actions when Bibledit shuts down.
{
  ProgressWindow progresswindow ("Shutting down...", false);
  
  // Open a configuration to read parameters from.
  extern Settings * settings;
    
  unsigned int startuptime = settings->genconfig.startup_time_get ();
  // Set about to vacuum the sqlite databases.
  // Vacuuming a database is done only when it got changed. Saves time.
  // Project related databases.
  progresswindow.set_fraction (0.2);
  vector <ustring> projects = projects_get_all ();
  for (unsigned int i = 0; i < projects.size(); i++) {
    project_vacuum (projects[i], startuptime);
  }
  // Stylesheets.
  progresswindow.set_fraction (0.4);
  vector <ustring> stylesheets;
  stylesheet_get_ones_available (stylesheets);
  for (unsigned int i = 0; i < stylesheets.size(); i++) {
    stylesheet_vacuum (stylesheets[i], startuptime);
  }
  // Notes.
  progresswindow.set_fraction (0.6);
  notes_vacuum (startuptime);
  // Git system.
  progresswindow.set_fraction (0.8);
  git_cleanup ();
}
