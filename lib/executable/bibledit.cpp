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
#include <config/logic.h>


void sigint_handler (int s)
{
  if (s) {};
  bibledit_stop_library ();
}


int main (int argc, char **argv) 
{
  if (argc) {};
  if (argv[0]) {};
  
  bibledit_initialize_library ();

  // Ctrl-C initiates a clean shutdown sequence, so there's no memory leak.
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = sigint_handler;
  sigemptyset (&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction (SIGINT, &sigIntHandler, NULL);

  // Get the executable path and base the document root on it.
  // Mac OS X: NSGetExecutablePath()
  char *linkname = (char *) malloc (256);
  memset (linkname, 0, 256); // valgrind uninitialized value(s)
  ssize_t r = readlink ("/proc/self/exe", linkname, 256);
  if (r) {};
  bibledit_set_web_root (filter_url_dirname (linkname).c_str());
  free (linkname);

  // Start the Bibledit library.
  bibledit_start_library ();
  cout << "Listening on http://localhost:" << config_logic_network_port () << endl;
  cout << "Press Ctrl-C to quit" << endl;
  
  // Server should restart itself at midnight.
  // This is to be sure that any memory leaks don't accumulate too much
  // in case the Bibledit server runs for months and years.
  bibledit_set_quit_at_midnight ();
  
  // Wait till Bibledit is shut down.
  while (bibledit_is_running ()) { };

  bibledit_shutdown_library ();

  return EXIT_SUCCESS;
}


