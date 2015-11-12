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

  
  // Health check on the database.
  bool recreate = request.database_search ()->checkup ();
  if (recreate) {
    Database_Logs::log ("Recreating damaged search database", Filter_Roles::manager ());
  }
  
  
  request.database_search ()->optimize ();
  
  
  // This checks whether the data in the search index matches the data in the Bibles.
  // Under normal circumstances the search index matches the data in the Bibles.
  // But it may happen that a record could not be updated in the search index, or that another error occurred.
  // Then the search index no longer is in sync with the Bible data.
  // Therefore below it is going to check whether everything matches.
  // It removes superfluous data, and checks that all Bible data is present in the search database.
  
  
  // Check for and delete extra Bibles from the search database.
  Database_Logs::log ("Trimming search database", Filter_Roles::manager ());
  vector <string> bibles = request.database_bibles ()->getBibles ();
  vector <string> searchBibles = request.database_search ()->getBibles ();
  for (auto & searchBible : searchBibles) {
    if (find (bibles.begin(), bibles.end(), searchBible) == bibles.end ()) {
      request.database_search ()->deleteBible (searchBible);
    } else {
  
      // Check for and delete extra books from the search database.
      vector <int> books = request.database_bibles ()->getBooks (searchBible);
      vector <int> searchBooks = request.database_search ()->getBooks (searchBible);
      for (auto searchBook : searchBooks) {
        if (find (books.begin(), books.end(), searchBook) == books.end()) {
          request.database_search ()->deleteBook (searchBible, searchBook);
        } else {
          
          // Check for and delete extra chapters from the search database.
          vector <int> chapters = request.database_bibles ()->getChapters (searchBible, searchBook);
          vector <int> searchChapters = request.database_search ()->getChapters (searchBible, searchBook);
          for (auto searchChapter : searchChapters) {
            if (find (chapters.begin(), chapters.end(), searchChapter) == chapters.end()) {
              request.database_search ()->deleteChapter (searchBible, searchBook, searchChapter);
            } else {
              
              // Check for extra verses in the search database, and take corrective action.
              string usfm = request.database_bibles ()->getChapter (searchBible, searchBook, searchChapter);
              vector <int> verses = usfm_get_verse_numbers (usfm);
              vector <int> searchVerses = request.database_search ()->getVerses (searchBible, searchBook, searchChapter);
              for (auto searchVerse : searchVerses) {
                if (find (verses.begin(), verses.end(), searchVerse) == verses.end()) {
                  request.database_search ()->deleteChapter (searchBible, searchBook, searchChapter);
                }
              }
            }
          }
        }
      }
    }
  }
  
  
  // Updating the search indexes if there's a difference with the data in the Bibles.
  // There's a difference if the raw USFM differs. 
  // When the raw USFM matches for a chapter, than this chapter is considered okay.
  bibles = request.database_bibles ()->getBibles ();
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
            string searchUSFM = request.database_search ()->getBibleVerseUsfm (bible, book, chapter, verse);
            if (searchUSFM != verseUSFM) {
              updateChapter = true;
            }
          }
        }
        if (updateChapter) {
          request.database_search ()->updateSearchFields (bible, book, chapter);
        }
      }
    }
  }
  
  
  Database_Logs::log ("Recreating Bible indexes ready", Filter_Roles::manager ());
  search_reindex_bibles_running = false;
}

