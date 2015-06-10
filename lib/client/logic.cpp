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


#include <client/logic.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <database/config/general.h>
#include <database/users.h>
#include <database/logs.h>
#include <sync/setup.h>
#include <config/logic.h>


// Returns whether Client mode is enabled.
bool client_logic_client_enabled ()
{
  if (!config_logic_client_prepared ()) return false;
  return Database_Config_General::getClientMode ();
}


// Sets the Client mode.
// $enable: boolean: true or false.
void client_logic_enable_client (bool enable)
{
  Database_Config_General::setClientMode (enable);
}


// Generates a URL for connecting to Bibledit Cloud.
// $address is the website.
// $port is the port number.
// $path is the path after the website.
string client_logic_url (string address, int port, string path)
{
  return address + ":" + convert_to_string (port) + "/" + path;
}


// This function does the initial connection from the client to the server.
// It receives settings from the server and applies them to the client.
// It returns the level of the user.
// It returns an empty string in case of failure or the response from the server.
string client_logic_connection_setup (string user, string hash)
{
  if (user.empty ()) {
    Database_Users database_users = Database_Users ();
    vector <string> users = database_users.getUsers ();
    if (users.empty()) return "";
    user = users [0];
    hash = database_users.getmd5 (user);
  }
  
  string encoded_user = bin2hex (user);
  
  string address = Database_Config_General::getServerAddress ();
  int port = Database_Config_General::getServerPort ();
  
  string url = client_logic_url (address, port, sync_setup_url ()) + "?user=" + encoded_user + "&pass=" + hash;
  
  string error;
  string response = filter_url_http_get (url, error);
  int iresponse = convert_to_int (response);
  
  if ((iresponse >= Filter_Roles::guest ()) && (iresponse <= Filter_Roles::admin ())) {
    // Set user's role on the client to be the same as on the server.
    Database_Users database_users = Database_Users ();
    database_users.updateUserLevel (user, iresponse);
  } else {
    Database_Logs::log (error, Filter_Roles::translator ());
  }
  
  if (response.empty ()) response = error;
  return response;
}


string client_logic_create_note_encode (string bible, int book, int chapter, int verse,
                                        string summary, string contents, bool raw)
{
  vector <string> data;
  data.push_back (bible);
  data.push_back (convert_to_string (book));
  data.push_back (convert_to_string (chapter));
  data.push_back (convert_to_string (verse));
  data.push_back (summary);
  data.push_back (convert_to_string (raw));
  data.push_back (contents);
  return filter_string_implode (data, "\n");
}


void client_logic_create_note_decode (string data,
                                      string& bible, int& book, int& chapter, int& verse,
                                      string& summary, string& contents, bool& raw)
{
  vector <string> lines = filter_string_explode (data, '\n');
  if (!lines.empty ()) {
    bible = lines [0];
    lines.erase (lines.begin());
  }
  if (!lines.empty ()) {
    book = convert_to_int (lines [0]);
    lines.erase (lines.begin());
  }
  if (!lines.empty ()) {
    chapter = convert_to_int (lines [0]);
    lines.erase (lines.begin());
  }
  if (!lines.empty ()) {
    verse = convert_to_int (lines [0]);
    lines.erase (lines.begin());
  }
  if (!lines.empty ()) {
    summary = lines [0];
    lines.erase (lines.begin());
  }
  if (!lines.empty ()) {
    raw = convert_to_bool (lines [0]);
    lines.erase (lines.begin());
  }
  contents = filter_string_implode (lines, "\n");
}


