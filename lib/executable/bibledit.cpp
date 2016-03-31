/*
Copyright (©) 2003-2016 Teus Benschop.

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
#include <filter/string.h>
#include <config/logic.h>
#include <unistd.h>
#ifdef HAVE_LIBPROC
#include <libproc.h>
#endif
#ifdef HAVE_EXECINFO
#include <execinfo.h>
#endif
#include <database/logs.h>


void sigint_handler (int s)
{
  if (s) {};
  bibledit_stop_library ();
}


string backtrace_path ()
{
  string path = filter_url_create_root_path ("tmp", "backtrace.txt");
  return path;
}


#ifdef HAVE_EXECINFO
// http://stackoverflow.com/questions/77005/how-to-generate-a-stacktrace-when-my-gcc-c-app-crashes
// To add linker flag -rdynamic is essential.
void sigsegv_handler (int sig)
{
  if (sig) {};

  // Information.
  cout << "Segmentation fault, writing backtrace to " << backtrace_path () << endl;

  void *array[20];
  size_t size;
  
  // Get void*'s for all entries on the stack
  size = backtrace (array, 20);

  // Write entries to file (to be logged next time bibledit starts).
  int fd = open (backtrace_path ().c_str (), O_WRONLY|O_CREAT, 0666);
  backtrace_symbols_fd (array, size, fd);
  close (fd);

  exit (1);
}
#endif


int main (int argc, char **argv) 
{
  if (argc) {};
  if (argv[0]) {};
  
  // Ctrl-C initiates a clean shutdown sequence, so there's no memory leak.
  signal (SIGINT, sigint_handler);
  
#ifdef HAVE_EXECINFO
  // Handler for logging segmentation fault.
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

  // Log possible backtrace from a previous crash.
  string backtrace = filter_url_file_get_contents (backtrace_path ());
  filter_url_unlink (backtrace_path ());
  if (!backtrace.empty ()) {
    Database_Logs::log ("Backtrace of the last segmentation fault:");
    vector <string> lines = filter_string_explode (backtrace, '\n');
    for (auto & line : lines) {
      Database_Logs::log (line);
    }
  }

  // Bibledit Cloud should restart itself at midnight.
  // This is to be sure that any memory leaks don't accumulate too much
  // in case Bibledit Cloud runs for months and years.
  bibledit_set_quit_at_midnight ();
  
  // Keep running till Bibledit stops or gets interrupted.
  while (bibledit_is_running ())
  {
  };

  bibledit_shutdown_library ();

  return EXIT_SUCCESS;
}
