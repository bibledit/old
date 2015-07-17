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


#include <sendreceive/usfmresources.h>
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
#include <sync/usfmresources.h>
#include <checksum/logic.h>
#include <database/logs.h>
#include <database/usfmresources.h>
#include <database/config/general.h>
#include <database/books.h>


int sendreceive_usfmresources_watchdog = 0;


void sendreceive_usfmresources_done ()
{
  sendreceive_usfmresources_watchdog = 0;
}


string sendreceive_usfmresources_text ()
{
  return translate("USFM resources") + ": ";
}


string sendreceive_usfmresources_sendreceive_text ()
{
  return sendreceive_usfmresources_text () + translate ("Synchronizing");
}


string sendreceive_usfmresources_up_to_date_text ()
{
  return sendreceive_usfmresources_text () + translate ("Up to date");
}


void sendreceive_usfmresources ()
{
  if (sendreceive_usfmresources_watchdog) {
    int time = filter_date_seconds_since_epoch ();
    if (time < (sendreceive_usfmresources_watchdog + 900)) {
      Database_Logs::log (sendreceive_usfmresources_text () + translate("Still busy"), Filter_Roles::translator ());
      return;
    }
    Database_Logs::log (sendreceive_usfmresources_text () + translate("Watchdog timeout"), Filter_Roles::translator ());
  }
  sendreceive_usfmresources_kick_watchdog ();
  
  
  Database_UsfmResources database_usfmresources = Database_UsfmResources ();
  Webserver_Request request;
  Sync_Logic sync_logic = Sync_Logic (&request);

  
  Database_Logs::log (sendreceive_usfmresources_sendreceive_text (), Filter_Roles::translator ());
  
 
  string address = Database_Config_General::getServerAddress ();
  int port = Database_Config_General::getServerPort ();
  string url = client_logic_url (address, port, sync_usfmresources_url ());

  
  map <string, string> post;
  string error;
  string response;
  
  
  // Request the checksum of all USFM resources from the server.
  // Compare it with the local checksum.
  // If the two match: Ready.
  post ["a"] = convert_to_string (Sync_Logic::usfmresources_get_total_checksum);
  response = sync_logic.post (post, url, error);
  if (!error.empty ()) {
    Database_Logs::log (sendreceive_usfmresources_text () + "Failure getting total checksum: " + error, Filter_Roles::translator ());
    sendreceive_usfmresources_done ();
    return;
  }
  string checksum = Sync_Logic::usfm_resources_checksum ();
  if (response == checksum) {
    Database_Logs::log (sendreceive_usfmresources_up_to_date_text (), Filter_Roles::translator ());
    sendreceive_usfmresources_done ();
    return;
  }
  
  
  // Request a list of all USFM resources available on the server.
  post ["a"] = convert_to_string (Sync_Logic::usfmresources_get_resources);
  response = sync_logic.post (post, url, error);
  if (!error.empty ()) {
    Database_Logs::log (sendreceive_usfmresources_text () + "Failure getting resources: " + error, Filter_Roles::translator ());
    sendreceive_usfmresources_done ();
    return;
  }
  vector <string> server_resources = filter_string_explode (response, '\n');
  
  
  // Delete any resource on the local client but not on the server.
  vector <string> client_resources = database_usfmresources.getResources ();
  vector <string> resources = filter_string_array_diff (client_resources, server_resources);
  for (auto & resource : resources) {
    database_usfmresources.deleteResource (resource);
  }
  
  
  // Deal with each USFM resource individually.
  for (auto & resource : server_resources) {
    
    
    // Request the checksum of the resources from the server.
    // Compare it with the checksum of the local resource.
    // If they match: Go to the next resource.
    post ["a"] = convert_to_string (Sync_Logic::usfmresources_get_resource_checksum);
    post ["r"] = resource;
    response = sync_logic.post (post, url, error);
    if (!error.empty ()) {
      Database_Logs::log (sendreceive_usfmresources_text () + "Failure getting checksum of resource: " + error, Filter_Roles::translator ());
      sendreceive_usfmresources_done ();
      return;
    }
    checksum = Sync_Logic::usfm_resource_checksum (resource);
    if (response == checksum) {
      continue;
    }
    

    // Request a list of all books in the resource on the server.
    post ["a"] = convert_to_string (Sync_Logic::usfmresources_get_books);
    post ["r"] = resource;
    response = sync_logic.post (post, url, error);
    if (!error.empty ()) {
      Database_Logs::log (sendreceive_usfmresources_text () + "Failure getting books of resource: " + error, Filter_Roles::translator ());
      sendreceive_usfmresources_done ();
      return;
    }
    vector <int> server_books;
    vector <string> sbooks = filter_string_explode (response, '\n');
    for (auto & book : sbooks) server_books.push_back (convert_to_int (book));

    
    // Delete any books from the client that are not on the server.
    vector <int> client_books = database_usfmresources.getBooks (resource);
    vector <int> books = filter_string_array_diff (client_books, server_books);
    for (auto & book : books) {
      database_usfmresources.deleteBook (resource, book);
    }
    
    
    // Deal with each book individually.
    for (auto & book : server_books) {
      
      
      // Request checksum of this book,
      // compare it with the local checksum,
      // and skip the book if the checksums match.
      post ["a"] = convert_to_string (Sync_Logic::usfmresources_get_book_checksum);
      post ["r"] = resource;
      post ["b"] = convert_to_string (book);
      response = sync_logic.post (post, url, error);
      if (!error.empty ()) {
        Database_Logs::log (sendreceive_usfmresources_text () + "Failure getting checksum of resource book: " + error, Filter_Roles::translator ());
        sendreceive_usfmresources_done ();
        return;
      }
      checksum = Sync_Logic::usfm_resource_book_checksum (resource, book);
      if (response == checksum) {
        continue;
      }
      
      
      string bookname = Database_Books::getEnglishFromId (book);
      Database_Logs::log (sendreceive_usfmresources_text () + "Synchronizing " + resource + " " + bookname, Filter_Roles::translator ());
      
      
      // Retrieve a list of chapters in the $book from the server.
      post ["a"] = convert_to_string (Sync_Logic::usfmresources_get_chapters);
      post ["r"] = resource,
      post ["b"] = convert_to_string (book);
      response = sync_logic.post (post, url, error);
      if (!error.empty ()) {
        Database_Logs::log (sendreceive_usfmresources_text () + "Failure getting chapters of resource book: " + error, Filter_Roles::translator ());
        sendreceive_usfmresources_done ();
        return;
      }
      vector <int> server_chapters;
      vector <string> schapters = filter_string_explode (response, '\n');
      for (auto & chapter : schapters) server_chapters.push_back (convert_to_int (chapter));
      
      
      // Delete local chapters not found on the server.
      vector <int> client_chapters = database_usfmresources.getChapters (resource, book);
      vector <int> chapters = filter_string_array_diff (client_chapters, server_chapters);
      for (auto & chapter : chapters) {
        database_usfmresources.deleteChapter (resource, book, chapter);
      }
      
      
      // Go through each chapter individually.
      for (auto & chapter : server_chapters) {

        
        sendreceive_usfmresources_kick_watchdog ();

        
        // Get the checksum of the chapter as it is on the server.
        post ["a"] = convert_to_string (Sync_Logic::usfmresources_get_chapter_checksum);
        post ["r"] = resource;
        post ["b"] = convert_to_string (book);
        post ["c"] = convert_to_string (chapter);
        response = sync_logic.post (post, url, error);
        if (!error.empty ()) {
          Database_Logs::log (sendreceive_usfmresources_text () + "Failure getting checksum of resource chapter: " + error, Filter_Roles::translator ());
          sendreceive_usfmresources_done ();
          return;
        }
        checksum = Sync_Logic::usfm_resource_chapter_checksum (resource, book, chapter);
        if (response == checksum) {
          continue;
        }
        
        
        // Download the chapter from the server, and store it locally on the client.
        post ["a"] = convert_to_string (Sync_Logic::usfmresources_get_chapter);
        post ["r"] = resource;
        post ["b"] = convert_to_string (book);
        post ["c"] = convert_to_string (chapter);
        response = sync_logic.post (post, url, error);
        if (!error.empty ()) {
          Database_Logs::log (sendreceive_usfmresources_text () + "Failure downloading resource chapter: " + error, Filter_Roles::translator ());
          sendreceive_usfmresources_done ();
          return;
        }
        database_usfmresources.storeChapter (resource, book, chapter, response);
      }
    }
  }

  
  // Done.
  Database_Logs::log (sendreceive_usfmresources_text () + "Now up to date", Filter_Roles::translator ());
  sendreceive_usfmresources_done ();
}


void sendreceive_usfmresources_kick_watchdog ()
{
  sendreceive_usfmresources_watchdog = filter_date_seconds_since_epoch ();
}
