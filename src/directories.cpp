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


#include "libraries.h"
#include "directories.h"
#include <glib.h>
#include <config.h>
#include "constants.h"
#include "utilities.h"
#include "gwrappers.h"
#include "shell.h"
#include "unixwrappers.h"
#include "tiny_utilities.h"
#include "restore.h"


void directories_check_structure()
{
  restore_all_stage_two ();
  gw_mkdir_with_parents(directories_get_root());
  gw_mkdir_with_parents(directories_get_projects());
  gw_mkdir_with_parents(directories_get_notes());
  gw_mkdir_with_parents(directories_get_stylesheets());
  gw_mkdir_with_parents(directories_get_configuration());
  gw_mkdir_with_parents(directories_get_pictures());
  gw_mkdir_with_parents(directories_get_resources());
  gw_mkdir_with_parents(directories_get_scripts());
  gw_mkdir_with_parents(directories_get_temp());
  gw_mkdir_with_parents(directories_get_templates());
  gw_mkdir_with_parents(directories_get_templates_user());
}


ustring directories_get_root()
// Returns the root directory of all data.
{
  return tiny_directories_get_root();
}


ustring directories_get_projects()
{
  // This returns the directory with all the projects.
  return tiny_directories_get_projects();
}


ustring directories_get_notes()
{
  // This returns the directory with the notes
  return gw_build_filename(directories_get_root(), "notes");
}


ustring directories_get_stylesheets()
{
  // This returns the directory with the stylesheets
  return gw_build_filename(directories_get_root(), "stylesheets");
}


ustring directories_get_configuration()
{
  // This returns the directory with the configuration
  return gw_build_filename(directories_get_root(), "configuration");
}


ustring directories_get_pictures()
{
  // This returns the directory with the pictures
  return gw_build_filename(directories_get_root(), "pictures");
}


ustring directories_get_resources()
{
  // This returns the directory with the resources.
  return gw_build_filename(directories_get_root(), "resources");
}


ustring directories_get_scripts()
{
  // This returns the directory with the scripts.
  return gw_build_filename(directories_get_root(), "scripts");
}


ustring directories_get_temp()
{
  // Returns the temporal directory bibledit uses.
  return gw_build_filename(g_get_tmp_dir(), "bibledit");
}


ustring directories_get_templates()
{
  // This returns the directory with the templates
  return gw_build_filename(directories_get_temp(), "templates");
}


ustring directories_get_templates_user()
{
  // This returns the directory with the User's custom raw templates
  return gw_build_filename(directories_get_root(), "templates");
}


ustring directories_get_package_data()
// Gives the package data directory, cross platform.
{
  ustring directory;
#ifdef WIN32
  gchar *path;
  path = g_find_program_in_path("bibledit-bin.exe");
  if (path) {
    directory = path;
    g_free(path);
    directory = gw_path_get_dirname(directory);
    directory = gw_path_get_dirname(directory);
    directory = gw_build_filename(directory, "share", "bibledit");
  }
#else
  directory = PACKAGE_DATA_DIR;
#endif
  return directory;
}


ustring directories_get_restore ()
// The directory, if found, to restore from.
{
  ustring directory;
  directory = directories_get_root() + ".restored";
  return directory;
}

