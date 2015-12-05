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


#include <resource/download.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/usfm.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <resource/logic.h>
#include <database/books.h>
#include <database/versifications.h>
#include <database/offlineresources.h>
#include <database/logs.h>
#include <access/bible.h>
#include <tasks/logic.h>
#include <journal/index.h>
#include <dialog/list.h>
#include <resource/external.h>
#include <config/logic.h>
#include <menu/logic.h>
#include <resource/admin.h>


string resource_download_url ()
{
  return "resource/download";
}


bool resource_download_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::manager ());
}


string resource_download (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  Database_OfflineResources database_offlineresources = Database_OfflineResources ();
  
  
  string page;
  Assets_Header header = Assets_Header (translate("Download resource"), request);
  header.addBreadCrumb (menu_logic_settings_menu (), menu_logic_settings_text ());
  header.addBreadCrumb (resource_admin_url (), menu_logic_resource_admin_text ());
  page = header.run ();
  Assets_View view;

  
  string name = request->query["name"];
  view.set_variable ("name", name);
                      
                      
  if (request->query.count ("clear")) {
    database_offlineresources.erase (name);
  }
  
  
  if (request->query.count ("download")) {
    tasks_logic_queue (DOWNLOADRESOURCE, {name});
    redirect_browser (request, journal_index_url ());
    return "";
  }
  
  
  int count = database_offlineresources.count (name);
  view.set_variable ("count", convert_to_string (count));
                      
                      
  page += view.render ("resource", "download");
  page += Assets_Page::footer ();
  return page;
}


void resource_download_job (string resource)
{
  Database_Logs::log (resource + ": Download has started", Filter_Roles::manager ());

  Database_OfflineResources database_offlineresources = Database_OfflineResources ();
  
  bool download = true;
  
  if (config_logic_external_resources_cache_configured ()) {
    database_offlineresources.link_to_central_cache (resource);
    if (database_offlineresources.exists (resource)) {
      Database_Logs::log (resource + ": Linked to central cache", Filter_Roles::manager ());
      download = false;
    }
  }

  if (download) {

    Database_Versifications database_versifications;
    
    string versification = resource_external_versification (resource);
    
    vector <int> books = database_versifications.getBooks (versification);
    for (auto & book : books) {
      
      string bookName = Database_Books::getEnglishFromId (book);
      
      bool downloaded = false;
      
      vector <int> chapters = database_versifications.getChapters (versification, book, true);
      for (auto & chapter : chapters) {
        string message1 = resource + ": " + bookName + " chapter " + convert_to_string (chapter);
        string message2;
        vector <int> verses = database_versifications.getVerses (versification, book, chapter);
        for (auto & verse : verses) {
          message2 += "; verse " + convert_to_string (verse) + ": ";
          bool download_verse = false;
          if (database_offlineresources.exists (resource, book, chapter, verse)) {
            message2 += "exists";
          } else {
            download_verse = true;
          }
          if (!download_verse) {
            string contents = database_offlineresources.get (resource, book, chapter, verse);
            if (contents.find ("http code") != string::npos) {
              message2 += " repair ";
              download_verse = true;
            }
          }
          if (download_verse) {
            string html = resource_external_get (resource, book, chapter, verse);
            database_offlineresources.store (resource, book, chapter, verse, html);
            message2 += "size " + convert_to_string (html.length ());
            downloaded = true;
          }
        }
        message2 += "; done";
        if (!downloaded) message2 = ": already in cache";
        Database_Logs::log (message1 + message2, Filter_Roles::manager ());
      }
    }

  }
  
  Database_Logs::log (resource + ": Completed", Filter_Roles::manager ());
}
