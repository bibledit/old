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


#include <sendreceive/resources.h>
#include <sendreceive/logic.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/date.h>
#include <tasks/logic.h>
#include <config/logic.h>
#include <database/config/general.h>
#include <database/logs.h>
#include <database/versifications.h>
#include <database/offlineresources.h>
#include <database/usfmresources.h>
#include <database/books.h>
#include <database/cache.h>
#include <client/logic.h>
#include <locale/translate.h>
#include <webserver/request.h>
#include <sync/logic.h>
#include <sync/resources.h>
#include <sword/logic.h>
#include <demo/logic.h>


int sendreceive_resources_watchdog = 0;


void sendreceive_resources_kick_watchdog ()
{
  sendreceive_resources_watchdog = filter_date_seconds_since_epoch ();
}


void sendreceive_resources_done ()
{
  sendreceive_resources_watchdog = 0;
}


bool sendreceive_resources_interrupt = false;


void sendreceive_resources ()
{
  if (sendreceive_resources_watchdog) {
    int time = filter_date_seconds_since_epoch ();
    if (time < (sendreceive_resources_watchdog + 900)) {
      return;
    }
    Database_Logs::log ("Resources: " + translate("Watchdog timeout"), Filter_Roles::translator ());
    sendreceive_resources_done ();
  }

  // If any of the prioritized synchronization tasks run, postpone the current task and do not start it.
  if (sendreceive_logic_prioritized_task_is_active ()) {
    sendreceive_resources_done ();
    this_thread::sleep_for (chrono::seconds (5));
    tasks_logic_queue (SYNCRESOURCES);
    return;
  }

  sendreceive_resources_interrupt = false;
  bool send_receive_priority_tasks_running = false;

  // If there's nothing to cache, bail out.
  vector <string> resources = Database_Config_General::getResourcesToCache ();
  if (resources.empty ()) return;
  
  sendreceive_resources_kick_watchdog ();

  // Error counter.
  int error_count = 0;
  
  // Resource to cache.
  string resource = resources [0];
  
  // Erase the two older storage locations that were used to cache resources in the earlier stages of Bibledit.
  {
    Database_OfflineResources database_offlineresources;
    Database_UsfmResources database_usfmresources;
    database_offlineresources.erase (resource);
    database_usfmresources.deleteResource (resource);
  }

  Database_Logs::log ("Starting to download resource:" " " + resource, Filter_Roles::consultant ());
  Database_Cache::create (resource);
  
  Database_Versifications database_versifications;
  
  vector <int> books = database_versifications.getMaximumBooks ();
  for (auto & book : books) {
    if (sendreceive_logic_prioritized_task_is_active ()) send_receive_priority_tasks_running = true;
    if (send_receive_priority_tasks_running) continue;
    if (sendreceive_resources_interrupt) continue;
    
    string bookName = Database_Books::getEnglishFromId (book);
    
    vector <int> chapters = database_versifications.getMaximumChapters (book);
    for (auto & chapter : chapters) {
      if (sendreceive_logic_prioritized_task_is_active ()) send_receive_priority_tasks_running = true;
      if (send_receive_priority_tasks_running) continue;
      if (sendreceive_resources_interrupt) continue;
      bool downloaded = false;
      string message1 = resource + ": " + bookName + " chapter " + convert_to_string (chapter);
      string message2;
      vector <int> verses = database_versifications.getMaximumVerses (book, chapter);
      for (auto & verse : verses) {
        if (sendreceive_logic_prioritized_task_is_active ()) send_receive_priority_tasks_running = true;
        if (send_receive_priority_tasks_running) continue;
        if (sendreceive_resources_interrupt) continue;
        message2 += "; verse " + convert_to_string (verse) + ": ";
        bool download_verse = false;
        if (Database_Cache::exists (resource, book, chapter, verse)) {
          message2 += "exists";
        } else {
          download_verse = true;
        }
        if (download_verse) {
          // Fetch the text for the passage.
          bool server_is_installing_module = false;
          int wait_iterations = 0;
          string html, error;
          do {
            // Fetch this resource from the server.
            string address = Database_Config_General::getServerAddress ();
            int port = Database_Config_General::getServerPort ();
            // If the client has not been connected to a cloud instance,
            // fetch the resource from the Bibledit Cloud demo.
            if (!client_logic_client_enabled ()) {
              address = demo_address ();
              port = demo_port ();
            }
            string url = client_logic_url (address, port, sync_resources_url ());
            url = filter_url_build_http_query (url, "r", filter_url_urlencode (resource));
            url = filter_url_build_http_query (url, "b", convert_to_string (book));
            url = filter_url_build_http_query (url, "c", convert_to_string (chapter));
            url = filter_url_build_http_query (url, "v", convert_to_string (verse));
            error.clear ();
            html = filter_url_http_get (url, error);
            server_is_installing_module = (html == sword_logic_installing_module_text ());
            if (server_is_installing_module) {
              Database_Logs::log ("Waiting while Bibledit Cloud installs the requested SWORD module");
              this_thread::sleep_for (chrono::seconds (60));
              wait_iterations++;
            }
          } while (server_is_installing_module && (wait_iterations < 5));
          // Cache it.
          if (error.empty ()) {
            if (!Database_Cache::exists (resource)) Database_Cache::create (resource);
            Database_Cache::cache (resource, book, chapter, verse, html);
            message2 += "size " + convert_to_string (html.length ());
          } else {
            message2.append ("error " + error);
            error_count++;
            this_thread::sleep_for (chrono::seconds (1));
          }
          downloaded = true;
        }
        sendreceive_resources_kick_watchdog ();
      }
      message2 += "; done";
      if (!downloaded) message2 = ": already in cache";
      if (downloaded) Database_Logs::log (message1 + message2, Filter_Roles::manager ());
    }
  }
  
  // Done.
  if (error_count) {
    string msg = "Error count while downloading resource: " + convert_to_string (error_count);
    Database_Logs::log (msg, Filter_Roles::consultant ());
  }
  Database_Logs::log ("Completed downloading resource:" " " + resource, Filter_Roles::consultant ());
  // In case of too many errors, schedule the resource download again.
  bool re_schedule_download = false;
  if (error_count > 10) {
    if (!sendreceive_resources_interrupt) {
      re_schedule_download = true;
      Database_Logs::log ("Too many errors: Re-scheduling resource download", Filter_Roles::consultant ());
    }
  }
  // In case the resource download was interrupted by higher priority tasks, schedule the download again.
  if (send_receive_priority_tasks_running) {
    re_schedule_download = true;
    Database_Logs::log ("Priority tasks are running: Re-scheduling resource download: " + resource, Filter_Roles::consultant ());
  }
  // Store new download schedule.
  resources = Database_Config_General::getResourcesToCache ();
  resources = filter_string_array_diff (resources, {resource});
  if (re_schedule_download) {
    resources.push_back (resource);
  }
  Database_Config_General::setResourcesToCache (resources);

  sendreceive_resources_done ();
  
  sendreceive_resources_interrupt = false;

  // If there's another resource waiting to be cached, schedule it for caching.
  if (!resources.empty ()) tasks_logic_queue (SYNCRESOURCES);
}


// This stops all resources that have been scheduled to install.
void sendreceive_resources_clear_all ()
{
  sendreceive_resources_interrupt = true;
  Database_Logs::log ("Interrupting resource installation", Filter_Roles::consultant ());
  Database_Config_General::setResourcesToCache ({});
}

