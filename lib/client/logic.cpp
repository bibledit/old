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
#include <database/config/general.h>


// Returns whether Client mode is enabled.
bool client_logic_client_enabled ()
{
  return Database_Config_General::getClientMode ();
}


// Sets the Client mode.
// $enable: boolean: true or false.
void client_logic_enable_client (bool enable)
{
  Database_Config_General::setClientMode (enable);
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



/* Todo
class Filter_Client // Todo
{
  
  
  // This function does the initial connection from the client to the server.
  // It receives settings from the server and applies them to the client.
  // It returns the level of the user.
  // It returns false in case of failure.
  public static function setup ($user = "", $hash = "")
  {
    $database_config_general = Database_Config_General::getInstance ();
    $database_users = Database_Users::getInstance ();
    
    if ($user == "") {
      $users = $database_users->getUsers ();
      if (empty ($users)) return false;
      $user = $users [0];
      $hash = $database_users->getmd5 ($user);
    }
    
    $encoded_user = bin2hex ($user);
    
    $address = Database_Config_General::getServerAddress ();
    
    $url = "$address/sync/setup?user=$encoded_user&pass=$hash";
    
    @$response = filter_url_file_get_contents ($url);
    if (($response >= Filter_Roles::guest ()) && ($response <= Filter_Roles::admin ())) {
      // Set user's role on the client to be the same as on the server.
      $database_users->updateUserLevel ($user, $response);
    }
    
    return $response;
  }
  
  
 
}
*/
