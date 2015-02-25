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


#include <resource/logic.h>
#include <webserver/request.h>
#include <access/bible.h>
#include <database/usfmresources.h>
#include <database/offlineresources.h>
#include <database/resources.h>
#include <database/mappings.h>
#include <database/config/bible.h>
#include <filter/string.h>
#include <filter/usfm.h>
#include <filter/text.h>
#include <resource/external.h>
#include <locale/translate.h>


vector <string> Resource_Logic::getNames (void * webserver_request)
{
  vector <string> names;
  
  // Take Bibles the user has read access to.
  vector <string> bibles = access_bible_bibles (webserver_request);
  names.insert (names.end(), bibles.begin (), bibles.end());
  
  // Take USFM Resources.
  Database_UsfmResources database_usfmresources = Database_UsfmResources ();
  vector <string> usfm_resources = database_usfmresources.getResources ();
  names.insert (names.end(), usfm_resources.begin(), usfm_resources.end());
  
  // Take external Resources.
  Database_Resources database_resources = Database_Resources ();
  vector <string> resources = database_resources.getNames ();
  names.insert (names.end (), resources.begin(), resources.end());
  
  names = array_unique (names);
  sort (names.begin(), names.end());
  
  return names;
}


string Resource_Logic::getExternal (void * webserver_request,
                                    string name, int book, int chapter, int verse,
                                    bool apply_mapping)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  vector <Passage> passages;
  if (apply_mapping) {
    Database_Resources database_resources = Database_Resources ();
    Database_Mappings database_mappings = Database_Mappings ();
    string bible = request->database_config_user()->getBible ();
    string bible_mapping = Database_Config_Bible::getVerseMapping (bible);
    string resource_mapping = database_resources.getMapping (name);
    passages = database_mappings.translate (bible_mapping, resource_mapping, book, chapter, verse);
  } else {
    passages.push_back (Passage ("", book, chapter, to_string (verse)));
  }
  string output;
  for (auto passage : passages) {
    string html = resource_external_get (name, passage.book, passage.chapter, convert_to_int (passage.verse));
    output.append (html);
  }
  return output;
}


string Resource_Logic::getHtml (void * webserver_request, string resource, int book, int chapter, int verse)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  string html;
  
  Database_Resources database_resources = Database_Resources ();
  Database_UsfmResources database_usfmresources = Database_UsfmResources ();
  Database_OfflineResources database_offlineresources = Database_OfflineResources ();
  Database_Mappings database_mappings = Database_Mappings ();
  
  vector <string> bibles = request->database_bibles()->getBibles ();
  vector <string> usfms = database_usfmresources.getResources ();
  vector <string> externals = database_resources.getNames ();
  
  bool isBible = find (bibles.begin(), bibles.end (), resource) != bibles.end ();
  bool isUsfm = find (usfms.begin (), usfms.end (), resource) != usfms.end ();
  bool isExternal = find (externals.begin (), externals.end (), resource) != externals.end ();
  cout << resource << " bible " << isBible << " usfm " << isUsfm << " external " << isExternal << endl; // Todo
  if (isBible || isUsfm) {
    string chapter_usfm;
    if (isBible) chapter_usfm = request->database_bibles()->getChapter (resource, book, chapter);
    if (isUsfm) chapter_usfm = database_usfmresources.getUsfm (resource, book, chapter);
    string verse_usfm = usfm_get_verse_text (chapter_usfm, verse);
    string stylesheet = request->database_config_user()->getStylesheet ();
    Filter_Text filter_text = Filter_Text (resource);
    filter_text.html_text_standard = new Html_Text (translate("Bible"));
    filter_text.addUsfmCode (verse_usfm);
    filter_text.run (stylesheet);
    html = filter_text.html_text_standard->getInnerHtml ();
  } else if (isExternal) {
    // Use offline copy if it exists, else fetch it online.
    if (database_offlineresources.exists (resource, book, chapter, verse)) {
      string bible = request->database_config_user()->getBible ();
      string bible_mapping = Database_Config_Bible::getVerseMapping (bible);
      string resource_mapping = database_resources.getMapping (resource);
      vector <Passage> passages = database_mappings.translate (bible_mapping, resource_mapping, book, chapter, verse);
      for (auto& passage : passages) {
        html.append (database_offlineresources.get (resource, passage.book, passage.chapter, convert_to_int (passage.verse)));
      }
    } else {
      html = Resource_Logic::getExternal (webserver_request, resource, book, chapter, verse, true);
    }
  } else {
    // Nothing found.
  }
  
  return html;
}

