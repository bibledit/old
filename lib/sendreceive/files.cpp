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


#include <sendreceive/files.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/merge.h>
#include <filter/date.h>
#include <tasks/logic.h>
#include <config/logic.h>
#include <client/logic.h>
#include <locale/translate.h>
#include <webserver/request.h>
#include <sync/logic.h>
#include <sync/files.h>
#include <checksum/logic.h>
#include <database/logs.h>
#include <database/config/general.h>


int sendreceive_files_watchdog = 0;


void sendreceive_files_done ()
{
  sendreceive_files_watchdog = 0;
}


string sendreceive_files_text ()
{
  return translate("Fies") + ": ";
}


string sendreceive_files_sendreceive_text ()
{
  return sendreceive_files_text () + translate ("Synchronizing");
}


string sendreceive_files_up_to_date_text ()
{
  return sendreceive_files_text () + translate ("Up to date");
}


void sendreceive_files ()
{
  if (sendreceive_files_watchdog) {
    int time = filter_date_seconds_since_epoch ();
    if (time < (sendreceive_files_watchdog + 900)) {
      Database_Logs::log (sendreceive_files_text () + translate("Still busy"), Filter_Roles::translator ());
      return;
    }
    Database_Logs::log (sendreceive_files_text () + translate("Watchdog timeout"), Filter_Roles::translator ());
  }
  sendreceive_files_kick_watchdog ();
  
  
  Webserver_Request request;
  Sync_Logic sync_logic = Sync_Logic (&request);

  
  Database_Logs::log (sendreceive_files_sendreceive_text (), Filter_Roles::translator ());
  
  
  string address = Database_Config_General::getServerAddress ();
  int port = Database_Config_General::getServerPort ();
  string url = client_logic_url (address, port, sync_files_url ());
  
  
  int version = 1;
  map <string, string> post;
  post ["v"] = convert_to_string (version);
  string error;
  string response;
  int iresponse = 0;
  

  // Request the checksum of all relevant files on the server.
  // Compare it with the local checksum for the same set of files.
  // If the two match: Ready.
  post ["a"] = convert_to_string (Sync_Logic::files_total_checksum);
  response = sync_logic.post (post, url, error);
  if (!error.empty ()) {
    Database_Logs::log (sendreceive_files_text () + "Failure requesting total checksum: " + error, Filter_Roles::translator ());
    sendreceive_files_done ();
    return;
  }
  iresponse = convert_to_int (response);
  int checksum = Sync_Logic::files_get_total_checksum (version);
  cout << "server/client total checksum " << iresponse << " " << checksum << endl; // Todo
  if (iresponse == checksum) {
    Database_Logs::log (sendreceive_files_up_to_date_text (), Filter_Roles::translator ());
    sendreceive_files_done ();
    return;
  }

  
  // Go through the relevant directories relevant to the version.
  vector <string> directories = Sync_Logic::files_get_directories (version);
  for (size_t d = 0; d < directories.size (); d++) {
    

    // Not the directory name itself is posted to the server,
    // but rather the offset of the directory in the entire list.
    post ["d"] = convert_to_string (d);
    string directory = directories [d];
    

    // Request the total checksum of a directory on the server.
    // Compare it with the local checksum for the directory on the client.
    // If the two match: This directory is ready.
    post ["a"] = convert_to_string (Sync_Logic::files_directory_checksum);
    response = sync_logic.post (post, url, error);
    if (!error.empty ()) {
      Database_Logs::log (sendreceive_files_text () + "Failure requesting directory checksum: " + error, Filter_Roles::translator ());
      continue;
    }
    iresponse = convert_to_int (response);
    int checksum = Sync_Logic::files_get_directory_checksum (directory);
    cout << "server/client directory checksum " << directory << " " << iresponse << " " << checksum << endl; // Todo
    if (iresponse == checksum) {
      Database_Logs::log (sendreceive_files_up_to_date_text (), Filter_Roles::translator ());
      continue;
    }

    
    // Retrieve the list of files in a directory on the server.
    post ["a"] = convert_to_string (Sync_Logic::files_directory_files);
    response = sync_logic.post (post, url, error);
    if (!error.empty ()) {
      Database_Logs::log (sendreceive_files_text () + "Failure requesting directory files: " + error, Filter_Roles::translator ());
      continue;
    }
    vector <string> server_files = filter_string_explode (response, '\n');

    
    // Delete files that exist locally but not on the server.
    vector <string> client_files = Sync_Logic::files_get_files (directory);
    vector <string> files = filter_string_array_diff (client_files, server_files);
    for (auto file : files) {
      string path = filter_url_create_root_path (file);
      filter_url_unlink (path);
    }
    

    
  }
  
  
  /* Todo

  
  
    // Deal with each file individually.
    for (auto & file : server_files) {
      
      
      // Request checksum of this file,
      // compare it with the local checksum,
      // and skip the file if the checksums match.
      post ["a"] = convert_to_string (Sync_Logic::offlineresources_get_file_checksum);
      post ["r"] = resource;
      post ["f"] = file;
      response = sync_logic.post (post, url, error);
      if (!error.empty ()) {
        Database_Logs::log (sendreceive_files_text () + "Failure requesting checksum file: " + error, Filter_Roles::translator ());
        sendreceive_files_done ();
        return;
      }
      checksum = Sync_Logic::offline_resource_file_checksum (resource, file);
      if (response == checksum) {
        continue;
      }

      
      sendreceive_files_kick_watchdog ();

      
      // Download the file from the server, and store it locally on the client.
      Database_Logs::log (sendreceive_files_text () + "Downloading " + resource + " " + file, Filter_Roles::translator ());
      // Create directory by saving empty file.
      database_offlineresources.save (resource, file, "");
      // Obtain url for file to download from server.
      post ["a"] = convert_to_string (Sync_Logic::offlineresources_get_file_filename);
      post ["r"] = resource;
      post ["f"] = file;
      response = sync_logic.post (post, url, error);
      if (!error.empty ()) {
        Database_Logs::log (sendreceive_files_text () + "Failure downloading file: " + error, Filter_Roles::translator ());
        sendreceive_files_done ();
        return;
      }
      string url = client_logic_url (address, port, response);
      // Local file path where to save resource.
      string filepath = database_offlineresources.filepath (resource, file);
      // Download and save file locally.
      filter_url_download_file (url, filepath, error);
      if (!error.empty ()) {
        Database_Logs::log (sendreceive_files_text () + "Failure downloading file: " + error, Filter_Roles::translator ());
        sendreceive_files_done ();
        return;
      }
    }

   */
  
  // Done.
  Database_Logs::log (sendreceive_files_text () + "Now up to date", Filter_Roles::translator ());
  sendreceive_files_done ();
}


void sendreceive_files_kick_watchdog ()
{
  sendreceive_files_watchdog = filter_date_seconds_since_epoch ();
}
