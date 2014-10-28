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


#include <library/bibledit.h>
#include <config/libraries.h>


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

  // Run Bibledit library in a thread, and wait till it is ready.
  bibledit_start ();
  cout << "Listening on http://localhost:8080" << endl;
  cout << "Press Ctrl-C to quit" << endl;
  while (bibledit_running ()) { };
  
  return EXIT_SUCCESS;
}


