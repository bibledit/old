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


#include <sendreceive/resources.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/date.h>
#include <tasks/logic.h>
#include <config/logic.h>
#include <database/config/general.h>
#include <database/logs.h>
#include <database/versifications.h>
#include <database/books.h>
#include <database/cache.h>
#include <client/logic.h>
#include <locale/translate.h>
#include <webserver/request.h>
#include <sync/logic.h>
#include <sync/resources.h>
#include <sword/logic.h>


int sendreceive_resources_watchdog = 0;


void sendreceive_resources_kick_watchdog ()
{
  sendreceive_resources_watchdog = filter_date_seconds_since_epoch ();
}


void sendreceive_resources_done ()
{
  sendreceive_resources_watchdog = 0;
}


void sendreceive_resources () // Todo
{
  if (sendreceive_resources_watchdog) {
    int time = filter_date_seconds_since_epoch ();
    if (time < (sendreceive_resources_watchdog + 900)) {
      return;
    }
    Database_Logs::log ("Resources: " + translate("Watchdog timeout"), Filter_Roles::translator ());
    sendreceive_resources_done ();
  }

  // If there's nothing to cache, bail out.
  vector <string> resources = Database_Config_General::getResourcesToCache ();
  if (resources.empty ()) return;
  
  sendreceive_resources_kick_watchdog ();

  // Resource to cache.
  string resource = resources [0];
  Database_Logs::log ("Starting to cache resource:" " " + resource, Filter_Roles::consultant ());
  string source = sword_logic_get_source (resource);
  string module = sword_logic_get_remote_module (resource);
  Database_Cache::create (module);
  
  Database_Versifications database_versifications = Database_Versifications ();
  
  // SWORD resources is assumed to have the English versification.
  string versification = "English";
  
  vector <int> books = database_versifications.getBooks (versification);
  for (auto & book : books) {
    
    string bookName = Database_Books::getEnglishFromId (book);
    
    vector <int> chapters = database_versifications.getChapters (versification, book, true);
    for (auto & chapter : chapters) {
      bool downloaded = false;
      string message1 = resource + ": " + bookName + " chapter " + convert_to_string (chapter);
      string message2;
      vector <int> verses = database_versifications.getVerses (versification, book, chapter);
      for (auto & verse : verses) {
        message2 += "; verse " + convert_to_string (verse) + ": ";
        bool download_verse = false;
        if (Database_Cache::exists (module, book, chapter, verse)) {
          message2 += "exists";
        } else {
          download_verse = true;
        }
        if (!download_verse) {
          string contents = Database_Cache::retrieve (module, book, chapter, verse);
          if (contents.find ("http code") != string::npos) {
            message2 += " repair ";
            download_verse = true;
          }
        }
        if (download_verse) {
          // Fetch the text for the passage: That already caches it.
          string html = sword_logic_get_text (source, module, book, chapter, verse);
          message2 += "size " + convert_to_string (html.length ());
          downloaded = true;
        }
      }
      message2 += "; done";
      if (!downloaded) message2 = ": already in cache";
      Database_Logs::log (message1 + message2, Filter_Roles::manager ());
    }
  }
  
  // Done.
  Database_Logs::log ("Completed caching resource:" " " + resource, Filter_Roles::consultant ());
  resources = Database_Config_General::getResourcesToCache ();
  resources = filter_string_array_diff (resources, {resource});
  Database_Config_General::setResourcesToCache (resources);
  sendreceive_resources_done ();
  
  // If there's another resources waiting to be cached, schedule it for caching.
  if (!resources.empty ()) tasks_logic_queue (SYNCRESOURCES);
}


