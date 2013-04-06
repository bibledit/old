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
#include "unixwrappers.h"
#include "myfnmatch.h"
#include "gwrappers.h"
#include "utilities.h"
#include "tiny_utilities.h"


bool unix_fnmatch(const char *pattern, const ustring & text)
// This is a wrapper for the fnmatch function on Unix, as this is not available
// on Windows and some versions of Mac OSX.
{
  return (myfnmatch(pattern, text.c_str(), 0) == 0);
}


void unix_cp(const ustring & from, const ustring & to)
// This is a wrapper for the cp function on Unix, as Windows uses another one.
{
#ifdef WIN32
  GwSpawn spawn("copy");
#else
  GwSpawn spawn("cp");
#endif
  spawn.arg(from);
  spawn.arg(to);
  spawn.run();
}


void unix_cp_r(const ustring & from, const ustring & to)
// This is a wrapper for the recursive cp function on Unix, as Windows uses another one.
{
#ifdef WIN32
  gw_mkdir_with_parents(to);
  GwSpawn spawn("xcopy");
  spawn.arg("/E /I /Y");
#else
  GwSpawn spawn("cp");
  spawn.arg("-r");
#endif
  spawn.arg(from);
  spawn.arg(to);
  spawn.run();
}


void unix_mv(const ustring & from, const ustring & to, bool force)
// This is a wrapper for the mv function on Unix, which is move on Windows.
{
#ifdef WIN32
  GwSpawn spawn("move");
  if (force)
	  spawn.arg ("/Y");
#else
  GwSpawn spawn("mv");
  if (force)
  	  spawn.arg ("-f");
#endif

  spawn.arg(from);
  spawn.arg(to);
  spawn.run();
}


void unix_rmdir(const ustring & dir)
{
#ifdef WIN32
  GwSpawn spawn("rmdir");
  spawn.arg("/s");
  spawn.arg("/q");
#else
  GwSpawn spawn("rm");
  spawn.arg("-rf");
#endif
  spawn.arg(dir);
  spawn.run();
}


void unix_kill(GPid pid)
{
  GwSpawn spawn("kill");
  spawn.arg("-HUP");
  spawn.arg(convert_to_string(pid));
  spawn.run();
}

