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
#include "upgrade.h"
#include "stylesheetutils.h"
#include "projectutils.h"
#include "projectutilsold.h"
#include "notes_utils.h"
#include "settings.h"
#include "statistics.h"
#include "git.h"


void upgrade (bool gui)
{
  stylesheets_upgrade ();
  projects_initial_check_old (gui);
  projects_initial_check (gui);
  notes_database_verify (gui);
  notes_categories_check ();
  upgrade_configuration ();
  vector <ustring> projects = projects_get_all ();
  for (unsigned int i = 0; i < projects.size(); i++) {
    upgrade_project_configuration (projects[i]);
  }
  statistics_initial_check_all (gui);
  git_initial_check_all (gui);
}
