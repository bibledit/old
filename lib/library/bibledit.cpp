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


#include <config/libraries.h>
#include <webserver/webserver.h>
#include <library/bibledit.h>
#include <config/globals.h>
#include <filter/url.h>
#include <libxml/threads.h>
#include <thread>
#include <timer/index.h>
#include <config/logic.h>
#include <database/config/general.h>
#include <database/logs.h>
#include <setup/index.h>
#include <library/locks.h>
#include <curl/curl.h>


// Get Bibledit's version number.
const char * bibledit_get_version_number ()
{
  return config_logic_version ();
}


// Get the port number that Bibledit's web server listens on.
const char * bibledit_get_network_port ()
{
  return config_logic_network_port ();
}


// Get the preparation notice embedded html page.
const char * bibledit_get_preparation_notice ()
{
  return setup_installation_notice ();
}


// Set the root folder for the web server.
void bibledit_set_web_root (const char * directory)
{
  config_globals_document_root = directory;
}


// Sets whether the library considers any device that connects to be touch-enabled.
// This is necessary for on devices as clients which are always logged-in.
// The detection of touch-enabled devices happens during login,
// so when the login is skipped, the device is not detected.
// Therefore the calling program can preset touch-enabled here through this library call.
void bibledit_set_touch_enabled (bool enabled)
{
  // Set global variable for use elsewhere in the library.
  // A value of zero does nothing,
  // so set it greater than or smaller than zero to have effect.
  if (enabled) config_globals_touch_enabled = 1;
  else config_globals_touch_enabled = -1;

}


// Sets a global flag, so the library will quit at midnight.
void bibledit_set_quit_at_midnight ()
{
  Database_Config_General::setJustStarted (true);
  config_globals_quit_at_midnight = true;
}


// Set the timezone in hours as an offset to UTC.
void bibledit_set_timezone_hours_offset_utc (int hours)
{
  config_globals_timezone_offset_utc = hours;
}


// Initialize library.
// To be called once during the lifetime of the app.
void bibledit_initialize_library ()
{
  // Must initialize libcurl before any threads are started.
  curl_global_init (CURL_GLOBAL_ALL);
  
  // Thread locking.
  thread_setup ();
  
  // Initialize libxml2.
  xmlInitThreads ();
  xmlInitParser ();
}


// Start library.
// Can be called multiple times during the lifetime of the app
void bibledit_start_library ()
{
  // Set running flag.
  config_globals_running = true;
  // Run the web server in a thread.
  config_globals_worker = new thread (webserver);
  // Run the timers in a thread.
  config_globals_timer = new thread (timer_index);
}


// Returns true if Bibledit is running.
bool bibledit_is_running ()
{
  this_thread::sleep_for (chrono::milliseconds (10));
  return config_globals_running;
}


// Stop the library.
// Can be called multiple times during the lifetime of the app
void bibledit_stop_library ()
{
  // Clear running flag.
  config_globals_running = false;
  
  // Connect to localhost to initiate the shutdown mechanism in the running server.
  struct sockaddr_in sa;
  sa.sin_family = AF_INET;
  sa.sin_port = htons (stoi (config_logic_network_port ()));
  //sa.sin_addr.s_addr = inet_addr ("127.0.0.1");
  inet_pton (AF_INET, "127.0.0.1", &(sa.sin_addr));
  char str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(sa.sin_addr), str, INET_ADDRSTRLEN);
  memset(sa.sin_zero, '\0', sizeof (sa.sin_zero));
  int mysocket = socket (PF_INET, SOCK_STREAM, 0);
  connect (mysocket, (struct sockaddr*) &sa, sizeof (sa));
  
  // Wait till the server and the timers shut down.
  config_globals_worker->join ();
  config_globals_timer->join ();
  
  // Clear memory.
  delete config_globals_worker;
  delete config_globals_timer;
}


// Shut the library down.
// To be called exactly once during the lifetime of the app.
void bibledit_shutdown_library ()
{
  // Libxml2.
  xmlCleanupThreads();
  xmlCleanupParser();
  
  // Remove thread locks.
  thread_cleanup ();
}


// Puts an entry in the journal.
void bibledit_log (const char * message)
{
  Database_Logs::log (message);
}
