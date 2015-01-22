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
#include "settings.h"


Settings::Settings(bool save_on_destroy):
session(0), genconfig(save_on_destroy)
/*
Before this we used to retrieve every setting from the databases, 
every time it was needed. 
This made bibledit sluggish in cases where many values were needed at once.
This new object is a much faster, in-memory, system for the settings.
It loads a value the first time it is requested, 
and then keeps it in memory for the rest of the time.
It will write the values back to disk on object destruction. 
[MAP: This last statement is true, but it is not good design. There 
should be a save_settings routine that is called explicitly, and object
destruction should not save any settings, because we don't know when that
routine will be called relative to all other object destructions, if at all
(i.e. if the program terminates early).
*/
{
  event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 300000, GSourceFunc(on_timeout), gpointer(this), NULL);
}

Settings::~Settings()
{
  for (unsigned int i = 0; i < projectconfigurations.size(); i++) {
    delete projectconfigurations[i];
  }
}


ProjectConfiguration * Settings::projectconfig(ustring project, bool save_on_destroy)
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
  ProjectConfiguration *projectconfiguration = new ProjectConfiguration(project, save_on_destroy);
  projectconfigurations.push_back(projectconfiguration);
  return projectconfigurations[projectconfigurations.size() - 1];
}

// A good example of a repetitively-called routine that saves
// settings every once in a while. 
void Settings::save ()
// Saves the configurations to disk.
{
  genconfig.save();
  for (unsigned int i = 0; i < projectconfigurations.size(); i++) {
    projectconfigurations[i]->save();
  }
}


bool Settings::on_timeout(gpointer user_data)
{
  ((Settings *) user_data)->timeout();
  return true;
}


void Settings::timeout ()
{
  save ();
}

