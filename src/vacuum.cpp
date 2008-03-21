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


#include "vacuum.h"
#include "shell.h"
#include "utilities.h"
#include "gwrappers.h"


#define BIBLEDIT_VACUUM "bibledit-vacuum"


void vacuum_database (const ustring& filename, unsigned int starttime)
// Vacuums the database given by "filename". 
// To speed things up, if the database was not modified, it does nothing.
// Waits till it is ready, lest the database operations wait on each other in deadlock.
{
  if (filename.empty())
    return;
  if (file_get_modification_time (filename) < starttime)
    return;
  GwSpawn spawn (BIBLEDIT_VACUUM);
  spawn.arg (filename);
  spawn.run ();
}


void vacuum_wait ()
// Waits till the vacuum operations, from a previous session of Bibledit, are 
// ready.
{
  while (program_is_running (BIBLEDIT_VACUUM)) {
    g_usleep (2000000);
  }
}
