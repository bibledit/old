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


void sendreceive_resources_delay_during_prioritized_tasks ()
{
  if (sendreceive_logic_prioritized_task_is_active ()) {
    // When prioritized sync actions ran,
    // it used to interrupt and reschedule a resource installation.
    // This gives a lot of clutter in the journal,
    // and it takes some time to restart the resource installation.
    // Rather it now delays the installatiion a bit while the priority tasks flag is on.
    // That delay is not visible in the Journal, it just happens silently.
    this_thread::sleep_for (chrono::seconds (10));
  }
}


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

  // If there's nothing to cache, bail out.
  vector <string> resources = Database_Config_General::getResourcesToCache ();
  if (resources.empty ()) return;
  
  sendreceive_resources_kick_watchdog ();

  // Error counter.
  int error_count = 0;
  
  // Resource to cache.
  string resource = resources [0];
  
  // Erase the two older storage locations that were used to cache resources in earlier versions of Bibledit.
  {
    Database_OfflineResources database_offlineresources;
    Database_UsfmResources database_usfmresources;
    database_offlineresources.erase (resource);
    database_usfmresources.deleteResource (resource);
  }

  Database_Logs::log ("Starting to install resource:" " " + resource, Filter_Roles::consultant ());
  
  Database_Versifications database_versifications;
  
  vector <int> books = database_versifications.getMaximumBooks ();
  for (auto & book : books) {
    sendreceive_resources_delay_during_prioritized_tasks ();
    if (sendreceive_resources_interrupt) continue;
    
    // Database layout is per book: Create a database for this book.
    Database_Cache::create (resource, book);

    // Last downloaded passage in a previous download operation.
    int last_downloaded_passage = 0;
    {
      pair <int, int> progress = Database_Cache::progress (resource, book);
      int chapter = progress.first;
      int verse = progress.second;
      Passage passage ("", book, chapter, convert_to_string (verse));
      last_downloaded_passage = filter_passage_to_integer (passage);
    }
    
    // List of passages recorded in the database that had errors on a previous download operation.
    vector <int> previous_errors;
    {
      vector <pair <int, int> > errors = Database_Cache::errors (resource, book);
      for (auto & element : errors) {
        int chapter = element.first;
        int verse = element.second;
        Passage passage ("", book, chapter, convert_to_string (verse));
        int numeric_error = filter_passage_to_integer (passage);
        previous_errors.push_back (numeric_error);
      }
    }
    
    string bookName = Database_Books::getEnglishFromId (book);
    
    vector <int> chapters = database_versifications.getMaximumChapters (book);
    for (auto & chapter : chapters) {
      sendreceive_resources_delay_during_prioritized_tasks ();
      if (sendreceive_resources_interrupt) continue;
      bool downloaded = false;
      string message = resource + ": " + bookName + " chapter " + convert_to_string (chapter);
      vector <int> verses = database_versifications.getMaximumVerses (book, chapter);
      for (auto & verse : verses) {
        sendreceive_resources_delay_during_prioritized_tasks ();
        if (sendreceive_resources_interrupt) continue;
        // Numeric representation of passage to deal with.
        Passage passage ("", book, chapter, convert_to_string (verse));
        int numeric_passage = filter_passage_to_integer (passage);
        // Conditions to download this verse:
        // 1. The passage is past the last downloaded passage.
        bool download_verse_past = numeric_passage > last_downloaded_passage;
        // 2. The passage was recorded as an error in a previous download operation.
        bool download_verse_error = in_array (numeric_passage, previous_errors);
        // Whether to download the verse.
        if (download_verse_past || download_verse_error) {
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
          // Record the passage as having been done in case it was a regular download,
          // rather than one to retry a previous download error.
          if (download_verse_past) Database_Cache::progress (resource, book, chapter, verse);
          // Clear the registered error in case the verse download corrects it.
          if (download_verse_error) Database_Cache::error (resource, book, chapter, verse, false);
          if (error.empty ()) {
            // Cache the verse data.
            if (!Database_Cache::exists (resource, book)) Database_Cache::create (resource, book);
            Database_Cache::cache (resource, book, chapter, verse, html);
          } else {
            // Record an error.
            Database_Cache::error (resource, book, chapter, verse, true);
            if (message.find (error) == string::npos) {
              message.append ("; " + error);
            }
            error_count++;
            this_thread::sleep_for (chrono::seconds (1));
          }
          downloaded = true;
        }
        sendreceive_resources_kick_watchdog ();
      }
      message += "; done";
      if (downloaded) Database_Logs::log (message, Filter_Roles::manager ());
    }
  }
  
  // Done.
  if (error_count) {
    string msg = "Error count while downloading resource: " + convert_to_string (error_count);
    Database_Logs::log (msg, Filter_Roles::consultant ());
  }
  Database_Logs::log ("Completed installing resource:" " " + resource, Filter_Roles::consultant ());
  // In case of too many errors, schedule the resource download again.
  bool re_schedule_download = false;
  if (error_count) {
    if (!sendreceive_resources_interrupt) {
      re_schedule_download = true;
      Database_Logs::log ("Errors: Re-scheduling resource installation", Filter_Roles::consultant ());
    }
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

