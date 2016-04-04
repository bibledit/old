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
  ustring projects;
  ustring notes;
  ustring stylesheets;
  ustring configuration;
  ustring pictures;
  ustring resources;
  ustring scripts;
  ustring temp;
  ustring templates;
  ustring templates_user;
  ustring restore;

  ustring fix_slashes(ustring &tofix);
  
  // Directory path to various utility programs we use. 3/2016 MAP
  ustring copy;
  ustring copy_recursive;
  ustring move;
  ustring rm;
  ustring rmdir;
  ustring tar;
  ustring zip;
  ustring unzip;
  ustring git;
  ustring bibledit_git;
  ustring curl;
  ustring gobiblecreator;
  ustring bibledit_shutdown;
  ustring php;
  ustring xetex;
  ustring teckit_compile;
  ustring head;
  ustring touch;
  ustring osis2mod;
  ustring mkdir;
  ustring cmdshell;
  // what about helpcommand?
  ustring tasklist; // aka ps in unix
  ustring merge;
  
 public: 
  // Must initialize this class with argv[0] so it knows where the
  // program was run from. See bibledit.cpp and bibledit-rdwrt.cpp for
  // this.
  directories(char *argv0);
  void find_utilities(void);
  ~directories();
  
  // A routine to dump the list of directories and utilities to the log file
  void print();

     void check_structure ();
	 
  // These should hold the correct platform path, with right style slashes
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
  ustring get_package_data ();       // gives the package data directory
  ustring get_restore ();            // the directory to restore from
  
  // To do: integrate runtime.h/.cpp with this stuff, perhaps in a Utilities object or similar.
  ustring get_copy ();
  ustring get_copy_recursive ();
  ustring get_move ();
  ustring get_rm ();
  ustring get_rmdir ();
  ustring get_mkdir ();
  ustring get_tar ();
  ustring get_zip ();
  ustring get_unzip ();
  ustring get_git ();
  ustring get_bibledit_git ();
  ustring get_curl ();
  ustring get_gobiblecreator ();
  ustring get_bibledit_shutdown ();
  ustring get_php ();
  ustring get_xetex ();
  ustring get_teckit_compile ();
  ustring get_head ();
  ustring get_touch ();
  ustring get_osis2mod ();
  ustring get_cmdshell ();
  // what about helpcommand?
  ustring get_tasklist (); // aka ps in unix
  ustring get_merge ();
  // what about bwoutpost?  
};

// Declared in bibledit.cpp and bibledit-rdwrt.cpp
extern directories *Directories;

#endif
