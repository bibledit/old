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


#include <search/rebibles.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <filter/usfm.h>
#include <database/logs.h>
#include <webserver/request.h>
#include <search/logic.h>


bool search_reindex_bibles_running = false;


void search_reindex_bibles () // Todo
{
  // One simultaneous instance.
  if (search_reindex_bibles_running) {
    Database_Logs::log ("Still indexing Bibles", Filter_Roles::manager ());
    return;
  }
  search_reindex_bibles_running = true;
  
  
  // This does not run as a result of a webserver request, so create one.
  Webserver_Request request;

  
  // This checks whether the data in the search index matches the data in the Bibles.
  // Under normal circumstances the search index matches the data in the Bibles.
  // But it may happen that a record could not be updated in the search index, or that another error occurred.
  // Then the search index no longer is in sync with the Bible data.
  // Therefore below it is going to check whether everything matches.
  // It removes superfluous data, and checks that all Bible data is present in the search database.
  
  
  // Updating the search indexes if there's a difference with the data in the Bibles.
  // There's a difference if the raw USFM differs. 
  // When the raw USFM matches for a chapter, than this chapter is considered okay.
  vector <string> bibles = request.database_bibles ()->getBibles ();
  for (auto & bible : bibles) {
    Database_Logs::log ("Updating search index for Bible " + bible, Filter_Roles::manager ());
    vector <int> books = request.database_bibles ()->getBooks (bible);
    for (auto book : books) {
      vector <int> chapters = request.database_bibles ()->getChapters (bible, book);
      for (auto chapter : chapters) {
        string usfm = request.database_bibles ()->getChapter (bible, book, chapter);
        vector <int> verses = usfm_get_verse_numbers (usfm);
        bool updateChapter = false;
        for (auto verse : verses) {
          if (!updateChapter) {
            string verseUSFM = usfm_get_verse_text (usfm, verse);
            /* Todo
            string searchUSFM = request.database_search ()->getBibleVerseUsfm (bible, book, chapter, verse);
            if (searchUSFM != verseUSFM) {
              updateChapter = true;
            }
             */
          }
        }
        if (updateChapter) {
          search_logic_index_chapter (bible, book, chapter);
        }
      }
    }
  }
  
  
  Database_Logs::log ("Recreating Bible indexes ready", Filter_Roles::manager ());
  search_reindex_bibles_running = false;
}

