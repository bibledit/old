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


#include <search/replacepre.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/passage.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <database/config/general.h>


string search_replacepre_url ()
{
  return "search/replacepre";
}


bool search_replacepre_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::translator ());
}


string search_replacepre (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  
  string siteUrl = Database_Config_General::getSiteURL ();
  
  
  // Get search variables from the query.
  string searchfor = request->query ["q"];
  string replacewith = request->query ["r"];
  bool casesensitive = (request->query ["c"] == "true");
  int id = convert_to_int (request->query ["id"]);
  
  
  // Get the Bible and passage for this identifier.
  /* Todo
  Passage details = request->database_search()->getBiblePassage (id);
  string bible = details.bible;
  int book = convert_to_int (details.book);
  int chapter = convert_to_int (details.chapter);
  string verse = details.verse;
  
  
  // Get the plain text.
  string text = search_logic_get_bible_verse_text (bible, book, chapter, convert_to_int (verse));
  
  // Clickable passage.
  string link = filter_passage_link_for_opening_editor_at (book, chapter, verse);
  
  
  string oldtext = filter_string_markup_words ({searchfor}, text);
  

  string newtext;
  if (casesensitive) {
    newtext = filter_string_str_replace (searchfor, replacewith, text);
  } else {
    newtext = text;
    vector <string> needles = filter_string_search_needles (searchfor, text);
    for (auto & needle : needles) {
      newtext = filter_string_str_replace (needle, replacewith, newtext);
    }
  }
  if (replacewith != "") newtext = filter_string_markup_words ({replacewith}, newtext);
  
  
  // The id sent to the browser contains bible identifier, book, chapter, and verse.
  int bibleID = request->database_bibles()->getID (bible);
  vector <string> bits = { convert_to_string (bibleID), convert_to_string (book), convert_to_string (chapter), verse};
  string s_id = filter_string_implode (bits, "_");
  
  
  // Create output.
  string output =
  "<div id=\"" + s_id + "\">\n"
  "<p><a href=\"replace\"> ✔ </a> <a href=\"delete\"> ✗ </a> " + link + "</p>\n"
  "<p>" + oldtext + "</p>\n"
  "<p>" + newtext + "</p>\n"
  "</div>\n";
  
  
  // Output to browser.
  return output;
   */
}
