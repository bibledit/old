/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include <iostream>
#include <cstdlib>
#include <webserver/webserver.h>
#include <library/bibledit.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <config/globals.h>
#include <filter/url.h>
#include <cstring>


using namespace std;


int bibledit (int argc, char **argv) 
{
  if (argc) {};
  if (argv[0]) {};

  cout << "Listening on http://localhost:8080" << endl;
  cout << "Press Ctrl-C to quit" << endl;

  // Get the executable path, and set the document root based on it.
  // Mac OS X: NSGetExecutablePath()
  // Linux: readlink /proc/self/exe
  // Solaris: getexecname()
  // FreeBSD: sysctl CTL_KERN KERN_PROC KERN_PROC_PATHNAME -1
  // FreeBSD if it has procfs: readlink /proc/curproc/file
  // NetBSD: readlink /proc/curproc/exe
  // DragonFly BSD: readlink /proc/curproc/file
  // Windows: GetModuleFileName() with hModule = NULL
  char *linkname = (char *) malloc (256);
  memset (linkname, 0, 256); // valgrind uninitialized value(s)
  ssize_t r = readlink ("/proc/self/exe", linkname, 256);
  if (r) {};
  config_globals_document_root = filter_url_dirname (linkname);
  free (linkname);

  webserver ();

  return EXIT_SUCCESS;
}

