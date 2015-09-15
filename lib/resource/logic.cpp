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
#include <database/logs.h>
#include <filter/string.h>
#include <filter/usfm.h>
#include <filter/text.h>
#include <filter/url.h>
#include <filter/archive.h>
#include <filter/shell.h>
#include <resource/external.h>
#include <locale/translate.h>
#include <config/logic.h>
#include <client/logic.h>
#include <lexicon/logic.h>


vector <string> Resource_Logic::getNames (void * webserver_request)
{
  vector <string> names;
  
  // Bibles the user has read access to.
  vector <string> bibles = access_bible_bibles (webserver_request);
  names.insert (names.end(), bibles.begin (), bibles.end());
  
  // USFM resources.
  Database_UsfmResources database_usfmresources;
  vector <string> usfm_resources = database_usfmresources.getResources ();
  names.insert (names.end(), usfm_resources.begin(), usfm_resources.end());
  
  // External resources.
  vector <string> external_resources = resource_external_names ();
  if (config_logic_client_prepared ()) {
    // A client displays only the downloaded external resources.
    Database_OfflineResources database_offlineresources;
    for (auto & resource : external_resources) {
      if (database_offlineresources.exists (resource)) {
        names.push_back (resource);
      }
    }
  } else {
    // A server displays all external resources.
    names.insert (names.end (), external_resources.begin(), external_resources.end());
  }
  
  // Image resources.
  Database_ImageResources database_imageresources;
  vector <string> image_resources = database_imageresources.names ();
  names.insert (names.end (), image_resources.begin(), image_resources.end());
  
  // Lexicon resources.
  vector <string> lexicon_resources = lexicon_logic_resource_names ();
  names.insert (names.end (), lexicon_resources.begin(), lexicon_resources.end());
  
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
  vector <string> lexicons = lexicon_logic_resource_names ();
  
  bool isBible = in_array (resource, bibles);
  bool isUsfm = in_array (resource, usfms);
  bool isExternal = in_array (resource, externals);
  bool isImage = in_array (resource, images);
  bool isLexicon = in_array (resource, lexicons);
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
      if (!config_logic_client_prepared ()) {
        html = Resource_Logic::getExternal (bible, resource, book, chapter, verse, true);
      }
    }
  } else if (isImage) {
    vector <string> images = database_imageresources.get (resource, book, chapter, verse);
    for (auto & image : images) {
      html.append ("<div><img src=\"/resource/imagefetch?name=" + resource + "&image=" + image + "\" alt=\"Image resource\" style=\"width:100%\"></div>");
    }
  } else if (isLexicon) {
    html = lexicon_logic_get_html (request, resource, book, chapter, verse);
  } else {
    // Nothing found.
  }
  
  return html;
}



// Imports the file at $path into $resource.
void resource_logic_import_images (string resource, string path)
{
  Database_ImageResources database_imageresources;
  
  Database_Logs::log ("Importing: " + filter_url_basename (path));
  
  // To begin with, add the path to the main file to the list of paths to be processed.
  vector <string> paths = {path};
  
  while (!paths.empty ()) {
  
    // Take and remove the first path from the container.
    path = paths[0];
    paths.erase (paths.begin());
    string basename = filter_url_basename (path);
    string extension = filter_url_get_extension (path);
    extension = unicode_string_casefold (extension);

    if (extension == "pdf") {
      
      Database_Logs::log ("Processing PDF: " + basename);
      
      // Retrieve PDF information.
      filter_shell_run ("", "pdfinfo", {path}, NULL, NULL);

      // Convert the PDF file to separate images.
      string folder = filter_url_tempfile ();
      filter_url_mkdir (folder);
      filter_shell_run (folder, "pdftocairo", {"-jpeg", path}, NULL, NULL);
      // Add the images to the ones to be processed.
      filter_url_recursive_scandir (folder, paths);
      
    } else if (filter_archive_is_archive (path)) {
      
      Database_Logs::log ("Unpacking archive: " + basename);
      string folder = filter_archive_uncompress (path);
      filter_url_recursive_scandir (folder, paths);
      
    } else {

      if (!extension.empty ()) {
        basename = database_imageresources.store (resource, path);
        Database_Logs::log ("Storing image " + basename );
      }
      
    }
  }

  Database_Logs::log ("Ready importing images");
}


string resource_logic_yellow_divider ()
{
  return "Yellow Divider";
}


string resource_logic_green_divider ()
{
  return "Green Divider";
}


string resource_logic_blue_divider ()
{
  return "Blue Divider";
}


string resource_logic_violet_divider ()
{
  return "Violet Divider";
}


string resource_logic_red_divider ()
{
  return "Red Divider";
}


string resource_logic_orange_divider ()
{
  return "Orange Divider";
}


bool resource_logic_is_divider (string resource)
{
  if (resource == resource_logic_yellow_divider ()) return true;
  if (resource == resource_logic_green_divider ()) return true;
  if (resource == resource_logic_blue_divider ()) return true;
  if (resource == resource_logic_violet_divider ()) return true;
  if (resource == resource_logic_red_divider ()) return true;
  if (resource == resource_logic_orange_divider ()) return true;
  return false;
}


string resource_logic_get_divider (string resource)
{
  vector <string> bits = filter_string_explode (resource, ' ');
  string colour = unicode_string_casefold (bits [0]);
  // The $ influences the resource's embedding through javascript.
  string html = "$<div class=\"fullwidth\" style=\"background-color: " + colour + ";\">&nbsp;</div>";
  return html;
}
