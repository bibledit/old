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

/*
** January 2015: Matt Postiff changed to a class providing methods to
** figure out the various directories. The interface is identical to
** what it was before except you call through Directories->XYZ()
** instead of directories_XYZ().
*/

#ifndef INCLUDED_DIRECTORIES_H
#define INCLUDED_DIRECTORIES_H


#include "libraries.h"

class directories {
 private:
  // Directory bibledit is run from (e.g. /usr/bin or
  // C:\Program Files\Bibledit\editor\bin)
  ustring rundir;
  // Executable name (e.g. bibledit-gtk)
  ustring exename;
  // Package data directory (e.g. /usr/share/bibledit-gtk or
  // C:\Program Files\Bibledit\editor\share\bibledit)
  ustring package_data;
  ustring root;
  
  ustring fix_slashes(ustring &tofix);

 public: 
  // Must initialize this class with argv[0] so it knows where the
  // program was run from. See bibledit.cpp and bibledit-rdwrt.cpp for
  // this.
  directories(char *argv0);
  ~directories();

     void check_structure ();
  ustring get_root ();
  ustring get_projects ();
  ustring get_notes ();
  ustring get_stylesheets ();
  ustring get_configuration ();
  ustring get_pictures ();
  ustring get_resources ();
  ustring get_scripts ();
  ustring get_temp ();
  ustring get_templates ();
  ustring get_templates_user ();
  ustring get_package_data ();
  ustring get_restore ();
};

// Declared in bibledit.cpp and bibledit-rdwrt.cpp
extern directories *Directories;

#endif
