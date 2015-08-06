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
#include <resource/admin.h>
#include <webserver/request.h>
#include <access/bible.h>
#include <database/usfmresources.h>
#include <database/offlineresources.h>
#include <database/imageresources.h>
#include <database/mappings.h>
#include <database/config/bible.h>
#include <database/config/general.h>
#include <filter/string.h>
#include <filter/usfm.h>
#include <filter/text.h>
#include <resource/external.h>
#include <locale/translate.h>
#include <config/logic.h>
#include <client/logic.h>


vector <string> Resource_Logic::getNames (void * webserver_request)
{
  vector <string> names;
  
  // Take Bibles the user has read access to.
  vector <string> bibles = access_bible_bibles (webserver_request);
  names.insert (names.end(), bibles.begin (), bibles.end());
  
  // Take USFM Resources.
  Database_UsfmResources database_usfmresources;
  vector <string> usfm_resources = database_usfmresources.getResources ();
  names.insert (names.end(), usfm_resources.begin(), usfm_resources.end());
  
  // Take external Resources.
  vector <string> external_resources = resource_external_names ();
  names.insert (names.end (), external_resources.begin(), external_resources.end());
  
  // Take Image resources.
  Database_ImageResources database_imageresources;
  vector <string> image_resources = database_imageresources.names ();
  names.insert (names.end (), image_resources.begin(), image_resources.end());
  
  sort (names.begin(), names.end());
  
  return names;
}


string Resource_Logic::getExternal (string bible, string resource, int book, int chapter, int verse, bool apply_mapping)
{
  vector <Passage> passages;
  if (apply_mapping) {
    Database_Mappings database_mappings = Database_Mappings ();
    string bible_mapping = Database_Config_Bible::getVerseMapping (bible);
    string resource_mapping = resource_external_mapping (resource);
    passages = database_mappings.translate (bible_mapping, resource_mapping, book, chapter, verse);
  } else {
    passages.push_back (Passage ("", book, chapter, convert_to_string (verse)));
  }
  string output;
  for (auto passage : passages) {
    string html = resource_external_get (resource, passage.book, passage.chapter, convert_to_int (passage.verse));
    output.append (html);
  }
  return output;
}


string Resource_Logic::getHtml (void * webserver_request, string resource, int book, int chapter, int verse)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  string html;
  
  Database_UsfmResources database_usfmresources;
  Database_OfflineResources database_offlineresources;
  Database_ImageResources database_imageresources;
  Database_Mappings database_mappings = Database_Mappings ();
  
  vector <string> bibles = request->database_bibles()->getBibles ();
  vector <string> usfms = database_usfmresources.getResources ();
  vector <string> externals = resource_external_names ();
  vector <string> images = database_imageresources.names ();
  
  bool isBible = find (bibles.begin(), bibles.end (), resource) != bibles.end ();
  bool isUsfm = find (usfms.begin (), usfms.end (), resource) != usfms.end ();
  bool isExternal = find (externals.begin (), externals.end (), resource) != externals.end ();
  bool isImage = find (images.begin (), images.end (), resource) != images.end ();
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
    string bible = request->database_config_user()->getBible ();
    if (database_offlineresources.exists (resource, book, chapter, verse)) {
      string bible_mapping = Database_Config_Bible::getVerseMapping (bible);
      string resource_mapping = resource_external_mapping (resource);
      vector <Passage> passages = database_mappings.translate (bible_mapping, resource_mapping, book, chapter, verse);
      for (auto& passage : passages) {
        html.append (database_offlineresources.get (resource, passage.book, passage.chapter, convert_to_int (passage.verse)));
      }
    } else {
      if (config_logic_client_prepared ()) {
        html = translate ("This resource is not available.");
        html.append (" ");
        html.append (translate ("To make it available, follow these steps:"));
        html.append (" ");
        if (resource_admin_acl (request)) {
          html.append (client_logic_link_to_cloud (resource_admin_url (), translate ("Go to Bibledit Cloud.")));
          html.append (" ");
          html.append (translate ("Cache this external resource there."));
        } else {
          html.append (client_logic_link_to_cloud ("", translate ("Ask a manager to cache this external resource in Bibledit Cloud.")));
        }
        html.append (" ");
        html.append (translate ("Go back on the Bibledit client here."));
        html.append (" ");
        html.append (translate ("Synchronize."));
      } else {
        html = Resource_Logic::getExternal (bible, resource, book, chapter, verse, true);
      }
    }
  } else if (isImage) {
    vector <string> images = database_imageresources.get (resource, book, chapter, verse);
    for (auto & image : images) {
      html.append ("<div><img src=\"/resource/imagefetch?name=" + resource + "&image=" + image + "\" alt=\"Image resource\" style=\"width:100%\"></div>");
    }
    // Todo
  } else {
    // Nothing found.
  }
  
  return html;
}

