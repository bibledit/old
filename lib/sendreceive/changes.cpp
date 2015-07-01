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


#include <sendreceive/changes.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <tasks/logic.h>
#include <config/logic.h>
#include <database/config/general.h>
#include <database/modifications.h>
#include <database/logs.h>
#include <client/logic.h>
#include <locale/translate.h>
#include <webserver/request.h>
#include <sync/logic.h>
#include <sync/changes.h>
#include <checksum/logic.h>


mutex mutex_sendreceive_changes;
bool sendreceive_changes_running = false;


void send_receive_changes_done ()
{
  mutex_sendreceive_changes.lock ();
  sendreceive_changes_running = false;
  mutex_sendreceive_changes.unlock ();
}


string sendreceive_changes_text ()
{
  return translate("Changes") + ": ";
}


string sendreceive_changes_sendreceive_text ()
{
  return sendreceive_changes_text () + translate ("Send/receive");
}


string sendreceive_changes_up_to_date_text ()
{
  return sendreceive_changes_text () + translate ("Up to date");
}


void sendreceive_changes ()
{
  mutex_sendreceive_changes.lock ();
  bool bail_out = sendreceive_changes_running;
  mutex_sendreceive_changes.unlock ();
  if (bail_out) return;
  mutex_sendreceive_changes.lock ();
  sendreceive_changes_running = true;
  mutex_sendreceive_changes.unlock ();

  
  Database_Logs::log (sendreceive_changes_sendreceive_text (), Filter_Roles::translator ());
  

  Webserver_Request request;
  Sync_Logic sync_logic = Sync_Logic (&request);
  
  
  string response = client_logic_connection_setup ();
  int iresponse = convert_to_int (response);
  if (iresponse < Filter_Roles::guest () || iresponse > Filter_Roles::admin ()) {
    Database_Logs::log (sendreceive_changes_text () + translate("Failure to initiate connection"), Filter_Roles::translator ());
    send_receive_changes_done ();
    return;
  }
  
  
  // Set the correct user in the session: The sole user on the Client.
  vector <string> users = request.database_users ()->getUsers ();
  if (users.empty ()) {
    Database_Logs::log (translate("No user found"), Filter_Roles::translator ());
    send_receive_changes_done ();
    return;
  }
  string user = users [0];
  request.session_logic ()->setUsername (user);
  string password = request.database_users ()->getmd5 (user);
  
  
  // The basic request to be POSTed to the server.
  // It contains the user's credentials.
  map <string, string> post;
  post ["u"] = bin2hex (user);
  post ["p"] = password;
  post ["l"] = convert_to_string (request.database_users ()->getUserLevel (user));
  
  
  // Error variables.
  string error;
  bool communication_errors = false;
  
  
  // Server URL to call.
  string address = Database_Config_General::getServerAddress ();
  int port = Database_Config_General::getServerPort ();
  string url = client_logic_url (address, port, sync_changes_url ());
  
  
  // Send the removed change notifications to the server. Todo
  vector <int> ids = request.database_config_user ()->getRemovedChanges ();
  
  
  // Compare the total checksum for the change notifications for the active user on client and server.
  // Take actions based on that.
  string client_checksum = Sync_Logic::changes_checksum (user);
  string server_checksum;
  post ["a"]  = convert_to_string (Sync_Logic::changes_get_checksum);
  response = sync_logic.post (post, url, error);
  if (!error.empty ()) {
    communication_errors = true;
    Database_Logs::log (sendreceive_changes_text () + "Failure receiving checksum: " + error, Filter_Roles::translator ());
  }
  else {
    server_checksum = response;
  }
  if (client_checksum == server_checksum) {
    Database_Logs::log (sendreceive_changes_up_to_date_text (), Filter_Roles::translator ()); // Todo test.
    send_receive_changes_done ();
    return;
  }
  //cout << "client/server checksums: " << client_checksum << " " << server_checksum << endl; // Todo
  
  
  Database_Modifications database_modifications;
  
  
  // Get all identifiers for the notifications on the server for the user.
  // Get the identifiers on the client.
  // Any identifiers on the client, but not on the server, remove them from the client.
  vector <int> client_identifiers = database_modifications.getNotificationIdentifiers (user, false);
  vector <int> server_identifiers;
  post ["a"]  = convert_to_string (Sync_Logic::changes_get_identifiers);
  response = sync_logic.post (post, url, error);
  if (!error.empty ()) {
    communication_errors = true;
    Database_Logs::log (sendreceive_changes_text () + "Failure receiving identifiers: " + error, Filter_Roles::translator ());
  }
  else {
    vector <string> ids = filter_string_explode (response, '\n');
    for (auto & id : ids) server_identifiers.push_back (convert_to_int (id));
  }
  cout << "client/server ids: " << client_identifiers.size () << " " << server_identifiers.size () << endl; // Todo
  
  
  
  // Done.
  Database_Logs::log (sendreceive_changes_text () + "Ready", Filter_Roles::translator ());
  send_receive_changes_done ();
}
