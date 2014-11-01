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
#include <thread>


using namespace std;


// Set the root folder for the web server.
void bibledit_root (string directory)
{
  config_globals_document_root = directory;
}


// Start the Bibledit server.
void bibledit_start () 
{
  // Run the web server in a thread.
  config_globals_worker = new thread (webserver);
}


// Returns true if Bibledit is running.
bool bibledit_running ()
{
  // Moved to C++: usleep (10000);
  this_thread::sleep_for (chrono::milliseconds (100)); // Todo back to 10
  return config_globals_running;
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
  //sa.sin_addr.s_addr = inet_addr ("127.0.0.1");
  inet_pton (AF_INET, "127.0.0.1", &(sa.sin_addr));
  char str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(sa.sin_addr), str, INET_ADDRSTRLEN);
  memset(sa.sin_zero, '\0', sizeof (sa.sin_zero));
  int mysocket = socket (PF_INET, SOCK_STREAM, 0);
  connect (mysocket, (struct sockaddr*) &sa, sizeof (sa));
  
  // Wait till the server shuts down.
  config_globals_worker->join ();
  
  // Clear memory.
#ifdef WIN32
#else
  xmlCleanupThreads();
#endif
  delete config_globals_worker;

}
