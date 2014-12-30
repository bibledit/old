/*
Copyright (©) 2003-2015 Teus Benschop.

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


#include <library/bibledit.h>
#include <config/libraries.h>
#include <filter/url.h>


void sigint_handler (int s)
{
  if (s) {};
  bibledit_stop ();
}


int main (int argc, char **argv) 
{
  if (argc) {};
  if (argv[0]) {};
  
  // Ctrl-C initiates a clean shutdown sequence, so there's no memory leak.
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = sigint_handler;
  sigemptyset (&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction (SIGINT, &sigIntHandler, NULL);

  // Get the executable path and base the document root on it.
  // Mac OS X: NSGetExecutablePath()
  // Solaris: getexecname()
  char *linkname = (char *) malloc (256);
  memset (linkname, 0, 256); // valgrind uninitialized value(s)
  ssize_t r = readlink ("/proc/self/exe", linkname, 256);
  if (r) {};
  bibledit_root (filter_url_dirname (linkname));
  free (linkname);

  // Start the Bibledit library.
  bibledit_start ();
  cout << "Listening on http://localhost:8080" << endl;
  cout << "Press Ctrl-C to quit" << endl;
  
  // Wait till Bibledit it is ready.
  while (bibledit_running ()) { };
  
  return EXIT_SUCCESS;
}


