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


#include <filter/string.h>
#include <filter/url.h>
#include <filter/date.h>
#include <database/books.h>
#include <database/config/general.h>
#include <database/config/user.h>
#include <cstdlib>
#include <webserver/request.h>


const char * config_logic_config_folder ()
{
  return "config";
}


// Returns the Bibledit version number.
const char * config_logic_version ()
{
  return VERSION;
}


// Return the network port configured for the server.
const char * config_logic_http_network_port ()
{
  return NETWORK_PORT;
}


// Return the secure network port for the secure server.
int config_logic_https_network_port ()
{
  int port = convert_to_int (NETWORK_PORT);
  // The secure port is the plain http port plus one.
  port++;
  return port;
}


// Returns whether demo mode is enabled during configure.
bool config_logic_demo_enabled ()
{
  return (strcmp (DEMO, "yes") == 0);
}


// Returns the maximum number of parallel tasks to run.
int config_logic_max_parallel_tasks ()
{
  return PARALLEL_TASKS;
}


// Returns whether bare browser mode is enable during configure.
bool config_logic_bare_browser ()
{
  return (strcmp (BARE_BROWSER, "yes") == 0);
}


// The configured admin's username.
string config_logic_admin_username ()
{
  return ADMIN_USERNAME;
}


// The configured admin's password.
string config_logic_admin_password ()
{
  return ADMIN_PASSWORD;
}


// The configured admin's email.
string config_logic_admin_email ()
{
  return ADMIN_EMAIL;
}


// Replacement function for missing "stoi" on some platforms, like Cygwin and Android.
int my_stoi (const string& str, void * idx, int base)
{
#ifdef HAVE_STOI
  size_t * index = (size_t *) idx;
  return stoi (str, index, base);
#else
  char ** endptr = reinterpret_cast <char **> (idx);
  int i = strtol (str.c_str(), endptr, base);
  return i;
#endif
}


// Returns whether the interface is supposed to be in basic mode.
// When the mode was flipped, this used to expire after some hours.
// But there may be people working on a tablet,
// who would want to permanently switch to advanced mode, without this mode to expire.
// Therefore the mode flip switch is now permanent, till flipped again.
bool config_logic_basic_mode (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  // When this is a touch-enabled device, the basic mode will be on.
  bool basic_mode = request->session_logic ()->touchEnabled ();
  
  // Check whether to flip basic <> advanced mode.
  if (request->database_config_user ()->getFlipInterfaceMode ()) {
    basic_mode = !basic_mode;
  }
  
  return basic_mode;
}


// This returns the URL of Bibledit Cloud that faces the user.
string config_logic_site_url (void * webserver_request)
{
  // When the administrator has entered a fixed value for the user-facing URL, take that.
  // It overrides everything.
  string url = config_logic_manual_user_facing_url ();
  if (!url.empty ()) return url;
  
  // If a webserver request is passed, take the host from there.
  // The results is that in a situation where 192.168.2.6 is the same as localhost,
  // uses can connect from localhost and also from 192.168.2.6.
  // In the past there was a situation that the admin set up a central server for the whole team on his localhost.
  // Then team members that connected to 192.168.2.6 were forwarded to localhost (which of course failed).
  // This solution deals with that.
  if (webserver_request) {
    Webserver_Request * request = (Webserver_Request *) webserver_request;
    if (!request->host.empty ()) {
      url = get_base_url (request);
      return url;
    }
  }
  
  // No URL found yet.
  // This occurs during scheduled tasks that require the URL to add it to emails sent out.
  // Take the URL stored on login.
  url = Database_Config_General::getSiteURL ();
  return url;
}


// This returns the filtered value of file userfacingurl.conf.
string config_logic_manual_user_facing_url ()
{
#ifdef HAVE_CLIENT
  return "";
#else
  // Read the configuration file.
  string path = filter_url_create_root_path (config_logic_config_folder (), "userfacingurl.conf");
  string url = filter_url_file_get_contents (path);
  // Remove white space.
  url = filter_string_trim (url);
  // The previous file contained dummy text by default. Remove that.
  if (url.length () <= 6) url.clear ();
  // Ensure it ends with a slash.
  if (url.find_last_of ("/") != url.length () - 1) url.append ("/");
  // Done.
  return url;
#endif
}


// Returns the path to the secure server's private key.
string config_logic_server_key_path ()
{
  // Try the correct config file first.
  string path = filter_url_create_root_path ("config", "server.key");
  if (file_exists (path)) return path;
  // Try the file for localhost next.
  path = filter_url_create_root_path ("config", "local.server.key");
  if (file_exists (path)) return path;
  // Nothing found.
  return "";
}


// Returns the path to the secure server's public certificate.
string config_logic_server_certificate_path ()
{
  // Try the correct config file first.
  string path = filter_url_create_root_path ("config", "server.crt");
  if (file_exists (path)) return path;
  // Try the file for localhost next.
  path = filter_url_create_root_path ("config", "local.server.crt");
  if (file_exists (path)) return path;
  // Nothing found.
  return "";
}


// Returns the path to the secure server's chain of certificates of the signing authorities.
string config_logic_authorities_certificates_path ()
{
  // Try the correct config file first.
  string path = filter_url_create_root_path ("config", "authorities.crt");
  if (file_exists (path)) return path;
  // Try the file for localhost next.
  path = filter_url_create_root_path ("config", "local.authorities.crt");
  if (file_exists (path)) return path;
  // Nothing found.
  return "";
}


// Whether to enforce https traffic for browser communications.
bool config_logic_enforce_https_browser ()
{
  return file_exists (filter_url_create_root_path ("config", "browser.https"));
}


// Whether to enforce https traffic for client communications.
bool config_logic_enforce_https_client ()
{
  return file_exists (filter_url_create_root_path ("config", "client.https"));
}


void config_logic_swipe_enabled (void * webserver_request, string & script)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  string true_false = "false";
  if (request->session_logic ()->touchEnabled ()) {
    if (request->database_config_user ()->getSwipeActionsAvailable ()) {
      true_false = "true";
    }
  }
  
  script.append ("\n");
  script.append ("var swipe_operations = ");
  script.append (true_false);
  script.append (";");
}
