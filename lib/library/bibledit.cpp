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


#include <config/libraries.h>
#include <webserver/webserver.h>
#include <library/bibledit.h>
#include <config/globals.h>
#include <filter/url.h>
#include <libxml/threads.h>


using namespace std;


// Start the Bibledit server.
void bibledit_start () 
{
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
  
  // Free XML resources.
  xmlCleanupThreads ();
}


// Stop the Bibledit server.
void bibledit_stop ()
{
  // Clear running flag.
  config_globals_running = false;
  
  // Connect to localhost to initiate the shutdown mechanism in the running server.
  struct sockaddr_in sa;
  sa.sin_family = AF_INET;
  sa.sin_port = htons (8080);
  sa.sin_addr.s_addr = inet_addr ("127.0.0.1");
  memset(sa.sin_zero, '\0', sizeof (sa.sin_zero));
  int mysocket = socket (PF_INET, SOCK_STREAM, 0);
  connect (mysocket, (struct sockaddr*) &sa, sizeof (sa));
}
