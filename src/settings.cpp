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
#include "settings.h"


Settings::Settings (bool save_on_destroy):
session (0),
genconfig (save_on_destroy)
/*
Before this we used to retrieve every setting from the databases, 
every time it was needed. 
This made bibledit sluggish in cases where many values were needed at once.
This new object is a much faster, in-memory, system for the settings.
It loads a value the first time it is requested, 
and then keeps it in memory for the rest of the time.
It will write the values back to disk on object destruction.
*/
{
}


Settings::~Settings ()
{
  for (unsigned int i = 0; i < projectconfigurations.size(); i++) {
    delete projectconfigurations[i];
  }
}


ProjectConfiguration * Settings::projectconfig (ustring project, bool save_on_destroy)
// Returns the ProjectConfiguration object for "project".
{
  // Accomodate an empty project: Take the one currently opened.
  if (project.empty()) {
    project = genconfig.project_get();
  }
  // If this configuration has been loaded already, return a pointer to it.
  for (unsigned int i = 0; i < projectconfigurations.size(); i++) {
    if (project == projectconfigurations[i]->project) {
      return projectconfigurations[i];
    }
  }
  // Configuraton was not loaded yet, create a new object and return a pointer to it.
  ProjectConfiguration * projectconfiguration = new ProjectConfiguration  (project, save_on_destroy);
  projectconfigurations.push_back (projectconfiguration);
  return projectconfigurations[projectconfigurations.size() - 1];
}

