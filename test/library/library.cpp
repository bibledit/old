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


#include <libraries.h>
#include <libxml/threads.h>
#include <curl/curl.h>


// Get Bibledit's version number.
const char * bibledit_get_version_number ()
{
  return "version";
}


// Get the port number that Bibledit's web server listens on.
const char * bibledit_get_network_port ()
{
  return "8080";
}


// Get the preparation notice embedded html page.
const char * bibledit_get_preparation_notice ()
{
  return "setup_installation_notice";
}


// Set the root folder for the web server.
void bibledit_set_web_root (const char * directory)
{
    cout << directory << endl;
}


void bibledit_set_touch_enabled (bool enabled)
{
    cout << enabled << endl;
}


// Sets a global flag, so the library will quit at midnight.
void bibledit_set_quit_at_midnight ()
{
}


// Set the timezone in hours as an offset to UTC.
void bibledit_set_timezone_hours_offset_utc (int hours)
{
    cout << hours << endl;
}


// Initialize library.
// To be called once during the lifetime of the app.
void bibledit_initialize_library ()
{
  // Must initialize libcurl before any threads are started.
  curl_global_init (CURL_GLOBAL_ALL);
  cout << curl_version () << endl;

  
  // Initialize libxml2.
  xmlInitThreads ();
  xmlInitParser ();
}


// Start library.
// Can be called multiple times during the lifetime of the app
void bibledit_start_library ()
{
}


// Returns true if Bibledit is running.
bool bibledit_is_running ()
{
    return true;
}


// Stop the library.
// Can be called multiple times during the lifetime of the app
void bibledit_stop_library ()
{
}


// Shut the library down.
// To be called exactly once during the lifetime of the app.
void bibledit_shutdown_library ()
{
  xmlCleanupThreads();
  xmlCleanupParser();
}


// Puts an entry in the journal.
void bibledit_log (const char * message)
{
    cout << message << endl;
}
