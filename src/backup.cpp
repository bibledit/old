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


#include "backup.h"
#include "gwrappers.h"
#include "directories.h"
#include "tiny_utilities.h"


void backup_bible (const ustring& bible, const ustring& filename)
// Backs up a complete bible.
{
  unlink (filename.c_str());
  GwSpawn spawn ("tar");
  spawn.arg ("-czf");
  spawn.arg (filename);
  spawn.arg (".");
  spawn.workingdirectory (tiny_project_directory (bible));
  spawn.progress ("Backing up Bible " + bible, false);
  spawn.run ();
}


void backup_notes (const ustring& filename)
{
  unlink (filename.c_str());
  GwSpawn spawn ("tar");
  spawn.arg ("-czf");
  spawn.arg (filename);
  spawn.arg (".");
  spawn.workingdirectory (directories_get_notes ());
  spawn.progress ("Backing up notes", false);
  spawn.run ();
}


void backup_all (const ustring& filename)
{
  unlink (filename.c_str());
  GwSpawn spawn ("tar");
  spawn.arg ("-czf");
  spawn.arg (filename);
  spawn.arg (".");
  spawn.workingdirectory (directories_get_root ());
  spawn.progress ("Backing up everything", false);
  spawn.run ();
}

