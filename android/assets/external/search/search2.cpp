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


#include <search/search2.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/passage.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <database/volatile.h>
#include <database/config/general.h>
#include <access/bible.h>
#include <ipc/focus.h>


string search_search2_url ()
{
  return "search/search2";
}


bool search_search2_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string search_search2 (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  
  Database_Volatile database_volatile = Database_Volatile ();
  

  string siteUrl = Database_Config_General::getSiteURL ();
  
  
  string bible = request->database_config_user()->getBible ();
  if (request->query.count ("bible")) bible = request->query ["bible"];

  
  bool hit_is_set = request->query.count ("h");
  bool query_is_set = request->query.count ("q");
  int identifier = convert_to_int (request->query ["i"]);
  string query = request->query ["q"];
  int hit = convert_to_int (request->query ["h"]);

  
  // Get one search hit.
  if (hit_is_set) {
    
    
    // Retrieve the search parameters from the volatile database.
    string query = database_volatile.getValue (identifier, "query");
    //bool casesensitive = convert_to_bool (database_volatile.getValue (identifier, "casesensitive"));
    bool plaintext = convert_to_bool (database_volatile.getValue (identifier, "plaintext"));
    
    
    // Get the Bible and passage for this identifier.
    Passage details = request->database_search()->getBiblePassage (hit);
    string bible = details.bible;
    int book = details.book;
    int chapter = details.chapter;
    string verse = details.verse;
    
    
    // Get the plain text or USFM.
    string text;
    if (plaintext) {
      text = request->database_search()->getBibleVerseText (bible, book, chapter, convert_to_int (verse));
    } else {
      text = request->database_search()->getBibleVerseUsfm (bible, book, chapter, convert_to_int (verse));
    }
    
    
    // Format it.
    string link = filter_passage_link_for_opening_editor_at (book, chapter, verse);
    text =  filter_string_markup_words ({query}, text);
    string output = "<div>" + link + " " + text + "</div>";
    
    
    // Output to browser.
    return output;
  }
  

  // Perform the initial search.
  if (query_is_set) {
    
    
    // Get extra search parameters and store them all in the volatile database.
    bool casesensitive = (request->query ["c"] == "true");
    bool plaintext = (request->query ["p"] == "true");
    bool currentbook = (request->query ["b"] == "true");
    string sharing = request->query ["s"];
    database_volatile.setValue (identifier, "query", query);
    database_volatile.setValue (identifier, "casesensitive", convert_to_string (casesensitive));
    database_volatile.setValue (identifier, "plaintext", convert_to_string (plaintext));
    
    
    // Deal with case sensitivity.
    // Deal with whether to search in the main text without notes, or in the raw USFM.
    // Fetch the initial set of hits.
    vector <int> hits;
    if (plaintext) {
      if (casesensitive) {
        hits = request->database_search()->searchBibleTextCaseSensitive (bible, query);
      } else {
        hits = request->database_search()->searchBibleText (bible, query);
      }
    } else {
      if (casesensitive) {
        hits = request->database_search()->searchBibleUsfmCaseSensitive (bible, query);
      } else {
        hits = request->database_search()->searchBibleUsfm (bible, query);
      }
    }
    
    
    // Deal with possible searching in the current book only.
    if (currentbook) {
      int book = Ipc_Focus::getBook (request);
      vector <int> bookhits;
      for (auto & hit : hits) {
        Passage details = request->database_search()->getBiblePassage (hit);
        if (book == details.book) {
          bookhits.push_back (hit);
        }
      }
      hits = bookhits;
    }
    
    
    // Deal with how to share the results.
    if (sharing != "load") {
      vector <int> loadedHits;
      vector <string> s_loadedHits = filter_string_explode (database_volatile.getValue (identifier, "hits"), '\n');
      for (auto & hit : s_loadedHits) loadedHits.push_back (convert_to_int (hit));
      if (sharing == "add") {
        hits.insert (hits.end(), loadedHits.begin(), loadedHits.end());
      }
      if (sharing == "remove") {
        hits = filter_string_array_diff (loadedHits, hits);
      }
      if (sharing == "intersect") {
        hits = array_intersect (loadedHits, hits);
      }
      hits = array_unique (hits);
    }
    
    
    // Generate one string from the hits.
    string output;
    for (auto & hit : hits) {
      if (!output.empty ()) output.append ("\n");
      output.append (convert_to_string (hit));
    }

    
    // Store search hits in the volatile database.
    database_volatile.setValue (identifier, "hits", output);
    
    
    // Output results.
    return output;
  }
  
  
  // Build the advanced search page.
  string page;
  Assets_Header header = Assets_Header (translate("Search"), request);
  header.setNavigator ();
  page = header.run ();
  Assets_View view;
  view.set_variable ("bible", bible);
  string script = "var searchBible = \"" + bible + "\";";
  view.set_variable ("script", script);
  page += view.render ("search", "search2");
  page += Assets_Page::footer ();
  return page;
}
