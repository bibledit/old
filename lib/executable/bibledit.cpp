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
#include <unistd.h>
#include <config.h>
#ifdef HAVE_LIBPROC
#include <libproc.h>
#endif
#ifdef HAVE_EXECINFO
#include <execinfo.h>
#endif


void sigint_handler (int s)
{
  if (s) {};
  bibledit_stop_library ();
}


void sigsegv_handler (int sig)
{
  void *array[10];
  size_t size;
  
  // get void*'s for all entries on the stack
  size = backtrace (array, 10);
  
  // print out all the frames to stderr
  fprintf (stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd (array, size, STDERR_FILENO);
  exit (1);
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
  
#ifdef HAVE_EXECINFO
  // Handler for segmentation fault.
  signal (SIGSEGV, sigsegv_handler);
#endif

  // Get the executable path and base the document root on it.
  string webroot;
  {
    // The following works on Linux but not on Mac OS X:
    char *linkname = (char *) malloc (256);
    memset (linkname, 0, 256); // valgrind uninitialized value(s)
    ssize_t r = readlink ("/proc/self/exe", linkname, 256);
    if (r) {};
    webroot = filter_url_dirname (linkname);
    free (linkname);
  }
  {
#ifdef HAVE_LIBPROC
    // The following works on Linux plus on Mac OS X:
    int ret;
    pid_t pid;
    char pathbuf [2048];
    pid = getpid ();
    ret = proc_pidpath (pid, pathbuf, sizeof (pathbuf));
    if (ret > 0 ) {
      webroot = filter_url_dirname (pathbuf);
    }
#endif
  }
  bibledit_initialize_library (webroot.c_str(), webroot.c_str());

  // Start the Bibledit library.
  bibledit_start_library ();
  bibledit_log ("The server started");
  cout << "Listening on http://localhost:" << config_logic_network_port () << endl;
  cout << "Press Ctrl-C to quit" << endl;
  
  // Bibledit Cloud should restart itself at midnight.
  // This is to be sure that any memory leaks don't accumulate too much
  // in case Bibledit Cloud runs for months and years.
  bibledit_set_quit_at_midnight ();
  
  // Keep running till Bibledit stops or gets interrupted.
  while (bibledit_is_running ()) { };

  bibledit_shutdown_library ();

  return EXIT_SUCCESS;
}
