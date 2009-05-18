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


#include "restore.h"
#include "gwrappers.h"
#include "statistics.h"
#include "utilities.h"
#include "projectutils.h"
#include "books.h"
#include "unixwrappers.h"
#include "directories.h"
#include "shell.h"
#include "gtkwrappers.h"
#include "htmlbrowser.h"
#include "tiny_utilities.h"
#include "snapshots.h"
#include "export_utils.h"
#include "notes_utils.h"


void restore_notes (const ustring& filename, vector <ustring>& feedback)
{
  /*
  unlink (filename.c_str());
  GwSpawn spawn ("tar");
  spawn.arg ("-czf");
  spawn.arg (filename);
  spawn.arg (".");
  spawn.workingdirectory (directories_get_notes ());
  spawn.progress ("Backing up notes", false);
  spawn.run ();
  */
}


void restore_all (const ustring& filename, vector <ustring>& feedback)
{
  /*
  unlink (filename.c_str());
  GwSpawn spawn ("tar");
  spawn.arg ("-czf");
  spawn.arg (filename);
  spawn.arg (".");
  spawn.workingdirectory (directories_get_root ());
  spawn.progress ("Backing up everything", false);
  spawn.run ();
  */
}

