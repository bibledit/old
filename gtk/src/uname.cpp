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

#include <glib.h>
#include "uname.h"
#include "utilities.h"

UNameType uname_get()
{
#ifndef WIN32
  string sysname;
  struct utsname info;
  if (uname(&info) == -1)
    perror("uname");
  else
    sysname = info.sysname;
  UNameType result;
  sysname = lowerCase(sysname);
  if (sysname.find("linux") != string::npos)
    result = untLinux;
  else if (sysname.find("darwin") != string::npos)
    result = untDarwin;
  else
    result = untUnknown;
  return result;
#endif
#ifdef WIN32
  return untWindows;
#endif
}

ustring uname_apostrophy(const ustring & filename, bool windows)
// Normally, that is on Linux, a filename can be put in apostrophies, like
// '/usr/bin/bibledit'. On Windows this should be "\usr\bin\bibledit".
// This function puts the right apostrophies around the filename and returns
// the resulting filename.
{
  ustring localfilename(filename);
  if (windows) {
    localfilename.insert(0, "\"");
    localfilename.append("\"");
  } else {
    localfilename.insert(0, "'");
    localfilename.append("'");
  }
  return localfilename;
}
