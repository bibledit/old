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
#include "upgrade.h"
#include "stylesheetutils.h"
#include "projectutils.h"
#include "notes_utils.h"
#include "settings.h"
#include "statistics.h"
#include "git.h"
#include "referencememory.h"
#include "snapshots.h"
#include "directories.h"
#include "gwrappers.h"
#include "kjv.h"


void upgrade()
{
  stylesheets_upgrade();
  projects_initial_check();
  notes_storage_verify();
  notes_categories_check();
  upgrade_configuration();
  vector < ustring > projects = projects_get_all();
  for (unsigned int i = 0; i < projects.size(); i++) {
    upgrade_project_configuration(projects[i]);
  }
  statistics_initial_check_all();
  references_memory_database_verify();
  snapshots_initialize_all ();
  git_upgrade ();
}


