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


#include <config/libraries.h>
#include <webserver/webserver.h>
#include <library/bibledit.h>
#include <config/globals.h>
#include <filter/url.h>
#include <filter/string.h>
#include <thread>
#include <timer/index.h>
#include <database/config/general.h>
#include <database/logs.h>
#include <setup/index.h>
#include <setup/logic.h>
#include <library/locks.h>
#ifdef HAVE_CLIENT
#else
#include <curl/curl.h>
#endif
#include <sendreceive/logic.h>


bool bibledit_started = false;


// Get Bibledit's version number.
const char * bibledit_get_version_number ()
{
  return config_logic_version ();
}



// Get the port number that Bibledit's web server listens on.
const char * bibledit_get_network_port ()
{
  return config_logic_http_network_port ();
}


// Initialize library.
// To be called once during the lifetime of the app.
// $package: The folder where the package data resides.
// $webroot: The document root folder for the web server.
void bibledit_initialize_library (const char * package, const char * webroot)
{
  // Must initialize libcurl before any threads are started.
#ifdef HAVE_CLIENT
#else
  curl_global_init (CURL_GLOBAL_ALL);
#endif
  
  // Thread locking.
  thread_setup ();
  
  // Initialize SQLite: Full thread safety: https://www.sqlite.org/threadsafe.html.
  // This is supported to prevent "database locked" errors.
  sqlite3_config (SQLITE_CONFIG_SERIALIZED);

  // Binary file mode on Windows.
#ifdef HAVE_VISUALSTUDIO
  _set_fmode (_O_BINARY);
#endif

  // Set the web root folder.
  config_globals_document_root = webroot;
  
  // Initialize SSL/TLS (after webroot has been set).
  filter_url_ssl_tls_initialize ();
  
  // Initialize data in a thread.
  thread setup_thread = thread (setup_conditionally, package);
  setup_thread.detach ();
  
  // Multiple start/stop guard.
  bibledit_started = false;
}


// Sets whether the library considers any device that connects to be touch-enabled.
// This is necessary for client devices which are always logged-in.
// The detection of touch-enabled devices happens during login,
// so when the login is skipped, the device is not detected.
// Therefore the calling program can preset touch-enabled here through this library call.
void bibledit_set_touch_enabled (bool enabled)
{
  // Set global variable for use elsewhere in the library.
  // A value of zero does nothing,
  // so set it greater than or smaller than zero to have effect.
  if (enabled) {
    config_globals_touch_enabled = 1;
  }
  else {
    config_globals_touch_enabled = -1;
  }
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


// Start library.
// Can be called multiple times during the lifetime of the app
void bibledit_start_library ()
{
  // Repeating start guard.
  if (bibledit_started) return;
  bibledit_started = true;

  // Setup server behaviour.
#ifdef HAVE_CLIENT
  config_globals_client_prepared = true;
#else
  config_globals_client_prepared = false;
#endif
  if (config_logic_demo_enabled ()) {
    config_globals_open_installation = true;
  }
  
  // Ignore SIGPIPE signal on Linux: When the browser cancels the request, it won't kill Bibledit.
  // On Windows, this is not needed.
#ifndef HAVE_VISUALSTUDIO
  signal (SIGPIPE, SIG_IGN);
#endif
  
  // Set running flag.
  config_globals_http_running = true;
  config_globals_https_running = true;
  
  // Whether the plain http server redirects to secure http.
  config_globals_enforce_https_browser = config_logic_enforce_https_browser ();
  config_globals_enforce_https_client = config_logic_enforce_https_client ();
  
  // Run the plain web server in a thread.
  config_globals_http_worker = new thread (http_server);
  
  // Run the secure web server in a thread.
  config_globals_https_worker = new thread (https_server);
  
  // Run the timers in a thread.
  config_globals_timer = new thread (timer_index);
  
  // Client should sync right after wake up.
  sendreceive_queue_startup ();
}


// Gets the last page that was opened via the menu.
const char * bibledit_get_last_page ()
{
  string href = Database_Config_General::getLastMenuClick ();
  return href.c_str();
}


// Returns true if Bibledit is running.
bool bibledit_is_running ()
{
  this_thread::sleep_for (chrono::milliseconds (10));
  if (config_globals_http_running) return true;
  if (config_globals_https_running) return true;
  return false;
}


// Returns a non-empty string if the client is synchronizing with or downloading from Bibledit Cloud.
const char * bibledit_is_synchronizing ()
{
  // If any of the sync tasks is running, the function considers bibledit to be synchronizing.
  // On a bad network, it may happen that a task gets stuck.
  // During the time that the task is stuck, till the watchdog kicks in,
  // the sync is considered to be running.
  // When mobile devices use this API call,
  // the devices will remain awake during the time the task is stuck.
  // The user may then have to manually put the device on standby.
  bool syncing = false;
  if (config_globals_syncing_bibles) syncing = true;
  if (config_globals_syncing_changes) syncing = true;
  if (config_globals_syncing_notes) syncing = true;
  if (config_globals_syncing_settings) syncing = true;
  if (config_globals_syncing_files) syncing = true;
  if (config_globals_syncing_resources) syncing = true;
  if (syncing) return "true";
  return "false";
}


// Stop the library.
// Can be called multiple times during the lifetime of the app
void bibledit_stop_library ()
{
  // Repeating stop guard.
  if (!bibledit_started) return;
  bibledit_started = false;

  // Clear running flag.
  config_globals_http_running = false;
  config_globals_https_running = false;
  
  // Connect to localhost to initiate the shutdown mechanism in the running server.
  string url = "http://localhost:";
  url.append (config_logic_http_network_port ());
  string error;
  filter_url_http_get (url, error, false);
  
  // Connect to the secure server to initiate its shutdown mechanism.
#ifndef HAVE_CLIENT
  url = "https://localhost:";
  url.append (convert_to_string (config_logic_https_network_port ()));
  filter_url_http_get (url, error, false);
  // Let the connection start, then close it.
  // The server will then abort the TLS handshake, and shut down.
  this_thread::sleep_for (chrono::milliseconds (1));
#endif
  
  // Wait till the servers and the timers shut down.
  config_globals_http_worker->join ();
  config_globals_https_worker->join ();
  config_globals_timer->join ();
  
  // Clear memory.
  delete config_globals_http_worker;
  delete config_globals_https_worker;
  delete config_globals_timer;
}


// Shut the library down.
// To be called exactly once during the lifetime of the app.
void bibledit_shutdown_library ()
{
  // Remove thread locks.
  thread_cleanup ();
  
  // Finalize SSL/TLS.
  filter_url_ssl_tls_finalize ();

  // Multiple start/stop guard.
  bibledit_started = false;
}


// Puts an entry in the journal.
void bibledit_log (const char * message)
{
  Database_Logs::log (message);
}
